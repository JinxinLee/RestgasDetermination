/*
 *  
 *  S.Vanniarajan  V.Suyam.Jothi@kvi.nl
 *
 *  This is the Mutivariate Event classification tool
 *  designed for PANDAROOT Analysis package for 
 *  PANDA Experiments.
 *       TMVA(Toolkit for MultiVariate Analysis) is a 
 *  two class classifier(signal and background). 
 *  available with ROOT package. mainly used for 
 *  Event Classification in High Energy Physics Experiments.
 *         
 *       This tool here is Designed from TMVA, for Multiclass 
 * Classification purpose. 
 *
 *                  S.Vanniarajan  01-08-08
 * Modified:
 * M. Babai
 */

//C++ headers
#include <iostream>

//Panda & ROOT headers
#include "TClonesArray.h"
#include "TGeant3TGeo.h"
#include "TGeant3.h"
#include "TVector3.h"
#include "TTree.h"
#include "TDatabasePDG.h"

#include "PndPidCand.h"
#include "FairRootManager.h"
#include "PndGpidCollTask.h"
#include "FairTrackParH.h"
#include "CbmMCTrack.h"

#define PI 3.14159265

using namespace std;

PndGpidCollTask::PndGpidCollTask() :
  FairTask("PndGpidCollTask") { }

// -----   Destructor   ----------------------------------------------------
PndGpidCollTask::~PndGpidCollTask()
 {
   destroy();
 }
void PndGpidCollTask::destroy()
{
  delete fTpcPoint;
  delete fMvdPoint;
  delete fTrack1;
  delete fPoint2;
}

InitStatus PndGpidCollTask::Init() {
cout<<"-I- PndGpidCollTask"<<endl;
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- PndGpidCollTask::Init: "
	 << "RootManager not instantised!" << endl;
    return kFATAL;
  }

  fPointArray1 = (TClonesArray*) ioman->GetObject("PndTpcPoint"); 
  if ( ! fPointArray1 ) {
    cout << "-W- PndGpidCollTask::Init: "
         << "No PndTpcPoint array!" << endl;
    return kERROR;
  }

 fMCTrackArr = (TClonesArray*) ioman->GetObject("MCTrack"); 
  if ( ! fMCTrackArr ) {
    cout << "-W- PndGpidCollTask::Init: "
         << "No MCTrack array!" << endl;
    return kERROR;
  }

  fTrackArray1 = (TClonesArray*) ioman->GetObject("PndTpcLheTrack"); 
  if ( ! fTrackArray1 ) {
    cout << "-W- PndGpidCollTask::Init: "
         << "No PndTpcLheTrack array!" << endl;
    return kERROR;
  }

 
  fPndTrackArr = new TClonesArray("PndPidCand");
  ioman->Register("PndPidCand","Geane", fPndTrackArr, kTRUE);
  

  return kSUCCESS;
}


void PndGpidCollTask::Exec(Option_t* opt) 
{
TObjArray* points;
PndTpcLheHit* hit;
    TClonesArray& clref2 = *fPndTrackArr;
    Int_t size2 = clref2.GetEntriesFast();
    PndPidCand *fTrack= new(clref2[size2]) PndPidCand();


   Int_t nTracks = fTrackArray1->GetEntriesFast();

for (Int_t l=0; l < nTracks;l++   )  
    {          //loop over track
       hit = dynamic_cast <PndTpcLheHit *> ( points->At(0) ); 
       if(hit == NULL) continue;
       PndTpcPoint *Point1 =(PndTpcPoint *) fPointArray1->At(hit->GetRefIndex());
       if(Point1 == NULL)continue;
       Int_t trId = Point1->GetTrackID();
       CbmMCTrack *mcstack = (CbmMCTrack *) fMCTrackArr->At(trId);
       if(mcstack == 0 ) cout<<"CbmMCTrack at trId not found"<<endl;
       Int_t PDGCode = mcstack->GetPdgCode();

       fTrack1 = (PndTpcLheTrack *)fTrackArray1->At(l);
       PndLhePidTrack *pidTrack = ( PndLhePidTrack *) fPidTrack->At(l);
       points = fTrack1->GetRHits();
     Double_t de_tpc=0;
     Double_t dx_tpc=0;

     Double_t de_mvd=0;
     Double_t dx_mvd=0;
     Double_t px=0;
     Double_t py=0;
     Double_t pz=0;
    for (Int_t m=0 ; m < fTrack1->GetNumberOfHits(); m++  )  
     {
          hit = dynamic_cast <PndTpcLheHit *> ( points->At(m) );  
          if (hit->GetDetectorId() == 1) 
          {
            fTpcPoint =(PndTpcPoint *) fPointArray1->At(hit->GetRefIndex());            
            de_tpc += fTpcPoint->GetEnergyLoss();
            dx_tpc += fTpcPoint->GetLength();

          } 
          if (hit->GetDetectorId() == 3) 
          {
            fMvdPoint =(PndMvdMCPoint *) fPointArray1->At(hit->GetRefIndex());            
            de_mvd += fMvdPoint->GetEnergyLoss();
            dx_mvd += fMvdPoint->GetLength();
          } 
     }
     Double_t mass;
     if (PDGCode == -321 || PDGCode == 321) mass = 0.493667;       // in GeV
     if (PDGCode == -11  || PDGCode == 11 ) mass = 0.000511;       // in GeV
     if (PDGCode == -13  || PDGCode == 13 ) mass = 0.1057;       // in GeV
     if (PDGCode == -211 || PDGCode == 211) mass = 0.1307;       // in GeV
     if (PDGCode == -2212 || PDGCode == 2212) mass = 0.938272;       // in GeV

     Double_t E,P,ThetaC;
     Double_t n = 1.47;
     px = fTrack1->GetPx(); 
     py = fTrack1->GetPy(); 
     pz = fTrack1->GetPz();
     P = sqrt(px*px+py*py+pz*pz);
     E = sqrt (P*P+mass*mass);
     ThetaC = acos(E/(n*P));
     smear(ThetaC,0.01);                    // 10 milli radian
     fTrack->Set("Emc", pidTrack->GetEmcELoss());  
     fTrack->Set("PMag",P);
     fTrack->Set("thetaC",ThetaC);
     fTrack->Set("Tof",pidTrack->GetTof());
     fTrack->Set("PDG",PDGCode);
     if (dx_tpc != 0) fTrack->Set("TpcDEdx",de_tpc/dx_tpc);
     if (dx_mvd != 0) fTrack->Set("MvdDEdx",de_mvd/dx_mvd);
     }
 
}

void PndGpidCollTask::smear(Double_t& theta,const Double_t& del )
{
Double_t The = theta;
Double_t sigThe;
sigThe = gRandom->Gaus(0,del);
theta += sigThe; 
return;
}



ClassImp(PndGpidCollTask)
