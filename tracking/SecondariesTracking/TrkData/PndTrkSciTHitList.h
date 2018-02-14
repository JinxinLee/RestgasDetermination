/** PndTrkSciTHitList

 ** @author Lia Lavezzi 
 **/

#ifndef PNDTRKSCITHITLIST_H
#define PNDTRKSCITHITLIST_H 1

#include "PndTrkHitList.h"

#include "TVector3.h"
#include "PndTrkHit.h"

#define MAXNOFSCITHITS 1000  // CHECK consistency

class PndTrkSciTHitList : public PndTrkHitList
{

 public:    

  PndTrkSciTHitList();
  PndTrkSciTHitList(Int_t iregion);
  ~PndTrkSciTHitList();    

  static  PndTrkSciTHitList* Instance();
  PndTrkSciTHitList* Instanciate();
  
  void AddHit(Int_t hitid, Int_t detid, FairHit *hit);
 
  void Clear() {
    ClearList();
    fscitInstance = NULL;
  }

 protected:

  static PndTrkSciTHitList * fscitInstance;

  Int_t fIRegion;
  ClassDef(PndTrkSciTHitList,1);
};


#endif
