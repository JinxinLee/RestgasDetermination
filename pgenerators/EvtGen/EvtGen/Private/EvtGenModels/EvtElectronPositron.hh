//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: EvtElectronPositron.hh,v 1.4 2008/01/08 17:15:11 steinke Exp $
//
// Description:
//            Generator of pbar p -> electron positron events
//            just a example, dummy diff cross section should be replace by correct one/
//
// Author List:
//
//------------------------------------------------------------------------
#ifndef EVTELECTRONPOSITRON_HH
#define EVTELECTRONPOSITRON_HH

#include "EvtGenBase/EvtDecayProb.hh"

class EvtParticle;

class EvtElectronPositron:public EvtDecayProb{

public:
  EvtElectronPositron();
  virtual ~EvtElectronPositron();
  std::string getName();
  EvtDecayBase* clone();
  void init();
  void initProbMax();
  void decay(EvtParticle *p);

private:
  double _pbarmom;
  double _s;
  double _GeDivGm;
};

#endif
