/** PndTrkNeighboringMap
 ** 
 ** @author Lia Lavezzi 
 **/

#ifndef PNDTRKNEIGHBORINGMAP_H
#define PNDTRKNEIGHBORINGMAP_H 1

#include "TVector3.h"
#include "TMap.h"
#include "PndTrkHit.h"
#include "PndTrkHitList.h"
#include "FairHit.h"

#include "TObjArray.h"

class PndTrkNeighboringMap : public TObject
{
  
 public:    

  PndTrkNeighboringMap(TClonesArray *tubearray);
  PndTrkNeighboringMap(const PndTrkNeighboringMap& thismap);
  ~PndTrkNeighboringMap();    
  PndTrkNeighboringMap& operator=(const PndTrkNeighboringMap &thismap);


  
  void AddNeighboringsToHit(PndTrkHit *hit, TObjArray *hits);
  TObjArray * GetNeighboringsToHit(PndTrkHit *hit);
  void SetOwnerValue(Bool_t enable = kTRUE) { hit2neigh.SetOwnerValue(enable); }
  TMapIter *GetIterator();
  TObjArray GetStandalone() { return fStandalone ; } 
  TObjArray GetSeeds(); //  { return fSeeds; }
  TObjArray GetCandseeds();

 protected:

  TMap hit2neigh;
  TClonesArray *fTubeArray;
  TObjArray fStandalone, fSeeds, fCandseeds;


  ClassDef(PndTrkNeighboringMap,1);
};


#endif
