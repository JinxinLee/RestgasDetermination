/* 
 * File:   EvtPP_PsiEtaPi0.hh
 * Author: luchinsky
 *
 * Created on October 5, 2014, 12:39 AM
 */

#ifndef EVTPP_PSIETAPI0_HH
#define	EVTPP_PSIETAPI0_HH

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


class EvtPP_PsiEtaPi0: public EvtDecayProb {
public:
    EvtPP_PsiEtaPi0();
    EvtPP_PsiEtaPi0(const EvtPP_PsiEtaPi0& orig);
    virtual ~EvtPP_PsiEtaPi0();

    std::string getName();
  EvtDecayBase* clone();
  void init();
//  void initProbMax() { setProbMax(2);};
  void decay(EvtParticle *p);

private:
    
    // momenta
    EvtVector4R p1,p2;
    
    // masses
    double mp, mp_2, mp_4;
    double mPi, mPi_2;
    double mPsi, wPsi, mPsi_2;
    double mEta, mEta_2;
    double mPsi2S, wPsi2S;
    double s;
    
    //constants
    double diag1, diag2, diag3, diag4;

    double Matr2_PsiEtaPi0(EvtParticle *root);
    
    int nCall;
    
    bool first_run;
  
  
  
};

#endif	/* EVTPP_PSIETAPI0_HH */

