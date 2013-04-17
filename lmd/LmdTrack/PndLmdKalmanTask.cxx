//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndLmdKalmanTask
//      see PndLmdKalmanTask.h for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//      Ralf Kliemt, TU Dresden             (Copied for MVD use)
//      Anastasia Karavdina, Uni Mainz      (Copied for LMD use)
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndLmdKalmanTask.h"

// C/C++ Headers ----------------------
#include <algorithm>
#include <iostream>
#include <assert.h>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "GFTrack.h"
#include "TDatabasePDG.h"

#include "PndSdsHit.h"
#include "FairMCPoint.h"

#include "PndSdsRecoHit.h"
#include "PndGeoHandling.h"
#include "GFRecoHitFactory.h"
#include "GFKalman.h"
#include "GFException.h"
#include "TH1D.h"
#include "TFile.h"
#include "TGeoTrack.h"
#include "TGeoManager.h"
#include "TLorentzVector.h"
#include "GFDetPlane.h"
#include "FairTrackParH.h"
#include "FairBaseParSet.h"
#include "LSLTrackRep.h"
//#include "GeaneTrackRep2.h"
#include "GeaneTrackRep.h"
#include "FairGeanePro.h"
#include "GFAbsTrackRep.h"
#include "GFConstField.h"
#include "PndDetectorList.h"
//#include "PndSdsRecoHit.h"
#include "RKTrackRep.h"
#include "PndTrackCand.h"
#include "PndTrack.h"
#include "PndGenfitAdapters.h"
#include "TMatrixFSym.h"
//#include "GFPandaField.h"
#include "PndGenfitField.h"
// Class Member definitions -----------


PndLmdKalmanTask::PndLmdKalmanTask()
  : FairTask("Kalman Filter"), fPersistence(kFALSE)
{
  //  fTrackBranchName = "GFTrackCandLmd";
  fTrackBranchName = "LMDTrackCand";
  //  fSdsHitBranchName = "LMDHitsStrip";
  fSdsHitBranchName = "LMDHitsPixel";
  PndGeoHandling::Instance();
  // hxpull = new TH1D("hxpull","x/#sigma_{x}",1e2,-10,10.);
  // hypull = new TH1D("hypull","y/#sigma_{y}",1e2,-10,10.);
  // hzpull = new TH1D("hzpull","z/#sigma_{z}",1e2,-10,10.);
  // hpxpull = new TH1D("hpxpull","px/#sigma_{px}",1e2,-10,10.);
  // hpypull = new TH1D("hpypull","x/#sigma_{py}",1e2,-10,10.);
  // hpzpull = new TH1D("hpzpull","pz/#sigma_{pz}",1e2,-10,10.);
  flGEANE = false;
  flRK = false;
  fscaleP = 1;
  fscaleM = 1;}

PndLmdKalmanTask::PndLmdKalmanTask(TString HitBranch, TString TrackBranch)
  : FairTask("Kalman Filter"), fPersistence(kFALSE)
{
  fTrackBranchName = TrackBranch;
  fSdsHitBranchName = HitBranch;
  PndGeoHandling::Instance();
  flGEANE = false;
  flRK = false;
  fscaleP = 1;
  fscaleM = 1;
}

PndLmdKalmanTask::~PndLmdKalmanTask()
{
  // if(fPH!=NULL)delete fPH;
  // if(fChi2H!=NULL)delete fChi2H;
}

InitStatus
PndLmdKalmanTask::Init()
{
  fTrackcount=0;
  fsensType=0;
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

  if(ioman==0)
    {
      Error("PndLmdKalmanTask::Init","RootManager not instantiated!");
      return kERROR;
    }

  // Get input collection
  fTrackArray=(TClonesArray*) ioman->GetObject(fTrackBranchName);

  if(fTrackArray==0)
    {
      Error("PndLmdKalmanTask::Init","GFTrackCandLmd array not found!");
      return kERROR;
    }

  fSdsHitsArray=(TClonesArray*) ioman->GetObject(fSdsHitBranchName);

  //Set output collection
  // fTrackFittedArray = new TClonesArray("PndLinTrack");
  // ioman->Register("LMDTrack", "PndLmd", fTrackFittedArray, kTRUE);
  
  fTrackFittedArray = new TClonesArray("PndTrack");
  ioman->Register("LMDPndTrack", "PndLmd", fTrackFittedArray, kTRUE);

  // Build hit factory -----------------------------
  fTheRecoHitFactory = new GFRecoHitFactory();

  TClonesArray* stripar=(TClonesArray*) ioman->GetObject(fSdsHitBranchName);
  if(stripar==0){ //TODO Convention on detector number needed
    Error("PndLmdKalmanTask::Init","LMDHitsStrip array not found");
  } else {
    fTheRecoHitFactory->addProducer(1,new GFRecoHitProducer<PndSdsHit,PndSdsRecoHit>(stripar));
    std::cout << "*** PndLmdKalmanTask::Init" << "\t" << "fSdsHitBranchName array found" << std::endl;
  }

  //read beam momentum from base
  FairRun* fRun = FairRun::Instance();
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairBaseParSet* par=(FairBaseParSet*)
    (rtdb->findContainer("FairBaseParSet"));
  fPbeam = par->GetBeamMom();
  //fPbeam -=8.77e-5;//TEST!!! energy loss for 11.91 GeV/c
  // fPbeam -=1e-4;//TEST!!! energy loss for 1.5 GeV/c
  std::cout<<"Beam Momentum for this run is "<<fPbeam<<std::endl;
  fPDGCode = -2212; //barp
  fCharge = -1;//barp

  fGeoH = PndGeoHandling::Instance();
  fPro = new FairGeanePro();
  if(flRK){
    std::cout<<"RKTrackRep will be used for track representation"<<std::endl;
    //  GFFieldManager::getInstance()->init(new GFPandaField());
    GFFieldManager::getInstance()->init(new PndGenfitField());
  }
  if(flGEANE){
    std::cout<<"GeaneTrackRep will be used for track representation"<<std::endl;
    fPro = new FairGeanePro();
  }
  return kSUCCESS;
 
}


void
PndLmdKalmanTask::Exec(Option_t* opt)
{
  fTrackFittedArray->Delete();

  if(fVerbose>1) std::cout<<"((((((((((((((((((((( PndLmdKalmanTask::Exec )))))))))))))))))))))"<<std::endl;
  Int_t counterGeaneTrk = 0;
  Int_t rec_tkr_count = 0;
  Int_t ntracks=fTrackArray->GetEntriesFast();

  // Detailed output
  if(fVerbose>1) std::cout<<" -I- PndLmdKalmanTask: contains "<<ntracks<<" Tracks."<<std::endl;
  GFKalman fitter;
  int numIttr = 1;
  fitter.setNumIterations(numIttr);
  for(Int_t itr=0;itr<ntracks;++itr){
      if(fVerbose>1) std::cout<<"starting track"<<itr<<std::endl;

    PndTrackCand* trackCand = (PndTrackCand*)fTrackArray->At(itr);
    const int Ntrkcandhits= trackCand->GetNHits();
    //Read info about 1st plane(sensor)
    PndTrackCandHit theHit = trackCand->GetSortedHit(0); //get 1st hit
    Int_t hitID = theHit.GetHitId();
    PndSdsHit* myHit = (PndSdsHit*)(fSdsHitsArray->At(hitID));
    TMatrixD  hitCov = myHit->GetCov();
    if(fVerbose>1) std::cout<<"hitCov:"<<std::endl;
    if(fVerbose>1) hitCov.Print();
    Int_t id =  myHit->GetSensorID();
    TString path = fGeoH->GetPath(id);
    TVector3 oo, uu, vv;
     fGeoH->GetOUVShortId(id, oo,uu,vv);
     if(fVerbose>1){
       std::cout<<"oo:"<<std::endl;
       oo.Print();
       std::cout<<"uu:"<<std::endl;
       uu.Print();
       std::cout<<"vv:"<<std::endl;
       vv.Print();
     }
 


     GFTrackCand* GFtrkCand = PndTrackCand2GenfitTrackCand(trackCand);
     TVector3 StartPos = GFtrkCand->getPosSeed();
     TVector3 StartDir = GFtrkCand->getDirSeed();
     TVector3 StartMom  = fPbeam*StartDir;
     TVector3 StartPosErr(sqrt(hitCov[0][0]),sqrt(hitCov[1][1]),sqrt(hitCov[2][2]));
     TVector3 StartDirErr(0.1*(sqrt(hitCov[0][0])),0.1*(sqrt(hitCov[1][1])),0.1*sqrt(hitCov[2][2]));//TODO: check this assumption (2*sigma_{x}/20cm)
     TVector3 StartMomErr=fPbeam*StartDirErr;
     //initial errors for Kalman must be large: this is usually done in order to give a low weight to the prefit
      StartPosErr *=fscaleP;
      StartMomErr *=fscaleM;
     //     StartMomErr *=1;
    if(fVerbose>2){
      unsigned int detid=12345, index=12345;
      std::cout<< "GFTrackCand no. "<<itr<<" has "<<GFtrkCand->getNHits()<<" hits."<<std::endl;
      std::cout<<"[ ihit | detid | index";
      for(unsigned int ihit=0;ihit<GFtrkCand->getNHits();ihit++){
	GFtrkCand->getHit(ihit,  detid,index); //detid and index are written here
	std::cout<<" ]\n[ "<<ihit<<" | "<<detid<<" | "<<index;
      }
      std::cout<<" ]"<<std::endl;
    }
 
    if(fVerbose>1){
    std::cout<<"*** BEFORE ***"<<std::endl;
    std::cout<<"StartPos:"<<std::endl;
    StartPos.Print();
    std::cout<<"StartPosErr:"<<std::endl;
    StartPosErr.Print();
    std::cout<<"StartMom:"<<std::endl;
    StartMom.Print();
    std::cout<<"StartMomErr:"<<std::endl;
    StartMomErr.Print();
    }

    GFAbsTrackRep *rep;
    // // /// The Runge Kutta trk rep ---------------
    if(flRK){
      if(fVerbose>1)
	std::cout<<"RKTrackRep will be used for track representation"<<std::endl;
      rep = new RKTrackRep(StartPos,StartMom,StartPosErr,StartMomErr,fPDGCode);
    }
    // // /// The Runge Kutta trk rep (END) ---------


   //  /// LinTrk rep ------------------
 // // TVector3 StartPos = GFtrkCand->getPosSeed();
 // // TVector3 StartDir = GFtrkCand->getDirSeed();
 // //     TVector3 StartMom  = fPbeam*StartDir;
 //    LSLTrackRep* rep = new LSLTrackRep(StartPos.Z(),StartPos.X(),StartPos.Y(),StartDir.X(),StartDir.Y(),1/fPbeam,
 // 				       StartPosErr.X(),StartPosErr.Y(),StartDirErr.X(),StartDirErr.Y(),
 // 				       1e-23,magFieldConst);
 //    //TODO: double siginvp = 1/1e-4 is it correct???

 //    /// LinTrk rep (END) ------------------

    ///GEANE track rep --------------------
    if(flGEANE){
    //   if(fVerbose>1)
    // 	std::cout<<"GeaneTrackRep will be used for track representation"<<std::endl;
    //   //TODO:  to initilaze   GeaneTrackRep one has to do "propagation". Why???
    //   FairTrackParH *fStart = new FairTrackParH(StartPos, StartMom, StartPosErr, StartMomErr, fCharge);
    //   FairTrackParH *fRes = new FairTrackParH();
    //   Double_t deltaZ = -5e-4;//go out of plane for 5mkm
    //   TVector3 dirCand = GFtrkCand->getDirSeed();
    //   if(fVerbose>1){
    // 	std::cout<<" ---- dirCand ---- "<<std::endl;
    // 	dirCand.Print();
    // }
    // TVector3 pointbackprop(StartPos.X()+deltaZ*dirCand.X(),StartPos.Y()+deltaZ*dirCand.Y(),(StartPos.Z()+deltaZ));
    // fPro->SetPoint(pointbackprop);
    // fPro->PropagateToPCA(1, -1);
    // Bool_t rc =  fPro->Propagate(fStart, fRes, fPDGCode);
    // if(fVerbose>1){
    // if(rc) std::cout<<"success in back propagation to init GeaneTrackRep !"<<std::endl;
    // else std::cout<<" =( no success in back propagation to init GeaneTrackRep ! =("<<std::endl;
    // }
    // if (rc)
    //   {
    // 	StartPos.SetXYZ(fRes->GetX(), fRes->GetY(), fRes->GetZ());
    // 	StartMom.SetXYZ(fRes->GetPx(), fRes->GetPy(), fRes->GetPz());
    // 	StartPosErr.SetXYZ(fRes->GetDX(), fRes->GetDY(), fRes->GetDZ());
    // 	StartMomErr.SetXYZ(fRes->GetDPx(), fRes->GetDPy(), fRes->GetDPz());
    //   }
    // if(fVerbose>1){
    //   std::cout<<"*** AFTER ***"<<std::endl;
    //   std::cout<<"StartPos:"<<std::endl;
    //   StartPos.Print();
    //   std::cout<<"StartPosErr:"<<std::endl;
    //   StartPosErr.Print();
    //   std::cout<<"StartMom:"<<std::endl;
    //   StartMom.Print();
    //   std::cout<<"StartMomErr:"<<std::endl;
    //   StartMomErr.Print();
    // }
      GFDetPlane start_pl(StartPos,uu,vv);//
      rep = new GeaneTrackRep(fPro,
			    start_pl,StartMom,
			    StartPosErr,StartMomErr,
			    fCharge,fPDGCode);
    }
    ///GEANE track rep (END) --------------------


    GFTrack* trk= new GFTrack(rep);
    trk->setCandidate(*GFtrkCand);

    // Load RecoHits
    try {
      trk->addHitVector(fTheRecoHitFactory->createMany(trk->getCand()));
      if(fVerbose>1){
	std::cout<<trk->getNumHits()<<" hits in track "
		 <<itr<<std::endl;
      }
    }
    catch(GFException& e) {
      std::cout <<" *** PndLmdKalmanTask::Exec "<< "\t" << "Genfit Exception: trk->addHitVector " << e.what() << std::endl;
      throw e;
    }

    // Start Fitter
    try{
      if(fVerbose>1){
	std::cout<<" ... GFtrk BEFORE ..."<<std::endl;
	trk->Print();
      }
      fitter.processTrack(trk);
      if(fVerbose>1){
	std::cout<<" ... GFtrk AFTER ..."<<std::endl;
	trk->Print();
      }
    }
    catch (GFException e){
      std::cout<<"*** FITTER EXCEPTION ***"<<std::endl;
      std::cout<<e.what()<<std::endl;
    }
    if (fVerbose>0) std::cout<<"successful FIT!"<<std::endl;
  

    // --- Get trk in  PndTrack format ---
    PndTrack* trkPnd = GenfitTrack2PndTrack(trk);
    if(fVerbose>1){
      std::cout<<"trkPnd AFTER GenFit "<<std::endl;
      trkPnd->Print();
      std::cout<<"Number of hits in trk-cand: "<<trackCand->GetNHits()<<std::endl;
    }
    trkPnd->SetTrackCand(*trackCand);
    trkPnd->SetRefIndex(itr);//TODO: check is it correct set RefIn like ID of trk-cand???
    trkPnd->SetChi2(trk->getChiSqu());
    // --- Save as PndTrack---
    TClonesArray& clref = *fTrackFittedArray;
    Int_t size = clref.GetEntriesFast();
    PndTrack *trackfit = new(clref[size]) PndTrack(*trkPnd);

 //    // --- Save as Lin trk ---
 //    FairTrackParP fFittedTrkParabolStart = trkPnd->GetParamFirst();
 //    TVector3 FinPos(fFittedTrkParabolStart.GetX(),fFittedTrkParabolStart.GetY(),fFittedTrkParabolStart.GetZ());
 //    TVector3 FinMom(fFittedTrkParabolStart.GetPx(),fFittedTrkParabolStart.GetPy(),fFittedTrkParabolStart.GetPz());
 //    double covMARS[6][6];
 //     fFittedTrkParabolStart.GetMARSCov(covMARS);
 // //    std::cout<<"covMARS:"<<std::endl;
 // //    std::cout<<"sqrt(covMARS[0][0]) = "<<sqrt(covMARS[0][0])<<" sqrt(covMARS[1][1]) = "<<sqrt(covMARS[1][1])<<" sqrt(covMARS[2][2]) = "<<sqrt(covMARS[2][2])<<std::endl;
 // // std::cout<<"sqrt(covMARS[3][3]) = "<<sqrt(covMARS[3][3])<<" sqrt(covMARS[4][4]) = "<<sqrt(covMARS[4][4])<<" sqrt(covMARS[5][5]) = "<<sqrt(covMARS[5][5])<<std::endl;

 //    // TVector3 FinPosErr(fFittedTrkParabolStart.GetDX(),fFittedTrkParabolStart.GetDY(),fFittedTrkParabolStart.GetDZ());
 //    // TVector3 FinMomErr(fFittedTrkParabolStart.GetDPx(),fFittedTrkParabolStart.GetDPy(),fFittedTrkParabolStart.GetDPz());
   

 //   //-------------------------------------
 //    // if(flGEANE){
 //    // //propagate out of middle of plane
 //    // FairTrackParH *fStartNEW = new FairTrackParH(FinPos, FinMom, FinPosErr, FinMomErr, fCharge);
 //    // FairTrackParH *fResNEW = new FairTrackParH();
 //    // Double_t deltaZ1 = -3.5e-2;//go out of plane for 350 mkm
 //    // TVector3 dirVec = FinMom*(1./FinMom.Mag());
 //    // TVector3 pointbackpropNEW(FinPos.X()+deltaZ1*dirVec.X(),FinPos.Y()+deltaZ1*dirVec.Y(),(FinPos.Z()+deltaZ1));
 //    // fPro->SetPoint(pointbackpropNEW);
 //    // fPro->PropagateToPCA(1, -1); 
 //    // Bool_t rcNEW =  fPro->Propagate(fStartNEW, fResNEW, fPDGCode);
 //    // if(fVerbose>1){
 //    //   if(rcNEW) std::cout<<"=) ! success in final propagation out of 1st plane ! (="<<std::endl;
 //    //   else std::cout<<" =( no success in final propagation out of 1st plane =("<<std::endl;
 //    // }
 //    // if (rcNEW)
 //    //   {
 //    // 	FinPos.SetXYZ(fResNEW->GetX(), fResNEW->GetY(), fResNEW->GetZ());
 //    // 	FinMom.SetXYZ(fResNEW->GetPx(), fResNEW->GetPy(), fResNEW->GetPz());
 //    // 	FinPosErr.SetXYZ(fResNEW->GetDX(), fResNEW->GetDY(), fResNEW->GetDZ());
 //    // 	FinMomErr.SetXYZ(fResNEW->GetDPx(), fResNEW->GetDPy(), fResNEW->GetDPz());
 //    //   }
 //    // }

 //    if(fVerbose>1){
 //      std::cout<<"Trk parameters after fit: "<<std::endl;
 //      std::cout<<"Position:"<<std::endl;
 //      FinPos.Print();
 //      // std::cout<<"Position error:"<<std::endl;
 //      // FinPosErr.Print();
 //      std::cout<<"Momentum:"<<std::endl;
 //      FinMom.Print();
 //      // std::cout<<"Momentum Error:"<<std::endl;
 //      // FinMomErr.Print();
 //    }

 //    TVector3 FinDir = FinMom*(1./FinMom.Mag());
 //    //  TVector3 FinDirErr = FinMomErr*(1./FinMom.Mag());
 //    double chi2GF = trk->getChiSqu();
 //    TClonesArray& clref = *fTrackFittedArray;
 //    Int_t size = clref.GetEntriesFast();
 //    PndLinTrack *trackfit = new(clref[size]) PndLinTrack("Lumi",FinPos.X(),FinDir.X(),FinPos.Y(),FinDir.Y(),FinPos.Z(),
 // 							 FinDir.Z(),chi2GF, -1,-1,itr);//TODO: set correct hit id!
 //  //   //    trackfit->Print();
 //  //   GFAbsTrackRep* clone = trk->getCardinalRep()->clone();
 //  //   TMatrixT<double> firstCov = clone->getFirstCov();
 //  //   std::cout<<" full: cov.matrix"<<std::endl;
 //  //   firstCov.Print();
 //  //   std::cout<<"sqrt(firstCov(0,0)) = "<<sqrt(firstCov(0,0))<<" sqrt(firstCov(1,1)) = "<<sqrt(firstCov(1,1))<<" sqrt(firstCov(2,2)) = "<<sqrt(firstCov(2,2))<<std::endl; 
 //  //   std::cout<<"FinDirErr: "<<std::endl;
 //  //   FinDirErr.Print(); 
 //  //   std::cout<<"sqrt(firstCov(3,3)) = "<<sqrt(firstCov(3,3))<<" sqrt(firstCov(4,4)) = "<<sqrt(firstCov(4,4))<<" sqrt(firstCov(5,5)) = "<<sqrt(firstCov(5,5))<<std::endl;
 //  // std::cout<<"FinPosErr: "<<std::endl;
 //  //   FinPosErr.Print();
 //    TMatrixDSym *COVmatrix = new TMatrixDSym(6);
 //    int iconver[6]={3, 4, 5, 0, 1, 2};
 //    for(int ji=0; ji<6; ji++){
 //      for(int ij=0; ij<6; ij++){
 // 	int km =  iconver[ji];
 // 	int mk = iconver[ij];
 // 	double scf=1;
 // 	if(km<3) scf *= 1/fPbeam;
 // 	if(mk<3) scf *= 1./fPbeam;
 // 	(*COVmatrix)(ji,ij) =scf*covMARS[km][mk];
 //      }
 //    }
 //    if(fVerbose>1){
 //      std::cout<<"Covariance Matrix:"<<std::endl;
 //      COVmatrix->Print();
 //    }
    
 //    //  std::cout<<" sqrt(((*COVmatrix)(0,0))) = "<<sqrt(((*COVmatrix)(0,0)))<<" sqrt(((*COVmatrix)(3,3))) = "<<sqrt(((*COVmatrix)(3,3)))<<std::endl
 //    // (*COVmatrix)(0,0) = FinPosErr.X()*FinPosErr.X();
 //    // (*COVmatrix)(1,1) = FinDirErr.X()*FinDirErr.X();
 //    // (*COVmatrix)(2,2) = FinPosErr.Y()*FinPosErr.Y();
 //    // (*COVmatrix)(3,3) = FinDirErr.Y()*FinDirErr.Y();
 //    // (*COVmatrix)(4,4) = FinPosErr.Z()*FinPosErr.Z();
 //    // (*COVmatrix)(5,5) = FinDirErr.Z()*FinDirErr.Z();
   
 //    trackfit->SetCovarianceMatrix(*COVmatrix);
  }
  if(fVerbose>1)
    std::cout<<"Fitting done"<<std::endl;
  return;
}
ClassImp(PndLmdKalmanTask);
