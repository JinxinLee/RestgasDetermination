#ifndef PNDMDTHIT_HH
#define PNDMDTHIT_HH

#include "CbmHit.h"
#include "TVector3.h"
#include "TMath.h"

class PndMdtHit : public CbmHit {

 public:
 
  PndMdtHit();
  
  PndMdtHit (Int_t detID, TVector3& pos, TVector3& dpos, Int_t index);
  
  virtual ~PndMdtHit();
  
  /** Output to screen (not yet implemented) **/
  virtual void Print(const Option_t* opt = 0) const {;}
  
  
  /** Public method Clear
   ** Resets the flag to -1
   **/
  void Clear();

  /** Accessors **/
  Int_t GetLayerID()             const { return fDetectorID<50 ? (Int_t)((fDetectorID-1)/2) : fDetectorID-51;};
  Int_t GetModule()              const { return fDetectorID<50 ? 1 : 2; };
  inline Int_t GetSector(); 

 private:
  
  ClassDef(PndMdtHit,1);
};

inline Int_t PndMdtHit::GetSector()
{
  if (GetModule()==2) return -1;
  
  TVector3 pos(0.,0.,0.);
  Position(pos);
  Int_t sec = int((pos.Phi()*TMath::RadToDeg()+180+22.5)/45);
  if (sec==8) sec = 0;
  
  return sec;
}
#endif
