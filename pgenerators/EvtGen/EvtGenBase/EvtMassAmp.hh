//-----------------------------------------------------------------------
// File and Version Information: 
//      $Id: EvtMassAmp.hh,v 1.10 2004/03/04 01:18:13 lange Exp $
// 
// Environment:
//      This software is part of the EvtGen package developed jointly
//      for the BaBar and CLEO collaborations. If you use all or part
//      of it, please give an appropriate acknowledgement.
//
// Copyright Information:
//      Copyright (C) 1998 Caltech, UCSB
//
// Module creator:
//      Alexei Dvoretskii, Caltech, 2001-2002.
//-----------------------------------------------------------------------

// Relativistic lineshape for a two-body decay of a resonance to two 
// pseudoscalars. The mass dependence of the width and the vertex factors
// are included in the calculation.

#ifndef EVT_MASSAMP_HH
#define EVT_MASSAMP_HH

#include "EvtGenBase/EvtPoint1D.hh"
#include "EvtGenBase/EvtAmplitude.hh"
#include "EvtGenBase/EvtPropBreitWignerRel.hh"
#include "EvtGenBase/EvtTwoBodyVertex.hh"

class EvtMassAmp : public EvtAmplitude<EvtPoint1D> {
public:

  EvtMassAmp(const EvtPropBreitWignerRel& prop, const EvtTwoBodyVertex& vd);
  EvtMassAmp(const EvtMassAmp& other); 
  virtual ~EvtMassAmp(); 

  virtual EvtComplex amplitude(const EvtPoint1D& p) const;

  virtual EvtAmplitude<EvtPoint1D>* clone() const
  { return new EvtMassAmp(*this); }
  
  void setBirthVtx(const EvtTwoBodyVertex& vb)
  {
    _vb = new EvtTwoBodyVertex(vb);
  }

  void addBirthFact() { _useBirthFact = true; }
  void addDeathFact() { _useDeathFact = true; }
  void addBirthFactFF() { _useBirthFactFF = true; }
  void addDeathFactFF() { _useDeathFactFF = true; }
  void fixUpMassForMax() { _fixUpMassForMax = true;}

private:
  
  EvtPropBreitWignerRel _prop;
  EvtTwoBodyVertex  _vd;
  EvtTwoBodyVertex* _vb;

  bool _useBirthFact;
  bool _useDeathFact;
  bool _useBirthFactFF;
  bool _useDeathFactFF;
  bool _fixUpMassForMax;
};


#endif
