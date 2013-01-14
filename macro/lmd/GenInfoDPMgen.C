//################################################################
//# Macros for DPM output check
//# author: Anastasia Karavdina
//# date: September, 2012
//#
//# to compile it add in "# install #" part of CMakeLists.txt lines:
//# add_executable(dpm_out_check GenInfoDPMgen.C)
//# target_link_libraries(dpm_out_check ${ROOT_LIBRARIES})
//#
//# to run it (and see options): 
//# ${PANDAROOT}/build/bin/./dpm_out_check --help
//################################################################

#include <iostream>
#include <sstream>

#include<TApplication.h>
#include<TCanvas.h>
#include<TROOT.h>
#include<TString.h>
#include<TChain.h>
#include<TFile.h>
#include<TClonesArray.h>
#include<TSystem.h>
#include<TH1.h>
#include<TH2.h>
#include<TRotation.h>
#include<TVector3.h>
#include<TMath.h>
#include<TGaxis.h>
#include<TNtuple.h>
#include<TLatex.h>
#include<TStopwatch.h>

#include<PndMCTrack.h>
#include<PndSdsMCPoint.h>
#include<string>
#include "TStopwatch.h"
#include "TString.h"
#include "TChain.h"
#include "TClonesArray.h"
#include "TH1.h"
#include "TH2.h"
#include "TLorentzVector.h"
#include "TDatabasePDG.h"
#include "TParticlePDG.h"
#include "TVector3.h"
#include "TCanvas.h"
#include "TFile.h"
//#include "PndMCTrack.h"
using namespace std;

// void GenInfoDPMgen(const int nEvents=2, const double beamMom = 1.5, const int startEvent=0, TString storePath="tmpOutputDPM", const int verboseLevel=3)
// {
int main(int __argc,char *__argv[]) {
  TString storePath="/data/FAIRsorf/pandaroot/trunk/macro/lmd/tmpOutputBkg";
  //  double beamMom = 1.5;
  TString beamMom ="";
  int startEvent=0;
  int nEvents=100;
  std::string pathStr="", beamMomStr="", startStr="", nEvStr="";
  // decode arguments
  if( __argc>1 && ( strcmp( __argv[1], "-help" ) == 0
		    || strcmp( __argv[1], "--help" ) == 0 ) ){
    
    std::cout << "This is script for checking DPM output with parameters\n"
	      <<"-path path to the file(s) \n"
	      <<"-s start event \n"
	      <<"-n number of events \n"
	      <<"-p beam momentum \n"
	      <<"Have fun! \n"
	      << std::endl;
    return 0;
  } 
  while ((optind < (__argc-1) ) && (__argv[optind][0]=='-')) {
    bool found=false;
    std::string sw = __argv[optind];
    if (sw=="-path"){
      optind++;
      pathStr = __argv[optind];
      found=true;
    }
    if (sw=="-s"){
      optind++;
      startStr = __argv[optind];
      found=true;
    }
    if (sw=="-n"){
      optind++;
      nEvStr = __argv[optind];
      found=true;
    }
    if (sw=="-p"){
      optind++;
      beamMomStr = __argv[optind];
      found=true;
    }
    if (!found){
      std::cout<< "Unknown switch: "
	       << __argv[optind] <<std::endl;
      optind++;
    }
  while ( (optind < __argc ) && __argv[optind][0]!='-' ) optind++; 
  }

  std::stringstream pathSStr(pathStr), beamSStr(beamMomStr),startSStr(startStr),nEvSStr(nEvStr);

  pathSStr >> storePath;
  beamSStr >> beamMom;
  startSStr >> startEvent;
  nEvSStr >> nEvents;
  // ----  Load libraries   -------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gSystem->Load("libLmdTrk");
  gSystem->Load("libLmdTrk");
  gSystem->Load("libPIPIGEN");
  // ------------------------------------------------------------------------
  
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------
  
  // ---- Input files --------------------------------------------------------
  TString simMC=storePath+"/Lumi_MC_";
  simMC += startEvent;
  simMC += ".root";
  TChain tMC("cbmsim");
  tMC.Add(simMC);
  // ---------------------------------------------------------------------------------

  //--- MC info -----------------------------------------------------------------
  TClonesArray* true_tracks=new TClonesArray("PndMCTrack");
  tMC.SetBranchAddress("MCTrack",&true_tracks);  //True Track to compare
  
  TClonesArray* true_points=new TClonesArray("PndSdsMCPoint");
  tMC.SetBranchAddress("LMDPoint",&true_points);  //True Points to compare
  //----------------------------------------------------------------------------------
  
  // ---- Output file ----------------------------------------------------------------
  TString out=storePath+"/GenInfo_out_DPM_all_beamMom_";
  //  int beamMomint= int(1000*beamMom);
  // double beamMomnew = beamMomint/1000;
  // cout<<"beamMom = "<<beamMom<<" beamMomint = "<<beamMomint<<" beamMomnew ="<<beamMomnew <<endl;
  out += beamMom;
  out += "_";
  out += startEvent;
  //  out += "_inel.root";
  out += "_inel_and_el.root";
  TFile *f = new TFile(out,"RECREATE");
  // ---------------------------------------------------------------------------------
  
  
  
  //--- Output histogram -----------------------------------------------------
  TH1 *hMCparticles = new TH1F("hMCparticles","Number of simulated particles;N;",5e4,0,5e4);
  TH1 *hMCpx = new TH1F("hMCpx","Px*;Px, GeV/c;",1e3,-1e1,1e1);
  TH1 *hMCpy = new TH1F("hMCpy","Py*;Py, GeV/c;",1e3,-1e1,1e1);
  TH1 *hMCpz = new TH1F("hMCpz","Pz*;Pz, GeV/c;",1e3,-1e1,1e1);
  TH1 *hMCphi = new TH1F("hMCphi","#phi*;#phi, rad;",1e2,-3.15,3.15);
  TH1 *hMCLabtheta = new TH1F("hMLabtheta","#theta (LAB);#theta, rad;",1e2,0,3.15);
  TH2 *hMCLABtheta_vs_ID = new TH2F("hMCLABtheta_vs_ID"," ;ID_{PDG};#theta(LAB), rad",6e3,-3e3,3e3,1e4,0,3.15);
  TH2 *hMCsmallLABtheta_vs_ID = new TH2F("hMCsmallLABtheta_vs_ID"," ;ID_{PDG};#theta(LAB), rad",6e3,-3e3,3e3,1e2,0,1.5e-2);
  TH2 *hMCsmallLABtheta_vs_Mass = new TH2F("hMCsmallLABtheta_vs_Mass"," ;Mass_{PDG};#theta(LAB), rad",1e3,-1,1,1e2,0,1.5e-2); 
  TH1 *hSumIDs = new TH1F("hSumIDs","sum of PDG IDs;",1e4,0,1e4);
  TH2 *hSumIDs_numPar = new TH2F("hSumIDs_numPar",";sum of PDG IDs;number of particles",1e4,0,1e4,20,0,20);
  TH2 *hSumIDs_numPar_smallTheta = new TH2F("hSumIDs_numPar_smallTheta",";sum of PDG IDs;number of particles",1e4,0,1e4,20,0,20);

 TNtuple *ntupMCTrk = new TNtuple("ntupMCTrk","Info about simulated trks","pdgid:x:y:z:mom:theta:phi");
 TNtuple *ntupMCHit = new TNtuple("ntupMCHit","Info about simulated hits","pdgid:x:y:z:mom:theta:phi:eloss:sensid");

 //int countPDGid[15];
 /// [0]=22 photon, [1]=111 pi0, [2]=130 K0_L, [3]=211 pi+, [4]=310 K0_S, [5]=321 K+
 /// [6]=333 phi, [7]=2112 n, [8]=2212 p, [9]=3122 lyambda, [10]=-211 pi-, [11]=-321 K-
 /// [12]= -2112 anti_n, [13]=-2212 anti_p, [14]=-3122 anti_lyambda
 // TLorentzVector PbarBeam, PTarget, particle;
 // PbarBeam.SetPxPyPzE(0.,0.,beamMom,TMath::Sqrt(cProtonMass*cProtonMass + beamMom*beamMom));
 // PTarget.SetPxPyPzE(0.,0.,0.,cProtonMass);

  vector <int> countPDGid(6246); //number of each possible particle, [i]= PDGid + countPDGid.size()/2.;
  vector <TString> process(10000);// process[i]= name of process with PGDis sum=i; 
  bool fsumID[1000][10000];
  for(int in=0;in<20;in++){
      for(int isum=0;isum<10000;isum++){
	fsumID[in][isum]=false;
      }
    }

  for (Int_t j=0; j<nEvents; j++){
  // for (Int_t j=25910; j<nEvents; j++){
    TString process_cur="";
    int sumIDnum=0;
    vector <int> vecPDGid;
    vector <int> smallTheta; 
    //   for(int pid=0;pid<3123;pid++){
    for(int pid=0;pid<6246;pid++){
      //cout<<"countPDGid["<<pid<<"]="<<countPDGid[pid]<<" current sum = "<<sumIDnum<<endl;
      countPDGid[pid]=0;
    }

  
  // MC info -----------------------------------------------------------------
  tMC.GetEntry(j); 
  

  // MC hit --------------------------------------------------------------------
  const int nMCpoints = true_points->GetEntriesFast();
  for(int ipmc=0;ipmc<nMCpoints;ipmc++){
    PndSdsMCPoint* MCPoint = (PndSdsMCPoint*)(true_points->At(ipmc));
    int MCp_id = MCPoint->GetTrackID();
    TVector3 MCpointV = MCPoint->GetPosition();
    TVector3 MCpointMom(MCPoint->GetPx(),MCPoint->GetPy(),MCPoint->GetPz());
    ntupMCHit->Fill(MCp_id,MCpointV.X(),MCpointV.Y(),MCpointV.Z(),MCpointMom.Theta(),MCpointMom.Phi(),MCPoint->GetEnergyLoss(),MCPoint->GetSensorID());
  }
  
  //MC trk --------------------------------------------------------------------

    const int nParticles = true_tracks->GetEntriesFast();
    // cout<<"==============================================="<<endl;
    if(j%500==0) 
    cout<<"Event #"<<j<<" has "<<nParticles<<" particle(s)"<<endl;
    // cout<<"And there are:"<<endl;
    hMCparticles->Fill(nParticles);
    double  TotCharge=0;
     for(int nk=0;nk<nParticles;nk++){

       PndMCTrack *mctrk =(PndMCTrack*) true_tracks->At(nk);
       TVector3 MomMCv = mctrk->GetMomentum();
       TVector3 PosMCv = mctrk->GetStartVertex();
       Int_t mcID = mctrk->GetPdgCode();
       if(fabs(mcID)>3122) cout<<"!!!!!!!! PDGid = "<<mcID<<endl;
       if(fabs(mcID)>3122) countPDGid[0] +=1;
       if(fabs(mcID)>3122) continue;
       ntupMCTrk->Fill(mcID,PosMCv.X(),PosMCv.Y(),PosMCv.Z(),MomMCv.Mag(),MomMCv.Theta(),MomMCv.Phi());
       vecPDGid.push_back(mcID);

     
       //       cout<<"#"<<nk<<"has PDGid="<<mcID<<endl;
       TDatabasePDG *fdbPDG = TDatabasePDG::Instance();
       TParticlePDG *fParticle = fdbPDG->GetParticle(mcID);
       double      mcMass = fParticle->Mass();
       Double_t  fCharge = fParticle->Charge();
       TotCharge += fCharge;
       //       int IDpos = fabs(mcID);
       int IDpos = mcID + 0.5*countPDGid.size();
       //       cout<<" IDpos = "<< IDpos<<endl;
       countPDGid[IDpos] +=1;
       TLorentzVector MomMC = mctrk->Get4Momentum();
       // = new TNtuple("ntupMCTrk","Info about simulated trks","x:y:z:mom:theta:phi");
 
      hMCLabtheta->Fill(MomMC.Theta());
      double Px = MomMC.Px();
      double Py = MomMC.Py();
      double Pz = MomMC.Pz();
      hMCpx->Fill(Px);
      hMCpy->Fill(Py);
      hMCpz->Fill(Pz); 
      hMCphi->Fill(MomMC.Phi());
      hMCLABtheta_vs_ID->Fill(mcID,MomMC.Theta());
      if(MomMC.Theta()<1.5e-2 && mcID<0){
      //   if(MomMC.Theta()>1.5e-2 && mcID<0){ //!!!TEST
    	hMCsmallLABtheta_vs_ID->Fill(mcID,MomMC.Theta());
    	hMCsmallLABtheta_vs_Mass->Fill(mcMass ,MomMC.Theta());
    	smallTheta.push_back(1);
      }
      else smallTheta.push_back(0);
     }
     if(nParticles!=vecPDGid.size()) continue;
     if(TotCharge!=0) continue;
    /// sort vecPDGid ---------
    Int_t k, x;
    bool ch=false; //Was element changed? 
    Int_t nch = 0; //How many times?
    for(Int_t n=0; n<vecPDGid.size(); n++) { // n - current position
      k=n; x=vecPDGid[n];
      for(Int_t m=n+1; m<vecPDGid.size(); m++)	// find the least element
    	if (vecPDGid[m]<x){
    	  k=m; x=vecPDGid[m];	        // k - index for the least element
    	  ch=true; nch++;
    	}
      vecPDGid[k] = vecPDGid[n]; vecPDGid[n] = x; // change position between the least and current elements
    }
    
    // for(Int_t n=0; n<vecPDGid.size(); n++) { 
    //   cout<<"vecPDGid["<<n<<"]="<<vecPDGid[n]<<endl;
    // }
    /// -----------------------
    sumIDnum = 0;
    // for(int pid=0;pid<3123;pid++){
    for(int pid=0;pid<6246;pid++){
      if(countPDGid[pid]==0) continue;
      //      cout<<"countPDGid["<<pid<<"]="<<countPDGid[pid]<<" current sum = "<<sumIDnum<<endl;
      sumIDnum += fabs(pid-0.5*countPDGid.size())*countPDGid[pid];

      //build name of process like a string
      std::string countID;
      std::stringstream outcount;
      outcount << countPDGid[pid];
      countID = outcount.str();
      if(countPDGid[pid]==1) countID = "";
      int pdgID = pid-0.5*countPDGid.size();
      std::string pidIDs;
      std::stringstream pidIDss;
      pidIDss << pdgID;
      pidIDs = pidIDss.str();

      switch (pdgID) {
      case 3122:
	process_cur += countID+"\$\\Laymbda$";
	break;
      case -3122:
	process_cur += countID+"\$\\bar{\\Laymbda}$";
	break;
      case 2212:
	process_cur += countID+"$p$";
	break;
      case -2212:
	process_cur += countID+"\$\\bar{p}$";
	break;
      case 2112:
	process_cur+=  countID+"$n$";
	break;
      case -2112:
	process_cur+=  countID+"\$\\bar{n}$";
	break;
      case 321:
	process_cur+=  countID +"\$K^{+}$";
	break;
      case -321:
	process_cur+=  countID +"\$K^{-}$";
	break;
      case 211:
	process_cur+=  countID +"\$\\pi^{+}";
	break;
      case -211:
	process_cur+=  countID +"\$\\pi^{-}";
	break;
      case 22:
	process_cur+=  countID +"\gamma";
	break;
      case 111:
	process_cur+=  countID +"\$\\pi^{0}";
	break;
      case 11:
	process_cur+=  countID +"e^{-}";
	break;
      case -11:
	process_cur+=  countID +"e^{+}";
	break;
      case 130:
	process_cur+=  countID +"\$K^{0}_{L}$";
	break;
      case 310:
	process_cur+=  countID +"\$K^{0}_{S}$";
	break;
      default:
	process_cur+= "+"+countID+"*(PDGid="+pidIDs+")";
      }
      //      cout<<" process_cur: "<<process_cur<<endl;
    }
    //    cout<<"sumIDnum = "<<sumIDnum<<" nParticles = "<<nParticles<<endl;
    if(fsumID[nParticles][sumIDnum]==false){
      //      cout<<"process[sumIDnum] = "<<process[sumIDnum]<<endl;
      if(process[sumIDnum]=="") process[sumIDnum]=process_cur;
      else process[sumIDnum]+=" or "+process_cur;
    }

    //    cout<<"sumIDnum="<<sumIDnum<<" process_cur:"<<process_cur<<endl;
    hSumIDs->Fill(sumIDnum);
    hSumIDs_numPar->Fill(sumIDnum,nParticles);
    fsumID[nParticles][sumIDnum]=true;

    for(int ks=0;ks<smallTheta.size();ks++){
      if(smallTheta[ks]!=0) hSumIDs_numPar_smallTheta->Fill(sumIDnum,nParticles);
    }
    //    cout<<"sumIDnum="<<sumIDnum<<" process_cur:"<<process_cur<<endl;
    //    cout<<"sumIDnum = "<<sumIDnum<<", "<<nParticles<<" particles"<<endl;
    //    sumIDnum = 0;
  }
  //  cout<<"we will go to write summery now!"<<endl;
  /// Read SumIDs info bin by bin ----------------------------------------------
  double sumMin = hSumIDs_numPar->GetXaxis()->GetXmin();
  double sumMax = hSumIDs_numPar->GetXaxis()->GetXmax();
  double nparMin = hSumIDs_numPar->GetYaxis()->GetXmin();
  double nparMax = hSumIDs_numPar->GetYaxis()->GetXmax();
  double sumStep = (sumMax-sumMin)/(hSumIDs_numPar->GetNbinsX());
  double nparStep = (nparMax-nparMin)/(hSumIDs_numPar->GetNbinsY());

  int xpbarp = (4424+1)/sumStep;
  int numpbarpTot = hSumIDs_numPar->GetBinContent(xpbarp,2);
  int numpbarpSmall = hSumIDs_numPar_smallTheta->GetBinContent(xpbarp,2);
  if(numpbarpTot==0) numpbarpTot=1;
  if(numpbarpSmall==0) numpbarpSmall=1;

  if((hSumIDs_numPar->GetBinContent(xpbarp,2))) 
  for(int xi=0;xi<hSumIDs_numPar->GetNbinsX();xi++){
    for(int yi=0;yi<hSumIDs_numPar->GetNbinsY();yi++){
      double numEv = hSumIDs_numPar->GetBinContent(xi,yi);
      // cout<<"xi = "<<xi<<" yi = "<<yi<<endl;
      //if(numEv!=0) cout<<"sum="<<sumStep*xi-1<<" with number of particles="<<nparStep*yi-1<<" has "<<numEv<<endl;
      //  if(numEv!=0) cout<<sumStep*xi-1<<" & "<<numEv<<" & "<<nparStep*yi-1<<" \\"<<endl;
      double numEvAtt = hSumIDs_numPar_smallTheta->GetBinContent(xi,yi);
      if(numEvAtt!=0){ 
	if(numpbarpTot!=1)
	cout<<sumStep*xi-1<<" & "<<numEv<<"("<<numEvAtt<<") & "<<nparStep*yi-1<<" \\"<<endl;
	else 
	  cout<<sumStep*xi-1<<" & "<<numEv<<"("<<numEv/numpbarpTot<<") "
	      <<"["<<numEvAtt<<"("<<numEvAtt/numpbarpSmall<<")"<<"] & "<<nparStep*yi-1<<" \\"<<endl;
      }
    }
  }
  /// ---------------------------------------------------------------------------

  cout<<"PDGis sum for process ================================="<<endl;
  for(int simID=0;simID<10000;simID++){
    if(process[simID]!="") cout<<simID<<" "<< process[simID]<<endl;
  }
  cout<<"PDGis sum for process (END) ==========================="<<endl;

  TCanvas *c1 = new TCanvas();
  c1->Divide(3,2);
  c1->cd(1);
  hMCpx->Draw();
  c1->cd(2);
  hMCpy->Draw();
  c1->cd(3);
  hMCpz->Draw();
  c1->cd(4);
  hMCphi->Draw();
  c1->cd(5);
  hMCLabtheta->Draw(); 
  c1->cd(6);
  //  hMCparticles->Draw();
   c1->Write();
  c1->Close();

 
  TCanvas *c3 = new TCanvas("theta_vs_ID");
  c3->Divide(2,2);
  c3->cd(1);
  hMCLABtheta_vs_ID->Draw();
  c3->cd(2);
  hMCsmallLABtheta_vs_ID->Draw();
  c3->cd(3);
  hMCsmallLABtheta_vs_Mass->Draw();
  c3->Write();
  c3->Close();

  hMCLABtheta_vs_ID->Write();
  hMCsmallLABtheta_vs_ID->Write(); 
  hMCsmallLABtheta_vs_Mass->Write(); 
  hSumIDs->Write();
  hSumIDs_numPar->Write();
  hSumIDs_numPar_smallTheta->Write();
  hMCpx->Write();
  hMCpy->Write();
  hMCpz->Write();
  hMCphi->Write();
  hMCLabtheta->Write();
  ntupMCTrk->Write();
  ntupMCHit->Write();
  f->Close();
}
