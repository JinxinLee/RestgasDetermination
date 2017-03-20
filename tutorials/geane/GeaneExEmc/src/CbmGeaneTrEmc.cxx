// Task for the exercise 1 of the GEANE tutorial
// Authors A. Fontana & P. Genova, Sept. 2007
// Adapted for EMC, JGM, 27/03/08

#include "TClonesArray.h"
#include "FairRootManager.h"
#include "FairGeaneTrEmc.h"
#include "TGeant3TGeo.h"
#include "TGeant3.h"
#include "TVector3.h"
#include "TTree.h"
#include "TDatabasePDG.h"
#include "FairTrajFilter.h"
#include "FairTrackParH.h"
#include <iostream>

using namespace std;

// -----   Default constructor   -------------------------------------------
FairGeaneTrEmc::FairGeaneTrEmc() :
  FairTask("Test") { }
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
FairGeaneTrEmc::~FairGeaneTrEmc() { }
// -------------------------------------------------------------------------


// -----   Public method Init   --------------------------------------------
InitStatus FairGeaneTrEmc::Init() {

  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- FairGeaneTrEmc::Init: "
	 << "RootManager not instantised!" << endl;
    return kFATAL;
  }
  
  // Get input array
  fPointArray1 = (TClonesArray*) ioman->GetObject("MCTrack"); 
  if ( ! fPointArray1 ) {
    cout << "-W- FairGeaneTrEmc::Init: "
         << "No MCTrack array!" << endl;
    return kERROR;
  }


  fPointArray2 = (TClonesArray*) ioman->GetObject("EmcPoint");
  if ( ! fPointArray2 ) {
    cout << "-W- FairGeaneTrEmc::Init: "
         << "No EmcPoint array!" << endl;
    return kERROR;
  }
 
  fTrackParGeane = new TClonesArray("FairTrackParH");
  ioman->Register("GeaneTrackPar","Geane", fTrackParGeane, kTRUE);
  
  fTrackParIni = new TClonesArray("FairTrackParH");
  ioman->Register("GeaneTrackIni","Geane", fTrackParIni, kTRUE);
  
  fTrackParFinal = new TClonesArray("FairTrackParH");
  ioman->Register("GeaneTrackFinal","Geane", fTrackParFinal, kTRUE);
  
    // Create and register output array

  fPro = new FairGeanePro();


 // if the vis manager is available then initialize it!
  
 /*  FairTrajFilter *fTrajFilter = FairTrajFilter::Instance();
   if(fTrajFilter) fTrajFilter->Init("GeaneTrk", "Geane");
*/

  return kSUCCESS;

}
// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void FairGeaneTrEmc::Exec(Option_t*) {
// 	cout << "FairGeaneTrEmc::Exec" << endl;
   fTrackParGeane->Delete();
   fTrackParIni->Delete();
   fTrackParFinal->Delete();
  
  cout<<fPointArray1->GetEntriesFast()<<"/"<<fPointArray2->GetEntriesFast()<<endl;
  
    fPoint1 = (PndMCTrack *)fPointArray1->At(0);
    Int_t trId=0;
    fPoint2=0;
    for (Int_t k=0; k<fPointArray2->GetEntriesFast(); k++)	{
    	fPoint2 = (PndEmcPoint *)fPointArray2->At(k);
	//    	cout << "loop for " << k << " / " << fPoint2->GetTrackID() << endl;
	//if(fPoint2->GetTrackID()==trId && (fPoint2->GetModule()==1 || fPoint2->GetModule()==2) ) break;
	if(fPoint2->GetTrackID()==trId) break;
	fPoint2=0;
    }
   
    if(fPoint2==0) return;

    if (fPoint2->GetModule()<3)
      {
	fPro->PropagateToVolume("Emc12",0,1);
      }
    else if (fPoint2->GetModule()==3)
      {
	fPro->PropagateToVolume("Emc3",0,1);
      }
    else if (fPoint2->GetModule()==4)
      {
	fPro->PropagateToVolume("Emc4",0,1);
      }
    else
      {
	return;
      }
    
    TVector3 StartPos    = fPoint1->GetStartVertex();
//    TVector3 StartPos    = TVector3 (0.,0.,0.);
    TVector3 StartPosErr = TVector3(0,0,0);
    TVector3 StartMom    = fPoint1->GetMomentum();
//    TVector3 StartMom    = TVector3 (1.,0.1,0.1);
    TVector3 StartMomErr = TVector3(0,0,0);
 
     TVector3 EndPos =TVector3 (fPoint2->GetX(),fPoint2->GetY(),fPoint2->GetZ());
     TVector3 EndPosErr=TVector3(0,0,0);
     TVector3 EndMom= TVector3 (fPoint2->GetPx(),fPoint2->GetPy(),fPoint2->GetPz());
     TVector3 EndMomErr=TVector3(0,0,0);

     Int_t PDGCode=fPoint1->GetPdgCode();
     TDatabasePDG *fdbPDG= TDatabasePDG::Instance();
     TParticlePDG *fParticle= fdbPDG->GetParticle(PDGCode);
     Double_t  fCharge= fParticle->Charge();

    TClonesArray& clref1 = *fTrackParIni;
    Int_t size1 = clref1.GetEntriesFast();
    FairTrackParH *fStart= new (clref1[size1]) FairTrackParH(StartPos, StartMom, StartPosErr, StartMomErr, fCharge);
      
    TClonesArray& clref = *fTrackParGeane;
    Int_t size = clref.GetEntriesFast();
    FairTrackParH *fRes=	new(clref[size]) FairTrackParH();
 
    TClonesArray& clref2 = *fTrackParFinal;
    Int_t size2 = clref2.GetEntriesFast();
    FairTrackParH *fFinal= new(clref2[size2]) FairTrackParH(EndPos, EndMom, EndPosErr, EndMomErr, fCharge);
   
    cout << "fStart = " << endl;
    fStart->Print();
    Bool_t rc =  fPro->Propagate(fStart, fRes,PDGCode);	

    cout << "fFinal = " << endl;
    fFinal->Print();
    cout << "fEnd = " << endl;
    fRes->Print();

  Double_t fLm,fPhi,cLm,sLm,cphi,sphi,fX_sc,fY_sc,fZ_sc,fX,fY,fZ;
  
  fLm = fRes->GetLambda();
  fPhi= fRes->GetPhi();
  fX = fFinal->GetX();
  fY = fFinal->GetY();
  fZ = fFinal->GetZ();
  cLm= TMath::Cos(fLm);
  sLm= TMath::Sin(fLm);
  cphi= TMath::Cos(fPhi);
  sphi= TMath::Sin(fPhi);

  fX_sc   = fX*cphi*cLm+ fY*cLm*sphi+fZ*sLm;
  fY_sc   = fY*cphi-fX*sphi; 
  fZ_sc   = fZ*cLm-fY*sLm*sphi-fX*sLm*cphi;

  fFinal->SetX_sc(fX_sc);
  fFinal->SetY_sc(fY_sc);
  fFinal->SetZ_sc(fZ_sc);

  //  }
  // }
  //  if (fPointArray1)  fPointArray1->Delete();
  //if (fPointArray2)  fPointArray2->Delete();
   
}



ClassImp(FairGeaneTrEmc)
