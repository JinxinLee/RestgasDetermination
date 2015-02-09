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
//
//------------------------------------------------------------------------


#ifndef EVTDEUTERONSPECTATOR_HH
#define EVTDEUTERONSPECTATOR_HH

#include "EvtGenBase/EvtDecayIncoherent.hh"
class EvtParticle;

class EvtDeuteronSpectator:public  EvtDecayIncoherent  {

public:

  EvtDeuteronSpectator();
  virtual ~EvtDeuteronSpectator();

  std::string getName();

  EvtDecayBase* clone();

  void decay(EvtParticle *p); 

  void init();

private:

  double _pmax;
  double _mmin;

};

#endif

