/*******************************************************************************
 * Project: BaBar detector at the SLAC PEP-II B-factory
 * Package: EvtGenBase
 *    File: $Id: EvtPropagator.hh,v 1.3 2003/06/20 17:20:11 dvoretsk Exp $
 *  Author: Alexei Dvoretskii, dvoretsk@slac.stanford.edu, 2001-2002
 *
 * Copyright (C) 2002 Caltech
 *******************************************************************************/

// Defines propagator as a function of mass and width

#ifndef EVT_PROPAGATOR_HH
#define EVT_PROPAGATOR_HH

#include <assert.h>
#include "EvtGenBase/EvtComplex.hh"
#include "EvtGenBase/EvtAmplitude.hh"
#include "EvtGenBase/EvtPoint1D.hh"

class EvtPropagator : public EvtAmplitude<EvtPoint1D> {
public:
  
  EvtPropagator(double fm0, double fg0)
    : _m0(fm0), _g0(fg0)
  {
    assert(fm0 > 0);
    assert(fg0 >= 0);
  }
  EvtPropagator(const EvtPropagator& other)
    : _m0(other._m0), _g0(other._g0)
  {}
  virtual ~EvtPropagator()
  {}
  
  // Accessors 

  inline double m0() const  { return _m0; }
  inline double g0() const  { return _g0; }
  
  // Modifiers (can be useful e.g. for fitting!)

  inline void set_m0(double fm0)  { assert(fm0>0);  _m0 = fm0; }
  inline void set_g0(double fg0)  { assert(fg0>=0); _g0 = fg0; }

protected:

  double _m0;
  double _g0;

}; 

#endif

