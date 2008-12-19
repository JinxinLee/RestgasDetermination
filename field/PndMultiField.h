// -------------------------------------------------------------------------
// -----                      PndMultiField header file                  -----
// -----                 Created 29/01/07  by M. Al/Turany               -----
// -------------------------------------------------------------------------
/** PndMultiField.h
 ** @author M.Al/Turany <m.al-turany@gsi.de>
  ** @since 29.01.2007
 ** @version1.0
 **
 ** Multiple Magnetic field maps 
 **/


#ifndef PNDMULTIMAP_H
#define PNDMULTIMAP_H 1

#include "CbmField.h"
#include "TObjArray.h"
#include <map>

class PndRegion;
class PndMultiFieldPar;
class PndMultiField : public CbmField {

public:

  /** Default constructor **/
  PndMultiField();

  /** Constructor from PndFieldPar **/
  PndMultiField(PndMultiFieldPar* fieldPar);

  /** Destructor **/
  virtual ~PndMultiField();

  /** Initialisation (read map from file) **/
  void Init();

  /**Adding a field to the collection*/

  void AddField(CbmField *field);
  TObjArray *GetFieldList(){return fMaps; }

  /** Get the field components at a certain point 
   ** @param x,y,z     Point coordinates (global) [cm]
   ** @value Bx,By,Bz  Field components [kG]
   **/
/*  virtual Double_t GetBx(Double_t x, Double_t y, Double_t z);
  virtual Double_t GetBy(Double_t x, Double_t y, Double_t z);
  virtual Double_t GetBz(Double_t x, Double_t y, Double_t z);
*/
  /** Screen output **/
  virtual void Print();
  ClassDef(PndMultiField,1) 

 /** Get magnetic field. For use of GEANT3
   ** @param point            Coordinates [cm]
   ** @param bField (return)  Field components [kG] 
   **/
  void GetFieldValue(const Double_t point[3], Double_t* bField);

 protected:
   
  TObjArray *fMaps;
  Int_t     fNoOfMaps;
  std::map <PndRegion*, CbmField* > fFieldMaps;//! 
  std::map <PndRegion*, CbmField* >::iterator fMapIter; //!

};

#endif



