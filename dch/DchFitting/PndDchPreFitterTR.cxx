/////////////////////////////////////////////////////////////
//  PndDchPreFitterTR
///////////////////////////////////////////////////////////////// 

#include <iostream>
#include <cmath>

#include "TClonesArray.h"	
#include "TVector3.h"
#include "TVector2.h"
#include "TString.h"
#include "TGeoMatrix.h"
#include "TH1F.h"
#include "TMath.h"
#include "TFile.h"

#include "CbmRootManager.h"
#include "CbmRunAna.h"
#include "CbmRun.h"

#include "PndDchPreFitterTR.h"
#include "PndDchHit.h"
#include "PndDchPoint.h"
#include "PndDchTrack.h"
#include "PndDchStructure.h"
#include "PndDchMapper.h"

using std::cout;
using std::endl;

// Default constructor
PndDchPreFitterTR::PndDchPreFitterTR() :
  CbmTask("very naive reconstruction for the dipole region") { 
  fInHitsAfterXZ = 0;
  fInHitsInXZ = 0;
  fInHitsBeforeXZ = 0;
  fInHitsYZ = 0;
  fPullPx = 0;
  fPullPy = 0;
  fPullPz = 0;
  fPullP  = 0;
  fDetIdList = 0;
}

// Destructor 
PndDchPreFitterTR::~PndDchPreFitterTR() {
  if (0 != fInHitsBeforeXZ) {
    fInHitsBeforeXZ->Delete();
    delete fInHitsBeforeXZ;
  }
  if (0 != fInHitsAfterXZ) {
    fInHitsAfterXZ->Delete();
    delete fInHitsAfterXZ;
  }
  if (0 != fInHitsInXZ) {
    fInHitsInXZ->Delete();
    delete fInHitsInXZ;
  }
  if (0 != fInHitsYZ) {
    fInHitsYZ->Delete();
    delete fInHitsYZ;
  }
  if( 0 != fDetIdList){
    delete fDetIdList;
  }
}

// Public method Init 
InitStatus PndDchPreFitterTR::Init() {
 

  cout << "PndDchPreFitterTR::Init()... " << endl;
  
  // Get RootManager
  CbmRootManager* ioman = CbmRootManager::Instance();
  TFile *infile = ioman->GetInFile();
  TGeoManager *geoMan = (TGeoManager*) infile->Get("CBMGeom");
  fStructure = PndDchStructure::Instance(geoMan);

  if ( ! ioman ) {
    cout << "-E- PndDchPreFitterTR::Init():\n\t "
	 << "RootManager not instantiated!" << endl;
    return kFATAL;
  }
  // Get input point array - only to get real value of momentum!
  fPointArray = (TClonesArray*) ioman->GetObject("PndDchPoint");
  if ( ! fPointArray ) {
    cout << "-W- PndDchPreFitterTR::Init(): "
	 << "No input PndDchPoint array!" << endl;
    return kERROR;
  }
  // Get input hit array
  fInHitArray = (TClonesArray*) ioman->GetObject("PndDchHit");
  if ( ! fInHitArray ) {
    cout << "-W- PndDchPreFitterTR::Init(): "
	 << "No input PndDchHit array!" << endl;
    return kERROR;
  }
  
  fDetIdList =  fStructure->GetDetectorIDList();
  fDetIdListSize = fDetIdList->GetSize();

  // Create and register output array of hots
  fHitArray = new TClonesArray("PndDchHit");
  ioman->Register("PndDchHot","Dch",fHitArray,kTRUE);


  // Get input track array
  fTrackArray = (TClonesArray*) ioman->GetObject("PndDchTrack");
  if ( !fTrackArray ) {
    cout << "-W- PndDchPreFitterTR::Init(): No PndDchTrack array!"
         << endl;
    return kERROR;
  }

  //output file for histos  
  fOutFile = "dchreco.root";
  
  fPullPx = new TH1F("pullPx","pullPx",400,-20,20);
  fPullPy = new TH1F("pullPy","pullPy",400,-20,20);
  fPullPz = new TH1F("pullPz","pullPz",400,-20,20);
  fPullP  = new TH1F("pullP" ,"pullP" ,400,-20,20);
  fPullX  = new TH1F("pullX","pullX" ,400,-0.03,0.03);
  fPullY  = new TH1F("pullY","pullY" ,400,-0.03,0.03);
  
  fZFieldBegin = 350.0;
  fZFieldEnd = 600.0;
  fField=0.895;  
  //TClonesArray of inhits to fit line before the dipole (Chamber 3  and 4) in XZ plane
  fInHitsBeforeXZ = new TClonesArray("TVector2");
  //TClonesArray of inhits to fit line after the dipole (Chamber 7  and 8) in XZ plane
  fInHitsAfterXZ = new TClonesArray("TVector2");
  //TClonesArray of inhits to fit circle in the dipole in XZ plane
  //(Chamber 5  and  6 and two inhits from crossing fit lines before nad 
  // after the dipole with the box field)
  fInHitsInXZ = new TClonesArray("TVector2");
  //TClonesArray of inhits to fit line in YZ plane
  fInHitsYZ = new TClonesArray("TVector2");

  cout << "-I- PndDchPreFitterTR: Intialization successfull" << endl;

  return kSUCCESS;

}

// Public method Exec 
void PndDchPreFitterTR::Exec(Option_t* opt) {
  //  cout<<"&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"<<endl;
  fInHitsBeforeXZ->Clear();
  fInHitsAfterXZ->Clear();
  fInHitsInXZ->Clear();
  fInHitsYZ->Clear();
  
  // Loop over PndDchHits creating 3 Arrays for track fit in XZ plane and one Array in YZ plane
  PndDchHit* inhit  = 0;
  PndDchPoint* point = 0;
  Int_t nInHits = fInHitArray->GetEntriesFast();
  Int_t nBefore,nIn,nAfter,nYZ;
  nBefore=nIn=nAfter=nYZ=0;
  TVector3 momentumSim(0.,0.,0.);
  Int_t trackID = 0;
  Int_t trackIDFirst = 0;
  Int_t chamber = 0;

  PndDchTrack *track = 0;

  Int_t nTracks = fTrackArray->GetEntriesFast();

  for(Int_t iTrack = 0; iTrack < nTracks; iTrack++) {
    track = (PndDchTrack*) fTrackArray->At(iTrack);
    Int_t iTrackHits = track->GetNofDchHits();
   
    for (Int_t iInHit=0; iInHit<iTrackHits; iInHit++) {
      inhit  = (PndDchHit*) fInHitArray->At(iInHit);
      if(iInHit==0)
	trackIDFirst = inhit->GetTrackID();
      trackID= inhit->GetTrackID();
      chamber = PndDchMapper::CalculateChamber(inhit->GetDetectorID());
      if(trackID==trackIDFirst){
	if (chamber == 3 || chamber == 4) {
	  //set starting value of momentum for fitting
	  point = (PndDchPoint*)fPointArray->At(inhit->GetRefIndex());
	  if(momentumSim.Mag()<10e-6)
	    if(0!=point)
	      point->Momentum(momentumSim);
	
	  TVector2 *pos = new((*fInHitsBeforeXZ)[nBefore]) TVector2(inhit->GetX(),inhit->GetZ());
	  nBefore++;
	  TVector2 *pos1 = new((*fInHitsYZ)[nYZ]) TVector2(inhit->GetY(),inhit->GetZ());
	  nYZ++;
	} else if (chamber == 5 || chamber == 6) {
	  TVector2 *pos = new((*fInHitsInXZ)[nIn]) TVector2(inhit->GetX(),inhit->GetZ());
	  nIn++;
	  TVector2 *pos1 = new((*fInHitsYZ)[nYZ]) TVector2(inhit->GetY(),inhit->GetZ());
	  nYZ++;
	} else if (chamber == 7 || chamber == 8) {
	  TVector2 *pos = new((*fInHitsAfterXZ)[nAfter]) TVector2(inhit->GetX(),inhit->GetZ());
	  nAfter++;
	  TVector2 *pos1 = new((*fInHitsYZ)[nYZ]) TVector2(inhit->GetY(),inhit->GetZ());
	  nYZ++;
	}
      }
    }
  }
  Bool_t isRecoTrackYZ  = GetLineParameters(fInHitsYZ,fTrackYZ);
  Bool_t isRecoTrackBeforeXZ = GetLineParameters(fInHitsBeforeXZ,fTrackBeforeXZ);
  if(isRecoTrackBeforeXZ){
    Double_t xIn = (fZFieldBegin-fTrackBeforeXZ.Y())/fTrackBeforeXZ.X();
    //artificial dch inhit for entering magnetic field
    TVector2 *pos = new((*fInHitsInXZ)[nIn]) TVector2(xIn,fZFieldBegin);
    nIn++;
  }
  Bool_t  isRecoTrackAfterXZ = GetLineParameters(fInHitsAfterXZ,fTrackAfterXZ);
  if(isRecoTrackAfterXZ){
    Double_t xOut = (fZFieldEnd-fTrackAfterXZ.Y())/fTrackAfterXZ.X();
    //artificial dch inhit for exiting the field
    TVector2 *pos = new((*fInHitsInXZ)[nIn]) TVector2(xOut,fZFieldEnd);
    nIn++;
  }
  Bool_t  isRecoTrackInXZ =  GetCircleParameters(fInHitsInXZ,fTrackInXZ);
  TVector3 momentum(0., 0., 0.);
  TVector3 position(0., 0., 0.);
  if(isRecoTrackYZ && isRecoTrackBeforeXZ && isRecoTrackInXZ && momentumSim.Mag()>10e-6){
    Bool_t isMomentum =  GetMomentum(momentum);
    //fill pull histos
    fPullPx->Fill(100*(momentum.X()-momentumSim.X())/momentumSim.X());
    fPullPy->Fill(100*(momentum.Y()-momentumSim.Y())/momentumSim.Y());
    fPullPz->Fill(100*(momentum.Z()-momentumSim.Z())/momentumSim.Z());
    fPullP->Fill(100*(momentum.Mag()-momentumSim.Mag())/momentumSim.Mag());
  }
  TVector3 positionSim(0.,0.,0.);  


  for(Int_t iTrack = 0; iTrack < nTracks; iTrack++) {
    track = (PndDchTrack*) fTrackArray->At(iTrack);
    Int_t iTrackHits = track->GetNofDchHits();
   
    for (Int_t iInHit=0; iInHit<iTrackHits; iInHit++) {
  
      inhit  = (PndDchHit*) fInHitArray->At(iInHit);
      chamber = PndDchMapper::CalculateChamber(inhit->GetDetectorID());
      if(chamber > 2 ){
	inhit->Position(positionSim);
	Bool_t isPosition =  GetInHitAtZ(inhit->GetZ(),position);
	if(isPosition){
	  fPullX->Fill(position.X()-positionSim.X());
	  fPullY->Fill(position.Y()-positionSim.Y());
	}
      }
    }
  }
  

  AddHits(trackIDFirst);
}

Int_t PndDchPreFitterTR::AddHits(Int_t trackID) {
  TClonesArray& clref = *fHitArray;
  Int_t size = clref.GetEntriesFast();
  TVector3 position(0.,0.,0.);  
  TVector3 momentum(0.,0.,0.);  
  for (Int_t i=0; i<fDetIdListSize; i++){
    Int_t detID =  fDetIdList->At(i);
    if(detID<3000)
      continue;
    const TGeoCombiTrans* trans = fStructure->GetTransMatrix(detID);
    const Double_t *translation = trans->GetTranslation();
    Bool_t isPosition =  GetInHitAtZ(translation[2],position); 
    Bool_t isMomentum =  GetMomentum(momentum);
    //   cout<<"i "<<i<<" isP "<<isMomentum<<" isX "<<isPosition<<endl;
    if(0 == trans || !isPosition)
      continue;  
    PndDchHit* hit = new(clref[size]) PndDchHit(trackID, detID, position, TVector3(0.2,0.2,0.2), 1 );
    //    hit->Print("");
    size++;
  }
  return size;
}

void PndDchPreFitterTR::Finish(){
  TFile* f = new TFile(fOutFile,"RECREATE");
  fPullPx->Write();
  fPullPy->Write();
  fPullPz->Write();
  fPullP->Write();
  fPullX->Write();
  fPullY->Write();
  f->Close();
  delete f;
}

Bool_t PndDchPreFitterTR::GetInHitAtZ(Double_t zpos, TVector3 &inhit){
  inhit.SetXYZ(0., 0., 0.);
  if(fTrackYZ.X()==0 ||  fTrackBeforeXZ.X()==0 || fTrackAfterXZ.X()==0)
    return kFALSE; 
  Double_t xpos=0;
  inhit.SetZ(zpos);
  inhit.SetY((zpos-fTrackYZ.Y())/fTrackYZ.X());
  //before dipole 
  if (zpos < fZFieldBegin) {
    xpos =(zpos-fTrackBeforeXZ.Y())/fTrackBeforeXZ.X();
  //in dipole
  } else if (zpos > fZFieldBegin && zpos < fZFieldEnd) {
    Double_t sgn=1;
    if(fTrackInXZ.Z()>zpos)
      sgn=-1;
    Double_t tmp=pow(fTrackInXZ.Z(),2)-pow(zpos-fTrackInXZ.Y(),2);
    if (tmp<0)
      return kFALSE;
    xpos=sgn*sqrt(tmp)+fTrackInXZ.X();
  //after dipole
  } else {
    xpos =(zpos-fTrackAfterXZ.Y())/fTrackAfterXZ.X();
  }
  inhit.SetX(xpos);

  // for debugin purpose  
  /*if (zpos > fZFieldBegin && zpos < 400.) {
    PndDchHit *pnt=0;
    Int_t i=1;
    do{
      pnt = (PndDchHit*) fInHitArray->At(i++);
      //} while (pnt->GetDetectorID()/1000 !=5 && i<fInHitArray->GetEntries());
    } while (fabs(pnt->GetZ()-zpos)>0.03 && i<fInHitArray->GetEntries());
    TVector3 pos;
    pnt->Position(pos);
    if(fabs(pos.Z()-zpos)<0.03){
      cout<<"IN DIPOLE"<<endl;
      cout<< "CROSSING INHIT REAL = ("<<pos.X()<<", "<<pos.Y()<<", "<<pos.Z()<<")"<<endl;
      cout<< "CROSSING INHIT Calculated = ("<<inhit.X()<<", "<<inhit.Y()<<", "<<inhit.Z()<<")"<<endl<<endl;
    }
    } */ 
  // end debuging
  
  return kTRUE;
}

Bool_t PndDchPreFitterTR::GetMomentum(TVector3& momentum ) {
  Double_t magXZ = 0.3*fField*fTrackInXZ(2)/100.;
 
  if(fTrackBeforeXZ.X()==0 || fTrackYZ.X()==0)
    return kFALSE;
  
  Double_t angleXZ = atan((fTrackBeforeXZ.X()));
  if(angleXZ<0)
    angleXZ+=TMath::Pi();
  Double_t pz = magXZ*sin(angleXZ);
  momentum.SetZ(pz);
  Double_t px = pz/fTrackBeforeXZ.X();
  momentum.SetX(px);
  Double_t py = pz/fTrackYZ.X();
  momentum.SetY(py);

  if(momentum.Mag()>10e-6)
    return kTRUE;
  else 
    return kFALSE;
}

Bool_t PndDchPreFitterTR::GetCircleParameters(TClonesArray *fInHits, TVector3& result) {
  Double_t n,x,y,w,xx,yy,xy,wx,wy,WG,WA,WB,WC;
  Double_t aa,bb,cc;
  x=y=xx=yy=xy=w=wx=wy=n=WG=WA=WB=WC=aa=bb=cc=0.0;
  
  TVector2* inhit  = 0;
  Int_t nInHits = fInHits->GetEntriesFast();
  if(nInHits<3)
    return kFALSE;
  //  cout<<"Circle: nInHits="<<nInHits<<endl;
  Double_t inhitTime;
  for (Int_t iInHit=0; iInHit<nInHits; iInHit++) {
    inhit  = (TVector2*) fInHits->At(iInHit);
    if ((inhit->X()!=0) && (inhit->Y()!=0)) {
      //  cout<<"Circle: x "<<inhit->X()<<" y "<<inhit->Y()<<endl;
      x  += inhit->X();
      y  += inhit->Y();
      xx += inhit->X()*inhit->X();
      yy += inhit->Y()*inhit->Y();
      xy += inhit->X()*inhit->Y();
      w  = xx + yy;
      wx += inhit->X()*inhit->X()*inhit->X()+inhit->Y()*inhit->Y()*inhit->X();
      wy += inhit->Y()*inhit->Y()*inhit->Y()+inhit->X()*inhit->X()*inhit->Y();
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
    //result = (X0, Z0, radius)
    result.SetXYZ(bb/2.,cc/2.,0.5*sqrt(dd)); 
  return kTRUE;
}



Bool_t PndDchPreFitterTR::GetLineParameters(TClonesArray *fInHits, TVector2& result) const {
  Double_t n,x,y,xx,xy,WG,WA,WB;
  x=y=xx=xy=n=WG=WA=WB=0.0;
  TVector2* inhit  = 0;
  Int_t nInHits = fInHits->GetEntriesFast();
  if(nInHits<2)
    return kFALSE;
  Double_t inhitTime;
  for (Int_t iInHit=0; iInHit<nInHits; iInHit++) {
    inhit  = (TVector2*) fInHits->At(iInHit);
    if ((inhit->X()!=0) && (inhit->Y()!=0)) {
      x  += inhit->X();
      y  += inhit->Y();
      xx += inhit->X()*inhit->X();
      xy += inhit->X()*inhit->Y();
      n++;
    }
  }
  WG = x*x - n*xx;
  WA = x*y - n*xy;
  WB = x*xy - xx*y;
  if (WG==0){
    return kFALSE;
  }
  //result = (slope, offset)
  result.Set(WA/WG,WB/WG);
  return kTRUE;
}



ClassImp(PndDchPreFitterTR)
