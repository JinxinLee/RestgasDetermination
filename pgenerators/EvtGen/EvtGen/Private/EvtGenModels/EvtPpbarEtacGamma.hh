//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: EvtPpbarEtacGamma.hh,v 1.2 2008/01/08 17:15:11 steinke Exp $
//
// Description:
//            Generator of pbar p -> h_c-> eta_c + gamma events
//
// Author List:
//
//------------------------------------------------------------------------
#ifndef EVTPPABRETACGAMMA_HH
#define EVTPPABRETACGAMMA_HH

#include "EvtGenBase/EvtDecayProb.hh"

class EvtParticle;

class EvtPpbarEtacGamma:public EvtDecayProb{

public:
  EvtPpbarEtacGamma();
  virtual ~EvtPpbarEtacGamma();
  std::string getName();
  EvtDecayBase* clone();
  void init();
  void initProbMax();
  void decay(EvtParticle *p);

private:
  double _pbarmom;
};

#endif
