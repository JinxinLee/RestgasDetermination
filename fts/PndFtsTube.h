#ifndef PNDFTSTUBE_H
#define PNDFTSTUBE_H 1

#include "TVector3.h"
#include "TMatrixT.h"
#include "TObject.h"

class PndFtsTube : public TObject
{

 public:

  /** Default constructor **/
  PndFtsTube();
  PndFtsTube(PndFtsTube &tube);

  ~PndFtsTube();
  PndFtsTube(Double_t x, Double_t y, Double_t z,
             Double_t r11, Double_t r12, Double_t r13,
             Double_t r21, Double_t r22, Double_t r23,
             Double_t r31, Double_t r32, Double_t r33,
             Double_t radin, Double_t radout, Double_t hl);


  TVector3 GetPosition();
  TMatrixT<Double_t> GetRotationMatrix();
  Double_t GetRadIn();
  Double_t GetRadOut();
  Double_t GetHalfLength();
  TVector3 GetWireDirection();

  bool IsSkew() {
    return (0.!=GetWireDirection().X()) || (0.!=GetWireDirection().Y()); // TODO CHECK IF THIS IS CORRECT
  }

 private:

  TVector3 fCenPosition;
  TMatrixT<double> fRotationMatrix;
  Double_t fRadIn, fRadOut, fHalfLength;


  ClassDef(PndFtsTube,1);
};


#endif
