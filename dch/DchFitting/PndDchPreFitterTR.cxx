// -------------------------------------------------------------------------------
// -----                 PndDchPreFitterTR source file                       -----
// -----            Created 26.02.2008  by A. Wronska, P.Hawranek            -----
// -------------------------------------------------------------------------------


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

#include "FairRootManager.h"
#include "FairRuntimeDb.h"
#include "FairBaseParSet.h"
#include "FairRunAna.h"
#include "FairRun.h"
#include "FairTrackParam.h"

#include "PndDchPreFitterTR.h"
#include "PndDchHit.h"
#include "PndDchPoint.h"
#include "PndDchCylinderHit.h"
#include "PndTrackCand.h"
#include "PndTrackCandHit.h"
#include "PndDchStructure.h"
#include "PndDchMapper.h"

using std::cout;
using std::endl;

PndDchPreFitterTR::PndDchPreFitterTR() :
  FairTask("Dch Prefitter for Forward Spectrometer") { 
  fInHitsAfterXZ = 0;
  fInHitsInXZ = 0;
  fInHitsBeforeXZ = 0;
  fInHitsYZ = 0;
  fDetIdList = 0;
}

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
  FairRootManager* ioman = FairRootManager::Instance();
  TFile *infile = ioman->GetInFile();
  if ( ! ioman ) {
    cout << "-E- PndDchPreFitterTR::Init():\n\t "
	 << "RootManager not instantiated!" << endl;
    return kFATAL;
  }
 // TGeoManager *geoMan = (TGeoManager*) infile->Get("FAIRGeom");
	TGeoManager *geoMan =gGeoManager;
  fStructure = PndDchStructure::Instance(geoMan);

  // Get input point array - only to get real value of momentum!
  fPointArray = (TClonesArray*) ioman->GetObject("PndDchPoint");
  if ( ! fPointArray ) {
    cout << "-W- PndDchPreFitterTR::Init(): "
	 << "No input PndDchPoint array!" << endl;
    return kERROR;
  }
  // Get input hit array
  fInHitArray = (TClonesArray*) ioman->GetObject("PndDchCylinderHit");
  if ( ! fInHitArray ) {
    cout << "-W- PndDchPreFitterTR::Init(): "
	 << "No input PndDchCylinderHit array!" << endl;
    return kERROR;
  }

  
  fDetIdList =  fStructure->GetDetectorIDList();
  fDetIdListSize = fDetIdList->GetSize();

  // Get input track array
  fTrackArray = (TClonesArray*) ioman->GetObject("DCHTrackCand");
  if ( !fTrackArray ) {
    cout << "-W- PndDchPreFitterTR::Init(): No DCHTrackCand array!"
         << endl;
    return kERROR;
  }

  FairRunAna *fRun= FairRunAna::Instance();
  Double_t fBeamMom = 0.;
  if(fRun){
    FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
    FairBaseParSet* par=(FairBaseParSet*)
      (rtdb->findContainer("FairBaseParSet"));
    fBeamMom= par->GetBeamMom();
    std::cout<<" Beam momentum read out from DB = "<<fBeamMom<<std::endl;
  }
  else{
    std::cout<<"PndDchPreFitterTR: fBeamMom could not be found"<<std::endl;
    //fBeamMom = 15.;
    exit(2);
  }

  fZFieldBegin = 360.0;
  fZFieldEnd = 580.0;
  fField= 0.895 * fBeamMom / 15.;
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

void PndDchPreFitterTR::Exec(Option_t* opt) {
  cout<<"&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"<<endl;
  cout<<"&                                                                              &"<<endl;
  cout<<"&              Entering PndDchPreFitterTR::Exec(...)                           &"<<endl;
  cout<<"&                                                                              &"<<endl;
  cout<<"&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"<<endl;
  
  Int_t nBeforeXZ,nInXZ,nAfterXZ,nYZ;
  
  Int_t nTracks = fTrackArray->GetEntries();
  PndTrackCand* track = 0;
  for(Int_t iTrack = 0; iTrack < nTracks; iTrack++) {
    track = (PndTrackCand*) fTrackArray->At(iTrack);

    fInHitsBeforeXZ->Delete();
    fInHitsAfterXZ->Delete();
    fInHitsInXZ->Delete();
    fInHitsYZ->Delete();
    
    TVector2* tmp;
    std::map<Int_t, TVector3> mapa;
    mapa = GetHitPointsInChambers(track);
    
    std::map<Int_t,TVector3>::iterator iter; 
    if(fVerbose>0){
      cout<<"Cross Points in Chambers"<<endl;
      for( iter = mapa.begin(); iter != mapa.end(); ++iter ) {
	cout<<iter->first<<endl;
	iter->second.Print();
      }
      cout<<"Cross Points in Chambers END"<<endl;
    }

    nBeforeXZ =0;
    nAfterXZ =0;
    nInXZ =0;
    nYZ =0;

    if (mapa.find(3) != mapa.end()) {
      tmp =     new((*fInHitsBeforeXZ)[nBeforeXZ++]) TVector2(mapa.find(3)->second.X(),mapa.find(3)->second.Z());
      tmp =     new((*fInHitsYZ)[nYZ++]) TVector2(mapa.find(3)->second.Y(),mapa.find(3)->second.Z());
    }
    if (mapa.find(4) != mapa.end()) {
      tmp =     new((*fInHitsBeforeXZ)[nBeforeXZ++]) TVector2(mapa.find(4)->second.X(),mapa.find(4)->second.Z());
      tmp =     new((*fInHitsYZ)[nYZ++]) TVector2(mapa.find(4)->second.Y(),mapa.find(4)->second.Z());
    }
    if (mapa.find(5) != mapa.end()) {
      tmp =     new((*fInHitsInXZ)[nInXZ++]) TVector2(mapa.find(5)->second.X(),mapa.find(5)->second.Z());
      tmp =     new((*fInHitsYZ)[nYZ++]) TVector2(mapa.find(5)->second.Y(),mapa.find(5)->second.Z());
    }
    if (mapa.find(6) != mapa.end()) {
      tmp =     new((*fInHitsInXZ)[nInXZ++]) TVector2(mapa.find(6)->second.X(),mapa.find(6)->second.Z());
      tmp =     new((*fInHitsYZ)[nYZ++]) TVector2(mapa.find(6)->second.Y(),mapa.find(6)->second.Z());
    }
    if (mapa.find(7) != mapa.end()) {
      tmp =     new((*fInHitsAfterXZ)[nAfterXZ++]) TVector2(mapa.find(7)->second.X(),mapa.find(7)->second.Z());
//     tmp =     new((*fInHitsYZ)[nYZ++]) TVector2(mapa.find(7)->second.Y(),mapa.find(7)->second.Z());
    }
    if (mapa.find(8) != mapa.end()) {
      tmp =     new((*fInHitsAfterXZ)[nAfterXZ++]) TVector2(mapa.find(8)->second.X(),mapa.find(8)->second.Z());
//     tmp =     new((*fInHitsYZ)[nYZ++]) TVector2(mapa.find(8)->second.Y(),mapa.find(8)->second.Z());
    }       
    Bool_t isRecoTrackBeforeXZ = GetLineParameters(fInHitsBeforeXZ,fTrackBeforeXZ);
    if(isRecoTrackBeforeXZ){
      Double_t xIn = (fZFieldBegin-fTrackBeforeXZ.Y())/fTrackBeforeXZ.X();
      //artificial dch inhit for entering magnetic field
      TVector2 *pos = new((*fInHitsInXZ)[nInXZ++]) TVector2(xIn,fZFieldBegin);
    }
    
    Bool_t  isRecoTrackAfterXZ = GetLineParameters(fInHitsAfterXZ,fTrackAfterXZ);
    if(isRecoTrackAfterXZ){
      Double_t xOut = (fZFieldEnd-fTrackAfterXZ.Y())/fTrackAfterXZ.X();
      //artificial dch inhit for exiting the field
      TVector2 *pos = new((*fInHitsInXZ)[nInXZ++]) TVector2(xOut,fZFieldEnd);
    }
    Bool_t isRecoTrackYZ  = GetLineParameters(fInHitsYZ,fTrackYZ);

    Bool_t  isRecoTrackInXZ =  GetCircleParameters(fInHitsInXZ,fTrackInXZ);

    TVector3 startMomentum(0., 0., 0.);
    TVector3 startPosition(0., 0., 0.);
    if(!(isRecoTrackYZ && isRecoTrackBeforeXZ  && isRecoTrackInXZ) ){
      cout<<"Tracking failed (YZ BeforeXZ InXZ AfterXZ) "
	  <<isRecoTrackYZ << isRecoTrackBeforeXZ << isRecoTrackInXZ<<isRecoTrackAfterXZ<<endl;
      continue;
    }
    Bool_t isMomentum =  GetMomentum(startMomentum);


    PndTrackCandHit candHit = track->GetSortedHit(0);
    Int_t idx = candHit.GetHitId();
    PndDchCylinderHit* chit = (PndDchCylinderHit*)fInHitArray->At(idx);
    GetInHitAtZ(chit->GetWireZcoordGlobal(),startPosition);
    
    Int_t chargeSign = GetChargeSign();

    const TMatrixFSym* covMatrix = new TMatrixFSym(15);
    FairTrackParam parset(startPosition.X(),startPosition.Y(),startPosition.Z(),
			 startMomentum.X()/ startMomentum.Z(),
			 startMomentum.Y()/ startMomentum.Z(),
			 chargeSign/ startMomentum.Mag(),
			 *covMatrix);

//    track->SetParamFirst(parset);
    track->setTrackSeed(startPosition,
			TVector3(startMomentum.X()/ startMomentum.Z(),
				 startMomentum.Y()/ startMomentum.Z(),
				 1.),
			chargeSign/startMomentum.Mag());
  } //end of loop over tracks

  cout<<"&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"<<endl;
  cout<<"&                                                                              &"<<endl;
  cout<<"&              Exiting PndDchPreFitterTR::Exec(...)                           &"<<endl;
  cout<<"&                                                                              &"<<endl;
  cout<<"&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"<<endl;
} // end of Exec()

void PndDchPreFitterTR::Finish(){
}

Bool_t PndDchPreFitterTR::GetInHitAtZ(Double_t zpos, TVector3 &inhit){
  inhit.SetXYZ(0., 0., 0.);
  Double_t xpos=0;
  inhit.SetZ(zpos);
  inhit.SetY((zpos-fTrackYZ.Y())/fTrackYZ.X());
  //before dipole 
  if (zpos < fZFieldBegin && fTrackBeforeXZ.X()!=0 ) {
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
  } else if( fTrackAfterXZ.X()!=0){
    xpos =(zpos-fTrackAfterXZ.Y())/fTrackAfterXZ.X();
  } else
    return kFALSE;
  inhit.SetX(xpos);  
  return kTRUE;
}

Int_t PndDchPreFitterTR::GetChargeSign(){
  Double_t zReal = ((TVector2*)fInHitsInXZ->At( fInHitsInXZ->GetEntries()-1))->Y();
  Double_t xReal = ((TVector2*)fInHitsInXZ->At( fInHitsInXZ->GetEntries()-1))->X();
  Double_t xExtrapolated = (zReal-fTrackBeforeXZ.Y())/fTrackBeforeXZ.X();
  if(xReal>xExtrapolated)
    return -1;
  return 1;
}

Bool_t PndDchPreFitterTR::GetMomentum(TVector3& momentum ) {
  Double_t magXZ = 0.3*fField*fTrackInXZ(2)/100.;
  //  std::cout<<"magXZ= "<<magXZ<<" fField= "<<fField<<" fTrackInXZ(2)= "<<fTrackInXZ(2)<<std::endl;
 
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
  Int_t nInHits = fInHits->GetEntries();
  if(nInHits<3)
    return kFALSE;
  for (Int_t iInHit=0; iInHit<nInHits; iInHit++) {
    inhit  = (TVector2*) fInHits->At(iInHit);
    if ((inhit->X()!=0) && (inhit->Y()!=0)) {
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
  if(fVerbose>0) std::cout<<" fitted circle radius is "<< result.Z()<<std::endl;
  return kTRUE;
}

Bool_t PndDchPreFitterTR::GetLineParameters(TClonesArray *fInHits, TVector2& result) const {
  Double_t n,x,y,xx,xy,WG,WA,WB;
  x=y=xx=xy=n=WG=WA=WB=0.0;
  TVector2* inhit  = 0;
  Int_t nInHits = fInHits->GetEntries();
  if(nInHits<2) {
    cout<<" nHits="<<nInHits<<endl;
    return kFALSE;
  }
  if(nInHits==2){
    TVector2* p1,*p2;
    p1=(TVector2*)fInHits->At(0);
    p2=(TVector2*)fInHits->At(1);

    if(TMath::Abs(p2->X()-p1->X())<1e-10) {
      Error("PndDchPreFitterTR::GetLineParameters","2 points paralel with x1=%f and x2=%f",p2->X(),p1->X());
      return kFALSE;
    }
    Double_t slope = (p2->Y()-p1->Y())/(p2->X()-p1->X());
    Double_t offset = p2->Y()-slope*p2->X();
    result.Set(slope, offset);
    return kTRUE;
  }
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

Bool_t PndDchPreFitterTR::GetCrossPoint(PndDchCylinderHit *chit1, PndDchCylinderHit *chit2, TVector2 &point){

 TVector2 wire1End1 = chit1->GetWireEnd1();
 TVector2 wire1End2 = chit1->GetWireEnd2();
 TVector2 wire2End1 = chit2->GetWireEnd1();
 TVector2 wire2End2 = chit2->GetWireEnd2();
 TVector2 dir1 = (wire1End2-wire1End1).Unit();
 TVector2 dir2 = (wire2End2-wire2End1).Unit();
 if((dir1-dir2).Mod()<1e-6 || (dir1+dir2).Mod()<1e-6) {
   return kFALSE; //lines are parallel
 }
 if(TMath::Abs((dir1+dir2).X())<1e-6){
   //   cout<<"The same X"<<endl;
   Double_t x = (wire1End1+wire2End1).X();
   wire1End1.Set(x/2,wire1End1.Y());
   dir1.Set(0.,1.);
 }

 if(TMath::Abs((dir1+dir2).Y())<1e-6){
   //   cout<<"The same Y"<<endl;
   Double_t y = (wire1End1+wire2End1).X();
   wire1End1.Set(wire1End1.X(),y/2);
   dir1.Set(1.,0.);
 }

 Double_t b = - (dir1.X()*(wire1End1.Y()-wire2End1.Y())+dir1.Y()*(wire2End1.X()-wire1End1.X()))/(dir2.X()*dir1.Y()+dir2.Y()*dir1.X());

 point = wire2End1 + b*dir2;
   
 return kTRUE;

}

std::map<Int_t , TVector3> PndDchPreFitterTR::GetHitPointsInChambers(PndTrackCand* tr) {
  Int_t nHits = tr->GetNHits();
  
  std::map<Int_t , TVector3> chamberHitMap; // contains map chamberNo<->average of hit wires
  std::map<Int_t , Int_t> chamberCountsMap; // contains map chamber No<->number of hits 
  std::map<Int_t,TVector3>::iterator iter; 
  std::map<Int_t,Int_t>::iterator iterCounts; 
  Int_t ch1, ch2;
  
  for(Int_t i = 0; i<nHits; i++){
    PndTrackCandHit candHit1 = tr->GetSortedHit(i);
    Int_t idx1 = candHit1.GetHitId();
    PndDchCylinderHit* chit1 = (PndDchCylinderHit*)fInHitArray->At(idx1);
    ch1 = fStructure->InWhichChamber(chit1->GetWireZcoordGlobal());
    
    for(Int_t j = i; j<nHits; j++){
      PndTrackCandHit candHit2 = tr->GetSortedHit(j);
      Int_t idx2 = candHit2.GetHitId();
      PndDchCylinderHit* chit2 = (PndDchCylinderHit*)fInHitArray->At(idx2);
      ch2 = fStructure->InWhichChamber(chit2->GetWireZcoordGlobal());
      
      if(ch1 != ch2)
	continue;
      
      TVector2 wi1, wi2, point;
      if (GetCrossPoint(chit1,chit2,point)) {
	TVector3 point3d(point.X(), point.Y(), 
			 (chit1->GetWireZcoordGlobal()+chit2->GetWireZcoordGlobal())/2.);
	iter = chamberHitMap.find(ch1);
	if( iter == chamberHitMap.end() ) {
	  chamberHitMap[ch1] = point3d; 
	} else {
	  point3d = point3d + iter->second;
	  chamberHitMap[ch1] = point3d; 
	}
	++chamberCountsMap[ch1];
      }
    }
  }
  for( iter = chamberHitMap.begin(); iter != chamberHitMap.end(); ++iter ) {
    iterCounts = chamberCountsMap.find(iter->first);
    iter->second = iter->second *(1./(Double_t)iterCounts->second);
  }
  return chamberHitMap;
}



ClassImp(PndDchPreFitterTR)
