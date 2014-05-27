#include "PndSttTube.h"

#include <iostream>
#include <math.h>
#include "TVector3.h"

PndSttTube::PndSttTube()
  :fCenPosition(TVector3(0,0,0)),
   fRotationMatrix(TMatrixT<double>(3,3)),
   fTubeParms(new PndSttTubeParameters()),
   fRadIn(-1),
   fRadOut(-1),
   fSectorID(-1),
   fLayerID(-1),
   fLayerLimit(kFALSE),
   fSectorLimit(0)
{
  fRotationMatrix[0][0] = -1.;
  fRotationMatrix[0][1] = -1.;
  fRotationMatrix[0][2] = -1.;
  
  fRotationMatrix[1][0] = -1.;
  fRotationMatrix[1][1] = -1.;
  fRotationMatrix[1][2] = -1.;
  
  fRotationMatrix[2][0] = -1.;
  fRotationMatrix[2][1] = -1.;
  fRotationMatrix[2][2] = -1.;
}

PndSttTube::PndSttTube(PndSttTube &tube)
  :fCenPosition(tube.GetPosition()),
   fRotationMatrix(TMatrixT<double>(3,3)),
   fTubeParms(tube.GetTubeParameters()),
   fRadIn(tube.GetRadIn()),
   fRadOut(tube.GetRadOut()),
   fSectorID(tube.GetSectorID()),
   fLayerID(tube.GetLayerID()),
   fLayerLimit(tube.IsLayerLimit()),
   fSectorLimit(tube.IsSectorLimit())
{ 
  fRotationMatrix.ResizeTo(3,3);
  fRotationMatrix = tube.GetRotationMatrix();
}

PndSttTube::PndSttTube(PndSttTubeParameters *parms,
		       Double_t x, Double_t y, Double_t z,
		       Double_t r11, Double_t r12, Double_t r13,
		       Double_t r21, Double_t r22, Double_t r23,
		       Double_t r31, Double_t r32, Double_t r33,
		       Double_t radin, Double_t radout)
  :fCenPosition(TVector3(x,y,z)),
   fRotationMatrix(TMatrixT<double>(3,3)),
   fTubeParms(parms),
   fRadIn(radin),
   fRadOut(radout),
   fSectorID(-1),
   fLayerID(-1),
   fLayerLimit(kFALSE),
   fSectorLimit(0)
{

  //  fCenPosition.SetXYZ(x,y,z);

  fRotationMatrix.ResizeTo(3,3);
  fRotationMatrix[0][0] = r11;
  fRotationMatrix[0][1] = r12;
  fRotationMatrix[0][2] = r13;

  fRotationMatrix[1][0] = r21;
  fRotationMatrix[1][1] = r22;
  fRotationMatrix[1][2] = r23;

  fRotationMatrix[2][0] = r31;
  fRotationMatrix[2][1] = r32;
  fRotationMatrix[2][2] = r33;

}


PndSttTube::~PndSttTube(){
  fCenPosition.Delete();
  fRotationMatrix.Delete();
}

TVector3 PndSttTube::GetPosition(){
  return fCenPosition; }

TMatrixT<Double_t> PndSttTube::GetRotationMatrix() {
  return fRotationMatrix; }

Double_t PndSttTube::GetRadIn() { 
  return fRadIn; }

Double_t PndSttTube::GetRadOut() { 
  return fRadOut; }

Double_t PndSttTube::GetHalfLength() { 
  return fTubeParms->GetHalfLength();
}

Int_t PndSttTube::GetTubeID() {
  return fTubeParms->GetTubeID();
}

TVector3 PndSttTube::GetWireDirection(){
  return TVector3(fRotationMatrix[0][2], 
		  fRotationMatrix[1][2], 
		  fRotationMatrix[2][2]);
}

PndSttTubeParameters *PndSttTube::GetTubeParameters() {
  return fTubeParms;
}

void PndSttTube::SetSectorID(int id){
  fSectorID = id;
}
void PndSttTube::SetLayerID(int id){
  fLayerID = id;
}

int PndSttTube::GetSectorID() {
  return fSectorID;
}

int PndSttTube::GetLayerID() {
  return fLayerID;
}


void PndSttTube::SetNeighborings(TArrayI neighborings) {
  fNeighborings = neighborings;
}

TArrayI PndSttTube::GetNeighborings() {
  return  fNeighborings;
}

Int_t PndSttTube::GetNeighboring(int i) {
  return  fNeighborings.At(i);
}

Bool_t PndSttTube::IsNeighboring(int tubeID) {
  for(int itube = 0; itube < fNeighborings.GetSize(); itube++) {
    if(GetNeighboring(itube) == tubeID) return kTRUE;
  }
  return kFALSE;
}

// CHECK this can be written in a better way
Double_t PndSttTube::GetDistance(PndSttTube *tube) {
  
 
  TVector3 extr1 = fCenPosition - GetHalfLength() * TVector3(fRotationMatrix[0][2], fRotationMatrix[1][2], fRotationMatrix[2][2]);
  TVector3 extr2 = fCenPosition + GetHalfLength() * TVector3(fRotationMatrix[0][2], fRotationMatrix[1][2], fRotationMatrix[2][2]);
  TVector3 tubeextr1 = tube->GetPosition() - tube->GetHalfLength() * tube->GetWireDirection();
  TVector3 tubeextr2 = tube->GetPosition() + tube->GetHalfLength() * tube->GetWireDirection();
  
  double distance = -1;
  if(TVector3(fRotationMatrix[0][2], fRotationMatrix[1][2], fRotationMatrix[2][2]).Cross(tube->GetWireDirection()).Mag() == 0)  {
    distance = ((fCenPosition - tubeextr1).Cross(fCenPosition - tubeextr2)).Mag()/(tubeextr2 - tubeextr1).Mag();
    return distance;    
  }
  else {
    distance = fabs((tubeextr1 - extr1).Dot((extr2  - extr1).Cross(tubeextr2 - tubeextr1)))/((extr2  - extr1).Cross(tubeextr2 - tubeextr1)).Mag();

    double s  = (( (extr2 - extr1).Dot(tubeextr2 - tubeextr1)) * ( (tubeextr2 - tubeextr1).Dot(extr1 - tubeextr1)) - ( (tubeextr2 - tubeextr1).Dot(tubeextr2 - tubeextr1)) * (  (extr2 - extr1).Dot(extr1 - tubeextr1))) / (((extr2 - extr1).Dot(extr2 - extr1)) * ((tubeextr2 - tubeextr1).Dot(tubeextr2 - tubeextr1)) - ( (extr2 - extr1).Dot(tubeextr2 - tubeextr1)) *( (extr2 - extr1).Dot(tubeextr2 - tubeextr1)) );
    double t = (( (extr2 - extr1).Dot(extr2 - extr1)) * ( (tubeextr2 - tubeextr1).Dot(extr1 - tubeextr1)) - ( (extr2 - extr1).Dot(tubeextr2 - tubeextr1)) * ( (extr2 - extr1).Dot(extr1 - tubeextr1)) )/ (((extr2 - extr1).Dot(extr2 - extr1)) * ((tubeextr2 - tubeextr1).Dot(tubeextr2 - tubeextr1)) - ( (extr2 - extr1).Dot(tubeextr2 - tubeextr1)) *( (extr2 - extr1).Dot(tubeextr2 - tubeextr1)) );
    
    TVector3 point1 = extr1 + s * (extr2 - extr1);
    TVector3 point2 = tubeextr1 +  t * (tubeextr2 - tubeextr1);
    
    //    std::cout << "GET DISTANCE " << distance << " " << (point1 - point2).Mag() << std::endl;
    //     point1.Print();
    //     point2.Print();
    
    if(point1.Z() < extr1.Z() || point1.Z() > extr2.Z()|| point2.Z() < tubeextr1.Z() || point2.Z() > tubeextr2.Z()) {
      //   std::cout << "point outside limits" << std::endl;
      distance =  (extr1 - tubeextr1).Mag() < (extr2 - tubeextr2).Mag() ? (extr1 - tubeextr1).Mag() : (extr2 - tubeextr2).Mag();

    }
    //  std::cout << "DISTANCE " << distance << std::endl;
    return distance;
  }
  
}




ClassImp(PndSttTube)
    
