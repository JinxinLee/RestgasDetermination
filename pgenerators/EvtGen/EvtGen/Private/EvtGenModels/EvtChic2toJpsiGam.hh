//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: EvtChic2toJpsiGam.hh,v 1.1 2008/05/27 16:54:34 jan Exp $
//
// Description:
// example generator for pbar p -> chi_c2 -> J/Psi gamma
// formula taken from note written by PANDA Ferrara group
//
// todo: check if everything is correct ( especially chi angle)
//
// Author List:
// Jan Zhong  Ruhr-Uni-Bochum
//      PANDA Ferrara group
//
//------------------------------------------------------------------------
#ifndef EVTCHIC2TOJPSIGAM_HH
#define EVTCHIC2TOJPSIGAM_HH

#include "EvtGenModels/EvtDecayNew.hh"

class EvtParticle;

class EvtChic2toJpsiGam:public EvtDecayNew
{
public:
  EvtChic2toJpsiGam();
  virtual ~EvtChic2toJpsiGam();
  std::string getName();
  EvtDecayBase* clone();
  void init();
  void initProbMax();
  void decay(EvtParticle *p);
 
private:
  double _B0;
  double _a2;
  double _a3;
  double _prob;

};

#endif

