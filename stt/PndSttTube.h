#ifndef PNDSTTTUBE_H
#define PNDSTTTUBE_H 1

#include "TVector3.h"
#include "TMatrixT.h"
#include "TObject.h" 
#include "TArrayI.h"
#include "PndSttTubeParameters.h"

class PndSttTubeParameters;
class PndSttTube : public TObject
{

 public:

  /** Default constructor **/
  PndSttTube();
  PndSttTube(PndSttTube &tube);

  ~PndSttTube();
  PndSttTube(PndSttTubeParameters *parms,
	     Double_t x, Double_t y, Double_t z,
	     Double_t r11, Double_t r12, Double_t r13,
	     Double_t r21, Double_t r22, Double_t r23,
	     Double_t r31, Double_t r32, Double_t r33,
	     Double_t radin, Double_t radout);


  TVector3 GetPosition();
  TMatrixT<Double_t> GetRotationMatrix();
  Double_t GetRadIn();
  Double_t GetRadOut();
  Double_t GetHalfLength();
  TVector3 GetWireDirection();
  PndSttTubeParameters *GetTubeParameters();
  Int_t GetTubeID();
  void SetSectorID(int id);
  void SetLayerID(int id);
  int GetLayerID();
  int GetSectorID();
  void SetNeighborings(TArrayI neighborings);
  TArrayI GetNeighborings();
  Int_t GetNeighboring(int i);
  Double_t GetDistance(PndSttTube *tube);
  Bool_t IsNeighboring(int tubeID);

  void SetSectorLimitFlag(Bool_t flag = kTRUE) { fSectorLimit = flag; }
  void SetLayerLimitFlag(Bool_t flag = kTRUE) { fLayerLimit = flag; }
  bool IsLayerLimit() { return fLayerLimit; }
  int IsSectorLimit()  { return fSectorLimit; }
  
  bool IsParallel() {
    return (GetWireDirection().Z() == 1);
  }

  bool IsSkew() {
    return !(GetWireDirection().Z() == 1);
  } 
 
 private:
  PndSttTubeParameters *fTubeParms;
  TVector3 fCenPosition;
  TMatrixT<double> fRotationMatrix;
  Double_t fRadIn, fRadOut;
  Int_t fSectorID, fLayerID;
  TArrayI fNeighborings;
  Bool_t fLayerLimit, fSectorLimit;
 
  ClassDef(PndSttTube,1);
};


#endif
