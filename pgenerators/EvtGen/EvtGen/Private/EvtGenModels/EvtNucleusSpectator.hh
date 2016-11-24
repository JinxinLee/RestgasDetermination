//--------------------------------------------------------------------------
// File and Version Information:
// 	      $Id: EvtNucleusSpectator.hh,v 1.0 2016/11/04 $
//
// Description:
//            Generator for pbar A -> (A-1)_spectator + pbarp system
//                          pbar A -> (A-1)_spectator + pbarn system
//            derived from EvtSingleParticle.hh
//
// Author List:
//            Albrecht Gillitzer
//
//------------------------------------------------------------------------


#ifndef EVTNUCLEUSSPECTATOR_HH
#define EVTNUCLEUSSPECTATOR_HH

#include "EvtGenBase/EvtDecayIncoherent.hh"
class EvtParticle;

class EvtNucleusSpectator:public  EvtDecayIncoherent  {

public:

  EvtNucleusSpectator();
  virtual ~EvtNucleusSpectator();

  std::string getName();

  EvtDecayBase* clone();

  void decay(EvtParticle *p); 

  void init();

private:

  double _targA;
  double _pmax;
  double _mmin;

};

#endif

