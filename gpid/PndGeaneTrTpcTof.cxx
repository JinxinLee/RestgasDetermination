// Task for the exercise 1 of the GEANE tutorial
// Authors A. Fontana & P. Genova, Sept. 2007
// Adapted for EMC, JGM, 27/03/08

#include "TClonesArray.h"
#include "FairRootManager.h"
#include "PndGeaneTrTpcTof.h"
#include "TGeant3TGeo.h"
#include "TGeant3.h"
#include "TVector3.h"
#include "PndPidCand.h"
#include "TTree.h"
#include "TDatabasePDG.h"

#include "FairTrackParH.h"
#include "CbmMCTrack.h"
#include <iostream>


#define PI 3.14159265



using namespace std;

// -----   Default constructor   -------------------------------------------
PndGeaneTrTpcTof::PndGeaneTrTpcTof() :
  FairTask("PndGeaneTrTpcTof") { }
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
PndGeaneTrTpcTof::~PndGeaneTrTpcTof()
 {
   destroy();
 }
void PndGeaneTrTpcTof::destroy()
{

//  delete t;
//  delete f;

  delete fPoint1; 
  delete fPoint2; 
  delete fTrack1; 
 
}

// -------------------------------------------------------------------------


// -----   Public method Init   --------------------------------------------
InitStatus PndGeaneTrTpcTof::Init() {
cout<<"-I- PndGeaneTrTpcTof"<<endl;
  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- PndGeaneTrTpcTof::Init: "
	 << "RootManager not instantised!" << endl;
    return kFATAL;
  }
  
  // Get input array
  fPointArray1 = (TClonesArray*) ioman->GetObject("PndTpcPoint"); 
  if ( ! fPointArray1 ) {
    cout << "-W- PndGeaneTrTpcTof::Init: "
         << "No PndTpcPoint array!" << endl;
    return kERROR;
  }

 fMCTrackArr = (TClonesArray*) ioman->GetObject("MCTrack"); 
  if ( ! fMCTrackArr ) {
    cout << "-W- PndGeaneTrTpcTof::Init: "
         << "No MCTrack array!" << endl;
    return kERROR;
  }

  fTrackArray1 = (TClonesArray*) ioman->GetObject("PndTpcLheTrack"); 
  if ( ! fTrackArray1 ) {
    cout << "-W- PndGeaneTrTpcTof::Init: "
         << "No PndTpcLheTrack array!" << endl;
    return kERROR;
  }

  fPointArray2 = (TClonesArray*) ioman->GetObject("TofHit");
  if ( ! fPointArray2 ) {
    cout << "-W- PndGeaneTrTpcTof::Init: "
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
void PndGeaneTrTpcTof::Exec(Option_t* opt) {
TObjArray* points;
PndTpcLheHit* hit;
CbmMCTrack* mcstack; 
   fTrackParGeane->Delete();
   fTrackParIni->Delete();
   fPndTrackArr->Delete();
   
// cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl; 
  cout<<fTrackArray1->GetEntriesFast()<<"/"<<fPointArray2->GetEntriesFast()<<endl;
 if (fTrackArray1->GetEntriesFast()==0) return;  
for (Int_t l=0; l<fTrackArray1->GetEntriesFast();l++   )  {          //loop over track
    fTrack1 = (PndTpcLheTrack *)fTrackArray1->At(l);
    points = fTrack1->GetRHits();
  //  hit = dynamic_cast <PndTpcLheHit *> ( points->At(points->GetEntriesFast()-1) );    //last point of Track as start
                                                                                       //position for geane
    hit = dynamic_cast <PndTpcLheHit *> ( points->At(0) );    //first point of Track as start
    if(hit == NULL) continue;
    fPoint1 =(PndTpcPoint *) fPointArray1->At(hit->GetRefIndex());
    if(fPoint1 == NULL)continue;
    Int_t trId = fPoint1->GetTrackID();
   mcstack = (CbmMCTrack *) fMCTrackArr->At(trId);
   if(mcstack == 0 ) cout<<"CbmMCTrack at trId not found"<<endl; 
    Int_t PDGCode = mcstack->GetPdgCode();

/*
  TVector3 Mom;       //    = fPoint1->GetMomentum();
    Mom = mcstack->GetMomentum();
    double p=Mom.Mag(); 
    TDatabasePDG *fdbPDG= TDatabasePDG::Instance();
    TParticlePDG *fParticle= fdbPDG->GetParticle(PDGCode);
    Double_t  fmass = fParticle->Mass();
    double energy = sqrt(p*p+fmass*fmass);
cout<<" energy and momentum   "<<energy<<"  "<<p<<endl;
//    if((energy/1.47*p) > 1.0 ) continue;

    Double_t thetaC = acos(energy/(1.47*p)) *180/PI;
*/


    TVector3 StartPos;         //    = fPoint1->GetStartVertex();
    fPoint1->Position(StartPos);
    TVector3 StartPosErr = TVector3(0,0,0);
    TVector3 StartMom;       //    = fPoint1->GetMomentum();
    fPoint1->Momentum(StartMom);
    TVector3 StartMomErr = TVector3(0,0,0);
    Float_t t1 = fPoint1->GetTime();
  //    Int_t PDGCode =  fTrack1->GetPid();
  //    TDatabasePDG *fdbPDG= TDatabasePDG::Instance();
  //    TParticlePDG *fParticle= fdbPDG->GetParticle(PDGCode);
  //    Double_t  fCharge= fParticle->Charge();
      Double_t  fCharge= fTrack1->GetCharge();


    TClonesArray& clref1 = *fTrackParIni;
    Int_t size1 = clref1.GetEntriesFast();
// cout<<"~~~~~~~~~~~before~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<PDGCode<<endl; 
   FairTrackParH *fStart= new (clref1[size1]) FairTrackParH(StartPos, StartMom, StartPosErr, StartMomErr, fCharge);
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
cout<<"Geane guess"<<fRes->GetX()<<" "<<fRes->GetY()<<"  "<<fRes->GetPhi()*180/PI<<"  "<<rc<<"  "<<PDGCode<<endl;
//fPro->GetTrkLength();

TVector3 tofv;
Double_t xo,yo;
Int_t min_i=0;
Double_t r_min=10000000;
//vector<double> dist;
//vector<double>::iterator it;
 xo = fRes->GetX();
 yo = fRes->GetY();
 for (Int_t k=0; k<fPointArray2->GetEntriesFast(); k++)	{
        Double_t x,y,r;
    	fPoint2 = (PndTofHit *)fPointArray2->At(k);
if (fPoint2 == 0)continue;
  //      it = dist.begin();
        tofv = fPoint2->GetPosition();
        x = tofv.X();
        y = tofv.Y();
cout<<"  "<<x
    <<"  "<<y
    <<"  "<<xo
    <<"  "<<yo<<endl;
        r = sqrt ((xo-x)*(xo-x)+(yo-y)*(yo-y));
        if ( r < r_min ) {
           r_min=r;
           min_i=k;
           }
  //      dist.insert(it,r);
//cout<<x<<setw(12)<<y<<setw(12)<<atan2(y,x)*180/PI<<setw(12)<<r<<setw(12)<<k<<endl;
    }
//cout<<fPoint2<<endl;
fPoint2 = (PndTofHit *)fPointArray2->At(min_i);
if (fPoint2 == 0 )continue;
//cout<<"min element is at   "<<min_i<<"  "<<r_min<<"  "<<fPoint2->GetTime()<<"  "<<fPro->GetTrkLength()<<endl;
 fTrack->Set("dr",r_min);
 Double_t t2=fPoint2->GetTime();
fTrack->Set("tof",t2-t1);
fTrack->Set("PMag",StartMom.Mag());
double p = StartMom.Mag();
Double_t s = gMC->TrackLength()/(30.0*(t2-t1));
double gamma2;
gamma2 =  1.0/(1-s*s);
Double_t m2 = p*p/(s*s*gamma2);
fTrack->Set("speed",s);
fTrack->Set("msquare",m2);

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



ClassImp(PndGeaneTrTpcTof)
