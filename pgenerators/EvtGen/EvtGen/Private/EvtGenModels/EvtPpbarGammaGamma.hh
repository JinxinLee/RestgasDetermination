//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: EvtPpbarPi0Pi0.hh,v 1.2 2008/01/08 17:15:11 steinke Exp $
//
// Description:
//            Generator of pbar p -> gamma + gamma
//
// Author List:
//
//------------------------------------------------------------------------
#ifndef EVTPPABRGAMMAGAMMA_HH
#define EVTPPABRGAMMAGAMMA_HH

#include "EvtGenBase/EvtDecayProb.hh"

class EvtParticle;

class EvtPpbarGammaGamma:public EvtDecayProb{

public:
	EvtPpbarGammaGamma();
	virtual ~EvtPpbarGammaGamma();
	std::string getName();
	EvtDecayBase* clone();
	void init();
	void initProbMax();
	void decay(EvtParticle *p);

private:
	
};

#endif

