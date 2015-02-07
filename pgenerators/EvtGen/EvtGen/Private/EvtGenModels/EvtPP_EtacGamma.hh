/*s 
 * File:   EvtPP_EtacGamma.h
 * Author: luchinsky
 *
 * Created on August 23, 2014, 10:25 PM
 */

#ifndef EVTPP_ETACGAMMA_H
#define	EVTPP_ETACGAMMA_H

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


//double const _ECM=3.518;

class EvtParticle;




class EvtPP_EtacGamma : public EvtDecayProb {
public:
    EvtPP_EtacGamma();
    EvtPP_EtacGamma(const EvtPP_EtacGamma& orig);
    virtual ~EvtPP_EtacGamma() { };
  std::string getName() {    return "PP_ETAC_GAMMA";};
  EvtDecayBase* clone() {    return new EvtPP_EtacGamma();};
  void init();
  void initProbMax() {};
  void decay(EvtParticle *p);
    
private:
    
    // momenta
    EvtVector4R p1,p2;
    
    // masses
    double mp, mPsi, mEtaC, mPsi2S, mHc;
    double wPsi, wEtaC, wPsi2S, wHc;
//    double s;
    
    //constants
    double diag1, diag2, diag3, diag4;

    double matr2(double s, double t, double u);
    
    int nCall;
    
    bool first_run;
};

#endif	/* EVTPP_ETACGAMMA_H */

