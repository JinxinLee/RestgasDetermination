#ifndef _PndCATrackFtsMCPointContainer_h_
#define _PndCATrackFtsMCPointContainer_h_

#include <FairMCPoint.h>

#include "TClonesArray.h"
#include <vector>

#include "PndFtsPoint.h"

using std::vector;

class PndCATrackFtsMCPointContainer : public TObject {

 public:

  PndCATrackFtsMCPointContainer();
  ~PndCATrackFtsMCPointContainer() {};

  PndFtsPoint* GetFtsPoint(Int_t i) {return FtsArray[i];}
  
  void SetFtsPoint(PndFtsPoint* mp) { FtsArray.push_back(mp); }
  
  int GetNFtsPoints() const { return FtsArray.size(); }
    
  double GetFtsPointX(int i) {return FtsArray[i]->FairMCPoint::GetX();}
  double GetFtsPointY(int i) {return FtsArray[i]->FairMCPoint::GetY();}
  double GetFtsPointZ(int i) {return FtsArray[i]->GetZ();}
  double GetFtsPointPx(int i) {return FtsArray[i]->GetPx();}
  double GetFtsPointPy(int i) {return FtsArray[i]->GetPy();}
  double GetFtsPointPz(int i) {return FtsArray[i]->GetPz();}

  vector<PndFtsPoint*> FtsArray;

  ClassDef(PndCATrackFtsMCPointContainer,1);
};

#endif // _PndCATrackMCPointContainer_h_ 
