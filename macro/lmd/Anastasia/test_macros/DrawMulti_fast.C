//draw multiplicity  of events versus time stamp
//
// to compile it add in  "# install #" part of ${PANDAROOT}/macro/lmd/Anastasia/test_macros/CMakeLists.txt lines:
// add_executable(DrawMulti DrawMulti_fast.C)
// target_link_libraries(DrawMulti  -lrt ${ROOT_LIBRARIES} LmdTool MathMore)
//
// and in ${PANDAROOT}/macro/lmd/CMakeLists.txt :
// add_subdirectory(Anastasia/test_macros)
//
//to run it
////# ${PANDAROOT}/build/bin/./DrawMulti momentum_string
#include "TString.h"
#include "TH2D.h"
#include "TTree.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TClonesArray.h"
#include "TChain.h"
#include "TString.h"
#include<iostream>

#include "LmdQA/PndLmdDigiQ.h"
using namespace std;
int main(int nargs, char** args){
  //  gROOT->Macro("$VMCWORKDIR/macro/lmd/Anastasia/test_macros/rootlogon.C");
  TString momStr = " ";
	if (nargs == 2) {
	  momStr = args[1];
	}
  //	gROOT->Macro("$VMCWORKDIR/macro/lmd/Anastasia/test_macros/rootlogon.C");
  //	TFile *f = new TFile("/home/karavdina/soft/pandaroot/macro/lmd/DPM_el_inel_15/Lumi_digi_0.root","READ");
  //	TFile *f = new TFile("/home/karavdina/soft/pandaroot/macro/lmd/DPMelinel_125000ev/mom_15/Lumi_DigisQA_0.root","READ");
	TString fname = "/panda/myResults/ONLINE/DPM_el_inel_";
	fname +=momStr;
	fname +="/Lumi_DigisQA_0.root";
	TString foutname = "MCtrk_in_25ns_";
	foutname +=momStr;
	foutname +="GeV.root";
	// Input file (sorted digi)
	TString DigiFile = fname;
	TChain tDigi("cbmsim");
	tDigi.Add(DigiFile);
	std::cout << "DigiFile  : " << DigiFile.Data() << std::endl;
	//--- DigiQ info -----------------------------------------------------------------
	TClonesArray* digiq_points = new TClonesArray("PndLmdDigiQ");
	tDigi.SetBranchAddress("LMDPixelDigisQ", &digiq_points); //Digi hits
	//--------------------------------------------------------------------------------
	int nEvents = tDigi.GetEntries();
	const int nstep = 1e4;
	const double tfr=25;//[ns] time frame
	int interrate = 40;//[ns] @ 1.5
	if(momStr=="15") interrate=100;//@15
	const int Nrun = 1e4*interrate;
	//	TH2 *hMCtrkTime[5]; 
	TH1 *hMCtrkTime1D[5]; 
	TH1 *hMCtrk[5]; 
	//	TH2D *htev = new TH2D("htev",";ev; t,ns",1e3,0,1e6,1e4,0,1e8);
	TLegend *leg = new TLegend(0.88,0.7,0.98,0.95);
	for(int j=0;j<5;j++){
	  TString name = "hnMCtrk_25ns_pl";
	  name +=j;
	  TString name2 = name+"_time";
	  //  TString plname = "plane #";
	  TString plname = "sensors #"; //test for sensors
	  plname +=j;
	  TString title = "; time, ns; trk/25ns";
	  TString title2 = "; trk/25ns";

	  //	  hMCtrkTime[j] = new TH2D(name2,title,1e4,0,2.5e5,2,0,2);
	  //	  hMCtrkTime[j] = new TH2D(name2,title,1e2,0,2.5e3,10,0,10);
	  //	  hMCtrkTime1D[j] = new TH1I(name2,title,4e4,0,1e5); //15 GeV
	  hMCtrkTime1D[j] = new TH1I(name2,title,2e4,0,5e5);//1.5 GeV
	  hMCtrk[j] = new TH1D(name,title2,1e2,0,1e2);
	  hMCtrk[j]->SetLineWidth(2);
	  hMCtrk[j]->SetMaximum(100);
	  hMCtrk[j]->SetLineColor(1+j);
	  leg->AddEntry(hMCtrk[j],plname,"l");
	}
	int countMultiPX=0;
	int countTotPX = 0;
	for (Int_t iEvent = 0; iEvent < nEvents; iEvent++) {
	  //cout<<"Nrun = "<<Nrun<<endl;
	//	for (Int_t iEvent = 0; iEvent < Nrun; iEvent++) { //10 s @ 1.5 GeV/c
	//	for (Int_t iEvent = 0; iEvent < 2e2; iEvent++) { //TEST
	  vector<int> MCid0; 	  vector<int> MCid1; 	  vector<int> MCid2; 	  vector<int> MCid3; vector<int> MCid4;
		tDigi.GetEntry(iEvent);
		const int nDigi = digiq_points->GetEntriesFast();
		for (Int_t i=0; i<nDigi; i++){
		  PndLmdDigiQ* DigiPoint = (PndLmdDigiQ*)(digiq_points->At(i)); // read digi hit
		  int MCtrk = DigiPoint->GetMCtrkID();
		  int pl = DigiPoint->GetPlane();
		  int pl_act = pl;
		  int senID = DigiPoint->GetSensorID();
		  int moduleID = DigiPoint->GetModule();
		  pl = senID; //test with sensors

		  int sd = DigiPoint->GetSide();
		  bool nSt=true;
		  for(int imc=0;imc<MCid0.size();imc++)
		    if(pl==0 && sd<1 && MCtrk==MCid0[imc]) nSt = false;
		  if(pl==0){
		    if(!nSt) countMultiPX++;
		    else countTotPX++;
		  }
		  for(int imc=0;imc<MCid1.size();imc++)
		    if(pl==1 && sd<1 && MCtrk==MCid1[imc]) nSt = false;
		  for(int imc=0;imc<MCid2.size();imc++)
		    if(pl==2 && sd<1 && MCtrk==MCid2[imc]) nSt = false;
		  for(int imc=0;imc<MCid3.size();imc++)
		    if(pl==3 && sd<1 && MCtrk==MCid3[imc]) nSt = false;
		  for(int imc=0;imc<MCid4.size();imc++)
		    if(pl==4 && sd<1 && MCtrk==MCid4[imc]) nSt = false;
		  if(nSt){
		    if(pl==0 && sd<1)  MCid0.push_back(MCtrk);
		    if(pl==1 && sd<1)  MCid1.push_back(MCtrk);
		    if(pl==2 && sd<1)  MCid2.push_back(MCtrk);
		    if(pl==3 && sd<1)  MCid3.push_back(MCtrk);
		    if(pl==5 && sd<1)  MCid4.push_back(MCtrk);

	
		  int hf = DigiPoint->GetHalf();
		  //	  int moduleID = 2*pl*5+hf*5+DigiPoint->GetModule();
	
		  //		  if(moduleID==0 && pl==0) cout<<"sensID: "<<senID<<endl;
		  bool iSig = DigiPoint->GetFlSig();
		  //if(iSig){ //only sig
		  //if(!iSig){ //only bkg
		  //	  if(0<1){ //all
		  if(0<1 && pl<5){ //all, 5 sensors
		  //		  if(0<1 && pl_act <1 && hf<1){ //all for modules of plane 0
		  //	   if(sd<1 && senID<200 && iSig){ //only side 0
		     //	  if(sd<1 && moduleID==0){ 
		    double tEv = DigiPoint->GetEvT();// global event time
		    double tSt = DigiPoint->GetTimeStamp();//time within event
		    double circ = iEvent/Nrun;
		    int run = circ;
		    //    cout<<"run = "<<run<<" "<<Nrun<<" "<<iEvent<<endl;
		    //	    double tTot = tEv+tSt;
		    double tTot = run*1e4*interrate+tEv+tSt;
		    //	    cout<<"tTot = "<<tTot<<endl;
		    //	    if(pl==0) htev->Fill(iEvent,tTot);
		    //		    if(tTot>2e5) continue;
		    //		    cout<<"Ev:"<<iEvent<<" modID:"<<moduleID<<" sensID: "<<senID<<" tTot = "<<tTot<<endl;
		    int nMCin1trk = DigiPoint->GetNIndices();
		    //	    hMCtrkTime[moduleID]->Fill(tTot,nMCin1trk);
		    hMCtrkTime1D[pl]->Fill(tTot);
		  }
		  }
		}
	}
	//projectionZ
	for(int j=0;j<5;j++){
	  //	  for(int ij=0;ij<1e4;ij++){//x-axis
	  for(int ij=0;ij<2e4;ij++){//x-axis
	    //	    for(int ik=0;ik<1;ik++){
	    //	      int Ntrk = (hMCtrkTime[j]->GetBinContent(ij,ik));
	    int Ntrk = (hMCtrkTime1D[j]->GetBinContent(ij));
	    //	      cout<<ij<<" "<<ik<<" "<<Ntrk<<endl;
	     
	      if(Ntrk>0){
		//	cout<<ij<<" "<<Ntrk<<endl;
		//      cout<<"Ntrk = "<<Ntrk<<" "<<hMCtrkTime[j]->GetBinContent(ij,ik)<<endl;
		hMCtrk[j]->Fill(Ntrk);
	      }
	      //  }
	  }
	}
	TCanvas *c1 = new TCanvas ("trk_25ns","trk_25ns",600,800);
	c1->Divide(1,2);
	c1->cd(1);
	// hMCtrkTime1D[0]->Draw();
	// c1->cd(2);
	// hMCtrkTime1D[1]->Draw();
	// c1->cd(3);
	// hMCtrkTime1D[2]->Draw();
	// c1->cd(4);
	// hMCtrkTime1D[3]->Draw();
	// c1->cd(5);
	double scl0 = 100.*1/(hMCtrk[0]->Integral());
	cout<<"scale for "<<0<<" "<<scl0<<" with int = "<<hMCtrk[0]->Integral()<<endl;
	//	cout<<"scl0 = "<<scl0<<endl;
	hMCtrk[0]->Scale(scl0);
	hMCtrk[0]->GetYaxis()->SetTitle(" events, %");
	hMCtrk[0]->SetLineColor(1);
	hMCtrk[0]->SetLineWidth(2);
	hMCtrk[0]->Draw();

	for(int j=1;j<5;j++){
	  double scl1 = 100.*1/(hMCtrk[j]->Integral());
	  cout<<"scale for "<<j<<" "<<scl1<<" with int = "<<hMCtrk[j]->Integral()<<endl;
	  hMCtrk[j]->Scale(scl1);
	  hMCtrk[j]->Draw("same");
	}
	leg->Draw();
	//	c1->cd(6);
	c1->cd(2);
	TH1D*hsum[5];
	for(int j=0;j<5;j++){
	  hsum[j] = (TH1D*)hMCtrk[j]->Clone("hsum");
	  if(j>0) hsum[0]->Add(hsum[j]);
	}
	double scl2 = 100.*1/(hsum[0]->Integral());
	hsum[0]->Scale(scl2);
	hsum[0]->Draw();
	c1->Print(foutname);

	cout<<"TOT number of pixel on pl#0 "<<countTotPX+countMultiPX<<" number of multiple pixels "<<countMultiPX<<" ("
	    << double(100.*countMultiPX/(countTotPX+countMultiPX))<<" %)"<<endl;
       // 	TCanvas c2;
       // 	c2.Divide(1,2);
       // for(int j=0;j<4;j++){
       // 	 c2.cd(j+1);
       // 	 hMCtrk[j]->Draw();
       // }

       // c2.cd(5);
       // 	hsum[0]->Draw();
       // 	c2.cd(6);
       // 	//	htev->Draw("colz");
       // c2.Print(foutname);
	return 0;
}
