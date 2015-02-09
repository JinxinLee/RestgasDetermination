//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: EvtLambdaLambdaBarPol.hh,v 1.2 2008/01/08 17:15:11 steinke Exp $
//
// Description:
//            Generator of pbar p -> LambdaBar Lambda events
//            based on the parametrised experimental data
//
//
// Author List:
// Aleksandra Wronska, UJ Cracow (diff cross section parametrisation taken from EvtLambdaLambdaBar.cc)
// Jan Zhong  (add polarisation, no spin correlation applied yet)
// Ralf Kliemt, TU Dresden (ported to pandaroot use)
//------------------------------------------------------------------------

#ifndef EVTLAMBDALAMBDABARPOL_HH
#define EVTLAMBDALAMBDABARPOL_HH

#include "EvtGenBase/EvtDecayProb.hh"

class EvtParticle;

class EvtLambdaLambdaBarPol:public EvtDecayProb
{
public:
  EvtLambdaLambdaBarPol();
  virtual ~EvtLambdaLambdaBarPol();
  std::string getName();
  EvtDecayBase* clone();
  void init();
  void initProbMax();
  void decay(EvtParticle *p);

private:
  double _a0, _a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8, _a9;
  double _pbeam;
  double _polarisation;
};

#endif

