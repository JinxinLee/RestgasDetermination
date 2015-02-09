//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: EvtChic1toJpsiGam.hh,v 1.1 2008/05/14 19:11:21 jan Exp $
//
// Description:
// example generator for pbar p -> chi_c1 -> J/Psi gamma
// formula taken from note written by PANDA Ferrara group
//
// todo: check if everything is correct ( especially chi angle)
//
// Author List:
// Jan Zhong  Ruhr-Uni-Bochum
//      PANDA Ferrara group
//
//------------------------------------------------------------------------
#ifndef EVTCHIC1TOJPSIGAM_HH
#define EVTCHIC1TOJPSIGAM_HH

#include "EvtGenModels/EvtDecayNew.hh"

class EvtParticle;

class EvtChic1toJpsiGam:public EvtDecayNew
{
public:
  EvtChic1toJpsiGam();
  virtual ~EvtChic1toJpsiGam();
  std::string getName();
  EvtDecayBase* clone();
  void init();
  void initProbMax();
  void decay(EvtParticle *p);
 
private:
  double _a2;
  double _prob;

};

#endif

