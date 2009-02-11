/////////////////////////////////////////////////////////////
//  PndDchSimpleFitter
///////////////////////////////////////////////////////////////// 

#include <iostream>
#include <cmath>

#include "TClonesArray.h"	
#include "TVector3.h"
#include "TVector2.h"
#include "TString.h"
#include "TH1F.h"
#include "TMath.h"
#include "TFile.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRun.h"

#include "PndDchSimpleFitter.h"
#include "PndDchPoint.h"

using std::cout;
using std::endl;

// Default constructor
PndDchSimpleFitter::PndDchSimpleFitter() :
  FairTask("very naive reconstruction for the dipole region") { 
  fPointsAfterXZ = 0;
  fPointsInXZ = 0;
  fPointsBeforeXZ = 0;
  fPointsYZ = 0;
  fPullPx = 0;
  fPullPy = 0;
  fPullPz = 0;
  fPullP  = 0;
}

// Destructor 
PndDchSimpleFitter::~PndDchSimpleFitter() {
  if (0 != fPointsBeforeXZ) {
    fPointsBeforeXZ->Delete();
    delete fPointsBeforeXZ;
  }
  if (0 != fPointsAfterXZ) {
    fPointsAfterXZ->Delete();
    delete fPointsAfterXZ;
  }
  if (0 != fPointsInXZ) {
    fPointsInXZ->Delete();
    delete fPointsInXZ;
  }
  if (0 != fPointsYZ) {
    fPointsYZ->Delete();
    delete fPointsYZ;
  }
}

// Public method Init 
InitStatus PndDchSimpleFitter::Init() {
 

  cout << "PndDchSimpleFitter::Init()... " << endl;
  
  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- PndDchSimpleFitter::Init():\n\t "
	 << "RootManager not instantiated!" << endl;
    return kFATAL;
  }
  // Get input array
  fPointArray = (TClonesArray*) ioman->GetObject("PndDchPoint");
  if ( ! fPointArray ) {
    cout << "-W- PndDchSimpleFitter::Init(): "
	 << "No EmcPoint array!" << endl;
    return kERROR;
  }


  fOutFile = "dchreco.root";
  
  fPullPx = new TH1F("pullPx","pullPx",400,-20,20);
  fPullPy = new TH1F("pullPy","pullPy",400,-20,20);
  fPullPz = new TH1F("pullPz","pullPz",400,-20,20);
  fPullP  = new TH1F("pullP" ,"pullP" ,400,-20,20);
  
  fZFieldBegin = 350.0;
  fZFieldEnd = 600.0;
  fField=0.895;  
  //TClonesArray of points to fit line before the dipole (Chamber 3  and 4) in XZ plane
  fPointsBeforeXZ = new TClonesArray("TVector2");
  //TClonesArray of points to fit line after the dipole (Chamber 7  and 8) in XZ plane
  fPointsAfterXZ = new TClonesArray("TVector2");
  //TClonesArray of points to fit circle in the dipole in XZ plane
  //(Chamber 5  and  6 and two points from crosing fit lines before nad after the dipole with the box field)
  fPointsInXZ = new TClonesArray("TVector2");
  //TClonesArray of points to fit line in YZ plane
  fPointsYZ = new TClonesArray("TVector2");

  cout << "-I- PndDchSimpleFitter: Intialization successfull" << endl;

  return kSUCCESS;

}

// Public method Exec 
void PndDchSimpleFitter::Exec(Option_t* opt) {
  fPointsBeforeXZ->Clear();
  fPointsAfterXZ->Clear();
  fPointsInXZ->Clear();
  fPointsYZ->Clear();
  
  // Loop over PndDchPoints creating 3 Arrays for track fit in XZ plane na i Array in YZ plane
  PndDchPoint* point  = 0;
  Int_t nPoints = fPointArray->GetEntriesFast();
  Int_t nBefore,nIn,nAfter,nYZ;
  nBefore=nIn=nAfter=nYZ=0;
  TVector3 momentumSim(0.,0.,0.);
  for (Int_t iPoint=0; iPoint<nPoints; iPoint++) {
    point  = (PndDchPoint*) fPointArray->At(iPoint);
    if (point->GetChamber() == 3 || point->GetChamber() == 4) {
      if(momentumSim.Mag()<10e-6)
	point->Momentum(momentumSim);
      TVector2 *pos = new((*fPointsBeforeXZ)[nBefore]) TVector2(point->GetX(),point->GetZ());
      nBefore++;
      TVector2 *pos1 = new((*fPointsYZ)[nYZ]) TVector2(point->GetY(),point->GetZ());
      nYZ++;
    } else if (point->GetChamber() == 5 || point->GetChamber() == 6) {
      TVector2 *pos = new((*fPointsInXZ)[nIn]) TVector2(point->GetX(),point->GetZ());
      nIn++;
      TVector2 *pos1 = new((*fPointsYZ)[nYZ]) TVector2(point->GetY(),point->GetZ());
      nYZ++;
    } else if (point->GetChamber() == 7 || point->GetChamber() == 8) {
      TVector2 *pos = new((*fPointsAfterXZ)[nAfter]) TVector2(point->GetX(),point->GetZ());
      nAfter++;
      TVector2 *pos1 = new((*fPointsYZ)[nYZ]) TVector2(point->GetY(),point->GetZ());
      nYZ++;
    }
  }
  
  TVector2 trackYZ(0., 0.);
  Bool_t isRecoTrackYZ  = GetLineParameters(fPointsYZ,trackYZ);
  TVector2 trackBeforeXZ(0., 0.);
  Bool_t isRecoTrackBeforeXZ = GetLineParameters(fPointsBeforeXZ,trackBeforeXZ);
  if(isRecoTrackBeforeXZ){
    Double_t xIn = (fZFieldBegin-trackBeforeXZ.Y())/trackBeforeXZ.X();
    //artificial dch point for entering magnetic field
    TVector2 *pos = new((*fPointsInXZ)[nIn]) TVector2(xIn,fZFieldBegin);
    nIn++;
  }
  TVector2 trackAfterXZ(0., 0.);
  Bool_t  isRecoTrackAfterXZ = GetLineParameters(fPointsAfterXZ,trackAfterXZ);
  if(isRecoTrackAfterXZ){
    Double_t xOut = (fZFieldEnd-trackAfterXZ.Y())/trackAfterXZ.X();
    //artificial dch point for exiting the field
    TVector2 *pos = new((*fPointsInXZ)[nIn]) TVector2(xOut,fZFieldEnd);
    nIn++;
  }
  TVector3 trackInXZ(0., 0., 0.);
  Bool_t  isRecoTrackInXZ =  GetCircleParameters(fPointsInXZ,trackInXZ);
  TVector3 momentum(0., 0., 0.);
  if(isRecoTrackYZ && isRecoTrackBeforeXZ && isRecoTrackInXZ && momentumSim.Mag()>10e-6){
    Bool_t isMomentum =  GetMomentum(trackInXZ,trackBeforeXZ,trackYZ,momentum);
    //fill pull histos
    fPullPx->Fill(100*(momentum.X()-momentumSim.X())/momentumSim.X());
    fPullPy->Fill(100*(momentum.Y()-momentumSim.Y())/momentumSim.Y());
    fPullPz->Fill(100*(momentum.Z()-momentumSim.Z())/momentumSim.Z());
    fPullP->Fill(100*(momentum.Mag()-momentumSim.Mag())/momentumSim.Mag());
  }
}

void PndDchSimpleFitter::Finish(){
  TFile* f = new TFile(fOutFile,"RECREATE");
  fPullPx->Write();
  fPullPy->Write();
  fPullPz->Write();
  fPullP->Write();
  f->Close();
  delete f;
}

Bool_t PndDchSimpleFitter::GetMomentum(TVector3 circle, TVector2 lineXZ, TVector2 lineYZ, TVector3& momentum ) {
  Double_t magXZ = 0.3*fField*circle(2)/100.;
  Int_t i = 0;


  // for debugin purpouse
  PndDchPoint *point;
  do{
    point = (PndDchPoint*) fPointArray->At(i++);
  } while (point->GetChamber() !=3);

  TVector3 mom;
  point->Momentum(mom);
  Double_t magXZzPointa = sqrt(mom.X()*mom.X()+mom.Z()*mom.Z());
  cout<< "magXZ = "<<magXZ<<" and from the point magXZ = "<<magXZzPointa<<endl<<endl;
  // end debuging


  if(lineXZ.X()==0 || lineYZ.X()==0)
    return kFALSE;
  
  Double_t angleXZ = atan((lineXZ.X()));
  if(angleXZ<0)
    angleXZ+=TMath::Pi();
  Double_t pz = magXZ*sin(angleXZ);
  momentum.SetZ(pz);
  Double_t px = pz/lineXZ.X();
  momentum.SetX(px);
  Double_t py = pz/lineYZ.X();
  momentum.SetY(py);


  //for debuging
  cout<<"xp = "<<lineXZ.X()<<" yp = "<<lineYZ.X()<<endl;;
  cout<<"momentum = ("<<momentum(0)<<","<<momentum(1)<<","<<momentum(2)<<")";
  cout<<" and form the Point momentum = ("<<mom(0)<<","<<mom(1)<<","<<mom(2)<<")"<<endl;


  
  if(momentum.Mag()>10e-6)
    return kTRUE;
  else 
    return kFALSE;
}

Bool_t PndDchSimpleFitter::GetCircleParameters(TClonesArray *fPoints, TVector3& result) {
  Double_t n,x,y,w,xx,yy,xy,wx,wy,WG,WA,WB,WC;
  Double_t aa,bb,cc;
  x=y=xx=yy=xy=w=wx=wy=n=WG=WA=WB=WC=aa=bb=cc=0.0;
  
  TVector2* point  = 0;
  Int_t nPoints = fPoints->GetEntriesFast();
  if(nPoints<3)
    return kFALSE;
  //  cout<<"Circle: nPoints="<<nPoints<<endl;
  Double_t pointTime;
  for (Int_t iPoint=0; iPoint<nPoints; iPoint++) {
    point  = (TVector2*) fPoints->At(iPoint);
    if ((point->X()!=0) && (point->Y()!=0)) {
      //  cout<<"Circle: x "<<point->X()<<" y "<<point->Y()<<endl;
      x  += point->X();
      y  += point->Y();
      xx += point->X()*point->X();
      yy += point->Y()*point->Y();
      xy += point->X()*point->Y();
      w  = xx + yy;
      wx += point->X()*point->X()*point->X()+point->Y()*point->Y()*point->X();
      wy += point->Y()*point->Y()*point->Y()+point->X()*point->X()*point->Y();
      n++;
    }
  }
  WG = n*xx*yy + 2*x*y*xy - n*xy*xy - x*x*yy - y*y*xx;
  WA = w*xx*yy + x*xy*wy + y*xy*wx - w*xy*xy - wx*x*yy - y*xx*wy;
  WB = n*wx*yy + w*xy*y + y*x*wy - n*xy*wy - w*x*yy - y*y*wx;
  WC = n*xx*wy + x*wx*y + w*x*xy - n*wx*xy - x*x*wy - w*xx*y;
  if (fabs(WG)<10e-6){
    return kFALSE;
  }
  aa = WA/WG;
  bb = WB/WG;
  cc = WC/WG;
  Double_t dd = bb*bb + cc*cc + 4*aa;
  if(dd<0)
    return kFALSE;
  else
    result.SetXYZ(bb/2.,cc/2.,0.5*sqrt(dd)); 
  return kTRUE;
}



Bool_t PndDchSimpleFitter::GetLineParameters(TClonesArray *fPoints, TVector2& result) const {
  Double_t n,x,y,xx,xy,WG,WA,WB;
  x=y=xx=xy=n=WG=WA=WB=0.0;
  TVector2* point  = 0;
  Int_t nPoints = fPoints->GetEntriesFast();
  if(nPoints<2)
    return kFALSE;
  Double_t pointTime;
  for (Int_t iPoint=0; iPoint<nPoints; iPoint++) {
    point  = (TVector2*) fPoints->At(iPoint);
    if ((point->X()!=0) && (point->Y()!=0)) {
      x  += point->X();
      y  += point->Y();
      xx += point->X()*point->X();
      xy += point->X()*point->Y();
      n++;
    }
  }
  WG = x*x - n*xx;
  WA = x*y - n*xy;
  WB = x*xy - xx*y;
  if (WG==0){
    return kFALSE;
  }
  result.Set(WA/WG,WB/WG);
  return kTRUE;
}



ClassImp(PndDchSimpleFitter)
