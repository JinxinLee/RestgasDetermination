/* 
 * File:   EvtPP_X_PsiRho_amp.cpp
 * Author: luchinsky
 * 
 * Created on November 20, 2015, 9:49 AM
 */

#include "EvtGenModels/EvtPP_X_PsiRho_amp.hh"

EvtPP_X_PsiRho_amp::EvtPP_X_PsiRho_amp() {
    M0=mD0+mD0star;
    delta=mDch+mDchstar-mD0-mD0star;
    mu1=1/(1/mD0+1/mD0star);
    mu2=1/(1/mDch+1/mDchstar);
    mX3872=M0+ef;
}

EvtPP_X_PsiRho_amp::EvtPP_X_PsiRho_amp(const EvtPP_X_PsiRho_amp& orig) {
    M0=mD0+mD0star;
    delta=mDch+mDchstar-mD0-mD0star;
    mu1=1/(1/mD0+1/mD0star); 
    mu2=1/(1/mDch+1/mDchstar);
    mX3872=M0+ef;
}

EvtPP_X_PsiRho_amp::~EvtPP_X_PsiRho_amp() {
}

void EvtPP_X_PsiRho_amp::initProbMax() {
    setProbMax(3.52e7);
}

void EvtPP_X_PsiRho_amp::init() {}

EvtDecayBase* EvtPP_X_PsiRho_amp::clone() { return new EvtPP_X_PsiRho_amp;}


std::string EvtPP_X_PsiRho_amp::getName() { return "PP_X_PsiRho_amp";}


EvtComplex EvtPP_X_PsiRho_amp::GammaRho(double e) {
    double re=0;
    if(e>-0.22)
        re=(0.22 + e)*(0.00482257247884953 + 0.019270294080687776*e - 0.04671367839239516*pow(e,2) + 0.04555400166840043*pow(e,3) - 0.01612897179826872*pow(e,4));
    double im=0;
    if(e<-0.52)
        im=(1.0750344957161986 + e)*(5.138300634159228 + e)*(-0.00007248779607875129 - 0.00031904332040682567*e - 0.00031108295230229124*pow(e,2) + 0.00020204398926602088*pow(e,3) + 0.00025456812297365203*pow(e,4));
    return EvtComplex(re,im);
}

EvtComplex EvtPP_X_PsiRho_amp::GammaOmega(double e) {
    double re=0;
    if(e>-0.08)
        re=(0.011796253489364951 + 0.18618690381100295*e - 0.4713412795464089*pow(e,2) + 0.5302956249880806*pow(e,3) - 0.21312126399444514*pow(e,4))*pow(0.08 + e,0.5);
    double im=0;
    if(e<-0.39)
        im=(0.1389466497882908 + e)*(0.36845257656244884 + e)*(-0.0006654546161676186 - 0.00195342835827875*e - 0.002920074216855949*pow(e,2) - 0.00240163046018167*pow(e,3) - 0.0007914194252011911*pow(e,4));
    return EvtComplex(re, im);
}


EvtComplex EvtPP_X_PsiRho_amp::invDD(double E) {
    EvtComplex k1, k2;
    EvtComplex I(0,1);
    if(E>0) k1=sqrt(2*mu1*E); else k1=I*sqrt(-2*mu1*E);
    if(E>delta) k2=sqrt(2*mu2*(E-delta)); else k2=I*sqrt(-2*mu2*(E-delta));
//    EvtComplex DD=E-ef+I*(g*k1/2+g*k2/2+GammaRho(E)/2+GammaOmega(E)/2);
    EvtComplex DD=E-ef+I*(g*k1/2+g*k2/2);
    return 1/DD;
    
}

void EvtPP_X_PsiRho_amp::decay(EvtParticle* root) {
    root->initializePhaseSpace(getNDaug(), getDaugs());
    EvtVector4R P=root->getP4(),
            pPsi=root->getDaug(0)->getP4(),
            k1=root->getDaug(1)->getP4(),
            k2=root->getDaug(2)->getP4();
    double m2=(k1+k2).mass2();
    EvtComplex I(0,1);
    double E=root->mass()-M0;

    EvtTensor4C Pk1k2=dual(EvtGenFunctions::directProd(P,k1-k2)); // e_abcd P_a (k1-k2)^b
    for(int iPsi=0; iPsi<3; ++iPsi) {
        for(int iPP=0; iPP<3; ++iPP) {
            EvtVector4C epsPsi=root->getDaug(0)->epsParent(iPsi);
            EvtVector4C epsPP=root->epsParent(iPP);
            EvtComplex amp=epsPP*Pk1k2.cont1(epsPsi)*invDD(E);
            amp=amp/(m2-(mRho+I*gRho/2)*(mRho+I*gRho/2));
            vertex(iPP,iPsi,amp);
        };
    };
}