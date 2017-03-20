/* 
 * File:   EvtPP_X_PsiRho_amp_amp.hh
 * Author: luchinsky
 *
 * Created on November 20, 2015, 9:49 AM
 */

#ifndef EVTPP_X_PSIPIPI
#define	EVTPP_X_PSIPIPI

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


class EvtPP_X_PsiPiPi: public EvtDecayAmp  {
public:
    EvtPP_X_PsiPiPi();
    EvtPP_X_PsiPiPi(const EvtPP_X_PsiPiPi&) = delete; // orig);
    virtual ~EvtPP_X_PsiPiPi();
    virtual std::string getName();
    virtual EvtDecayBase* clone();

    virtual void init();

    virtual void decay(EvtParticle* p);




private:
// parameters
    double fRho,fOmega,g,ef;
// masses
    double mPsi,mRho,mOmega,mD0,mD0star,mDch,mDchstar,mPi,mp;
    double M0,delta,mu1,mu2,mX3872;
// widths
    double gRho,gOmega;
    double maxAmp;
    bool first_run;
    

    EvtComplex GammaRho(double e);
    EvtComplex GammaOmega(double e);
    EvtComplex invDD(double e);
    inline double sp(EvtVector4R p1, EvtVector4R p2) {return p1*p2;}
    virtual void initProbMax();


};

#endif	/* EVTPP_X_PSIPIPI */

