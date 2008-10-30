//-----------------------
// Pnd headers
//-----------------------
#include "PndDchStructure.h"
#include "PndDchPoint.h"

//-----------------------
// ROOT headers
//-----------------------
#include "TGeoMatrix.h"
#include "TGeoManager.h"
#include "TVector3.h"		
#include "TRotation.h"
#include "TMath.h"		
#include "TString.h"		
#include "TArrayI.h"

//---------------
// C++ Headers --
//---------------
#include "stdlib.h"
#include <iostream>
using std::cout;
using std::endl;

PndDchStructure* PndDchStructure::fgInstance = 0;

PndDchStructure::~PndDchStructure(){
  if(0!=fDetectorIdList)
    delete fDetectorIdList;
}
		
PndDchStructure* PndDchStructure::Instance (TGeoManager *geoMan) {
  if (fgInstance == 0) {
    if (geoMan==0){
      cout<<"Empty geometry passed to PndDchStructure"<<endl;
      abort(); 
    }
    else
      fgInstance = new PndDchStructure(geoMan);
  }
  return fgInstance;
}

PndDchStructure* PndDchStructure::Instance () {
  // If PndDchStructure is called without input parameters
  // it means that it should have been instantiated with TGeoManager before
  if (fgInstance == 0) {
    cout<<"Wrong call for PndDchStructure. It should be instantiated first with TGeoManager"<<endl;
    abort();
  }
  else
    return fgInstance;
}

PndDchStructure::PndDchStructure(TGeoManager *geoMan) {
  
  TString nodePath;
  TGeoCombiTrans* planeMatrix;
  Double_t xlo, xup, cellSize;
  Int_t detID;
  Int_t maxDetID = 0;
  
  TGeoIterator next(geoMan->GetTopVolume());
  TGeoNode *node;
  while ((node=next())) {
    next.GetPath(nodePath);
    
    bool isDchSplane=nodePath.Contains("dchSplane");
    if (!isDchSplane) continue;
    
    detID = GetDetectorID(nodePath); 
    if(detID>maxDetID) maxDetID = detID;
    
    planeMatrix = new TGeoCombiTrans(*next.GetCurrentMatrix());

    fPlaneGeoTransMap[detID] = planeMatrix;
    Double_t dx = node->GetVolume()->GetShape()->GetAxisRange(1,xlo,xup)/2.;
    Double_t dy = node->GetVolume()->GetShape()->GetAxisRange(2,xlo,xup)/2.;
    Double_t dz = node->GetVolume()->GetShape()->GetAxisRange(3,xlo,xup)/2.;
    fPlaneHalfSizeMap[detID] = TVector3(dx,dy,dz);
  }

  fNuOfChambers = PndDchMapper::CalculateChamber(detID);

  if(!SetWireOrientation())
    cout<<"PndDchStructure::PndDchStructure(...): \n \t Failed in SetWireOrientation()\n";

  fDetectorIdList = NULL;
  InitDetectorIDList();
}

Double_t PndDchStructure::GetCellSize(Int_t detID) const {
  // returns cell size for a sensitive plane with detectorID = detID
  std::map<Int_t, TVector3>::const_iterator kIt;
  if((kIt = fPlaneHalfSizeMap.find(detID)) == fPlaneHalfSizeMap.end())
    return 0;
  else 
      return (kIt->second).Z()*2.;
}

const TVector3 PndDchStructure::GetPlaneHalfSizes(Int_t detID) const{
  std::map<Int_t, TVector3>::const_iterator kIt;
  if((kIt = fPlaneHalfSizeMap.find(detID)) == fPlaneHalfSizeMap.end())
    return TVector3(0,0,0);
  else 
    return kIt->second;
}

const TGeoCombiTrans* PndDchStructure::GetTransMatrix(Int_t detID) const {
  // returns transformation matrix for a sensitive plane with detectorID = detID
  std::map<Int_t, TGeoCombiTrans*>::const_iterator kIt;
  if((kIt = fPlaneGeoTransMap.find(detID)) ==  fPlaneGeoTransMap.end())
    return 0;
  else
    return kIt->second;
}

TArrayI* PndDchStructure::GetDetectorIDList() {
  if(0==fDetectorIdList)
    std::cout<<"PndDchStructure: fDchDetectorList not initialised!"<<std::endl; 
  return fDetectorIdList;
}

Int_t PndDchStructure::InitDetectorIDList() {
  if(0 != fDetectorIdList){
    std::cout<<"PndDchStructure::InitDetectorIDList():"<<std::endl<<
      "   fDetectorIdList was already initialised!!!"<<std::endl;
    std::cout<< " and has size of "<<fDetectorIdList->GetSize()<<std::endl;
  }
  Int_t size=0;
  fDetectorIdList = new TArrayI();
  std::map<Int_t, TGeoCombiTrans*>::const_iterator kIt;
  for (kIt = fPlaneGeoTransMap.begin(); kIt != fPlaneGeoTransMap.end();kIt++ ){
    Int_t detID = kIt->first;
    fDetectorIdList->Set(size+1);
    fDetectorIdList->AddAt(detID,size);
    size++;
  }

  std::cout<<"Content of DchIDList:"<<std::endl;
  for(Int_t i=0; i<fDetectorIdList->GetSize(); i++)
    std::cout<<i<<"\t"<< fDetectorIdList->At(i)<<std::endl;
  std::cout<<std::endl;
  
  return fDetectorIdList->GetSize();
}
Int_t PndDchStructure::GetDetectorID(TString nodePath) {
  // The following code extract information about detID from the path name 
	
  TString tmp = "";
  Int_t ch, pl;
  ch = pl = 0;
  tmp = nodePath(nodePath.Sizeof()-2,1);
  pl    = tmp.Atoi();
  tmp = nodePath(nodePath.Sizeof()-4,1);
  ch    = tmp.Atoi();
  
  Int_t detID = PndDchMapper::CalculateDetectorID(ch, pl);
  
  return detID;
}

Bool_t PndDchStructure::SetWireOrientation(void){
  // adds rotation of wires w.r.t. the y-axis to 
  // each transformation in fPlaneGeoTransMap
  Bool_t result = kTRUE;
  
  std::map<Int_t, TGeoCombiTrans*>::const_iterator kIterator;
  Int_t detID, plane, chamber;
  Double_t wireangle;
  for (kIterator = fPlaneGeoTransMap.begin(); kIterator != fPlaneGeoTransMap.end();kIterator++ ){
    detID = plane = chamber = 0;
    wireangle = 0.;
    TGeoCombiTrans *trans = kIterator->second;
    detID   = kIterator->first;
    plane   = PndDchMapper::CalculatePlane(detID);
    chamber = PndDchMapper::CalculateChamber(detID);
    if (chamber>2){
      if(plane==1 || plane==2)	    wireangle =   0.;
      else if(plane==3 || plane==4) wireangle =  30.;
      else if(plane==5 || plane==6) wireangle = -30.;
      else {result=kFALSE;}
    } else if (chamber==2){
      if(plane==1 || plane==2)      wireangle =   0.;
      else if(plane==3 || plane==4) wireangle = -45.;
      else if(plane==5 || plane==6) wireangle =  45.;
      else if(plane==7 || plane==8) wireangle =  90.;
      else {result=kFALSE;}
    } else if (chamber==1){
      if(plane==1)                  wireangle =   0.;
      else if(plane==2)             wireangle =  90.;
      else {result=kFALSE;}
    } 
    else {result=kFALSE;}
    trans->RotateZ(wireangle);
  }
  return result;
}


// -----   Public method InWhichChamber   -------------------------------------------
const Int_t PndDchStructure::InWhichChamber(Double_t globalZ) const{
  std::map<Int_t, TGeoCombiTrans*>::const_iterator kIterator;
  Int_t detID;
  for (kIterator = fPlaneGeoTransMap.begin(); kIterator != fPlaneGeoTransMap.end();kIterator++ ){
    detID = 0;
    TGeoCombiTrans *combiTrans = kIterator->second;
    const Double_t* trans = combiTrans->GetTranslation();
    detID   = kIterator->first;
    TVector3 hs =  GetPlaneHalfSizes(detID);
    if (TMath::Abs(globalZ - trans[2] ) < hs.Z()) {
      return PndDchMapper::CalculateChamber(detID);
    }
  }
  return -1;
}
// -----   Public method Print   -------------------------------------------
void PndDchStructure::Print(const Option_t*) const {

  cout << "-I- PndDchStructure: " << endl;
  std::map<Int_t, TVector3>::const_iterator kIt2 = fPlaneHalfSizeMap.begin();    
  while (kIt2 != fPlaneHalfSizeMap.end()) {
    cout << "DetID "<<(*kIt2).first<<" CellSize " << (*kIt2).second.Z()*2. << endl;
    kIt2++;
  }
  std::map<Int_t, TGeoCombiTrans*>::const_iterator kIt3;
  for (kIt3 = fPlaneGeoTransMap.begin(); kIt3 != fPlaneGeoTransMap.end();kIt3++ ){
    TGeoCombiTrans *trans = kIt3->second;
    cout << "DetID "<<(*kIt3).first<<" PlaneGeoTrans "<<trans<<endl;
    trans->Print();
   
  }
}
// -------------------------------------------------------------------------





ClassImp(PndDchStructure)
