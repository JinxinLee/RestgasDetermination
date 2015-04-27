/* 
 * File:   EvtPP_Pi0Eta.hh
 * Author: luchinsky
 *
 * Created on October 9, 2014, 2:21 PM
 */

#ifndef EVTPP_PI0ETA_HH
#define	EVTPP_PI0ETA_HH

#include "EvtGenBase/EvtDecayProb.hh"
#include "EvtGenBase/EvtVector4R.hh"

#include "EvtGenBase/EvtPatches.hh"
#include <stdlib.h>
#include "EvtGenBase/EvtParticle.hh"
#include "EvtGenBase/EvtGenKine.hh"
#include "EvtGenBase/EvtPDL.hh"
#include "EvtGenBase/EvtReport.hh"
#include "EvtGenBase/EvtDiracParticle.hh"
#include "EvtGenBase/EvtDiracSpinor.hh"
#include "EvtGenBase/EvtGammaMatrix.hh"
#include "EvtGenBase/EvtVector4C.hh"
#include "EvtGenBase/EvtTensor4C.hh"
#include "EvtGenBase/EvtVectorParticle.hh"

#include <string>
#include <iostream>
#include <fstream>

class EvtParticle;

using namespace std;


class EvtPP_Pi0Eta: public EvtDecayProb {
public:
  EvtPP_Pi0Eta():  EvtDecayProb(), p1(EvtVector4R()), p2(EvtVector4R()), mp(0.938), mPi(0.140), mEta(0.5), s(10), first_run(true) {};
    EvtPP_Pi0Eta(const EvtPP_Pi0Eta& orig);
    virtual ~EvtPP_Pi0Eta();
    string getName();
  EvtDecayBase* clone();
  void init();
//  void initProbMax() { setProbMax(2);};
  void decay(EvtParticle *p);
    
private:
    
    // momenta
    EvtVector4R p1,p2;

    double mp, mPi,mEta, s;
    //constants
    bool first_run;

};

#endif	/* EVTPP_PI0ETA_HH */

