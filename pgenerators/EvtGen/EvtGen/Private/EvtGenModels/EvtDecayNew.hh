//--------------------------------------------------------------------------
//
// Environment:
//      This software is part of the EvtGen package developed jointly
//      for the BaBar and CLEO collaborations.  If you use all or part
//      of it, please give an appropriate acknowledgement.
//
// Copyright Information: See EvtGen/COPYRIGHT
//      Copyright (C) 1998      Caltech, UCSB
//
// Module: EvtGen/EvtDecayNew.hh
//
// Description:
//
// Modification history:
//
// SS   April 11, 2014  1st implementation from part of EvtChic1toJpsiGam from Jan Zhong
//
//------------------------------------------------------------------------

#ifndef EvtDecayNew_HH
#define EvtDecayNew_HH

#include "EvtGenBase/EvtDecayBase.hh"

class EvtParticle;

class EvtDecayNew : public EvtDecayBase{

public:

  EvtDecayNew();
  void makeDecay(EvtParticle* p, bool recursive=true);

  void setProb(double prob) { _prob=prob;}
  double getProb() { return _prob;}
  inline void setWeight(double weight) {_weight=weight;}

  virtual ~EvtDecayNew() {}


private:

  double _prob;
  double _weight;
  
};



#endif




