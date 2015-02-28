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
    EvtPP_EtacGamma(): p1(EvtVector4R()), p2(EvtVector4R()),
      mp(0.938), mPsi(3.097),  mEtaC(2.982),  mPsi2S(3.686), mHc(3.526),
      wPsi(90e-6), wPsi2S(300e-6), wHc(700e-6),
      first_run(true) {};
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
    double wPsi, wPsi2S, wHc;
//    double s;
    

    double matr2(double s, double t, double u);
    
    
    bool first_run;
};

#endif	/* EVTPP_ETACGAMMA_H */

