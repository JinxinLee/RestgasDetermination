/** PndTrkSttHitList

** @author Lia Lavezzi 
**/

#ifndef PNDTRKSTTHITLIST_H
#define PNDTRKSTTHITLIST_H 1

#include "PndTrkHitList.h"

#include "TVector3.h"
#include "PndTrkHit.h"
#include "PndSttHit.h"

#define MAXNOFSTTHITS 1000  // CHECK consistency

class PndTrkSttHitList : public PndTrkHitList
{

 public:    

  PndTrkSttHitList();
  PndTrkSttHitList(TClonesArray *tubearray);
   ~PndTrkSttHitList();    

  static PndTrkSttHitList * Instance();
  PndTrkSttHitList* Instanciate();

 void AddHit(Int_t hitid, Int_t detid, FairHit *hit);


 protected:
  static PndTrkSttHitList * fInstance;
  TClonesArray *fTubeArray;
  ClassDef(PndTrkSttHitList,1);
};


#endif
