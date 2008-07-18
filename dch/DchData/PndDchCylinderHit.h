// -------------------------------------------------------------------------
// -----                  PndDchCylinderHit header file                -----
// -----                Created 20.03.2008  by A. Wronska              -----
// -------------------------------------------------------------------------

/** \class PndDchCylinderHit
 *  \author A.Wronska <wronska@if.uj.edu.pl>
 *  \date 20.03.2008
 *  \brief Class for spatial dch hit description as a cylinder
 **/

#ifndef PNDDCHCYLINDERHIT_H
#define PNDDCHCYLINDERHIT_H 1

// Pnd includes

// ROOT includes
#include "TObject.h"

class PndDchDigi;

class PndDchCylinderHit : public TObject{
 public:
  /** Default constructor **/
  PndDchCylinderHit();

  /** Constructor **/
  PndDchCylinderHit(Int_t digiidx, Double_t xLoc, Double_t zGlo,
		    Double_t dist, Double_t distErr, Double_t alpha);
  
  /** Destructor **/
  virtual ~PndDchCylinderHit();
  
  /** Public method Print **/
  void Print();
  
  /** Accessors  **/
  Int_t    GetDigiIndex()        const {return fDigiIndex;}  
  Double_t GetWireXcoordLocal()  const {return fWireXcoordLocal;}
  Double_t GetWireZcoordGlobal() const {return fWireZcoordGlobal;}
  Double_t GetDistance()         const {return fDistance;}
  Double_t GetDistanceError()    const {return fDistanceError;}
  Double_t GetWireAngle()        const {return fAlpha;}
  
  /** Modifiers  **/
  
 private:
  Int_t    fDigiIndex;         ///< index of mother digi in the the global array
  Double_t fWireXcoordLocal;   ///< x coordinate (cm) of the fired wire in the local (plane) ref.frame
  Double_t fWireZcoordGlobal;  ///< z coordinate (cm) of the fired wire in the global ref.frame
  Double_t fDistance;          ///< distance from the wire (cm) calculated from drift time (=radius)
  Double_t fDistanceError;     ///< uncertainty (=resolution) of distance from the wire (cm) 
  Double_t fAlpha;             ///< inclination angle (rad) of the wire w.r.t the y axis (y:fAlpha=0)
  
  ClassDef(PndDchCylinderHit,1);
  
};

#endif

