#ifndef PNDMDTTRK_HH
#define PNDMDTTRK_HH

#include "FairHit.h"
#include "TVector3.h"

#include <iostream>

using std::cout;
using std::endl;

class PndMdtTrk : public TObject {

 public:
 
  PndMdtTrk();
  
   virtual ~PndMdtTrk();
  
  /** Output to screen (not yet implemented) **/
  virtual void Print(const Option_t* opt = 0) const {;}
  
  
  /** Public method Clear
   ** Resets the flag to -1
   **/
  void Clear();

  /** Accessors **/
  void GetHitList(Int_t *hit)            { for (Int_t ii=0; ii<15; ii++) hit[ii] = fHitList[ii]; };
  void GetHitMult(Int_t *hit)            { for (Int_t ii=0; ii<15; ii++) hit[ii] = fHitMult[ii]; };
  void GetHitAngle(Float_t *hit)         { for (Int_t ii=0; ii<15; ii++) hit[ii] = fHitAngle[ii]; };
  Int_t   GetHitNumber(Int_t lay)  const { return fHitList[lay]; };
  Int_t   GetHitMult  (Int_t lay)  const { return fHitMult[lay]; };
  Float_t GetHitAngle (Int_t lay)  const { return fHitAngle[lay]; };
  Int_t GetHitCount()              const { return fHitCount; }; 
  Int_t GetLayerCount()            const { return fLayerCount; };
  Int_t GetHitBit()                const { return fHitBit; }; 
  Int_t GetHitBit(Int_t lay)       const { return (fHitBit & (1 << lay)); };
  Int_t GetModule()                const { return fModule;}; 
  Float_t GetChi2()                const { return fChi2;};
 
  /** Modifiers **/
  void SetHitNumber(Int_t lay, Int_t trackId); 
  void SetHitMult  (Int_t lay, Int_t mult);
  void SetHitAngle (Int_t lay, Float_t angle);
  void SetBit      (Int_t lay)    { fHitBit = fHitBit | (1 << lay); };

  void SetHitCount(Int_t hit)     { fHitCount = hit; }; 
  void SetLayerCount(Int_t lay)   { fLayerCount = lay; };
  void SetHitBit  (Int_t bit)     { fHitBit = bit;   };
  void SetModule  (Int_t mod)     { fModule = mod;   };
  void SetChi2    (Float_t chi2)  { fChi2 = chi2;   };
  

 private:
  
  Int_t fHitList[15];      // List of indexes of MdtHit
  Int_t fHitMult[15];      // Number of MdtHits inside the correlation
  Float_t fHitAngle[15];   // Distance of the closest point to the projected one
  Int_t fHitCount;         // Number of hits inside correlation
  Int_t fLayerCount;       // Number of fired layers
  Int_t fHitBit;           // Layer Bit
  Int_t fModule;           // Module number of the first hit
  Float_t fChi2;           // global chi2 of the MDT correlation

  ClassDef(PndMdtTrk,1);
};

inline void PndMdtTrk::SetHitNumber(Int_t lay, Int_t trackId)  
{ 
  if (lay>15) 
    {
      cout << " -E- PndMdtTrk::SetHitNumber: Layer > 15 !!!!!" << endl;
    }
  else
    {
      fHitList[lay] = trackId;
      SetBit(lay); 
    }
}

inline void PndMdtTrk::SetHitMult(Int_t lay, Int_t mult)  
{ 
  if (lay>15) 
    {
      cout << " -E- PndMdtTrk::SetHitMult: Layer > 15 !!!!!" << endl;
    }
  else
    {
      fHitMult[lay] = mult;
    }
}

inline void PndMdtTrk::SetHitAngle(Int_t lay, Float_t angle)  
{ 
  if (lay>15) 
    {
      cout << " -E- PndMdtTrk::SetHitAngle: Layer > 15 !!!!!" << endl;
    }
  else
    {
      fHitAngle[lay] = angle;
    }
}

#endif
