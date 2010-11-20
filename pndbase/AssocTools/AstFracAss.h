//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: AstFracAss.hh,v 1.1.1.1 2005/03/29 16:49:05 steinke Exp $
//
// Description:
//	Class AstFracAss.  Defines a fractional association between 2 objects
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//     David Brown
//
// Copyright Information:
//	Copyright (C) 2000		Lawrence Berkeley Laboratory
//
// Bertram Kopf (RUB)  migrated to PandaRoot
//------------------------------------------------------------------------

#ifndef ASTFRACASS_HH
#define ASTFRACASS_HH

#include "AstFracElem.h"
#include "AstAbsAssociation.h"

template <class A,class B> class AstSTLFracMap;

template <class A,class B> 
class AstFracAss : public AstFracElem<B>, 
		   public AstAbsAssociation<A,B> {

public:

// construct give the weight and the things being associated

  AstFracAss();
  AstFracAss(const A*,const B*,double weight);
  AstFracAss(const AstFracAss& other);

  virtual ~AstFracAss();

  // accessors
  virtual const A* firstArgument() const;
  virtual const B* secondArgument() const;


  // Assignment op
  AstFracAss& operator = (const AstFracAss& other);

  // allow arithmetic operations (on the weights!)
  AstFracAss& operator += (const AstFracAss&);
  AstFracAss& operator -= (const AstFracAss&);



  // operators needed for RW sorting

  bool operator < (const AstFracAss& other) const;
  bool operator > (const AstFracAss& other) const;
  bool operator == (const AstFracAss& other) const;
  bool operator != (const AstFracAss& other) const;

protected:

private:
  
  void rescale(double factor) { AstFracElem<B>::rescale(factor); }

  friend class AstSTLFracMap<A,B>;

  const A* _aelem; // association element

};

#include  "AstFracAss.cxx"

#endif

