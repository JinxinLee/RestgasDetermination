// -------------------------------------------------------------------------
// -----                   PndLmdGeaneTask source file                 -----
// -----                 Created 18/07/08  by T.Stockmanns             -----
// -----           modified for Lmd by M. Michel & A.Karavdina         -----
// -------------------------------------------------------------------------
// libc includes
#include <iostream>

// Root includes
#include "TROOT.h"
#include "TClonesArray.h"
#include "TVector3.h"


// framework includes
#include "FairRootManager.h"
#include "PndLmdGeaneTask.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairHit.h"
#include "PndMCTrack.h"
#include "FairBaseParSet.h"
// PndMvd includes
#include "PndSdsMCPoint.h"
#include "FairTrackParH.h"
#include "FairTrackParP.h"
#include "TGeant3.h"
#include "TDatabasePDG.h"
//PndLmd includes
#include "PndLinTrack.h"

#include <vector>
#include <map>


// -----   Default constructor   -------------------------------------------
PndLmdGeaneTask::PndLmdGeaneTask() : FairTask("Geane Task for PANDA Lmd"), fEventNr(0), fUseMVDPoint(false)
{
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndLmdGeaneTask::~PndLmdGeaneTask()
{
}

// -----   Public method Init   --------------------------------------------
InitStatus PndLmdGeaneTask::Init()
{

  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( !ioman){
    std::cout << "-E- PndLmdGeaneTask::Init: "<< "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  // Get input arrays
  fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");
  if (!fMCTracks){
    std::cout << "-W- PndLmdGeaneTask::Init: "<< "No MCTrack" << " array!" << std::endl;
    return kERROR;
  }

  fMCHits = (TClonesArray*) ioman->GetObject("LMDPoint");
  if ( !fMCHits)	{
    std::cout << "-W- PndLmdGeaneTask::Init: "<< "No LMDPoint"<<" array!" << std::endl;
    return kERROR;
  }

  fTracks = (TClonesArray*) ioman->GetObject("LMDTrack");
  if (!fTracks){
    std::cout << "-W- PndLmdGeaneTask::Init: "<< "No Track" << " array!" << std::endl;
    return kERROR;
  }

  fTrackParGeane = new TClonesArray("FairTrackParH");
  ioman->Register("GeaneTrackPar","Geane", fTrackParGeane, kTRUE);

  fTrackParIni = new TClonesArray("FairTrackParH");
  ioman->Register("GeaneTrackIni","Geane", fTrackParIni, kTRUE);

  fTrackParFinal = new TClonesArray("FairTrackParH");
  ioman->Register("GeaneTrackFinal","Geane", fTrackParFinal, kTRUE);

  fDetName = new TClonesArray("TObjString");
  ioman->Register("DetName", "Geane", fDetName, kTRUE);

  fPro = new FairGeanePro();
  //  fGeoH = new PndGeoHandling(gGeoManager);
  fGeoH = PndGeoHandling::Instance();
  FairRun* fRun = FairRun::Instance();
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairBaseParSet* par=(FairBaseParSet*)
    (rtdb->findContainer("FairBaseParSet"));
  fPbeam = par->GetBeamMom();

  return kSUCCESS;
}
// -------------------------------------------------------------------------
void PndLmdGeaneTask::SetParContainers()
{
  // Get Base Container
 /// FairRun* ana = FairRun::Instance();
 //s FairRuntimeDb* rtdb=ana->GetRuntimeDb();

}


// -----   Public method Exec   --------------------------------------------
void PndLmdGeaneTask::Exec(Option_t* opt)
{
  std::map<int, std::vector<int> > mcHitMap;						//Track ->  MCHits
  fTrackParGeane->Delete();
  fTrackParIni->Delete();
  fTrackParFinal->Delete();
  fDetName->Delete();

  mcHitMap = AssignHitsToTracks();
  // cout<<"Beam Momentum = "<<fPbeam<<endl;
  // std::cout << "------------Event " << fEventNr << "-------------" << std::endl;
  fEventNr++;

  //go through all tracks
  int glI = fTracks->GetEntriesFast();
  int glMC = fMCTracks->GetEntriesFast();
  // cout<<"glI = "<<glI<<" glMC = "<<glMC<<endl;

  for (Int_t i = 0; i<glI;i++){ 
  // for (Int_t i = 0; i<glMC;i++){ //TEST!
    // //  //Get particle momentum
    //PndMCTrack* myTrack = (PndMCTrack*)(fMCTracks->At(0)); //TO DO: use beam energy instead of MC info
    // // cout<<"myTrack "<<myTrack<<endl;
    // TVector3 momMC = myTrack->GetMomentum();
    // Double_t mom_mag = momMC.Mag();
    // PndSdsMCPoint* mcPoint0 = (PndSdsMCPoint*)(fMCHits->At(0));
   
    // if (myTrack->GetMotherID() < 0){

      TVector3 StartPos, StartPosErr, StartMom, StartMomErr, StartO, StartU, StartV;
      int p = 0;
  
      // //Calculate parameters of lmd-track from MC-points
      // PndSdsMCPoint* mcPointB = (PndSdsMCPoint*)(fMCHits->At(0));
      // PndSdsMCPoint* mcPointE = (PndSdsMCPoint*)(fMCHits->At(2));
      // TVector3 mcB = (mcPointB->GetPosition());
      // TVector3 mcE = (mcPointE->GetPosition());
      // Double_t p1mc = (mcB.X()-mcE.X())/(mcB.Z()-mcE.Z());
      // Double_t p0mc = 0.5*(mcB.X()+mcE.X()-p1mc*(mcE.Z()+mcB.Z()));
      // Double_t p3mc = (mcB.Y()-mcE.Y())/(mcB.Z()-mcE.Z());
      // Double_t p2mc = 0.5*(mcB.Y()+mcE.Y()-p3mc*(mcB.Z()+mcE.Z()));
      // cout<<"mc=("<<p0mc<<", "<<p1mc<<", "<<p2mc<<", "<<p3mc<<")"<<endl;

      ///Get parameters of real track
      PndLinTrack* recTrack = (PndLinTrack*)(fTracks->At(i));
      Double_t parrecTrk[4];
      recTrack->GetPar(parrecTrk);
      Double_t p0 = parrecTrk[0];
      Double_t p1 = parrecTrk[1];
      Double_t p2 = parrecTrk[2];
      Double_t p3 = parrecTrk[3];
      // cout<<"real=("<<p0<<", "<<p1<<", "<<p2<<", "<<p3<<")"<<endl;
      Double_t parrecTrkErr[4];
      recTrack->GetParErr(parrecTrkErr);
     
      //Vector of particle momentum and starting point
      TVector3  DirVec =  recTrack->GetDirectionVec();
      // //      Double_t theta = DirVec.Theta();
      StartMom = TVector3(p1*fPbeam,p3*fPbeam,fPbeam);

      // cout<<"StartMom:"<<endl;
      // StartMom.Print();
      StartPos = recTrack->GetStartVec();
      // cout<<"StartPos:"<<endl;
      // StartPos.Print();
      StartMomErr = TVector3(parrecTrkErr[1]*fPbeam,parrecTrkErr[3]*fPbeam,0);
      StartPosErr = TVector3(parrecTrkErr[0],parrecTrkErr[2],0);
      // // StartMomErr = TVector3(0.001,0.001,0.001);
      // // StartPosErr = TVector3(0.001,0.001,0.001);
      // cout<<"Errors:"<<endl;
      // cout<<"StartPosErr:"<<endl;
      // StartPosErr.Print();
      // cout<<"StartMomErr:"<<endl;
      // StartMomErr.Print();

      // //Get Start point and momentum from MC
      // PndSdsMCPoint* mcPoint = (PndSdsMCPoint*)(fMCHits->At(0));
      // // PndSdsMCPoint* mcPoint = (PndSdsMCPoint*)(fMCHits->At(1));
      // // cout<<"mcPoint->GetTrackID() = "<<mcPoint->GetTrackID()<<endl;
      // //  if(mcPoint->GetTrackID()!=0) continue;
      // // cout<<"mcPoint:"<<endl;
      // // cout<<(*mcPoint)<<endl;
      // StartPos = mcPoint->GetPosition();
      // // StartPos = mcPoint->GetPositionOut();
      // double zPos = StartPos.Z();
      // cout<<"zPos = "<<zPos<<endl;
      // StartPos.SetZ(zPos-0.0001); //TEST!
      // if(zPos>1101) continue;
      // TVector3 MCmom;
      // mcPoint->Momentum(MCmom);
      // //      mcPoint->MomentumOut(MCmom);
      // Double_t momMagMCpoint = MCmom.Mag();
      // // cout<<"mom_mag(MC Trk) = "<<mom_mag<<" momMagMCpoint = "<<momMagMCpoint
      // // 	  <<" #delta = "<<(mom_mag-momMagMCpoint)<<endl;
      // // cout<<"mom_mag(MC Trk)-8.9 = "<<mom_mag-8.9<<endl;
      // // cout<<"momMagMCpoint-8.9 = "<<momMagMCpoint-8.9<<endl;
      // //      if(fabs(mom_mag-momMagMCpoint)>5e-07) break; ///TEST!!!
      // // if(MCmom.Mag()<1e-1) return;
      // //  StartMom = TVector3(-MCmom.X(),-MCmom.Y(),-MCmom.Z()); //!!!
      // StartMom = TVector3(MCmom.X(),MCmom.Y(),MCmom.Z());

      // // cout<<"StartPos:"<<endl;
      // // StartPos.Print();
      // // cout<<"StartMom:"<<endl;
      // // StartMom.Print();
      // // // TVector3 pMCtrk = myTrack->GetMomentum();
      // // // cout<<"pMCtrk:"<<endl;
      // // // pMCtrk.Print();

      // // StartPosErr = TVector3(0,0,0); //TO DO:use real errors
      // // StartMomErr = TVector3(0,0,0);
      
      //Charge of particle
      //   Int_t PDGCode = myTrack->GetPdgCode();
      Int_t PDGCode = -2212;
      TDatabasePDG *fdbPDG = TDatabasePDG::Instance();
      TParticlePDG *fParticle = fdbPDG->GetParticle(PDGCode);
      Double_t  fCharge = fParticle->Charge();
     
      TClonesArray& clref1 = *fTrackParIni;
      Int_t size1 = clref1.GetEntriesFast();
     
      // FairTrackParP *fStart = 
      // 	new (clref1[size1]) FairTrackParP(StartPos, StartMom, StartPosErr, StartMomErr, fCharge, StartO, StartU, StartV);
      FairTrackParH *fStart = 
  	new (clref1[size1]) FairTrackParH(StartPos, StartMom, StartPosErr, StartMomErr, fCharge);
    
      TClonesArray& clref = *fTrackParGeane;
      Int_t size = clref.GetEntriesFast();
      FairTrackParH *fRes = new(clref[size]) FairTrackParH();

      
      //	This is right way
      //	TVector3 vtx = myTrack->GetStartVertex();;
      TVector3 vtx(0.,0.,0.);
      fPro->SetPoint(vtx);
      fPro->PropagateToPCA(1,-1);
      //fPro->BackTrackToVirtualPlaneAtPCA(1);//1 means "closest approach to point"
      Bool_t isProp =	fPro->Propagate(fStart, fRes, PDGCode);
      
      
      // /*Therefore let's make it in wrong way*/
      // //Propagation to PCA
      // TVector3 vtx = myTrack->GetStartVertex();
      // fPro->SetPoint(vtx);
      // fPro->BackTrackToVirtualPlaneAtPCA(1);
      // fPro->Propagate(fStart, fRes, PDGCode);
      // //fPro->PropagateToVirtualPlaneAtPCA(1);
      // // // //Forward propagation
      // // // fPro->PropagateToPCA(1,1);
      // // //Backward propagation
      // // fPro->PropagateToPCA(1,-1);
      // //fPro->Propagate(fStart, fRes, -PDGCode);
       
      // // //Propagation to plane
      // // TVector3 EndO = myTrack->GetStartVertex();
      // // TVector3 EndU = TVector3(1.0, 0.0, 0.0);
      // // TVector3 EndV = TVector3(0.0, 1.0, 0.0);  
      // // fPro->PropagateFromPlane(StartU, StartV);
      // // fPro->PropagateToPlane(EndO, EndU, EndV);
      // // fPro->Propagate(fStart, fRes, -PDGCode);

      // std::cout << std::endl;
      // std::cout << "Propagation Points: " << std::endl;
      // std::cout << fRes->GetX() << ", ";//" +/- " << fRes->GetDX() << std::endl;
      // std::cout << fRes->GetY() << ", ";//" +/- " << fRes->GetDY() << std::endl;
      // std::cout << fRes->GetZ(); //" +/- " << fRes->GetDZ() << std::endl;
      // std::cout << std::endl;
      
      // std::cout << "Propagation Momentum: " << std::endl;
      
      // std::cout << fRes->GetPx() << ", "; //" +/- " << fRes->GetDPx() << std::endl;
      // std::cout << fRes->GetPy() << ", "; //" +/- " << fRes->GetDPy() << std::endl;
      // std::cout << fRes->GetPz() << std::endl;//<< " +/- " << fRes->GetDPz() << std::endl;
      

      //True value for TEST
      PndMCTrack* myTrack = (PndMCTrack*)(fMCTracks->At(0));
      TVector3 momMC = myTrack->GetMomentum();
      // std::cout << "Momentum from MC: " << std::endl;
      //  momMC.Print();

      // std::cout << std::endl;
      // std::cout << "Err of Propag. Point: " << std::endl;
      // std::cout << fRes->GetDX() << ", ";//" +/- " << fRes->GetDX() << std::endl;
      // std::cout << fRes->GetDY() << ", ";//" +/- " << fRes->GetDY() << std::endl;
      // std::cout << fRes->GetDZ(); //" +/- " << fRes->GetDZ() << std::endl;
      // std::cout << std::endl;

      // std::cout << "Err of Propag. Point is SC: " << std::endl;
      // std::cout << fRes->GetDX_sc() << ", ";//" +/- " << fRes->GetDX() << std::endl;
      // std::cout << fRes->GetDY_sc() << ", ";//" +/- " << fRes->GetDY() << std::endl;
      // std::cout << fRes->GetDZ_sc(); //" +/- " << fRes->GetDZ() << std::endl;
      // std::cout << std::endl;
      
      // std::cout << "Err of Propag. Momentum: " << std::endl;
      
      // std::cout << fRes->GetDPx() << ", "; //" +/- " << fRes->GetDPx() << std::endl;
      // std::cout << fRes->GetDPy() << ", "; //" +/- " << fRes->GetDPy() << std::endl;
      // std::cout << fRes->GetDPz() << std::endl;//<< " +/- " << fRes->GetDPz() << std::endl;

      
      // std::cout << "Porpagation Covariance Matrix: " << std::endl;
      
      // Double_t CovMatrix[15];
      // Double_t CovMatrix66[6][6];
      // fRes->GetCov(CovMatrix);
      // fRes->GetMARSCov(CovMatrix66);
      
      // std::cout << CovMatrix66[3][3] << ", " << CovMatrix66[3][4] << ", " << CovMatrix66[3][5] << std::endl;
      // std::cout << CovMatrix66[4][4] << ", " << CovMatrix66[4][5] << ", " << CovMatrix66[5][5] << std::endl;
      // std::cout << CovMatrix66[0][3] << ", " << CovMatrix66[1][3] << ", " << CovMatrix66[2][3] << std::endl;
      // std::cout << CovMatrix66[0][4] << ", " << CovMatrix66[1][4] << ", " << CovMatrix66[2][4] << std::endl;
      // std::cout << CovMatrix66[0][5] << ", " << CovMatrix66[1][5] << ", " << CovMatrix66[2][5] << std::endl;
      // std::cout << CovMatrix66[0][0] << ", " << CovMatrix66[0][1] << ", " << CovMatrix66[0][2] << std::endl;
      // std::cout << CovMatrix66[1][1] << ", " << CovMatrix66[1][2] << ", " << CovMatrix66[2][2] << std::endl;
      // std::cout << std::endl;

      // TVector3 iniMom = TVector3(-fRes->GetPx(),-fRes->GetPy(),-fRes->GetPz());
      // cout<<"Initial momentum of particle:"<<endl;
      // iniMom.Print();
      if(isProp == kTRUE) new((*fTrackParFinal)[i]) FairTrackParH(*(fRes)); //save Track
      else cout<<"!!!!!!!!!!!!!!!!!!!!!!! isProp FALSE!!!!!!!!!!!!!!!!!"<<endl;
      //  }
  }

   
  // // // for (std::map<int, std::vector<int> >::const_iterator it = mcHitMap.begin(); it!= mcHitMap.end(); it++){  //go through all tracks
  // // //   PndMCTrack* myTrack = (PndMCTrack*)(fMCTracks->At(it->first));
  // // //   std::vector<int> MChits = it->second;
  // // //   if (MChits.size() > 1 && myTrack->GetMotherID() < 0){
  // // //     TVector3 StartPos, StartPosErr, StartMom, StartMomErr, StartO, StartU, StartV;
  // // //     int p = 0;
  // // //     if (fUseMVDPoint){
  // // //       PndSdsMCPoint* startPoint = (PndSdsMCPoint*)(fMCHits->At(0));
  // // //       std::cout << "StartPoint: " << *startPoint << std::endl;
  // // //       StartPos = (startPoint->GetPosition());
  // // //       StartPosErr = TVector3(0.0, 0.0, 0.0);
  // // //       StartMom = TVector3(startPoint->GetPx(), startPoint->GetPy(), startPoint->GetPz());
  // // //       StartMomErr = TVector3(0.0, 0.0, 0.0);
  // // //       fGeoH->GetOUVId(startPoint->GetDetName(), StartO, StartU, StartV);
  // // //       p = 1;
  // // //     }
  // // //     else{
  // // //       StartPos = myTrack->GetStartVertex();
  // // //       StartPosErr = TVector3(0.0, 0.0, 0.0);
  // // //       StartMom = myTrack->GetMomentum();
  // // //       StartMomErr = TVector3(0.0, 0.0, 0.0);
  // // //       StartO = myTrack->GetStartVertex();
  // // //       StartU = TVector3(1.0, 0.0, 0.0);
  // // //       StartV = TVector3(0.0, 1.0, 0.0);
  // // //       p = 0;
  // // //     }

  // // //     fPro->PropagateFromPlane(StartU, StartV);

  // // //     Int_t PDGCode = myTrack->GetPdgCode();
  // // //     TDatabasePDG *fdbPDG= TDatabasePDG::Instance();
  // // //     TParticlePDG *fParticle= fdbPDG->GetParticle(PDGCode);
  // // //     Double_t  fCharge= fParticle->Charge();

  // // //     TClonesArray& clref1 = *fTrackParIni;
  // // //     Int_t size1 = clref1.GetEntriesFast();
  // // //     FairTrackParP *fStart = 
  // // // 	new (clref1[size1]) FairTrackParP(StartPos, StartMom, StartPosErr, StartMomErr, fCharge, StartO, StartU, StartV);

  // // //     for (; p < MChits.size(); p++){	
  // // // 	//go through all hits in track
  // // //       PndSdsMCPoint* myPoint = (PndSdsMCPoint*)(fMCHits->At(MChits[p]));
  // // //       std::cout << "PropagationPoint: " << *myPoint << std::endl;

  // // //       TVector3 StopPos(myPoint->GetPosition());
  // // //       TVector3 StopPosErr(0.0, 0.0, 0.0);
  // // //       TVector3 StopMom(myPoint->GetPx(), myPoint->GetPy(), myPoint->GetPz());
  // // //       TVector3 StopMomErr(0.0, 0.0, 0.0);

  // // //       TVector3 o, u, v;
  // // //       fGeoH->GetOUVId(myPoint->GetDetName(), o, u, v);

  // // //       TClonesArray& clref2 = *fTrackParFinal;
  // // //       Int_t size2 = clref2.GetEntriesFast();
  // // //       FairTrackParP *fStop= new (clref2[size2]) FairTrackParP(StopPos, StopMom, StopPosErr, StopMomErr, fCharge, o, u, v);

  // // //       TClonesArray& clref = *fTrackParGeane;
  // // //       Int_t size = clref.GetEntriesFast();
  // // //       FairTrackParP *fRes = new(clref[size]) FairTrackParP();

  // // //       //std::cout << "DetName: " << fGeoH->GetPath(myPoint->GetDetName()) << std::endl;
  // // //       TClonesArray& cDetRef = *fDetName;
  // // //       Int_t size3 = cDetRef.GetEntriesFast();
  // // //       new (cDetRef[size3]) TObjString(fGeoH->GetPath(myPoint->GetDetName()));

  // // //       std::cout << "Propagation Plane:" << std::endl;
  // // //       std::cout << "o: " << o[0] << " " << o[1] << " " << o[2] << std::endl;
  // // //       std::cout << "u: " << u[0] << " " << u[1] << " " << u[2] << std::endl;
  // // //       std::cout << "v: " << v[0] << " " << v[1] << " " << v[2] << std::endl;

  // // //       TVector3 endPoint(myPoint->GetPosition());
  // // //       //fPro->SetPoint(endPoint);
  // // //       //fPro->PropagateToPCA(1);
  // // //       fPro->PropagateToPlane(o, u, v);
  // // // 	fPro->Propagate(fStart, fRes, PDGCode);
  // // // 	//	fPro->Propagate(fStop, fRes, PDGCode);

  // // //       std::cout << std::endl;
  // // //       std::cout << "Propagation Points: " << std::endl;
  // // //       std::cout << fRes->GetX() << ", ";//" +/- " << fRes->GetDX() << std::endl;
  // // //       std::cout << fRes->GetY() << ", ";//" +/- " << fRes->GetDY() << std::endl;
  // // //       std::cout << fRes->GetZ(); //" +/- " << fRes->GetDZ() << std::endl;
  // // //       std::cout << std::endl;

  // // //       std::cout << "Propagation Momentum: " << std::endl;

  // // //       std::cout << fRes->GetPx() << ", "; //" +/- " << fRes->GetDPx() << std::endl;
  // // //       std::cout << fRes->GetPy() << ", "; //" +/- " << fRes->GetDPy() << std::endl;
  // // //       std::cout << fRes->GetPz() << std::endl;//<< " +/- " << fRes->GetDPz() << std::endl;

  // // //       std::cout << "Charge: " << fRes->GetQ() << std::endl;

  // // //       std::cout << "Porpagation Covariance Matrix: " << std::endl;

  // // //       Double_t CovMatrix[15];
  // // //       Double_t CovMatrix66[6][6];
  // // //       fRes->GetCov(CovMatrix);
  // // //       fRes->GetMARSCov(CovMatrix66);

  // // //       std::cout << CovMatrix66[3][3] << ", " << CovMatrix66[3][4] << ", " << CovMatrix66[3][5] << std::endl;
  // // //       std::cout << CovMatrix66[4][4] << ", " << CovMatrix66[4][5] << ", " << CovMatrix66[5][5] << std::endl;
  // // //       std::cout << CovMatrix66[0][3] << ", " << CovMatrix66[1][3] << ", " << CovMatrix66[2][3] << std::endl;
  // // //       std::cout << CovMatrix66[0][4] << ", " << CovMatrix66[1][4] << ", " << CovMatrix66[2][4] << std::endl;
  // // //       std::cout << CovMatrix66[0][5] << ", " << CovMatrix66[1][5] << ", " << CovMatrix66[2][5] << std::endl;
  // // //       std::cout << CovMatrix66[0][0] << ", " << CovMatrix66[0][1] << ", " << CovMatrix66[0][2] << std::endl;
  // // //       std::cout << CovMatrix66[1][1] << ", " << CovMatrix66[1][2] << ", " << CovMatrix66[2][2] << std::endl;
  // // //       std::cout << std::endl;

  // // //       TVector3 global(fRes->GetX(), fRes->GetY(), fRes->GetZ());
  // // //       TVector3 local = fGeoH->MasterToLocalId(global, myPoint->GetDetName());

  // // //       std::cout << "Propagation Point local: " << std::endl;
  // // //       std::cout << local[0] << " " << local[1] << " " << local[2] << std::endl;
  // // //       std::cout << std::endl;
  // // //     }
  // // //   }
  // // // }

  fMCTracks->Delete();
  fMCHits->Delete();
}

void PndLmdGeaneTask::Finish()
{
}

std::map<int, std::vector<int> > PndLmdGeaneTask::AssignHitsToTracks()
{
	std::map<int, std::vector<int> > result;
	for (int i = 0; i < fMCHits->GetEntriesFast(); i++){											//get all MC Hits
		PndSdsMCPoint* myPoint = (PndSdsMCPoint*)(fMCHits->At(i));									//sort MCHits with Tracks
		//PndMCTrack* myTrack = (PndMCTrack*)(fMCTracks->At(myPoint->GetTrackID()));
		result[myPoint->GetTrackID()].push_back(i);

	}
	return result;
}

ClassImp(PndLmdGeaneTask);
