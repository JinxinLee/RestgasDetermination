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

// Class Member definitions -----------


PndLmdKalmanTask::PndLmdKalmanTask()
  : FairTask("Kalman Filter"), fPersistence(kFALSE)
{
  //  fTrackBranchName = "GFTrackCandLmd";
  fTrackBranchName = "LMDTrackCand";
  //  fSdsHitBranchName = "LMDHitsStrip";
  fSdsHitBranchName = "LMDHitsPixel";
  PndGeoHandling::Instance();
}

PndLmdKalmanTask::PndLmdKalmanTask(TString HitBranch, TString TrackBranch)
  : FairTask("Kalman Filter"), fPersistence(kFALSE)
{
  fTrackBranchName = TrackBranch;
  fSdsHitBranchName = HitBranch;
  PndGeoHandling::Instance();
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
  // fTrackArray = new TClonesArray("PndLinTrack");
  // ioman->Register("LMDTrack", "PndLmd", fTrackArray, kTRUE);

  if(fTrackArray==0)
    {
      Error("PndLmdKalmanTask::Init","GFTrackCandLmd array not found!");
      return kERROR;
    }

  fSdsHitsArray=(TClonesArray*) ioman->GetObject(fSdsHitBranchName);

  //Set output collection
  // fGFTrackArray = new TClonesArray("GFTrack");
  // ioman->Register("GFTrackLmd", "PndLmd", fGFTrackArray, kTRUE);
  // fTrackFittedArray = new TClonesArray("PndTrack");
  // ioman->Register("PndTrackLmd", "TrkFitted", fTrackFittedArray, kTRUE);
  fTrackFittedArray = new TClonesArray("PndLinTrack");
  ioman->Register("LMDTrack", "PndLmd", fTrackFittedArray, kTRUE);

  // fTrackParFinal = new TClonesArray("FairTrackParH");
  // ioman->Register("GeaneTrackFinal","Geane", fTrackParFinal, kTRUE);

  // Build hit factory -----------------------------
  fTheRecoHitFactory = new GFRecoHitFactory();

  // TClonesArray* stripar=(TClonesArray*) ioman->GetObject("LMDHitsStrip");
  TClonesArray* stripar=(TClonesArray*) ioman->GetObject(fSdsHitBranchName);
  if(stripar==0){ //TODO Convention on detector number needed
    Error("PndLmdKalmanTask::Init","LMDHitsStrip array not found");
  } else {
    // fTheRecoHitFactory->addProducer
    //   (ioman->GetBranchId("LMDHitsStrip"),new GFRecoHitProducer<PndSdsHit,PndSdsRecoHit>(stripar));
    //TODO: why ioman->GetBranchId("LMDHitsStrip") gives 7 and not 1 ???
    fTheRecoHitFactory->addProducer(1,new GFRecoHitProducer<PndSdsHit,PndSdsRecoHit>(stripar));
    std::cout << "*** PndLmdKalmanTask::Init" << "\t" << "fSdsHitBranchName array found" << std::endl;
  }

  //TO DO: implement for pixel!
  // TClonesArray* pixelar=(TClonesArray*) ioman->GetObject("LMDHitsPixel");
  // if(pixelar==0){
  //   Error("PndLmdKalmanTask::Init","LMDHitsPixel array not found");
  // } else { //TODO Convention on detector number needed
  //   fTheRecoHitFactory->addProducer
  //     (ioman->GetBranchId("LMDHitsPixel"),new GFRecoHitProducer<PndSdsHit,PndSdsRecoHit>(pixelar));
  // }



  fPro = new FairGeanePro();


  // // setup histograms
  // fPH=new TH1D("pH","p",100,0.4,0.6);
  // fChi2H=new TH1D("chi2H","chi2",100,0,20);
  // fMassV0=new TH1D("massV0","massV0",100,0,5);
  // fMassETAC=new TH1D("massEta","massEta",100,2.5,3.5);
  // fMasses=new TH1D("masses","masses",100,0,5);

  //read beam momentum from base
  FairRun* fRun = FairRun::Instance();
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairBaseParSet* par=(FairBaseParSet*)
    (rtdb->findContainer("FairBaseParSet"));
  fPbeam = par->GetBeamMom();
  //fPbeam -=8.77e-5;//TEST!!! energy loss for 11.91 GeV/c
  // fPbeam -=1e-4;//TEST!!! energy loss for 1.5 GeV/c
  // std::cout<<"Beam Momentum for this run is "<<fPbeam<<std::endl;
  fPDGCode = -2212; //barp
  fCharge = -1;//barp

  fGeoH = PndGeoHandling::Instance();

  return kSUCCESS;
 
}


void
PndLmdKalmanTask::Exec(Option_t* opt)
{
  // //TODO: make init mag.field in global way???
  // //needed for The Runge Kutta trk rep and lin trk rep
  // gfMagField = GFFieldManager::getInstance();
  // GFConstField *magFieldConst = new GFConstField(0.,0.,0.);
  // gfMagField->init(magFieldConst);

  // FairRunAna* fRun = FairRunAna::Instance();
  // fRun->GetField()->GetFieldValue(pnt, H);

  fTrackFittedArray->Delete();

  if(fVerbose>1) std::cout<<"((((((((((((((((((((( PndLmdKalmanTask::Exec )))))))))))))))))))))"<<std::endl;
  Int_t counterGeaneTrk = 0;
  Int_t rec_tkr_count = 0;
  Int_t ntracks=fTrackArray->GetEntriesFast();
  //  PndTrack* tAfter = NULL;

  // Detailed output
  if(fVerbose>1) std::cout<<" -I- PndLmdKalmanTask: contains "<<ntracks<<" Tracks."<<std::endl;
  // if(fVerbose>2){
  //   std::cout<< " Detailed Debug info on the tracks:"<<std::endl;
  //   unsigned int detid=12345, index=12345;
  //   for(Int_t itr=0;itr<ntracks;++itr){
  //     GFTrackCand* trcnd = (GFTrackCand*)fTrackArray->At(itr);
  //     std::cout<< "GFTrackCand no. "<<itr<<" has "<<trcnd->getNHits()<<" hits."<<std::endl;
  //     std::cout<<"[ ihit | detid | index";
  //     for(unsigned int ihit=0;ihit<trcnd->getNHits();ihit++){
  //       trcnd->getHit(ihit,  detid,index); //detid and index are written here
  //       std::cout<<" ]\n[ "<<ihit<<" | "<<detid<<" | "<<index;
  //     }
  //     std::cout<<" ]"<<std::endl;
  //   }
  // }

  // Fitting ---------------- can go to another task!
  GFKalman fitter;
  int numIttr = 1;
  fitter.setNumIterations(numIttr);
  //   fitter.setVerbose(fVerbose);

  //  std::vector<TLorentzVector*> particles;
  //std::vector<Int_t> signs;
 
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
     //  FairRootManager* ioman = FairRootManager::Instance();
     //  TString fGeoFile = ioman->GetInFile()->GetName();
    
     TString path = fGeoH->GetPath(id);
     // std::cout<<"!!! PndLmdKalmanTask::Exec !!!"<<std::endl;
     // std::cout<<"Detector path: "<<path.Data()<<std::endl;
     TVector3 oo, uu, vv;
     fGeoH->GetOUVShortId(id, oo,uu,vv);
     double ooZnew = oo.Z()-(fGeoH->GetSensorDimensionsShortId(id).Z());//shift c.s to plane surface
     oo.SetZ(ooZnew);
     if(fVerbose>1){
       std::cout<<"oo:"<<std::endl;
       oo.Print();
       std::cout<<"uu:"<<std::endl;
       uu.Print();
       std::cout<<"vv:"<<std::endl;
       vv.Print();
     }
     GFDetPlane start_pl(oo,uu,vv);


     GFTrackCand* GFtrkCand = PndTrackCand2GenfitTrackCand(trackCand);
     TVector3 StartPos = GFtrkCand->getPosSeed();
     TVector3 StartDir = GFtrkCand->getDirSeed();
     TVector3 StartMom  = fPbeam*StartDir;
     //     TVector3 StartPosErr(0.0050,0.0050,0);//TODO: check this assumption
     //     TVector3 StartMomErr(1e-3,1e-3,1e-5);//TODO: check this assumption
     TVector3 StartPosErr(sqrt(hitCov[0][0]),sqrt(hitCov[1][1]),sqrt(hitCov[2][2]));
     TVector3 StartDirErr(fabs(hitCov[0][2]),fabs(hitCov[2][0]),0.);//TODO: check this assumption
     TVector3 StartMomErr=fPbeam*StartDirErr;
     //convert initial params to local frame of 1st sensor
     TVector3 StartPos_loc = fGeoH->MasterToLocalShortId(StartPos, id);
     TVector3 StartMom_loc = fGeoH->MasterToLocalShortId(StartMom, id);
     TVector3 StartPosErr_loc = fGeoH->MasterToLocalShortId(StartPosErr, id); //TODO: not sure it's correct
     TVector3 StartMomErr_loc = fGeoH->MasterToLocalShortId(StartMomErr, id);

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
    //TODO: Aligment and Kalman fillter???
  

    // std::cout<<"*** BEFORE ***"<<std::endl;
    // std::cout<<"StartPos:"<<std::endl;
    // StartPos.Print();
    // std::cout<<"StartPosErr:"<<std::endl;
    // StartPosErr.Print();
    // std::cout<<"StartMom:"<<std::endl;
    // StartMom.Print();
    // std::cout<<"StartMomErr:"<<std::endl;
    // StartMomErr.Print();
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
    // /// The Runge Kutta trk rep ---------------
    // RKTrackRep* rep = new RKTrackRep(StartPos,StartMom,StartPosErr,StartMomErr,fPDGCode);

   
    // //  RKTrackRep* rep = new RKTrackRep(start_pl,StartMom_loc,fPDGCode);
  
    // /// The Runge Kutta trk rep (END) ---------


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
    //TODO: aaaaaa!
 
    // TVector3 U(1.,0.,0.);
    // TVector3 V(0.,1.,0.);
   
    // rotateFromLumiFrame(U,false);
    // rotateFromLumiFrame(V,false);
    // std::cout<<"*** U & V ***"<<std::endl;
    // U.Print();
    // V.Print();
   

    //    GFDetPlane start_pl(StartPos,StartMom,V);
    FairTrackParH *fStart = new FairTrackParH(StartPos, StartMom, StartPosErr, StartMomErr, fCharge);
    FairTrackParH *fRes = new FairTrackParH();
    //   Double_t deltaZ = -7.5e-3;//go out of plane for 75 mkm
    //  Double_t deltaZ = +7.5e-3;//go out of plane for 75 mkm
    Double_t deltaZ = -5e-2;//go out of plane for 500 mkm
    //Double_t deltaZ = -1e2;//go out of plane for 500 mkm
    // Double_t deltaZ = +3e1;//go out in plane
    TVector3 dirCand = GFtrkCand->getDirSeed();
    if(fVerbose>1){
      std::cout<<" ---- dirCand ---- "<<std::endl;
      dirCand.Print();
    }
    TVector3 pointbackprop(StartPos.X()+deltaZ*dirCand.X(),StartPos.Y()+deltaZ*dirCand.Y(),(StartPos.Z()+deltaZ));
    //TVector3 pointbackprop(0.,0.,0.);
    //    TVector3 pointbackprop(StartPos.X(),StartPos.Y(),StartPos.Z()+0.0001);
    fPro->SetPoint(pointbackprop);
    fPro->PropagateToPCA(1, -1);
    // fPro->BackTrackToVirtualPlaneAtPCA(1);
    Bool_t rc =  fPro->Propagate(fStart, fRes, fPDGCode);
    if(fVerbose>1){
    if(rc) std::cout<<"success in back propagation to origin!"<<std::endl;
    else std::cout<<" =( no success in back propagation to origin! =("<<std::endl;
    }
    if (rc)
      {
 	StartPos.SetXYZ(fRes->GetX(), fRes->GetY(), fRes->GetZ());
 	StartMom.SetXYZ(fRes->GetPx(), fRes->GetPy(), fRes->GetPz());
 	StartPosErr.SetXYZ(fRes->GetDX(), fRes->GetDY(), fRes->GetDZ());
 	StartMomErr.SetXYZ(fRes->GetDPx(), fRes->GetDPy(), fRes->GetDPz());
      }
    if(fVerbose>1){
      std::cout<<"*** AFTER ***"<<std::endl;
      std::cout<<"StartPos:"<<std::endl;
      StartPos.Print();
      std::cout<<"StartPosErr:"<<std::endl;
      StartPosErr.Print();
      std::cout<<"StartMom:"<<std::endl;
      StartMom.Print();
      std::cout<<"StartMomErr:"<<std::endl;
      StartMomErr.Print();
    }
      // GFDetPlane start_pl(StartPos,U,V);
    //  TVector3 StartDir = StartMom*(1./StartMom.Mag());
    // GFDetPlane start_pl(StartPos,StartDir);
    // std::cout<<"start_pl:"<<std::endl;
    // start_pl.Print();
    GeaneTrackRep* rep = new GeaneTrackRep(fPro,
    					   start_pl,StartMom,
    					   StartPosErr,StartMomErr,
    					   fCharge,fPDGCode);
    // GeaneTrackRep2* rep = new GeaneTrackRep2(start_pl,StartMom,
    // 					     StartPosErr,StartMomErr,fPDGCode);

    ///GEANE track rep (END) --------------------
    //  rep->setPropDir(-1);
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
      // std::cout << e.what() << std::endl;
      throw e;
    }

    // Start Fitter
    try{
      //   std::cout<<"starting fit"<<std::endl;
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
     if(fVerbose>1){
       std::cout<<"GFTrack: "<<std::endl;
       trk->Print();
     }

    // //--Convert GF trk to FairTrackParH
    // GFAbsTrackRep* clone = trk->getCardinalRep()->clone();
    // TMatrixT<double> firstState = clone->getFirstState();
    // TMatrixT<double> lastState = clone->getLastState();
    // TMatrixT<double> firstCov = clone->getFirstCov();
    // TMatrixT<double> lastCov = clone->getLastCov();
    // GFDetPlane firstPlane = clone->getFirstPlane();
    // std::cout<<"Convert GF trk to PndTrack:"<<std::endl;
    // std::cout<<"firstPlane:"<<std::endl;
    // firstPlane.Print();
    // GFDetPlane lastPlane = clone->getLastPlane();
    // std::cout<<"firstState:"<<std::endl;
    // firstState.Print();
    // std::cout<<"firstCov:"<<std::endl;
    // firstCov.Print();
    // double firstCova[15];
    // int count=0;;
    // for(int i=0; i<5;++i){
    //   for(int j=i;j<5;++j){
    // 	firstCova[count++]=firstCov[i][j];
    //   }
    // }
    // FairTrackParP fFittedTrkParabolStart(firstState[3][0],firstState[4][0],firstState[1][0],firstState[2][0],firstState[0][0],firstCova,firstPlane.getO(),firstPlane.getU(),firstPlane.getV(),1);

    // --- Get trk in  PndTrack format ---
    PndTrack* trkPnd = GenfitTrack2PndTrack(trk);
    if(fVerbose>1){
      std::cout<<"trkPnd AFTER GenFit "<<std::endl;
      trkPnd->Print();
      std::cout<<"Number of hits in trk-cand: "<<trackCand->GetNHits()<<std::endl;
    }
    trkPnd->SetTrackCand(*trackCand);
    trkPnd->SetRefIndex(itr);//TODO: check is it correct set RefIn like ID of trk-cand???
    // --- Save as Lin trk ---
    FairTrackParP fFittedTrkParabolStart = trkPnd->GetParamFirst();
    TVector3 FinPos(fFittedTrkParabolStart.GetX(),fFittedTrkParabolStart.GetY(),fFittedTrkParabolStart.GetZ());
    TVector3 FinMom(fFittedTrkParabolStart.GetPx(),fFittedTrkParabolStart.GetPy(),fFittedTrkParabolStart.GetPz());
    TVector3 FinPosErr(fFittedTrkParabolStart.GetDX(),fFittedTrkParabolStart.GetDY(),fFittedTrkParabolStart.GetDZ());
    TVector3 FinMomErr(fFittedTrkParabolStart.GetDPx(),fFittedTrkParabolStart.GetDPy(),fFittedTrkParabolStart.GetDPz());
   

    //propagate to middle of plane
    FairTrackParH *fStartNEW = new FairTrackParH(FinPos, FinMom, FinPosErr, FinMomErr, fCharge);
    FairTrackParH *fResNEW = new FairTrackParH();
    Double_t deltaZ1 = -deltaZ;//go in of plane for 75 mkm
    TVector3 dirVec = FinMom*(1./FinMom.Mag());
    TVector3 pointbackpropNEW(FinPos.X()+deltaZ1*dirVec.X(),FinPos.Y()+deltaZ1*dirVec.Y(),(FinPos.Z()+deltaZ1));
    fPro->SetPoint(pointbackpropNEW);
    fPro->PropagateToPCA(1, +1);
    //    fPro->PropagateToPCA(1, -1);
    Bool_t rcNEW =  fPro->Propagate(fStartNEW, fResNEW, fPDGCode);
    if(fVerbose>1){
      if(rcNEW) std::cout<<"=) ! success in propagation to origin of trk-cand ! (="<<std::endl;
      else std::cout<<" =( no success in propagation to origin of trk-cand =("<<std::endl;
    }
    if (rcNEW)
      {
 	FinPos.SetXYZ(fResNEW->GetX(), fResNEW->GetY(), fResNEW->GetZ());
 	FinMom.SetXYZ(fResNEW->GetPx(), fResNEW->GetPy(), fResNEW->GetPz());
 	FinPosErr.SetXYZ(fResNEW->GetDX(), fResNEW->GetDY(), fResNEW->GetDZ());
 	FinMomErr.SetXYZ(fResNEW->GetDPx(), fResNEW->GetDPy(), fResNEW->GetDPz());
      }


    TVector3 FinDir = FinMom*(1./FinMom.Mag());//TODO: accurate check is needed!
    TVector3 FinDirErr = FinMomErr*(1./FinMom.Mag());//TODO: accurate check is needed!
    // TVector3 FinDirErr = FinMomErr;//TODO: accurate check is needed!
    double chi2GF = trk->getChiSqu();
    //    if(chi2GF>30.) continue;//TODO: chech this cut!!!
    TClonesArray& clref = *fTrackFittedArray;
    Int_t size = clref.GetEntriesFast();
    PndLinTrack *trackfit = new(clref[size]) PndLinTrack("Lumi",FinPos.X(),FinDir.X(),FinPos.Y(),FinDir.Y(),FinPos.Z(),
							 FinDir.Z(),chi2GF, -1,-1,itr);//TODO: set correct hit id!
    GFAbsTrackRep* clone = trk->getCardinalRep()->clone();
    TMatrixT<double> firstCov = clone->getFirstCov();
    //    TMatrixT<double> firstCov = clone->getLastCov();
    TMatrixDSym *COVmatrix = new TMatrixDSym(6);
    (*COVmatrix)(0,0) = FinPosErr.X()*FinPosErr.X();
    (*COVmatrix)(1,1) = FinDirErr.X()*FinDirErr.X();
    (*COVmatrix)(2,2) = FinPosErr.Y()*FinPosErr.Y();
    (*COVmatrix)(3,3) = FinDirErr.Y()*FinDirErr.Y();
    (*COVmatrix)(4,4) = FinPosErr.Z()*FinPosErr.Z();
    (*COVmatrix)(5,5) = FinDirErr.Z()*FinDirErr.Z();
    // (*COVmatrix)(0,0) = firstCov(3,3)/(numIttr*numIttr);
    // (*COVmatrix)(0,1) = firstCov(3,1)/(FinMom.Mag()*numIttr*numIttr);
    // (*COVmatrix)(1,1) = firstCov(1,1)/(FinMom.Mag()*FinMom.Mag()*numIttr*numIttr);//TODO: accurate check is needed!
    // (*COVmatrix)(2,2) = firstCov(4,4)/(numIttr*numIttr);
    // (*COVmatrix)(2,3) = firstCov(4,2)/(FinMom.Mag()*numIttr*numIttr);
    // (*COVmatrix)(3,3) = firstCov(2,2)/(FinMom.Mag()*FinMom.Mag()*numIttr*numIttr);//TODO: accurate check is needed!
    // (*COVmatrix)(4,4) = 0;
    // (*COVmatrix)(5,5) = firstCov(0,0)/(FinMom.Mag()*FinMom.Mag()*numIttr*numIttr);//TODO: accurate check is needed!
    // (*COVmatrix)(0,0) = firstCov(3,3);
    // (*COVmatrix)(0,1) = firstCov(3,1);
    // (*COVmatrix)(1,1) = firstCov(1,1);//TODO: accurate check is needed!
    // (*COVmatrix)(2,2) = firstCov(4,4);
    // (*COVmatrix)(2,3) = firstCov(4,2);
    // (*COVmatrix)(3,3) = firstCov(2,2);//TODO: accurate check is needed!
    // (*COVmatrix)(4,4) = StartPosErr.Z()* StartPosErr.Z();
    // (*COVmatrix)(5,5) = firstCov(0,0)/(FinMom.Mag()*FinMom.Mag());//TODO: accurate check is needed!

    trackfit->SetCovarianceMatrix(*COVmatrix);
    // new((*fTrackFittedArray)[rec_tkr_count]) PndLinTrack(*(trackfit)); //save Track
    // delete trackfit;
    // rec_tkr_count++;
    // // // --- BackPropagate it to IP ---
    // Int_t ierr;
    // FairTrackParP fFittedTrkParabolStart = trkPnd->GetParamFirst();
    // FairTrackParP fFittedTrkParabolStart2 = trkPnd->GetParamLast();
    // FairTrackParH *fFittedTrkStart = new FairTrackParH(&fFittedTrkParabolStart,ierr);
    // // FairTrackParH *fFittedTrkStart = new FairTrackParH(&fFittedTrkParabolStart2,ierr);
    // FairTrackParH *fFittedRes = new FairTrackParH();
    // TVector3 IP(0.,0.,0.);
    // // fPro->SetPoint(pointbackprop);
    // fPro->SetPoint(IP);
    // fPro->PropagateToPCA(1, -1);
    // bool isProp = fPro->Propagate(fFittedTrkStart, fFittedRes, fPDGCode);
    // std::cout<<"... START: ..."<<std::endl;
    // fFittedTrkStart->Print();
    // // std::cout<<"... wait we have 2nd approx: ..."<<std::endl;
    // // fFittedTrkParabolStart2.Print();
    // std::cout<<"... AND: ..."<<std::endl;
    // fFittedRes->Print();

    // if(isProp == kTRUE){
    // 	new((*fTrackParFinal)[counterGeaneTrk]) FairTrackParH(*(fFittedRes)); //save Track
    // 	counterGeaneTrk++;
    //   }
    //   else{
    // 	if(fVerbose>2){
    // 	  std::cout<<"!!! Back-propagation with GEANE didn't return result !!!"<<std::endl;
    // 	// cout<<"StartPos:"<<endl;
    // 	// StartPos.Print();
    // 	// cout<<"StartPosErr:"<<endl;
    // 	// StartPosErr.Print();
	
    // 	// cout<<"StartMom: "<<StartMom.Mag()<<endl;
    // 	// StartMom.Print();
    // 	// cout<<"StartMomErr: "<<StartMomErr.Mag()<<endl;
    // 	// StartMomErr.Print();  
    // 	}
    // 	new((*fTrackParFinal)[counterGeaneTrk]) FairTrackParH(); //save NULL
    // 	//	new((*fTrackParFinal)[counterGeaneTrk]) FairTrackParP(); //save NULL
    // 	counterGeaneTrk++;
    //   }
    // new((*fTrackFittedArray)[fTrackcount]) PndTrack(*(trkPnd)); //save Track
    // fTrackcount++;

    // //  PndLinTrack* trkLin = GenfitTrack2PndLinTrack(trk);

    // // double dZ = sqrt(1-tAfter->GetTx()*tAfter->GetTx()-tAfter->GetTy()*tAfter->GetTy());
    // // double accuracy = 1e-6;//TODO: find how to extract if from KalmanFillter
    // //    TMatrixDSym *COVmatrix = new TMatrixDSym(6);
    // // TMatrixFSym COVmatrix;
    // // tAfter->CovMatrix(COVmatrix);
    // // std::cout<<"CovMatrix:"<<std::endl;
    // // COVmatrix.Print();
    // // PndLinTrack* trackfit = new PndLinTrack("Lumi",tAfter->GetX(),tAfter->GetTx(),tAfter->GetY(), tAfter->GetTx(),tAfter->GetZ(), dZ, accuracy, firstHit, lastHit, track);
    // // // PndLinTrack* trackfit = new PndLinTrack("Lumi", parFit[0], parFit[1], parFit[2], parFit[3], parFit[4], parFit[5], 
    // // // 					    accuracy, firstHit, lastHit, track);
    // // trackfit->SetCovarianceMatrix(*COVmatrix);
    // // new((*fTrackArray)[fTrackcount]) PndLinTrack(*(trackfit)); //save Track
    // // //    new((*fTrackFittedArray)[fTrackcount]) PndTrack(*(tAfter)); //save Track
    // // fTrackcount++;
  }
  if(fVerbose>1)
    std::cout<<"Fitting done"<<std::endl;
  return;
}

// PndLinTrack* PndLmdKalmanTask::GenfitTrack2PndLinTrack(const GFTrack* tr){
//   PndLinTrack* resLintrk = NULL;
//   PndTrack* tAfter=NULL;
//   try
//     {
//       tAfter = (PndTrack*)GenfitTrack2PndTrack(tr);
//     }
//   catch (GFException e)
//     {
//       std::cout<<"*** PndGenfitAdapters EXCEPTION ***"<<std::endl;
//       std::cout<<e.what()<<std::endl;  
//     } 
//   tAfter->Print();
  
//   GFAbsTrackRep* clone = tr->getCardinalRep()->clone();
//   TMatrixT<double> firstState = clone->getFirstState();
//   TMatrixT<double> lastState = clone->getLastState();
//   TMatrixT<double> firstCov = clone->getFirstCov();
//   TMatrixT<double> lastCov = clone->getLastCov();
//   GFDetPlane firstPlane = clone->getFirstPlane();
//   GFDetPlane lastPlane = clone->getLastPlane();
//   std::cout<<"First Plane:"<<std::endl;
//   firstPlane.Print();
//   std::cout<<"Last Plane:"<<std::endl;
//   lastPlane.Print();
//   return resLintrk;
 
//   //make the FairTrackParP for first and last hit
//   double firstCova[15];
//   int count=0;;
//   for(int i=0; i<5;++i){
//      for(int j=i;j<5;++j){
//   	   firstCova[count++]=firstCov[i][j];
//      }
//   }
//   double lastCova[15];
//   count=0;;
//   for(int i=0; i<5;++i){
//     for(int j=i;j<5;++j){
//   	  lastCova[count++]=lastCov[i][j];
//     }
//   }

//   //  calculation of spu = sign[p·(DJ x DK)]
//   double first_pro(0), last_pro(0), first_spu, last_spu;
//   bool exc(false);

//   try{
//     first_pro = gtr->getMom(firstPlane).Dot(firstPlane.getNormal());
//     last_pro = gtr->getMom(lastPlane).Dot(lastPlane.getNormal());
//   }
//   catch (GFException& e){
//     exc=true;
//     std::cerr<<"could not convert GenfitTrack to PndTrack"<<std::endl;
//     e.what();
//   }

//   first_spu = (first_pro>=0) ? 1 : -1;
//   last_spu = (last_pro>=0) ? 1 : -1;
    
//   FairTrackParP first(firstState[3][0],firstState[4][0],firstState[1][0],firstState[2][0],firstState[0][0],firstCova,firstPlane.getO(),firstPlane.getU(),firstPlane.getV(),first_spu);
//   FairTrackParP last(lastState[3][0],lastState[4][0],lastState[1][0],lastState[2][0],lastState[0][0],lastCova,lastPlane.getO(),lastPlane.getU(),lastPlane.getV(),last_spu);
    
//   //copy the trackCand
//   GFTrackCand genfitCand = tr->getCand();
//   PndTrackCand* pndCand = GenfitTrackCand2PndTrackCand(&genfitCand);
//   PndTrack* retVal =  new PndTrack(first,last,*pndCand);
//   retVal->SetChi2(tr->getChiSqu());
//   retVal->SetNDF(tr->getNDF());
//   if (tr->getNDF()==0 || exc) {
//     retVal->SetFlag(-1);
//   }
//   else {
//   	  retVal->SetFlag(1);
//   }

//   delete pndCand;
//   //return retVal;
//    return resLintrk;
// }

ClassImp(PndLmdKalmanTask);
