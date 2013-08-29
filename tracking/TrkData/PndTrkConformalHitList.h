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

#include "TObjArray.h"

#include <iostream>
/* #include <cmath> */
/* #include <vector> */
/* #include <iterator> */
/* #include <algorithm> */

#define MAXNOFHITS 1000  // CHECK consistency

//class TObjArray;
class PndTrkConformalHitList : public TObject
{

 public:    

  PndTrkConformalHitList();
  PndTrkConformalHitList(PndTrkConformalTransform *conformal);
  PndTrkConformalHitList(const PndTrkConformalHitList& hlist);
  
  ~PndTrkConformalHitList();    
  PndTrkConformalHitList& operator=(const PndTrkConformalHitList& hlist);



 
  // CHECK private?
  void AddHit(PndTrkConformalHit *chit); 
  PndTrkConformalHit *GetHit(int index);

  void Reset();

  void SetConformalTransform(PndTrkConformalTransform *conformal) { fConformal = conformal; }
  PndTrkConformalTransform  *GetConformalTransform() { return fConformal; }
  inline Int_t  GetNofHits() { return hitlist.GetEntriesFast(); }
  
  void Print();
  void Draw(Color_t color = kBlack);
 
 protected:

  /*   std::vector<PndTrkConformalHit *> hitlist; */
  TObjArray hitlist;
  PndTrkConformalTransform *fConformal;
  
  ClassDef(PndTrkConformalHitList,1);
};


#endif
