/* 
 * File:   EvtPP_X_PsiRho_amp_amp.hh
 * Author: luchinsky
 *
 * Created on November 20, 2015, 9:49 AM
 */

#ifndef EVTPP_X_PSIRHO_AMP_HH
#define	EVTPP_X_PSIRHO_AMP_HH

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
#include "EvtGenBase/EvtDecayAmp.hh"

#include <string>
#include <iostream>
#include <fstream>


//double const _ECM=3.518;

class EvtParticle;


class EvtPP_X_PsiRho_amp: public EvtDecayAmp  {
public:
    EvtPP_X_PsiRho_amp();
    EvtPP_X_PsiRho_amp(const EvtPP_X_PsiRho_amp& orig);
    virtual ~EvtPP_X_PsiRho_amp();
    virtual std::string getName();
    virtual EvtDecayBase* clone();

    virtual void init();

    virtual void decay(EvtParticle* p);




private:
// parameters
    const double fRho=0.0070, fOmega=0.036, g=0.3, ef=-11.0e-3;
// masses
    const double mPsi=3.097, mRho=775e-3, mOmega=782e-3,
    mD0=1864.84e-3, mD0star=2006.96e-3, mDch=1869.61e-3, mDchstar=2010.26e-3,
    mPi=0.140, mp=0.938,
    M0=mD0+mD0star,
    delta=mDch+mDchstar-mD0-mD0star,
    mu1=1/(1/mD0+1/mD0star), mu2=1/(1/mDch+1/mDchstar),
    mX3872=M0+ef;
// widths
    const double gRho=147.8e-3, gOmega=8.5e-3;

    EvtComplex GammaRho(double e);
    EvtComplex GammaOmega(double e);
    EvtComplex invDD(double e);
    inline double sp(EvtVector4R p1, EvtVector4R p2) {return p1*p2;}
    virtual void initProbMax();


};

#endif	/* EVTPP_X_PSIRHO_AMP_HH */

