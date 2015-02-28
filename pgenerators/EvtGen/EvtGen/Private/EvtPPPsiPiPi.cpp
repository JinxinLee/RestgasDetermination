        /* 
 * File:   EvtPPPsiPiPi.cxx
 * Author: luchinsky
 * 
 * Created on April 28, 2014, 10:57 AM
 */

#include "EvtGenModels/EvtPPPsiPiPi.hh"
#include "EvtGenModels/EvtPPPsiPiPi_matr2_f0.hh"
using namespace std;



EvtPPPsiPiPi::EvtPPPsiPiPi() : EvtDecayProb(), p1(EvtVector4R()), p2(EvtVector4R()), mp(0.938), mp2(pow(mp,2)),  mp4(pow(mp,4)),
                               mpi(0.135), mpi2(pow(mpi,2)), mpi4(pow(mpi,4)), mPsi(3.097),  mPsi2(pow(mPsi,2)), mPsi4(pow(mPsi,4)),
                               mPsi2S(3.686), mHc(3.526), mmu(0.001),
			       GammaPsi2S(300e-6), GammaHc(700e-6),  
			       diag1(1), diag2(1), diag3(1), diag4(1)
{
}

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


EvtPPPsiPiPi::EvtPPPsiPiPi(const EvtPPPsiPiPi& orig) : p1(orig.p1), p2(orig.p2), mp(orig.mp), mp2(orig.mp2), mp4(orig.mp4),
                                                       mpi(orig.mpi), mpi2(orig.mpi2), mpi4(orig.mpi4), mPsi(orig.mPsi), mPsi2(orig.mPsi2), mPsi4(orig.mPsi4),
                                                       mPsi2S(orig.mPsi2S), mHc(orig.mHc), mmu(orig.mmu),
                                                       GammaPsi2S(orig.GammaPsi2S), GammaHc(orig.GammaHc),
                                                       diag1(orig.diag1), diag2(orig.diag2), diag3(orig.diag3), diag4(orig.diag4)
{
}

void EvtPPPsiPiPi::init() {
    checkNArg(0);

    
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

    double s=root->getP4Lab().mass2();
    p2 = EvtVector4R(mp, 0, 0, 0);
    p1 = root->getP4Lab() - p2;
    
    EvtVector4R  p = root->getDaug(0)->getP4Lab(),
      k1 = root->getDaug(1)->getP4Lab(),
      k2 = root->getDaug(2)->getP4Lab();

            
    
    double prob = Matr2_2(s,p,k1,k2);
    
    // add f0 form factor
    double deltaM2 = pow(mPsi2S - mPsi,2);
    double K = 0.15;
    double q2=(k1+k2).mass2();
    double Fpp = q2 - K*deltaM2*(1+2*mpi*mpi/q2);
    prob = prob*pow(Fpp,2);

    setProb(prob);

    return;

}



