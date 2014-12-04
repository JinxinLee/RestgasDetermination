/** PndTrkGemHitList

 ** @author Lia Lavezzi 
 **/

#ifndef PNDTRKGEMHITLIST_H
#define PNDTRKGEMHITLIST_H 1

#include "PndTrkHitList.h"

#include "TVector3.h"
#include "PndTrkHit.h"
#include "PndGemHit.h"

#define MAXNOFGEMHITS 1000  // CHECK consistency

class PndTrkGemHitList : public PndTrkHitList
{

 public:    

  PndTrkGemHitList();
  PndTrkGemHitList(Int_t iregion);
  ~PndTrkGemHitList();    

  static  PndTrkGemHitList* Instance();
  PndTrkGemHitList* Instanciate();
  
  void AddHit(Int_t hitid, Int_t detid, FairHit *hit);
 
 protected:

  static PndTrkGemHitList * fgemInstance;

  Int_t fIRegion;
  ClassDef(PndTrkGemHitList,1);
};


#endif
