#include "PndSttTube.h"

#include <iostream>

PndSttTube::PndSttTube()
 :fCenPosition(TVector3(0,0,0)),
  fRotationMatrix(TMatrixT<double>(3,3)),
  fRadIn(0),
  fRadOut(0),
  fHalfLength(0)
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
   fRadIn(tube.GetRadIn()),
   fRadOut(tube.GetRadOut()),
   fHalfLength(tube.GetHalfLength())
{ 
  fRotationMatrix.ResizeTo(3,3);
  fRotationMatrix = tube.GetRotationMatrix();
}

PndSttTube::PndSttTube(Double_t x, Double_t y, Double_t z,
		       Double_t r11, Double_t r12, Double_t r13,
		       Double_t r21, Double_t r22, Double_t r23,
		       Double_t r31, Double_t r32, Double_t r33,
		       Double_t radin, Double_t radout, Double_t hl) 
 :fCenPosition(TVector3(0,0,0)),
  fRotationMatrix(TMatrixT<double>(3,3)),
  fRadIn(0),
  fRadOut(0),
  fHalfLength(0)
{

  fCenPosition.SetXYZ(x,y,z);

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
  return fHalfLength; }

TVector3 PndSttTube::GetWireDirection(){
  return TVector3(fRotationMatrix[0][2], 
		  fRotationMatrix[1][2], 
		  fRotationMatrix[2][2]);}


ClassImp(PndSttTube)
    
