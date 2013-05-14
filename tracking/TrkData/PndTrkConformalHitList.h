/** PndTrkConformalHitList
 ** 
 ** @author Lia Lavezzi 
 **/

#ifndef PNDTRKCONFORMALHITLIST_H
#define PNDTRKCONFORMALHITLIST_H 1

#include "TVector3.h"
#include "PndTrkConformalHit.h"
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
  PndTrkConformalHitList(double x, double y, double delta);
 ~PndTrkConformalHitList();    

  // CHECK private?
  void AddHit(PndTrkConformalHit *chit); 
  PndTrkConformalHit *GetHit(int index);

  void ResetTo(double x, double y, double delta);

  void SetTranslation(double x, double y);
  void SetRotation(double delta); 

  inline Int_t  GetNofHits() { return hitlist.size(); }
  
  void Print();
  void Draw(Color_t color = kBlack);

 protected:

  std::vector<PndTrkConformalHit *> hitlist;
  TVector2 fTrasl;
  Double_t fAngle;

  ClassDef(PndTrkConformalHitList,1);
};


#endif
