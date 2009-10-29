//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Abstract Field Class for Panda TPC
//
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Cristoforo Simonetto   TUM          (original author)
//      Felix Boehmer          TUM
//
//-----------------------------------------------------------

#ifndef TPCABSFIELD_H
#define TPCABSFIELD_H

#include "TVector3.h"
#include <ostream>

template<class t>

class PndTpcAbsField {

public:

  //Constructor ----------------------------------------------
 
  PndTpcAbsField(const t& nominalValue,const TVector3& relPos=TVector3(0,0,0));

  virtual ~PndTpcAbsField() {;}

  //Operators ------------------------------------------------

  template <class typeOstr>
  friend std::ostream& operator<<(std::ostream&, const PndTpcAbsField<typeOstr>&);


  //Accessors ------------------------------------------------

  t nominal() const {return fnominal;}
  TVector3 relPosition() const {return frelPosition;}
  double scale() const {return fscale;}
  virtual void print(std::ostream&) const;

  // the following functions are special cases. Use only on the appropriate 
  // fields. The types of the fields need to be convertible to meet the 
  // functions'expectations
 
  TVector3 grad(const TVector3&, const double step = 0.01) const;

  double divergence(const TVector3&, const double step = 0.01) const;

  TVector3 curl(const TVector3&, const double step = 0.01) const;

  // this is the derived classes' job:

  // return the field value (t) at the given position (multiply with fscale)
  virtual t value(const TVector3&) const = 0;
  
  // check if the queried point lies within the the field boundaries:
  virtual bool pointOk(const TVector3&) const = 0;

  // derivative of the field in a given direction. 
  virtual t derivative(const TVector3&, TVector3&) const;  //currently not used

  
  // Modifiers ---------------------------------------------

  void setNominal(const t& Nominal) {fnominal = Nominal;}
  void setRelPosition(const TVector3& relPos) {frelPosition = relPos;}
  void setScale(const double Scale) {fscale = Scale;}

protected:

  TVector3 frelPosition;
  t fnominal;
  double fscale;

private:

};

#endif




  
