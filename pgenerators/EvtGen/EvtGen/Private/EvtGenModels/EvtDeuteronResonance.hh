//--------------------------------------------------------------------------
// File and Version Information:
// 	      $Id: EvtDeuteronSpectator.hh,v 1.0 2012/02/08 $
//
// Description:
//            Generator for pbar d -> pbarn system p(spectator)
//            derived from EvtSingleParticle.hh
//
// Author List:
//            Albrecht Gillitzer
//            Alexander Blinov
//
//------------------------------------------------------------------------


#ifndef EVTDEUTERONRESONANCE_HH
#define EVTDEUTERONRESONANCE_HH

#include "EvtGenBase/EvtDecayIncoherent.hh"
class EvtParticle;

class EvtDeuteronResonance:public  EvtDecayIncoherent  {

public:

  EvtDeuteronResonance() {}
  virtual ~EvtDeuteronResonance();

  std::string getName();

  EvtDecayBase* clone();

  void decay(EvtParticle *p); 

  void init();

private:

  double _pmax;
  double _mmin;
  double _rmass;
  double _rwidth;

};

#endif

