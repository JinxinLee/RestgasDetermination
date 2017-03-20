/* 
 * File:   EvtPP_X_PsiOmega_amp.hh
 * Author: luchinsky
 *
 * Created on November 20, 2015, 12:25 PM
 */

#ifndef EVTPP_X_PSIPIPIPI
#define	EVTPP_X_PSIPIPIPI

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



class EvtParticle;


class EvtPP_X_PsiPiPiPi: public EvtDecayAmp  {
public:
    EvtPP_X_PsiPiPiPi();
    EvtPP_X_PsiPiPiPi(const EvtPP_X_PsiPiPiPi&) = delete; // orig);
    virtual ~EvtPP_X_PsiPiPiPi();
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
    
    bool first_run;
    double maxAmp;

    EvtComplex GammaRho(double e);
    EvtComplex GammaOmega(double e);
    EvtComplex invDD(double e);
    inline double sp(EvtVector4R p1, EvtVector4R p2) {return p1*p2;}
    virtual void initProbMax();


};

#endif	/* EVTPP_X_PSIPIPIPI */

