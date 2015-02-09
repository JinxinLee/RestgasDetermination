//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: EvtLambdaLambdaBarHE.hh,
//
// Description:
//            Generator of pbar p -> Lambda0 anti-Lambda0 events
//            just a example, dummy diff cross section should be replace by correct one/
//
// Author List:
//
//------------------------------------------------------------------------
#ifndef EVTLAMBDA_LAMBDABAR_HH
#define EVTLAMBDA_LAMBDABAR_HH

#include "EvtGenBase/EvtDecayProb.hh"

class EvtParticle;

class EvtLambdaLambdaBarHE:public EvtDecayProb{

public:
  EvtLambdaLambdaBarHE();
  virtual ~EvtLambdaLambdaBarHE();
  std::string getName();
  EvtDecayBase* clone();
  void init();
  void initProbMax();
  void decay(EvtParticle *p);

private:
  double p_lambdabar;
  double energy;
  double pbar_p;
};

#endif
