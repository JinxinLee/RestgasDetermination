//----------------------------------------------------------
//
//------------------

#include "TGeoMatrix.h"

#include "FairRootManager.h"

#include "PndDchMapper.h"
#include "PndDchDrifter.h"
#include "PndDchPoint.h"
#include "PndDchStructure.h"

#include <iostream>
#include <vector>
//#include <cmath>
//#include <ctgmath>
#include "math.h"

using std::cout;
using std::endl;
//using std::round;
//using std::pow;
//using std::sqrt;

PndDchMapper* PndDchMapper::fgMapperInstance = 0;

PndDchMapper* PndDchMapper::Instance() {
  if (0 == fgMapperInstance)
    fgMapperInstance = new PndDchMapper();
  return fgMapperInstance;
}

PndDchMapper::PndDchMapper() {
  // Geometry loading
  FairRootManager* ioman = FairRootManager::Instance();
  TFile *infile = ioman->GetInFile();
  TGeoManager *geoMan = (TGeoManager*) infile->Get("FAIRGeom");
  fDchStructure = PndDchStructure::Instance(geoMan);
}

PndDchMapper::~PndDchMapper() {} 

Int_t PndDchMapper::WhichWire(Int_t detID, Double_t xLocal) const {
  Int_t plane  = CalculatePlane(detID);
  Int_t numWire =(Int_t) round(xLocal/fDchStructure->GetCellSize(detID));
  if(plane%2==1)
    numWire=(Int_t) round(xLocal/fDchStructure->GetCellSize(detID)+0.5);
  return numWire;
}

Double_t PndDchMapper::WirePosXlocal(Int_t wire, Int_t detID) const {
  Int_t plane  = CalculatePlane(detID);
  Double_t cellSize =  fDchStructure->GetCellSize(detID);
  Double_t xLocal = cellSize*wire;
  if( plane%2 == 1 )
    xLocal -= 0.5*cellSize;
  return xLocal;
}

Bool_t PndDchMapper::TransformToLocal(Double_t* localPosition, 
				      Double_t* localMomentum, 
				      const PndDchPoint* point) const {
  Int_t detID = point->GetDetectorID();
  Double_t globalPosition[3] = {point->GetX(),point->GetY(),point->GetZ()};
  Double_t globalMomentum[3] = {point->GetPx(),point->GetPy(),point->GetPz()};
  const TGeoCombiTrans* trans = fDchStructure->GetTransMatrix(detID);
  if(0 == trans)
    return kFALSE;
  trans->MasterToLocal(globalPosition, localPosition);
  trans->MasterToLocalVect(globalMomentum, localMomentum);
  //  std::cout<<"ch = "<<point->GetChamber()<<"\t pl= "<<point->GetPlane()<<std::endl;
  // TVector3 glo(globalPosition);
  // TVector3 loc(localPosition);
  //  std::cout<<"global pos =  ";
  // glo.Print();
  // std::cout<<"local pos =  ";
  // loc.Print();
  return kTRUE;
}

Bool_t PndDchMapper::TransformToGlobal(Double_t* globalPosition, Double_t* globalWireDirection, 
				       const Double_t localX, const Int_t detID ) const {
  Double_t localPosition[3] = {localX,0,0};
  Double_t localWireDirection[3] = {0,1,0};
  const TGeoCombiTrans* trans = fDchStructure->GetTransMatrix(detID);
  if(0 == trans)
    return kFALSE;
  trans->LocalToMaster(localPosition, globalPosition);
  trans->LocalToMasterVect(localWireDirection,globalWireDirection);
  return kTRUE;
}


Int_t PndDchMapper::GetFiredWires(std::vector<Int_t> &wireID,
				   std::vector<Double_t> &distance,
				   const PndDchPoint* point) const {
  Int_t detID = point->GetDetectorID();
  Int_t plane = CalculatePlane(detID);
  Double_t cellSize = fDchStructure->GetCellSize(detID);
  Double_t localPosition[3];
  Double_t localMomentum[3];
  if(!TransformToLocal(localPosition,localMomentum,point)){
    cout<<" PndDchMapper::GetFiredWires(...)\n\t No transformation matrix found for this point!\n"; 
    return 0;
  }
  
  //ph todo !!!!
  if (localMomentum[0] == 0 || localMomentum[2] == 0) { 
    Int_t numWire = WhichWire(detID,localPosition[0]);
    Double_t xWire = numWire * cellSize - 0.5*(plane%2)*cellSize;
    Double_t d=sqrt(pow((localPosition[0]-xWire),2)+pow((localPosition[2]),2));
    wireID.push_back(numWire);
    distance.push_back(d);
  } else {
    Double_t a = localMomentum[2]/localMomentum[0];
    Double_t b = localPosition[2]-a*localPosition[0];
    // calculate first wire
    Int_t numWireFirst = WhichWire(detID,localPosition[0]);
    Double_t zOut = cellSize/2;
    Double_t xOut = (zOut-b)/a;
    //calculate last wire;
    Int_t numWireLast = WhichWire(detID,xOut);
    if (numWireLast < numWireFirst) {
      Int_t numTemp = numWireFirst;
      numWireFirst = numWireLast;
      numWireLast = numTemp;
    }
    for (Int_t numWire = numWireFirst; numWire <= numWireLast; numWire++) {
      Double_t xWire = numWire * cellSize - 0.5*(plane%2)*cellSize;
      Double_t c = xWire/a;
      Double_t xCross = (c-b)/(a+1./a);
      Double_t zCross = a*xCross+b;
      Double_t d = sqrt(pow((xCross-xWire),2)+pow((zCross),2));
      wireID.push_back(numWire);
      distance.push_back(d);
      //cout<<"localPosition = ("<<localPosition[0]<<" , "<<localPosition[1]<<" , "<<localPosition[2]<<")"<<endl;
      if (d > 0.5*cellSize*sqrt(2.)){
        //cout<< "PndDchMapper::GetFiredWires(...):\n\t Distance from wire too large!\n";
	return 0;
      }
    }
  }
  return wireID.size();
}


void PndDchMapper::Print(const Option_t*) const {
  // fDchPoInt_t->Print();
}
