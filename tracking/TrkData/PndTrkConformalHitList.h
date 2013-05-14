/** PndTrkConformalHitList
 ** 
 ** @author Lia Lavezzi 
 **/

#ifndef PNDTRKCONFORMALHITLIST_H
#define PNDTRKCONFORMALHITLIST_H 1

#include "TVector3.h"
#include "PndTrkConformalHit.h"
#include "PndTrkConformalTransform.h"
#include "FairHit.h"

#include <iostream>
#include <cmath>
#include <vector>
#include <iterator>
#include <algorithm>

#define MAXNOFHITS 1000  // CHECK consistency

class PndTrkConformalHitList : public TObject
{

 public:    

  PndTrkConformalHitList();
  PndTrkConformalHitList(PndTrkConformalTransform *conformal);
 ~PndTrkConformalHitList();    

  // CHECK private?
  void AddHit(PndTrkConformalHit *chit); 
  PndTrkConformalHit *GetHit(int index);

  void Reset();

  void SetConformalTransform(PndTrkConformalTransform *conformal) { fConformal = conformal; }
  PndTrkConformalTransform  *GetConformalTransform() { return fConformal; }
  inline Int_t  GetNofHits() { return hitlist.size(); }
  
  void Print();
  void Draw(Color_t color = kBlack);
 
 protected:

  std::vector<PndTrkConformalHit *> hitlist;
  PndTrkConformalTransform *fConformal;

  ClassDef(PndTrkConformalHitList,1);
};


#endif
