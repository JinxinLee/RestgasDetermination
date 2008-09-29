// -------------------------------------------------------------------------
// -----                PndDchMapper header file                       -----
// -----                Created 01/08/07  by A. Wronska                -----
// -------------------------------------------------------------------------

/** \class PndDchMapper
 *  \author A.Wronska <aleksandra.wronska@uj.edu.pl>
 *  \date 01/08/07
 *  \brief Class mapping positions (x,y,z) to set of dch data
 * 
 *  Class mapping positions (x,y,z) to set of dch data
 *  (like chamber, plane, wire, distance form wire) 
 *  and vice versa
 **/

#ifndef PNDDCHMAPPER_H
#define PNDDCHMAPPER_H

// ROOT includes
#include "TObject.h"
// c++ includes
#include <vector>

class PndDchPoint;
class PndDchStructure;

class PndDchMapper : public TObject {

public:

  /** Public method Instance()
   ** for access to a singleton object
   **/
  static PndDchMapper* Instance();
  
  /** Destructor **/
  ~PndDchMapper();
  
  /** Public method CalculateDetectorID(...) **/
  static Int_t CalculateDetectorID(Int_t ch, Int_t pl) {
    return fgkOffset*ch+pl;
  }

  /** Public method CalculatePlane(...) **/
  static Int_t CalculatePlane(Int_t detID) {
    return detID%fgkOffset;
  }
  
  /** Public method CalculateChamber(...) **/
  static Int_t CalculateChamber(Int_t detID) {
    return detID/fgkOffset;
  }
  
  /** Public method WhichWire(...)
   ** calculates number of nearest wire
   **/
  Int_t WhichWire(Int_t detID, Double_t xLocal) const;

  /** Public method GetFiredWires(...)
   ** determines wires that fired for a given MCpoint
   **/
  Int_t GetFiredWires(std::vector<Int_t> &wireID,
		      std::vector<Double_t> &distance, const PndDchPoint* point) const;

  /** Public method TransforToLocal(...)
   ** transforms position and momentum vectors
   **/
  Bool_t TransformToLocal(Double_t* localPosition, Double_t* localMomentum,
			  const PndDchPoint* point) const;

  /** Public method WirePosXlocal(...)
   ** gives local wire X coordinate in the plane
   **/
  Double_t WirePosXlocal(Int_t wireNo, Int_t detID) const;
  void Print(const Option_t*) const;
  
 protected:
  /** Constructor of the singleton object **/
  PndDchMapper();

  static const Int_t fgkOffset = 1000; ///< parameter used to build detector ID
  
 private:
  PndDchStructure *fDchStructure;        ///< pointer to structure of dch setup
  static PndDchMapper* fgMapperInstance; ///< pointer to the singleton object
  
  ClassDef(PndDchMapper, 1);
  
};

#endif
