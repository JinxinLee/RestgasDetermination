        /* 
 * File:   EvtPPPsiPiPi.cxx
 * Author: luchinsky
 * 
 * Created on April 28, 2014, 10:57 AM
 */

#include "EvtGenModels/EvtPPPsiPiPi.hh"
#include "EvtGenModels/EvtPPPsiPiPi_matr2_f0.hh"
using namespace std;



EvtPPPsiPiPi::~EvtPPPsiPiPi() {
}

std::string EvtPPPsiPiPi::getName() {
    return "PP_PSIPiPi";
}

EvtDecayBase *EvtPPPsiPiPi::clone() {
    return new EvtPPPsiPiPi();
}


void EvtPPPsiPiPi::initProbMax()  { 
  setProbMax(57.); 
  
}

void EvtPPPsiPiPi::init() {
    checkNArg(3);
    diag1 = getArg(0); diag2=getArg(1); diag3=getArg(2); diag4=diag3;

    
    mp = EvtPDL::getMass(EvtPDL::getId("p+"));
    mpi = EvtPDL::getMass(EvtPDL::getId("pi0"));
    mPsi = EvtPDL::getMass(EvtPDL::getId("J/psi"));
    mmu = EvtPDL::getMass(EvtPDL::getId("e-"));
    mPsi2S = EvtPDL::getMass(EvtPDL::getId("psi(2S)"));
    mHc = EvtPDL::getMass(EvtPDL::getId("h_c"));
    GammaPsi2S = 300e-6, GammaHc = 700e-6;

    mp2=pow(mp,2); mp4=pow(mp,4);
    mpi2=pow(mpi,2); mpi4=pow(mpi,4);
    mPsi2=pow(mPsi,2); mPsi4=pow(mPsi,4);

    

}

void EvtPPPsiPiPi::decay(EvtParticle* root) {
    root->initializePhaseSpace(getNDaug(), getDaugs());
    EvtVectorParticle *psi = (EvtVectorParticle*) root->getDaug(0);
    EvtId dau[2] = {EvtPDL::getId("e-"), EvtPDL::getId("e+")};

    s=root->getP4Lab().mass2();
    double beta = sqrt(1 - 4 * mp * mp / s);
    p2 = EvtVector4R(mp, 0, 0, 0);
    p1 = root->getP4Lab() - p2;
    
    p = root->getDaug(0)->getP4Lab();
    k1 = root->getDaug(1)->getP4Lab();
    k2 = root->getDaug(2)->getP4Lab();
    q = k1 + k2;

            
    pp1 = p*p1; pp2 = p*p2; q2 = q.mass2();
    k1p1=k1*p1; k1p2=k1*p2; k2p=k2*p; k2p2=k2*p2; k2p1=k2*p1; k1p=k1*p;
    k1k2=k1*k2; k1q=k1*q; k2q=k2*q;
    
    double prob = Matr2_2();
    
    // add f0 form factor
    double deltaM2 = pow(mPsi2S - mPsi,2);
    double K = 0.15;
    double Fpp = q2 - K*deltaM2*(1+2*mpi*mpi/q2);
    prob = prob*pow(Fpp,2);

    setProb(prob);

    return;

}



