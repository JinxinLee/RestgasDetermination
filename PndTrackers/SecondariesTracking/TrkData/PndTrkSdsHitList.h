/** PndTrkSdsHitList

 ** @author Lia Lavezzi 
 **/

#ifndef PNDTRKSDSHITLIST_H
#define PNDTRKSDSHITLIST_H 1

#include "PndTrkHitList.h"

#include "TVector3.h"
#include "PndTrkHit.h"
#include "PndSdsHit.h"

#define MAXNOFSDSHITS 1000  // CHECK consistency

class PndTrkSdsHitList : public PndTrkHitList
{

 public:    

  PndTrkSdsHitList();
  PndTrkSdsHitList(Int_t iregion);
  ~PndTrkSdsHitList();    

  static  PndTrkSdsHitList* Instance();
  PndTrkSdsHitList* Instanciate();
  static PndTrkSdsHitList* InstancePixel();
  PndTrkSdsHitList* InstanciatePixel();
  static PndTrkSdsHitList* InstanceStrip();
  PndTrkSdsHitList* InstanciateStrip();

  void AddHit(Int_t hitid, Int_t detid, FairHit *hit);

  void SetRegion(Int_t reg) { fIRegion = reg; }

  void Clear() {
    ClearList();
    fpixInstance = NULL;
    fstrInstance = NULL;
  }

 protected:

  static PndTrkSdsHitList * fpixInstance;
  static PndTrkSdsHitList * fstrInstance;

  Int_t fIRegion;
  ClassDef(PndTrkSdsHitList,1);
};


#endif
