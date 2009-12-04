#ifndef PNDMDTHIT_HH
#define PNDMDTHIT_HH

#include "FairHit.h"
#include "TVector3.h"
#include "TMath.h"

class PndMdtHit : public FairHit {

 public:
 
  PndMdtHit();
  
  PndMdtHit (Int_t detID, TVector3& pos, TVector3& dpos, Int_t index);
  
  virtual ~PndMdtHit();
  
  /** Output to screen (not yet implemented) **/
  virtual void Print(const Option_t* opt = 0) const {;}
  
  
  /** Public method Clear
   ** Resets the flag to -1
   **/
  //void Clear();  // not implemented

  /** Accessors **/
  Short_t GetModule()      const { return (fDetectorID/1000000);};
  Short_t GetSector()      const { return ((fDetectorID/100000)%10);};
  Short_t GetLayerID()     const { return ((fDetectorID/1000)%100);};
  Short_t GetBox()         const { return ((fDetectorID/10)%100);};
  Short_t GetWire()        const { return (fDetectorID%10);};

 private:
  
  ClassDef(PndMdtHit,1);
};


#endif
