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


// EvtPP_PsiEtaPi0::EvtPP_PsiEtaPi0() {
//     mp = 0;
//     mPi=0;
//     mPsi = 0;
//     mEta=0;
//     wPsi = 0;
//     mPsi2S = 0;
//     wPsi2S = 0;
// 
//     diag1 = 1;
//     diag2 = 1;
//     diag3 = 1;
//     diag4 = 1;
//   
//     nCall = 0;
// 
//     first_run = true;
// }


class EvtPP_PsiEtaPi0: public EvtDecayProb {
public:
    EvtPP_PsiEtaPi0():
        p1(EvtVector4R()), p2(EvtVector4R()),
        mp(0.938), mp_2(pow(mp,2)), mp_4(pow(mp,4)), mPi(0.135),  mPi_2(pow(mPi,2)),
        mPsi(3.097), wPsi(90e-6), mPsi_2(pow(mPsi,2)), mEta(0.450), mEta_2(pow(mEta,2)),
        mPsi2S(3.686), wPsi2S(3.17e-4), s(10), first_run(true)   {};
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

    double Matr2_PsiEtaPi0(EvtParticle *root);
    
    
    bool first_run;
  
  
  
};

#endif	/* EVTPP_PSIETAPI0_HH */

