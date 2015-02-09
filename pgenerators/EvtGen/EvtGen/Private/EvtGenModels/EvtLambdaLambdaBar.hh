//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: EvtLambdaLambdaBar.hh,v 1.2 2008/01/08 17:15:11 steinke Exp $
//
// Description:
//            Generator of pbar p -> LambdaBar Lambda events
//            based on the parametrised experimental data
//
// Author List:
//     Aleksandra Wronska UJ Cracow            (original author)
//     Ralf Kliemt, Tu Dresden                 (ported for pandaroot use)
//
//------------------------------------------------------------------------
#ifndef EVTLAMBDALAMBDABAR_HH
#define EVTLAMBDALAMBDABAR_HH

#include "EvtGenBase/EvtDecayProb.hh"

class EvtParticle;

class EvtLambdaLambdaBar:public EvtDecayProb{

public:
  EvtLambdaLambdaBar();
  virtual ~EvtLambdaLambdaBar();
  std::string getName();
  EvtDecayBase* clone();
  void init();
  void initProbMax();
  void decay(EvtParticle *p);

private:
  double _a0, _a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8, _a9;
  double _pbeam;
};

#endif

//--------------------------------------------------------------
// $Log: EvtLambdaLambdaBar.hh,v $
// Revision 1.2  2008/01/08 17:15:11  steinke
// merged with new version from BaBar
//
// Revision 1.1  2006/11/06 14:06:14  wronska
// initial import
//
//--------------------------------------------------------------
