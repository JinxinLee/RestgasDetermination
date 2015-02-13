//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: EvtPpbarPi0Gamma.hh,v 1.2 2008/01/08 17:15:11 steinke Exp $
//
// Description:
//            Generator of pbar p -> pi0 + pi0
//
// Author List:
//
//------------------------------------------------------------------------
#ifndef EVTPPABRPI0GAMMA_HH
#define EVTPPABRPI0GAMMA_HH

#include "EvtGenBase/EvtDecayProb.hh"

class EvtParticle;

class EvtPpbarPi0Gamma:public EvtDecayProb{

public:
	EvtPpbarPi0Gamma();
	virtual ~EvtPpbarPi0Gamma();
	std::string getName();
	EvtDecayBase* clone();
	void init();
	void initProbMax();
	void decay(EvtParticle *p);

private:
	
};

#endif

