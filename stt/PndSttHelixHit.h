// ======================================
//
// hits recostructed via HELIX FIT in STT
//
// ======================================
#ifndef PNDSTTHELIXHIT_H
#define PNDSTTHELIXHIT_H 1


#include "TVector3.h"
#include "CbmHit.h"

class PndSttHit;

class PndSttHelixHit : public CbmHit 
{

 public:    

  /** Default constructor **/
  PndSttHelixHit();

  /** Standard constructor 
  *@param detID     Detector unique volume ID
  *@param pos       Position coordinates X Y Z [cm]
  *@param dpos      Errors in position coordinates [cm]
  *@param mcindex   Index of corresponding MCPoint
  *@param hitindex  Index of corresponding CbmHit
  *@param edep      Deposited energy

  **/
  
  PndSttHelixHit(Int_t detID, TVector3& pos, TVector3& dpos,
		 Int_t mcindex, Int_t hitindex, Double_t edep);

  PndSttHelixHit(Int_t detID, 
		 TVector3& pos, TVector3& dpos, 
		 Int_t mcindex, Int_t hitindex, Double_t edep,
		 TVector3& cpos, TVector3& dcpos, 
		 Double_t isochrone,
		 Double_t isochroneError, TVector3 wireDir);

  void CopyHitToHelixHit(PndSttHit *aHit, Int_t hitindex);


/** Destructor **/
  virtual ~PndSttHelixHit();    

  /** Output to screen **/
  virtual void Print(const Option_t* opt = 0) const {;}

  /** Public method Clear  **/
  void Clear();

  /** Accessors **/
  Int_t GetHitIndex() {return fHitIndex;}

  /** Accessors **/
  Double_t GetIsochrone() const { return fIsochrone; }; 
  Double_t GetIsochroneError() const { return fIsochroneError; }; 
  TVector3 GetWireDirection() const { return fWireDirection; };
  Double_t GetXcen()             const { return fXcen;       };
  Double_t GetYcen()             const { return fYcen;       };
  Double_t GetZcen()             const { return fZcen;       };
  Double_t GetDXcen()             const { return fDxcen;       };
  Double_t GetDYcen()             const { return fDycen;       };
  Double_t GetDZcen()             const { return fDzcen;       };

  Double_t GetHalfLength()       const { return fhl;                       };

  /** Modifiers **/
  void SetHitIndex(Int_t hitindex) {fHitIndex = hitindex;} 
  void SetIsochrone(Double_t isochrone) { fIsochrone = isochrone; };
  void SetIsochroneError(Double_t isochroneError) { fIsochroneError = isochroneError; };
  void SetWireDirection(TVector3 newWire) { fWireDirection = newWire; }

  void SetXcen(Double_t x) { fXcen = x; }
  void SetYcen(Double_t y) { fYcen = y; }
  void SetZcen(Double_t z) { fZcen = z; }
 
  void SetDXcen(Double_t dx) { fDxcen = dx; }
  void SetDYcen(Double_t dy) { fDycen = dy; }
  void SetDZcen(Double_t dz) { fDzcen = dz; }
 
  void Print();

 protected:
 
  /** This variable contains the hit index **/    
  Int_t fHitIndex;
  /** This variable contains the energy **/
  Double_t fEdep;
  /** Position of the center **/ 
  Double32_t fXcen, fYcen, fZcen; 
  Double32_t fDxcen, fDycen, fDzcen;
  /** Wire direction **/
  TVector3 fWireDirection;
  /** isochrone **/
  Double_t fIsochrone, fIsochroneError;
  /** half length **/
  Double_t fhl;

  ClassDef(PndSttHelixHit,1);
};


#endif
