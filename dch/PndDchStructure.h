// -------------------------------------------------------------------------
// -----                PndDchStructure header file                    -----
// -----                Created 31/07/07  by A. Wronska                -----
// -------------------------------------------------------------------------

/** \class PndDchStructure
 *  \author A.Wronska<aleksandra.wronska@uj.edu.pl>
 *  \date 31/07/07
 *  \brief Class for keeping info about the geometry of dch
 * 
 *  Class for keeping information about the geometry of dch 
 *  To be used in digi and reco procedures
 **/

#ifndef PNDDCHSTRUCTURE_H
#define PNDDCHSTRUCTURE_H

// c++ includes
#include <map>
#include <string>		
// ROOT includes
#include "TObject.h"
#include "TString.h"

class TGeoManager;
class TGeoCombiTrans;
class TArrayI;

class PndDchStructure: public TObject
{

 public:
  /** Public method Instance()
   ** for access to a singleton object
   **/
  static PndDchStructure* Instance();
  
  /** Public method Instance(...)
   ** for access to a singleton object for the first time
   **/
  static PndDchStructure* Instance(TGeoManager*);

  /** Destructor **/
  virtual ~PndDchStructure();
  
  /** Public method GetTransMatrix(...) **/
  const TGeoCombiTrans* GetTransMatrix(Int_t detID) const;  

  /** Public method GetDetectorIDList(...) **/
  TArrayI*              GetDetectorIDList(); 

  /** Public method GetCellSize(...) **/
  Double_t              GetCellSize(Int_t detID) const;  
  
  /** Public method Print(...)
   ** provides output to screen
   **/
  virtual void Print(const Option_t*) const;
  
 protected:
  /** Constructor of the singleton object **/
  PndDchStructure(TGeoManager*);		
  
 private:
  static PndDchStructure* fgInstance; ///< pointer to the singleton object
  TArrayI* fDetectorIdList;           ///< list of all detIDs of dch
  std::map<Int_t , TGeoCombiTrans* > fPlaneGeoTransMap;///< map detID to det geo tranformation
  std::map<Int_t , Double_t  > fCellSizeMap;   ///< map detID to det cell size

  
  
  /** Private method GetDetectorID(...) **/
  Int_t  GetDetectorID(TString nodePath); 

  /** Private method InitDetectorIDList() **/
  Int_t  InitDetectorIDList(); 

  /** Private method SetWireOrientation() **/
  Bool_t SetWireOrientation(void );
  
  ClassDef(PndDchStructure,1)
};

#endif  // PNDDCHSTRUCTURE_HH
