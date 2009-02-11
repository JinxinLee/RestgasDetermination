// Task for the exercise 1 of the GEANE tutorial
// Authors A. Fontana & P. Genova, Sept. 2007
// Adapted for EMC, JGM, 27/03/08

#include "TClonesArray.h"
#include "FairRootManager.h"
#include "PndGeaneTrTpcIdealTof.h"
#include "TGeant3TGeo.h"
#include "TGeant3.h"
#include "TVector3.h"
#include "PndPidCand.h"
#include "TTree.h"
#include "TDatabasePDG.h"

#include "FairTrackParH.h"
#include "PndMCTrack.h"
#include <iostream>

#define PI 3.14159265


using namespace std;

// -----   Default constructor   -------------------------------------------
PndGeaneTrTpcIdealTof::PndGeaneTrTpcIdealTof() :
  FairTask("PndGeaneTrTpcIdealTof") { }
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndGeaneTrTpcIdealTof::~PndGeaneTrTpcIdealTof()
{
  destroy();
}
void PndGeaneTrTpcIdealTof::destroy()
{
  
  //  delete t;
  //  delete f;
  delete fPoint1; 
  delete fPoint2; 
  delete fTrack1; 
}

// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
InitStatus PndGeaneTrTpcIdealTof::Init() {
  cout<<"-I- PndGeaneTrTpcIdealTof"<<endl;
  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if (!ioman){
    cout << "-E- PndGeaneTrTpcIdealTof::Init: "
	 << "RootManager not instantised!" << endl;
    return kFATAL;
  }
  
  // Get input array
  fPointArray1 = (TClonesArray*) ioman->GetObject("PndTpcPoint"); 
  
  if (!fPointArray1){
    cout << "-W- PndGeaneTrTpcIdealTof::Init: "
         << "No PndTpcPoint array!" << endl;
    return kERROR;
  }

  fMCTrackArr = (TClonesArray*) ioman->GetObject("MCTrack"); 
  if (!fMCTrackArr){
    cout << "-W- PndGeaneTrTpcIdealTof::Init: "
         << "No MCTrack array!" << endl;
    return kERROR;
  }
  
  fTrackArray1 = (TClonesArray*) ioman->GetObject("PndTpcLheTrack"); 
  if (!fTrackArray1) {
    cout << "-W- PndGeaneTrTpcIdealTof::Init: "
         << "No PndTpcLheTrack array!" << endl;
    return kERROR;
  }
  
  fPointArray2 = (TClonesArray*) ioman->GetObject("TofPoint");
  if (!fPointArray2) {
    cout << "-W- PndGeaneTrTpcIdealTof::Init: "
         << "No TofHit array!" << endl;
    return kERROR;
  }
 
  fTrackParGeane = new TClonesArray("FairTrackParH");
  ioman->Register("GeaneTrackPar","Geane", fTrackParGeane, kTRUE);
  
  fTrackParIni = new TClonesArray("FairTrackParH");
  ioman->Register("GeaneTrackIni","Geane", fTrackParIni, kTRUE);
  
  fPndTrackArr = new TClonesArray("PndPidCand");
  ioman->Register("PndPidCand","Geane", fPndTrackArr, kTRUE);
  //  fTrackParFinal = new TClonesArray("FairTrackParH");
  //  ioman->Register("GeaneTrackFinal","Geane", fTrackParFinal, kTRUE);
  
  // Create and register output array
  fGeane = new FairGeane(); 
  fPro = new FairGeanePro();
  fPro->PropagateToVolume("tofB01",0,1);
  
  return kSUCCESS;
}
// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void PndGeaneTrTpcIdealTof::Exec(Option_t* opt) {
TObjArray* points;
PndTpcLheHit* hit;
PndMCTrack* pndbase; 
   fTrackParGeane->Delete();
   fTrackParIni->Delete();
   fPndTrackArr->Delete();
   
// cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
   cout<<fTrackArray1->GetEntriesFast()<<"/"<<fPointArray2->GetEntriesFast()<<endl;
   if (fTrackArray1->GetEntriesFast()==0) return;  
   for (Int_t l=0; l<fTrackArray1->GetEntriesFast();l++   ){//loop over track
     fTrack1 = (PndTpcLheTrack *)fTrackArray1->At(l);
     points = fTrack1->GetRHits();
     //  hit = dynamic_cast <PndTpcLheHit *> (
     //  points->At(points->GetEntriesFast()-1) ); //last point of
     //  Track as start
     //position for geane
     //first point of Track as start
     hit = dynamic_cast <PndTpcLheHit *> ( points->At(0) );
     if(hit == NULL) continue;
     fPoint1 =(PndTpcPoint *) fPointArray1->At(hit->GetRefIndex());
     if(fPoint1 == NULL)continue;
     Int_t trId = fPoint1->GetTrackID();
     pndbase = (PndMCTrack *) fMCTrackArr->At(trId);
     if(pndbase == 0 ) cout<<"PndMCTrack at trId not found"<<endl; 
     Int_t PDGCode = pndbase->GetPdgCode();

     TVector3 StartPos;         //    = fPoint1->GetStartVertex();
     fPoint1->Position(StartPos);
     TVector3 StartPosErr = TVector3(0,0,0);
     TVector3 StartMom;       //    = fPoint1->GetMomentum();
     fPoint1->Momentum(StartMom);
     TVector3 StartMomErr = TVector3(0,0,0);
     Float_t t1 = fPoint1->GetTime();
     Double_t  fCharge= fTrack1->GetCharge();
     
     TClonesArray& clref1 = *fTrackParIni;
     Int_t size1 = clref1.GetEntriesFast();
     // cout<<"~~~~~~~~~~~before~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<PDGCode<<endl;
     FairTrackParH *fStart= new (clref1[size1]) FairTrackParH(StartPos, StartMom, 
							    StartPosErr, StartMomErr,
							    fCharge);
     // cout<<"~~~~~~~~~~~after~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl; 
     
     TClonesArray& clref = *fTrackParGeane;
     Int_t size = clref.GetEntriesFast();
     FairTrackParH *fRes=	new(clref[size]) FairTrackParH();
     
     TClonesArray& clref2 = *fPndTrackArr;
     Int_t size2 = clref2.GetEntriesFast();
     PndPidCand *fTrack= new(clref2[size2]) PndPidCand();
     
     // cout << "fStart = " << endl;
     fStart->Print();
     Bool_t rc =  fPro->Propagate(fStart, fRes,PDGCode);
     if(!rc)continue;
     cout<<"Geane guess"<<fRes->GetX()<<" "<<fRes->GetY()
	 <<"  "<<fRes->GetPhi()*180/PI<<"  "<<rc<<"  "
	 <<PDGCode<<endl;
     //fPro->GetTrkLength();
     
     TVector3 tofv;
     Double_t xo,yo,zo;
     Double_t x,y,z,r;
     TVector3 Mom;
     Int_t min_i=0;
     Double_t r_min=10000000;
     //vector<double> dist;
     //vector<double>::iterator it;
     xo = fRes->GetX();
     yo = fRes->GetY();
     zo = fRes->GetZ();
     /*
       for (Int_t k=0; k<fPointArray2->GetEntriesFast(); k++)	{
       
       fPoint2 = (PndTofPoint *)fPointArray2->At(k);
       if (fPoint2 == 0)continue;
       //      it = dist.begin();
       fPoint2->PositionIn(tofv);
       //    tofv = fPoint2->GetPosition();
       x = tofv.X();
       y = tofv.Y();
       z = tofv.Z();
       cout<<"  "<<x
       <<"  "<<y
       <<"  "<<xo
       <<"  "<<yo
       <<"  "<<fPointArray2->GetEntriesFast()<<endl;
       r = sqrt ((xo-x)*(xo-x)+(yo-y)*(yo-y)+(zo-z)*(zo-z));
       if ( r < r_min ) {
       r_min=r;
       min_i=k;
       fTrack->Set("x1",x);
       fTrack->Set("y1",y);
       fTrack->Set("z1",z);
       }
       //      dist.insert(it,r);
       //cout<<x<<setw(12)<<y<<setw(12)<<atan2(y,x)*180/PI
       //<<setw(12)<<r<<setw(12)<<k<<endl;
       }
     */
     
     //cout<<fPoint2<<endl;
     //fPoint2 = (PndTofPoint *)fPointArray2->At(min_i);
     
     PndMCTrack *mctrack = (PndMCTrack *) fMCTrackArr->At(0);
     fPoint2 = (PndTofPoint *)fPointArray2->At(0);
     if (fPoint2 == 0 || mctrack == 0 )continue;
     fPoint2->PositionIn(tofv);
     x = tofv.X();
     y = tofv.Y();
     z = tofv.Z();
     r = sqrt ((xo-x)*(xo-x)+(yo-y)*(yo-y)+(zo-z)*(zo-z));
     fPoint2->MomentumIn(Mom);
     //Mom = mctrack->GetMomentum();
     //cout<<"min element is at   "<<min_i<<"  "<<r_min
     //<<"  "<<fPoint2->GetTime()<<"  "<<fPro->GetTrkLength()<<endl;
     fTrack->Set("dr",r);
     fTrack->Set("xo",xo);
     fTrack->Set("yo",yo);
     fTrack->Set("zo",zo);
     fTrack->Set("x1",x);
     fTrack->Set("y1",y);
     fTrack->Set("z1",z);
     fTrack->Set("PMag",Mom.Mag());
     fTrack->Set("px",StartMom.Px());
     fTrack->Set("py",StartMom.Py());
     fTrack->Set("pz",StartMom.Pz());
     Double_t t2=fPoint2->GetTime();
     fTrack->Set("tof",t2-t1);
     //fTrack->Set("PMag",StartMom.Mag());
     Double_t speed = gMC->TrackLength()/(30.0*(t2-t1));//speed in Natural Units
     fTrack->Set("speed",speed);
     //fTrack->SetLen(fPro->GetTrkLength());
     fTrack->Set("length",gMC->TrackLength());
     fTrack->Set("PDG",PDGCode);
     //fTrack->SetThetaC(thetaC);
     //gMC->TrackLength();
     /*
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
     */
     //  if (fPointArray1)  fPointArray1->Delete();
     //if (fPointArray2)  fPointArray2->Delete();
   }   
}

ClassImp(PndGeaneTrTpcIdealTof)
