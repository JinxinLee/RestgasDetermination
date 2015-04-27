/* 
 * File:   EvtPP_Pi0Gamma.hh
 * Author: luchinsky
 *
 * Created on October 8, 2014, 3:23 PM
 */

#ifndef EVTPP_PI0GAMMA_HH
#define	EVTPP_PI0GAMMA_HH

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

class EvtPP_Pi0Gamma: public EvtDecayProb {
public:
  EvtPP_Pi0Gamma(): EvtDecayProb(), p1(EvtVector4R()), p2(EvtVector4R()), mp(0.938), mPi(0.135), s(10), first_run(true) {};
    EvtPP_Pi0Gamma(const EvtPP_Pi0Gamma& orig);
    virtual ~EvtPP_Pi0Gamma();
    EvtDecayBase *clone() {return new EvtPP_Pi0Gamma();}
    std::string getName() {return "PP_PI0GAMMA";}
    void init();
    void decay(EvtParticle *root);
private:
    
    // momenta
    EvtVector4R p1,p2;

    double mp, mPi,s;
    //constants
    bool first_run;
};

#endif	/* EVTPP_PI0GAMMA_HH */

