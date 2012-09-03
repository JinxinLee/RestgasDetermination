//-----------------------------------------------------------
//
// Description:
//      Task to generate and store some useful histograms
//
// Author List:
//	Anastasia Karavdina
//      Mathias Michel
//
//-----------------------------------------------------------

// This Class' Header ------------------
#include "PndLmdQATask.h"

// C/C++ Headers ----------------------
#include <iostream>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "PndLinTrack.h"
#include "TrackData/PndTrackCand.h"
#include "PndSdsHit.h"
#include "PndSdsMCPoint.h"
#include "TrackData/PndTrackCandHit.h"

#include "TFile.h"
#include "TLorentzVector.h"
#include "FairTrackParH.h"
#include "PndMCTrack.h"
#include <TMath.h>
#include <TVector3.h>
#include <TRandom2.h>
//#include <TStyle.h>
//#include <TCanvas.h>
#include <TPolyLine3D.h>
#include <Math/Vector3D.h>
#include "TH2D.h"
#include "TH1D.h"
#include "TH2F.h"
#include "TH1F.h"
using namespace ROOT::Math;
using namespace std;

PndLmdQATask::PndLmdQATask(TString mcHitBranch, TString mcTrkBranch, TString hitBranch, TString trackBranch, TString geaneBranch, TString outFile)
  : FairTask("Histogram creator")
{
  fmcHitName=mcHitBranch;
  fmcTrkName=mcTrkBranch;
  fHitName=hitBranch;
  fTrkName=trackBranch;
  fGeaneName=geaneBranch;
  foutFile=outFile;
  fEvent=0;
}

PndLmdQATask::~PndLmdQATask()
{
  cout<<"PndLmdQATask::~PndLmdQATask()"<<endl;
}



void PndLmdQATask::FinishTask()
{
  cout<<"PndLmdQATask::FinishTask()"<<endl;

  //Write histos
  TFile *f = new TFile(foutFile,"RECREATE");

  //OutputResolutionAndPulls Directory
  f->mkdir("TrackReso");
  f->cd("TrackReso");

  hchi2->Write();

  hchi2ang->Write();
  hMom2D->Write();
  hMomchi2->Write();
  hMomR->Write();
  hMomXY->Write();
  hMomZ->Write();
  hchi2R->Write();
  hangR->Write();
  hangMCRec->Write();
  hResMom->Write();
  hResMomIO->Write();
  hResMomIMC->Write();
  hErrMom->Write();
  hPullMom->Write();
  hResTheta->Write();
  hErrTheta->Write();
  hPullTheta->Write();
  hResPhi->Write();
  hErrPhi->Write();
  hPullPhi->Write();

  hResMomX->Write();
  hResMomY->Write();
  hResMomZ->Write();
  hErrMomX->Write();
  hErrMomY->Write();
  hErrMomZ->Write();
  hPullMomX->Write();
  hPullMomY->Write();
  hPullMomZ->Write();
  
  hResPointX->Write();
  hResPointY->Write();
  hResPointZ->Write();
  hErrPointX->Write();
  hErrPointY->Write();
  hErrPointZ->Write();
  hPullPointX->Write();
  hPullPointY->Write();
  hPullPointZ->Write();

  hPointIN->Write();
  hPointOUT->Write();

  hThetaUneff->Write();
  hThetaeffR->Write();
  hPhiUneff->Write();
  hnRecnMC->Write();
  hPullThetaAng->Write();
  hPullThetaR->Write();

  f->cd();

  //RecoReso Directory
  f->mkdir("RecoReso");
  f->cd("RecoReso");

  h2dPnts;
  
  hResHitX->Write();
  hResHitY->Write();
  hResHitZ->Write();
  hErrHitX->Write();
  hErrHitY->Write();
  hErrHitZ->Write();
  hPullHitX->Write();
  hPullHitY->Write();
  hPullHitZ->Write();

  f->cd();
  f->Close();
  cout<<"Number of missed tracks is "<<mistrk<<" and number of ghost tracks is "<<ghosttrk<<endl;


  //clean up stuff
  delete hchi2;

  delete hchi2ang;
  delete hMom2D;
  delete hMomchi2;
  delete hMomR;
  delete hMomXY;
  delete hMomZ;
  delete hchi2R;
  delete hangR;
  delete hangMCRec;
  delete hResMom;
  delete hResMomIO;
  delete hResMomIMC;
  delete hErrMom;
  delete hPullMom;
  delete hResTheta;
  delete hErrTheta;
  delete hPullTheta;
  delete hResPhi;
  delete hErrPhi;
  delete hPullPhi;

  delete hResMomX;
  delete hResMomY;
  delete hResMomZ;
  delete hErrMomX;
  delete hErrMomY;
  delete hErrMomZ;
  delete hPullMomX;
  delete hPullMomY;
  delete hPullMomZ;
  
  delete hResPointX;
  delete hResPointY;
  delete hResPointZ;
  delete hErrPointX;
  delete hErrPointY;
  delete hErrPointZ;
  delete hPullPointX;
  delete hPullPointY;
  delete hPullPointZ;

  delete hPointIN;
  delete hPointOUT;

  delete hThetaUneff;
  delete hThetaeffR;
  delete hPhiUneff;
  delete hnRecnMC;
  delete hPullThetaAng;
  delete hPullThetaR;

  delete h2dPnts;
  
  delete hResHitX;
  delete hResHitY;
  delete hResHitZ;
  delete hErrHitX;
  delete hErrHitY;
  delete hErrHitZ;
  delete hPullHitX;
  delete hPullHitY;
  delete hPullHitZ;
}

InitStatus PndLmdQATask::Init()
{
  tot = 0;
  all=0; uneff=0; mistrk=0; ghosttrk=0;

  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

  if(ioman==0)
    {
      Error("PndLmdQATask::Init","RootManager not instantiated!");
      return kERROR;
    }

  // Get input collection
  fmcHitArray=(TClonesArray*) ioman->GetObject(fmcHitName);
  if(fmcHitArray==0)
    {
      Error("PndLmdQATask::Init","mcHit-array not found!");
      return kERROR;
    }

  fHitArray=(TClonesArray*) ioman->GetObject(fHitName);
  if(fHitArray==0)
    {
      Error("PndLmdQATask::Init","hit-array not found!");
      return kERROR;
    }

  fTrkArray=(TClonesArray*) ioman->GetObject(fTrkName);
  if(fTrkArray==0)
    {
      Error("PndLmdQATask::Init","track-array not found!");
      return kERROR;
    }

  fmcTrkArray=(TClonesArray*) ioman->GetObject(fmcTrkName);
  if(fmcTrkArray==0)
    {
      Error("PndLmdQATask::Init","mcTrk-array not found!");
      return kERROR;
    }

  fGeaneArray=(TClonesArray*) ioman->GetObject(fGeaneName);
  if(fGeaneArray==0)
    {
      Error("PndLmdQATask::Init","geane-array not found!");
      return kERROR;
    }

  hchi2 = new TH1F("hchi2", "#chi^{2}", 3e2,0,10);

  hchi2ang = new TH2D("chi2vsang", "hchi2 vs. angle between Pmc and Prec; #delta#alpha, rad;#chi^{2}", 
			   1e3,0.,0.001, 1e3,0,100);
  hMom2D = new TH2D("hMom2D","P_{MC} vs. P_{rec}; P_{rec},GeV/c; P_{MC},GeV/c",
			 1e3,-1e-6,1e-6,1e3,-1e-6,1e-6);
  hMomchi2 = new TH2D("hMomchi2","#delta P vs. #chi^{2};#chi^{2};#deltaP, GeV/c",1e3,0,100,1e4,-1e-2,1e-2);
  hMomR = new TH2D("hMomR","#delta P vs. R_{PCA};R_{PCA}, cm;#deltaP, GeV/c",1e3,0,1,1e4,-1e-2,1e-2);
  hMomXY = new TH2D("hMomXY","#delta P vs. XY_{PCA};XY_{PCA}, cm;#deltaP, GeV/c",1e3,0,1,1e4,-1e-2,1e-2);
  hMomZ = new TH2D("hMomZ","#delta P vs. Z_{PCA};Z_{PCA}, cm;#deltaP, GeV/c",1e3,-0.05,0.05,1e4,-1e-2,1e-2);
  hchi2R = new TH2D("hchi2R","#chi^{2} vs. R_{PCA};R_{PCA}, cm;#chi^{2}",1e4,0,1e0,1e3,0,1e1);
  hangR = new TH2D("hangR","Angle between Pmc and Prec vs. R_{PCA};R_{PCA}, cm;#delta#alpha, rad",
			1e3,0,1,1e3,0.,0.001);
  hangMCRec = new TH1F("hangMCRec", "Angle between Pmc and Prec;#delta#alpha, rad", 1e4,0.,1);
  hResMom = new TH1F("hResMom","P_{MC}-P_{rec};#deltaP,GeV/c",1e3,-1e-5,1e-5);
  hResMomIO = new TH1F("hResMomIO","P^{trk-fit}_{rec}-P^{geane}_{rec};#deltaP,GeV/c",1e3,-1e-5,1e-5);
  hResMomIMC = new TH1F("hResMomIMC","P_{MC}-P^{trk-fit}_{rec};#deltaP,GeV/c",1e3,-1e-5,1e-5);
  hErrMom = new TH1F("hErrMom","#sigma_{P};#sigmaP,GeV/c",1e3,0,1e-3);
  hPullMom = new TH1F("hPullMom","(P_{MC}-P_{rec})/#sigma_{P};",1e2,-1e-2,1e-2);
  hResTheta = new TH1F("hResTheta","#theta_{MC}-#theta_{rec};#delta#theta,rad",1e3,-2e-3,2e-3);
  hErrTheta = new TH1F("hErrTheta","#sigma(#theta_{rec});#sigma,rad",200,0,2e-3);
  hPullTheta = new TH1F("hPullTheta","(#theta_{MC}-#theta_{rec})/#sigma_{#theta};",1e2,-5,5);
  hResPhi = new TH1F("hResPhi","#phi_{MC}-#phi_{rec};#delta#phi,rad",1e3,-0.1,0.1);
  hErrPhi = new TH1F("hErrPhi","#sigma(#phi_{rec});#sigma,rad",200,0,0.2);
  hPullPhi = new TH1F("hPullPhi","(#phi_{MC}-#phi_{rec})/#sigma_{#phi};",1e2,-5,5);

  hResMomX = new TH1F("hResMomX","(P_{MC}-P_{rec})_{X};#deltaP,GeV/c",2e2,-7e-3,7e-3);
  hResMomY = new TH1F("hResMomY","(P_{MC}-P_{rec})_{Y};#deltaP,GeV/c",2e2,-7e-3,7e-3);
  hResMomZ = new TH1F("hResMomZ","(P_{MC}-P_{rec})_{Z};#deltaP,GeV/c",2e2,-1e-4,1e-4);
  hErrMomX = new TH1F("hErrMomX","#sigma_{Px};#sigmaPx,GeV/c",1e3,0,1e-3);
  hErrMomY = new TH1F("hErrMomY","#sigma_{Py};#sigmaPy,GeV/c",1e3,0,1e-3);
  hErrMomZ = new TH1F("hErrMomZ","#sigma_{Pz};#sigmaPz,GeV/c",1e4,0,1e-2);
  hPullMomX = new TH1F("hPullMomX","(P_{MC}-P_{rec})_{X}/#sigma_{Px};",1e2,-10,10);
  hPullMomY = new TH1F("hPullMomY","(P_{MC}-P_{rec})_{Y}/#sigma_{Py};",1e2,-10,10);
  hPullMomZ = new TH1F("hPullMomZ","(P_{MC}-P_{rec})_{Z}/#sigma_{Pz};",4e2,-10,10);
  
  hResPointX = new TH1F("hResPointX","X_{MC}-X_{rec};#deltaX,cm",2e2,-1.5,1.5);
  hResPointY = new TH1F("hResPointY","Y_{MC}-Y_{rec};#deltaY,cm",2e2,-1.5,1.5);
  hResPointZ = new TH1F("hResPointZ","Z_{MC}-Z_{rec};#deltaZ,cm",2e2,-3e-2,3e-2);
  hErrPointX = new TH1F("hErrPointX","#sigma_{X};#sigmaX,cm",1e2,0,5e-1);
  hErrPointY = new TH1F("hErrPointY","#sigma_{Y};#sigmaY,cm",1e2,0,5e-1);
  hErrPointZ = new TH1F("hErrPointZ","#sigma_{Z};#sigmaZ,cm",1e2,0,5e-3);
  hPullPointX = new TH1F("hPullPointX","(X_{MC}-X_{rec})/#sigma_{X};",1e2,-10,10);
  hPullPointY = new TH1F("hPullPointY","(Y_{MC}-Y_{rec})/#sigma_{Y};",1e2,-10,10);
  hPullPointZ = new TH1F("hPullPointZ","(Z_{MC}-Z_{rec})/#sigma_{Z};",1e2,-10,10);

  hPointIN = new TH1F("hPointIN","Initial point ;xy,cm",4e2,10,50);
  hPointOUT = new TH1F("hPointOUT","Final point ;xy,cm",1e3,0,1);

  hThetaUneff = new TH1F("hThetaUneff","#theta for missed tracks;#theta, rad",1e2,1e-4,1.3e-2);
  hThetaeffR = new TH2F("hThetaeffR","#theta for reconstructed tracks vs. R_{PCA};R_{PCA}, cm; #theta, rad",
			       1e3,0,2.,1e3,1e-4,1.3e-2);
  hPhiUneff = new TH1F("hPhiUneff","#phi for missed tracks;#phi, rad", 1e2,-TMath::Pi(),TMath::Pi());
  hnRecnMC = new TH2F("hnRecnMC","Number reconstracted tracks vs. Number simulated tracks; N_{MC}; N_{rec}",
			   20,0,20,20,0,20);
  hPullThetaAng = new TH2F("hPullThetaAng","|(#theta_{MC}-#theta_{rec})|/#sigma_{#theta} vs. Angle between Pmc and Prec; #delta#alpha, rad; |(#theta_{MC}-#theta_{rec})|/#sigma_{#theta}", 1e2,0.,1e-3,1e2,0,10);
  hPullThetaR = new TH2F("hPullThetaR","|(#theta_{MC}-#theta_{rec})|/#sigma_{#theta} vs. R_{PCA};R_{PCA}, cm;  |(#theta_{MC}-#theta_{rec})|/#sigma_{#theta}", 1e2,0.,1,1e2,0,10);

  h2dPnts = new TH2D("h2dPnts", "xy mc hits first plane in, cm",1e3,16.,41., 1e3,-10.,10.);
  
  hResHitX = new TH1F("hResHitX","X_{MC}-X_{rec};#deltaX,cm",2e2,-0.01,0.01);
  hResHitY = new TH1F("hResHitY","Y_{MC}-Y_{rec};#deltaY,cm",2e2,-0.01,0.01);
  hResHitZ = new TH1F("hResHitZ","Z_{MC}-Z_{rec};#deltaZ,cm",2e2,-0.001,0.001);
  hErrHitX = new TH1F("hErrHitX","#sigma_{X};#sigmaX,cm",1e2,0,5e-1);
  hErrHitY = new TH1F("hErrHitY","#sigma_{Y};#sigmaY,cm",1e2,0,5e-1);
  hErrHitZ = new TH1F("hErrHitZ","#sigma_{Z};#sigmaZ,cm",1e2,0,5e-3);
  hPullHitX = new TH1F("hPullHitX","(X_{MC}-X_{rec})/#sigma_{X};",1e2,-10,10);
  hPullHitY = new TH1F("hPullHitY","(Y_{MC}-Y_{rec})/#sigma_{Y};",1e2,-10,10);
  hPullHitZ = new TH1F("hPullHitZ","(Z_{MC}-Z_{rec})/#sigma_{Z};",1e2,-10,10);

  std::cout << "-I- PndLmdQATask: Initialisation successfull" << std::endl;
  return kSUCCESS;
}

void PndLmdQATask::Exec(Option_t* opt)
{
  std::cout<<"PndLmdQATask::Exec"<<std::endl;

  ResoAndPulls();
  HitReco();

  std::cout<<"PndLmdQATask::Exec successfull"<<std::endl;
  return;
}

void PndLmdQATask::HitReco()
{
    const int nMCHits = fmcHitArray->GetEntriesFast();
    const int nRecHits = fHitArray->GetEntriesFast();

    for (Int_t i=0; i<nRecHits; i++){
      PndSdsHit *hit = (PndSdsHit*) fHitArray->At(i);
      if(hit->GetRefIndex()<0) continue;
      PndSdsMCPoint *mc = (PndSdsMCPoint*) fmcHitArray->At(hit->GetRefIndex());
      if(!mc) continue;

      TVector3 recovec(hit->GetX(),hit->GetY(),hit->GetZ());
      TVector3 mcvec(mc->GetX()+mc->GetXOut(),mc->GetY()+mc->GetYOut(),mc->GetZ()+mc->GetZOut()); mcvec*=0.5;
      TVector3 dvec(recovec-mcvec);

      if(hit->GetZ()<1135.) //Acc of first plane
	h2dPnts->Fill(mc->GetX(),mc->GetY());  // = new TH2D("h2dPnts", "xy reconstructed points, cm",2e2,-20.,20., 2e2,-20.,20.);
  
      hResHitX->Fill(dvec.X());  // = new TH1F("hResPointX","X_{MC}-X_{rec};#deltaX,cm",2e2,-1.5,1.5);
      hResHitY->Fill(dvec.Y());  // = new TH1F("hResPointY","Y_{MC}-Y_{rec};#deltaY,cm",2e2,-1.5,1.5);
      hResHitZ->Fill(dvec.Z());  // = new TH1F("hResPointZ","Z_{MC}-Z_{rec};#deltaZ,cm",2e2,-3e-2,3e-2);
      //TODO
      hErrHitX->Fill(0);  // = new TH1F("hErrPointX","#sigma_{X};#sigmaX,cm",1e2,0,5e-1);
      hErrHitY->Fill(0);  // = new TH1F("hErrPointY","#sigma_{Y};#sigmaY,cm",1e2,0,5e-1);
      hErrHitZ->Fill(0);  // = new TH1F("hErrPointZ","#sigma_{Z};#sigmaZ,cm",1e2,0,5e-3);
      hPullHitX->Fill(0);  // = new TH1F("hPullPointX","(X_{MC}-X_{rec})/#sigma_{X};",1e2,-10,10);
      hPullHitY->Fill(0);  // = new TH1F("hPullPointY","(Y_{MC}-Y_{rec})/#sigma_{Y};",1e2,-10,10);
      hPullHitZ->Fill(0);  // = new TH1F("hPullPointZ","(Z_{MC}-Z_{rec})/#sigma_{Z};",1e2,-10,10);

    }
    //----------------------------------------------------------------------------------
    return;
}

void PndLmdQATask::ResoAndPulls()
{
    fEvent++;

    // Read GEANE & MC info -----------------------------------------------------------------
    const int nGeaneTrks = fGeaneArray->GetEntriesFast();
    const int nParticles = fmcTrkArray->GetEntriesFast();
    const int nRecHits = fHitArray->GetEntriesFast();
    const int nRecTrks = fTrkArray->GetEntriesFast();

    hnRecnMC->Fill(nParticles,nGeaneTrks);
    //  cout<<"#"<<j<<" nGeaneTrks="<<nGeaneTrks<<" nParticles="<<nParticles
    //	<<" nRecTrks"<<nRecTrks<<" nRecHits = "<<nRecHits<<endl;
  
    if(nGeaneTrks<nParticles)
         mistrk += nParticles-nGeaneTrks;
    
    if(nGeaneTrks>nParticles)
      ghosttrk += nGeaneTrks-nParticles;

    //if(nParticles!=1 || nRecTrks<1) continue;
    if(nParticles!=1) return;
    cout<<"# "<< fEvent-1 <<"\t nGeaneTrks="<<nGeaneTrks<<" nParticles="<<nParticles<<" nRecTrks="<<nRecTrks<<endl;
    vector<int> missedTrk;
    missedTrk.resize(nParticles);
    vector<int> ghostTrk;
    ghostTrk.resize(nGeaneTrks);
    for(int jmc=0;jmc<nParticles;jmc++)
      missedTrk[jmc] = 1;
    for (Int_t i=0; i<nGeaneTrks; i++){
      PndLinTrack *trk = (PndLinTrack*)fTrkArray->At(i);
      Int_t rectrkID = trk->GetTCandID();
      ///Particle momentum from reconstruction (track fit)
      Double_t parrecTrk[4];
      //trk->GetPar(parrecTrk);
      Double_t p0 = parrecTrk[0];
      Double_t p1 = parrecTrk[1];
      Double_t p2 = parrecTrk[2];
      Double_t p3 = parrecTrk[3];

      Double_t parrecTrkErr[4];
      //trk->GetParErr(parrecTrkErr);

      //Vector of particle momentum and starting point
      TVector3  DirVec =  trk->GetDirectionVec();
      Double_t fPbeam = 4.06;
      Double_t Pnorm = fPbeam/DirVec.Mag();
      TVector3 StartMom = TVector3(p1*Pnorm,p3*Pnorm,Pnorm); 
      TVector3 StartPos = trk->GetStartVec();
      // hPointIN->Fill(TMath::Hypot(StartPos.X(),StartPos.Y()));
      ///------------------------------------------------


      double chi2 = trk->GetChiSquare();
   
      //  if(chi2>0.1) continue;

      FairTrackParH *fRes = (FairTrackParH*)fGeaneArray->At(i);
        if(!fRes) { cout << "no fRes!" << endl; return;}
      Double_t lyambda = fRes->GetLambda();
      Double_t thetaRec = TMath::Pi()/2. - lyambda;
      Double_t err_lyambda = fRes->GetDLambda();
          // cout<<"err_lyambda = "<<err_lyambda<<" sqrt(err_lyambda)"<<TMath::Sqrt(err_lyambda)<<endl;
      Double_t phi = fRes->GetPhi();
      Double_t err_phi = fRes->GetDPhi();
      TVector3 MomRec = fRes->GetMomentum();
      Double_t CovRec[15];
      fRes->GetCov(CovRec);
      TVector3 PosRec = fRes->GetPosition();
      hThetaeffR->Fill(PosRec.Mag(),thetaRec);
     
      // if(PosRec.Mag()>1 || chi2>10.){
      // 	if(angMCRec<1e-3) missedTrk[jk] = 0;
      // 	continue; //cut on 1 cm
      // }

      //  if(PosRec.Mag()<0.1) continue;
      //      if(PosRec.Mag()<0.5 || chi2>10.) continue; //cut on 1 cm
      hchi2->Fill(chi2);
      hchi2R->Fill(PosRec.Mag(),chi2);
      hPointOUT->Fill(PosRec.Mag());
      hPointIN->Fill(TMath::Hypot(StartPos.X(),StartPos.Y()));
      //Double_t errMomRec = TMath::Sqrt(CovRec[0])*MomRec.Mag2();
      //      Double_t ErrPcalc = TMath::Sqrt(errPx*errPx+errPy*errPy+errPz*errPz);
      Double_t errPx = fRes->GetDPx();
      Double_t errPy = fRes->GetDPy();
      Double_t errPz = fRes->GetDPz();
      Double_t errMomRec = TMath::Sqrt(errPx*errPx+errPy*errPy+errPz*errPz);
      hErrMomX->Fill(errPx);
      hErrMomY->Fill(errPy);
      hErrMomZ->Fill(errPz);
      //  cout<<"errPx = "<<errPx<<" errPy = "<<errPy<<" errPz = "<<errPz<<endl;
      Double_t errX = fRes->GetDX();
      Double_t errY = fRes->GetDY();
      Double_t errZ = fRes->GetDZ();
      hErrPointX->Fill(errX);
      hErrPointY->Fill(errY);
      hErrPointZ->Fill(errZ);
     
      for(Int_t jk=0; jk< nParticles;jk++){
	//	missedTrk[jk] = false;
      	PndMCTrack *mctrk =(PndMCTrack*) fmcTrkArray->At(jk);
	Int_t mcID = mctrk->GetPdgCode();
	if(fVerbose>0 && mcID!=-2212){
	  cout << "Event: "<<fEvent-1<<" MC trk#"<<jk
	       <<" mcID "<<mcID
	       <<" rectrkID "<<rectrkID<<" [nGeaneTrks="<<nGeaneTrks<<" nParticles="<<nParticles<<"]"<<endl;
	}
	//	if(mcID!=-2212) continue;
	TVector3 MomMC = mctrk->GetMomentum();
	Double_t angMCRec = MomMC.Angle(MomRec); // Angle between two vectors

	//	if(fabs(MomMC.Mag()-MomRec.Mag())<5e-7) continue;
	//	if((MomMC.Mag()-MomRec.Mag())>-6e-7) continue;
	//	cout<<"#"<<j<<" nGeaneTrks="<<nGeaneTrks<<" nParticles="<<nParticles<<" nRecTrks"<<nRecTrks<<endl;
	//	if(chi2>0.5) continue;
	hangMCRec->Fill(angMCRec);
	// if(PosRec.Mag()>1 || chi2>10.){
	//	if(angMCRec<1e-3) missedTrk[jk] = 0;
	//   continue;
	// }

	if(PosRec.Mag()>0.8 || chi2>10.) return;
	// if(angMCRec>1e-3 && mcID==-2212) missedTrk.push_back(0);
	// else missedTrk.push_back(1);
	//	if(angMCRec>1e-3) missedTrk[jk] = 1;
	//	if(angMCRec>1e-3) continue;
	//	if(angMCRec>1e-3) continue;
	 if(angMCRec>1e-3) return;
	// else missedTrk[jk] = 0;
	
	hchi2ang->Fill(angMCRec,chi2);
	hMomchi2->Fill(chi2,(MomMC.Mag()-MomRec.Mag()));

	hMom2D->Fill(MomRec.Mag()-4.06,MomMC.Mag()-4.06);
	hResMom->Fill(MomMC.Mag()-MomRec.Mag());
	hResMomIO->Fill(StartMom.Mag()-MomRec.Mag());
	hResMomIMC->Fill(MomMC.Mag()-StartMom.Mag());
	hPullMom->Fill((MomMC.Mag()-MomRec.Mag())/errMomRec);
	hErrMom->Fill(errMomRec);

	hResMomX->Fill(MomMC.X()-MomRec.X());
	hResMomY->Fill(MomMC.Y()-MomRec.Y());
	hResMomZ->Fill(MomMC.Z()-MomRec.Z());
	hPullMomX->Fill((MomMC.X()-MomRec.X())/errPx);
	hPullMomY->Fill((MomMC.Y()-MomRec.Y())/errPy);
	hPullMomZ->Fill((MomMC.Z()-MomRec.Z())/errPz);

	// if(fabs(MomMC.Mag()-4.06)>1e-8){
	//   cout<<"MomMC.Mag()-4.06 = "<<MomMC.Mag()-4.06<<endl;	
	//   cout<<"MomRec.Mag()-4.06 = "<<MomRec.Mag()-4.06<<endl;	
	//   cout<<"Delta P = "<<MomMC.Mag()-MomRec.Mag()<<" errMomRec = "<<errMomRec
	//       <<" Pull = "<<(MomMC.Mag()-MomRec.Mag())/errMomRec<<endl;
	// }
	hResTheta->Fill(MomMC.Theta()-thetaRec);
	hErrTheta->Fill(err_lyambda);
	hPullTheta->Fill((MomMC.Theta()-thetaRec)/err_lyambda);
	hPullThetaAng->Fill(angMCRec,fabs(MomMC.Theta()-thetaRec)/err_lyambda);
	hPullThetaR->Fill(PosRec.Mag(),fabs(MomMC.Theta()-thetaRec)/err_lyambda);
	hResPhi->Fill(MomMC.Phi()-phi);
	hErrPhi->Fill(err_phi);
	hPullPhi->Fill((MomMC.Phi()-phi)/err_phi);
	TVector3 MCvertex = mctrk->GetStartVertex();
	hResPointX->Fill(PosRec.X()-MCvertex.X());
	hResPointY->Fill(PosRec.Y()-MCvertex.Y());
	hResPointZ->Fill(PosRec.Z()-MCvertex.Z());
	// cout<<"diff for coord: dx="<<PosRec.X()-MCvertex.X()
	//     <<" dy="<<PosRec.Y()-MCvertex.Y()<<" dz="<<PosRec.Z()-MCvertex.Z()
	//     <<". And PosRec.Mag() = "<<PosRec.Mag()<<endl;
	//	cout<<"MCvertex.Mag() = "<<MCvertex.Mag()<<endl;
	//	MCvertex.Print();
	
	hMomR->Fill(PosRec.Mag(),(MomMC.Mag()-MomRec.Mag()));
	hMomZ->Fill(PosRec.Z(),(MomMC.Mag()-MomRec.Mag()));
	//	cout<<"Hypot = "<<TMath::Hypot(PosRec.X(),PosRec.Y())<<" Perp = "<<PosRec.Perp()<<endl;
	hMomXY->Fill(PosRec.Perp(),(MomMC.Mag()-MomRec.Mag()));

	hangR->Fill(PosRec.Mag(),angMCRec);

	hPullPointX->Fill((PosRec.X()-MCvertex.X())/errX);
	hPullPointY->Fill((PosRec.Y()-MCvertex.Y())/errY);
	hPullPointZ->Fill((PosRec.Z()-MCvertex.Z())/errZ);
      }

      


    }
    //----------------------------------------------------------------------------------

    ///Unefficiency --------------------------------------------------------------
    for(Int_t jk=0; jk< nParticles;jk++){
      if(missedTrk[jk]!=0) return;
	PndMCTrack *mctrk =(PndMCTrack*) fmcTrkArray->At(jk);
	TVector3 MomMC = mctrk->GetMomentum();
	hThetaUneff->Fill(MomMC.Theta());
	hPhiUneff->Fill(MomMC.Phi());
    }
    ///---------------------------------------------------------------------------

  return;
}

ClassImp(PndLmdQATask);
