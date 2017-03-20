// Task for the exercise 1 of the GEANE tutorial
// Authors A. Fontana & P. Genova, Sept. 2007

#include "TClonesArray.h"
#include "FairRootManager.h"
#include "FairGeaneTr.h"
#include "TGeant3TGeo.h"
#include "TGeant3.h"
#include "TVector3.h"
#include "CbmPlanePoint.h"
#include "TTree.h"
#include "TDatabasePDG.h"

#include "FairTrackParH.h"
#include <iostream>

using namespace std;

// -----   Default constructor   -------------------------------------------
FairGeaneTr::FairGeaneTr() :
  FairTask("Test") { }
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
FairGeaneTr::~FairGeaneTr() { }
// -------------------------------------------------------------------------


// -----   Public method Init   --------------------------------------------
InitStatus FairGeaneTr::Init() {

  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- FairGeaneTr::Init: "
	 << "RootManager not instantised!" << endl;
    return kFATAL;
  }
  
 
  fTrackParGeane = new TClonesArray("FairTrackParH");
  ioman->Register("GeaneTrackPar","Geane", fTrackParGeane, kTRUE);
  
  fTrackParIni = new TClonesArray("FairTrackParH");
  ioman->Register("GeaneTrackIni","Geane", fTrackParIni, kTRUE);
  
  fTrackParFinal = new TClonesArray("FairTrackParH");
  ioman->Register("GeaneTrackFinal","Geane", fTrackParFinal, kTRUE);
  
  // Get input array

  fPointArray1 = (TClonesArray*) ioman->GetObject("Plane1Point");

  fPointArray2 =  (TClonesArray*)ioman->GetObject("Plane2Point");

  // Create and register output array

  fPro = new FairGeanePro();

  fPro->PropagateToVolume("mL3",0,1);



  return kSUCCESS;

}
// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void FairGeaneTr::Exec(Option_t*) {
// 	cout << "FairGeaneTr::Exec" << endl;
   fTrackParGeane->Delete();
   fTrackParIni->Delete();
   fTrackParFinal->Delete();
  
  Int_t NoOfEntries=fPointArray1->GetEntriesFast();
  cout<<fPointArray2->GetEntriesFast()<<endl;
  for (Int_t i=0; i<NoOfEntries; i++)	{
    fPoint1 = (CbmPlanePoint *)fPointArray1->At(i);
//	cout << "first loop for " << i << "from "<< NoOfEntries << " entries " << endl;
    Int_t trId=fPoint1->GetTrackID();
    fPoint2=0;
    for (Int_t k=0; k<fPointArray2->GetEntriesFast(); k++)	{
    	fPoint2 = (CbmPlanePoint *)fPointArray2->At(k);
  //  	cout << "second loop for " << k << endl;
	if(fPoint2->GetTrackID()==trId) break;
    }
   
    if(fPoint2==0) break;
    
    TVector3 StartPos    = TVector3 (fPoint1->GetXIn(),fPoint1->GetYIn(),fPoint1->GetZIn());
//    TVector3 StartPos    = TVector3 (0.,0.,0.);
    TVector3 StartPosErr = TVector3(0,0,0);
    TVector3 StartMom    = TVector3 (fPoint1->GetPxIn(),fPoint1->GetPyIn(),fPoint1->GetPzIn());
//    TVector3 StartMom    = TVector3 (1.,0.1,0.1);
    TVector3 StartMomErr = TVector3(0,0,0);
 
     TVector3 EndPos =TVector3 (fPoint2->GetXIn(),fPoint2->GetYIn(),fPoint2->GetZIn());
     TVector3 EndPosErr=TVector3(0,0,0);
     TVector3 EndMom= TVector3 (fPoint2->GetPxIn(),fPoint2->GetPyIn(),fPoint2->GetPzIn());
     TVector3 EndMomErr=TVector3(0,0,0);


    Int_t PDGCode= 13;

    TDatabasePDG *fdbPDG= TDatabasePDG::Instance();
    TParticlePDG *fParticle= fdbPDG->GetParticle(PDGCode);
    Double_t  fCharge= fParticle->Charge();

    TClonesArray& clref1 = *fTrackParIni;
    Int_t size1 = clref1.GetEntriesFast();
    FairTrackParH *fStart= new (clref1[size1]) FairTrackParH(StartPos, StartMom, StartPosErr, StartMomErr, fCharge);
  //  fStart->Print();
      
    TClonesArray& clref = *fTrackParGeane;
    Int_t size = clref.GetEntriesFast();
    FairTrackParH *fRes=	new(clref[size]) FairTrackParH();
   // fRes->Print();
 
    TClonesArray& clref2 = *fTrackParFinal;
    Int_t size2 = clref2.GetEntriesFast();
    FairTrackParH *fFinal= new(clref2[size2]) FairTrackParH(EndPos, EndMom, EndPosErr, EndMomErr, fCharge);
   
   
    Bool_t rc =  fPro->Propagate(fStart, fRes,PDGCode);	

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

  }
  // }
   fPointArray1->Delete();
   fPointArray2->Delete();
   
}



ClassImp(FairGeaneTr)
