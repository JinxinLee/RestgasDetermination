/*
 *====================================================================
 *
 *  CBM KF Track Quality
 *  
 *  Authors: M.Zyzak
 *
 *  e-mail : 
 *
 *====================================================================
 *
 *  KF Fit performance 
 *
 *====================================================================
 */

#ifndef _PndCATrackMCPointContainer_h_
#define _PndCATrackMCPointContainer_h_

#include <FairMCPoint.h>

#include "TClonesArray.h"
#include <vector>

#include "PndSttPoint.h"
#include "PndSdsMCPoint.h"

using std::vector;

class PndCATrackMCPointContainer : public TObject {

 public:

  PndCATrackMCPointContainer();
  ~PndCATrackMCPointContainer() {};

  PndSdsMCPoint* GetMvdPoint(Int_t i) {return MvdArray[i];}
  PndSttPoint* GetSttPoint(Int_t i) {return SttArray[i];}

  void SetMvdPoint(PndSdsMCPoint* mp) { MvdArray.push_back(mp); }
  void SetSttPoint(PndSttPoint* sp) { SttArray.push_back(sp); }

  int GetNMvdPoints() const { return MvdArray.size(); }
  int GetNSttPoints() const { return SttArray.size(); }

  
  double GetMvdPointX(int i) {return MvdArray[i]->FairMCPoint::GetX();}
  double GetMvdPointY(int i) {return MvdArray[i]->FairMCPoint::GetY();}
  double GetMvdPointZ(int i) {return MvdArray[i]->GetZ();}
  double GetMvdPointPx(int i) {return MvdArray[i]->GetPx();}
  double GetMvdPointPy(int i) {return MvdArray[i]->GetPy();}
  double GetMvdPointPz(int i) {return MvdArray[i]->GetPz();}

  double GetSttPointX(int i) {return SttArray[i]->FairMCPoint::GetX();}
  double GetSttPointY(int i) {return SttArray[i]->FairMCPoint::GetY();}
  double GetSttPointZ(int i) {return SttArray[i]->GetZ();}
  double GetSttPointPx(int i) {return SttArray[i]->GetPx();}
  double GetSttPointPy(int i) {return SttArray[i]->GetPy();}
  double GetSttPointPz(int i) {return SttArray[i]->GetPz();}

  vector<PndSttPoint*> SttArray;
  vector<PndSdsMCPoint*> MvdArray;

  ClassDef(PndCATrackMCPointContainer,1);
};

#endif // _PndCATrackMCPointContainer_h_ 
