/* 
 * File:   EvtPP_PsiEtaPi0.cpp
 * Author: luchinsky
 * 
 * Created on October 5, 2014, 12:39 AM
 */

#include "EvtGenModels/EvtPP_PsiEtaPi0.hh"

using namespace std;


EvtPP_PsiEtaPi0::EvtPP_PsiEtaPi0(const EvtPP_PsiEtaPi0& orig) : p1(orig.p1), p2(orig.p2), mp(orig.mp),  mp_2(orig.mp_2), mp_4(orig.mp_4),
    mPi(orig.mPi), mPi_2(orig.mPi_2), mPsi(orig.mPsi), wPsi(orig.wPsi), mPsi_2(orig.mPsi_2), 
    mEta(orig.mEta), mEta_2(orig.mEta_2), mPsi2S(orig.mPsi2S), wPsi2S(orig.wPsi2S), s(orig.s), first_run(orig.first_run)
{
}

EvtPP_PsiEtaPi0::~EvtPP_PsiEtaPi0() {}

EvtDecayBase *EvtPP_PsiEtaPi0::clone() { return new EvtPP_PsiEtaPi0();}

std::string EvtPP_PsiEtaPi0::getName() {return "PP_PSIETAPI0";}

void EvtPP_PsiEtaPi0::init() {
    checkNArg(0);
  
    mp = EvtPDL::getMass(EvtPDL::getId("p+"));
    mPi=EvtPDL::getMass(EvtPDL::getId("pi0"));
    mPsi = EvtPDL::getMass(EvtPDL::getId("J/psi")); mPsi_2=mPsi*mPsi;
    mEta=EvtPDL::getMass(EvtPDL::getId("eta"));  mEta_2=mEta*mEta;
    wPsi = EvtPDL::getWidth(EvtPDL::getId("J/psi"));
    mPsi2S = EvtPDL::getMass(EvtPDL::getId("psi(2S)"));
    wPsi2S = EvtPDL::getWidth(EvtPDL::getId("psi(2S)"));
}


double EvtPP_PsiEtaPi0::Matr2_PsiEtaPi0(EvtParticle *root) {
  //const int diag1=1, diag2=1, diag3=1, diag4=1;
    EvtVector4R p=root->getDaug(0)->getP4Lab(), 
            kEta=root->getDaug(1)->getP4Lab(), 
            kPi=root->getDaug(2)->getP4Lab(),
            q=p+kEta;
    
    double q2=q.mass2(), t=(p1-q).mass2(), u=(p2-q).mass2(),
            pp1=p*p1, pp2=p*p2, pq=p*q, pp=p*p, kPip=kPi*p;
    

    double matr2=2*(4*q2*pow(kPip,2)*(-t + pow(mp,2))*(-u + pow(mp,2))*(-(t*u) + q2*(s + t + u) - (2*q2 + t + u)*pow(mp,2) + 3*pow(mp,4) - pow(q2,2)) - 
     4*kPip*(-t + pow(mp,2))*(-u + pow(mp,2))*(pp2*q2*(2*pow(mp,4) + pow(mp,2)*(q2 + s - 3*t - u + pow(mPi,2)) + 
           (q2 - t)*(q2 - s - t - u + 3*pow(mPi,2))) + 
        pp1*q2*(-2*pow(mp,4) + pow(mp,2)*(q2 + s - t + u - 3*pow(mPi,2)) + (q2 - u)*(q2 - s - t - u + 3*pow(mPi,2))) + 
        pq*(-2*q2*t*u + pow(mp,4)*(-4*q2 + 2*s + t + u - 2*pow(mPi,2)) - 
           pow(mp,2)*(q2*(2*s - 5*t - 3*u) + (t + u)*(s + t + u) - (t + 3*u)*pow(mPi,2)) - s*pow(q2,2) + t*pow(q2,2) + u*pow(q2,2) - 
           pow(mPi,2)*(2*t*u + q2*(s - 2*(t + u)) + 2*pow(q2,2)) + q2*pow(s,2) - q2*pow(t,2) + u*pow(t,2) - q2*pow(u,2) + t*pow(u,2))) + 
     pp*(-4*pow(mp,12) - 4*pow(mp,10)*(5*q2 - s - 3*t - 3*u + 6*pow(mPi,2)) - 
        4*pow(mp,8)*(2*s*t + 2*s*u + 9*t*u - q2*(3*s + 10*(t + u)) + 2*(8*q2 - s - 7*t - 6*u)*pow(mPi,2) + 2*pow(mPi,4) + pow(q2,2) + 
           3*pow(t,2) + 3*pow(u,2)) + t*u*((s - 2*(t + u))*pow(q2,3) + 
           q2*(2*(t + u)*pow(s,2) + pow(s,3) - s*pow(t - u,2) - 2*(t + u)*pow(t - u,2)) - 4*pow(t,2)*pow(u,2) + 
           pow(q2,2)*(-2*pow(s,2) + 4*(pow(t,2) + pow(u,2)))) - 
        4*pow(mp,6)*(-4*s*t*u + (6*q2 - 4*t - 3*u)*pow(mPi,4) - u*pow(q2,2) - pow(q2,3) - s*pow(t,2) - 9*u*pow(t,2) - pow(t,3) - 
           s*pow(u,2) - 9*t*pow(u,2) + pow(mPi,2)*(4*s*t + 3*s*u + 27*t*u - q2*(11*s + 35*t + 15*u) + 10*pow(t,2) + 7*pow(u,2)) + 
           q2*(s*(4*t + 5*u) + pow(s,2) + 5*(4*t*u + pow(t,2) + pow(u,2))) - pow(u,3)) + 
        pow(mp,4)*((s - 6*(t + u))*pow(q2,3) + pow(q2,2)*(-2*pow(s,2) + 8*pow(t,2) + 4*pow(u,2)) - 
           4*t*u*(2*s*(t + u) + 3*(3*t*u + pow(t,2) + pow(u,2))) + 
           pow(mPi,4)*(q2*(9*s + 38*t + 6*u) + 8*pow(q2,2) - 4*(6*t*u + 2*pow(t,2) + pow(u,2))) + 
           q2*(6*(t + u)*pow(s,2) + pow(s,3) + s*(26*t*u + 3*pow(t,2) + 7*pow(u,2)) - 
              2*(-21*u*pow(t,2) + pow(t,3) - 21*t*pow(u,2) + pow(u,3))) + 
           2*pow(mPi,2)*((s + 4*(t - 4*u))*pow(q2,2) + 4*pow(q2,3) - 
              q2*(34*s*t + 18*s*u + 76*t*u + 5*pow(s,2) + 28*pow(t,2) - 4*pow(u,2)) + 
              2*(18*u*pow(t,2) + 2*pow(t,3) + 15*t*pow(u,2) + s*(6*t*u + 2*pow(t,2) + pow(u,2)) + pow(u,3)))) - 
        pow(mp,2)*(pow(mPi,4)*(-4*t*u*(3*t + 2*u) + 8*(t + u)*pow(q2,2) + q2*(20*t*u + 9*s*(t + u) + 6*pow(t,2) - 10*pow(u,2))) - 
           4*(s + 3*(t + u))*pow(t,2)*pow(u,2) + pow(q2,3)*(s*(t + u) - 2*(4*t*u + pow(t,2) + pow(u,2))) + 
           2*pow(mPi,2)*(4*(t + u)*pow(q2,3) + 2*t*u*(3*s*t + 2*s*u + 9*t*u + 3*pow(t,2) + 2*pow(u,2)) + 
              pow(q2,2)*(s*(t + u) - 2*(3*t*u + 2*pow(t,2) + 7*pow(u,2))) - 
              q2*(2*s*t*(4*t + 15*u) + 5*(t + u)*pow(s,2) + 2*(8*u*pow(t,2) + pow(t,3) + 8*t*pow(u,2) - 3*pow(u,3)))) + 
           pow(q2,2)*(-2*(t + u)*pow(s,2) + 4*(2*u*pow(t,2) + pow(t,3) + t*pow(u,2) + pow(u,3))) + 
           q2*((t + u)*pow(s,3) + 2*pow(s,2)*(4*t*u + pow(t,2) + pow(u,2)) - s*(-5*u*pow(t,2) + pow(t,3) - 9*t*pow(u,2) + pow(u,3)) - 
              2*(pow(t,4) - 12*pow(t,2)*pow(u,2) + pow(u,4)))) + 
        pow(mPi,4)*(q2*t*u*(9*s - 2*(t + u)) - 4*pow(t,2)*pow(u,2) + 2*pow(q2,2)*pow(t + u,2)) + 
        2*pow(mPi,2)*(2*(s + t + u)*pow(t,2)*pow(u,2) + q2*t*u*(-4*s*(t + u) - 5*pow(s,2) + 2*(pow(t,2) + pow(u,2))) - 
           pow(q2,2)*(5*u*pow(t,2) + pow(t,3) + 5*t*pow(u,2) + s*(-3*t*u + pow(t,2) + pow(u,2)) + pow(u,3)) + pow(q2,3)*pow(t + u,2))) + 
     2*(-2*q2*(-t + pow(mp,2))*pow(pp2,2)*((q2 - s - t)*t*u + pow(mp,6) + (q2*(-2*t + u) + t*(2*s + 2*t + u))*pow(mPi,2) + 
           pow(mp,4)*(q2 - s - 2*t - u + 3*pow(mPi,2)) - 2*t*pow(mPi,4) + 
           pow(mp,2)*(s*t + s*u + 2*t*u - q2*(t + u) + (q2 - 2*s - 5*t - u)*pow(mPi,2) + 2*pow(mPi,4) + pow(t,2))) - 
        2*q2*(-u + pow(mp,2))*pow(pp1,2)*(t*(q2 - s - u)*u + pow(mp,6) + (q2*(t - 2*u) + u*(2*s + t + 2*u))*pow(mPi,2) + 
           pow(mp,4)*(q2 - s - t - 2*u + 11*pow(mPi,2)) - 2*u*pow(mPi,4) + 
           pow(mp,2)*(s*t + s*u + 2*t*u - q2*(t + u) + (q2 - 2*s - 5*t - 9*u)*pow(mPi,2) + 2*pow(mPi,4) + pow(u,2))) + 
        2*pp2*pq*(-t + pow(mp,2))*(2*pow(mp,8) + pow(mp,6)*(3*q2 - s - 4*t - 4*u + 7*pow(mPi,2)) + (3*t*u - q2*(2*t + u))*pow(mPi,4) + 
           t*u*((s + 2*t)*u - q2*(s + t + 2*u) + pow(q2,2)) + 
           pow(mPi,2)*(-(t*u*(3*s + 3*t + 2*u)) + q2*(s*(2*t + u) + 2*t*(t + 2*u)) + (-2*t + u)*pow(q2,2)) + 
           pow(mp,4)*(s*t + 2*s*u + 8*t*u - q2*(3*s + 4*t + 5*u) + (10*q2 - 5*s - 10*t - 9*u)*pow(mPi,2) + 4*pow(mPi,4) + 2*pow(q2,2) + 
              pow(s,2) + 2*pow(t,2) + 2*pow(u,2)) + pow(mp,2)*
            ((3*q2 - 3*t - 4*u)*pow(mPi,4) - (t + 2*u)*pow(q2,2) - u*(4*t*(t + u) + s*(2*t + u) + pow(s,2)) + 
              q2*(6*t*u + s*(t + 3*u) + pow(t,2) + 2*pow(u,2)) + 
              pow(mPi,2)*(3*s*t + 5*s*u + 12*t*u - q2*(3*s + 8*(t + u)) + pow(q2,2) + 3*pow(t,2) + 2*pow(u,2)))) + 
        2*pp1*(-u + pow(mp,2))*(-(pp2*q2*(-t + pow(mp,2))*
              (-2*q2*s - q2*t + s*t - q2*u + s*u + 2*t*u + 2*pow(mp,4) + (4*q2 - 3*(2*s + t + u))*pow(mPi,2) + 
                2*pow(mp,2)*(q2 - s - t - u + 5*pow(mPi,2)) + 5*pow(mPi,4) + pow(q2,2) + pow(s,2))) + 
           pq*(2*pow(mp,8) + pow(mp,6)*(5*q2 - 3*s - 4*t - 4*u + 17*pow(mPi,2)) + (3*t*u - q2*(t + 2*u))*pow(mPi,4) + 
              t*u*(-(q2*(s + 2*t + u)) + t*(s + 2*u) + pow(q2,2)) + 
              pow(mPi,2)*(-(t*u*(3*s + 2*t + 3*u)) + q2*(2*u*(2*t + u) + s*(t + 2*u)) + (t - 2*u)*pow(q2,2)) + 
              pow(mp,4)*(4*s*t + 3*s*u + 8*t*u - q2*(3*s + 7*t + 6*u) + (20*q2 - 7*s - 23*t - 16*u)*pow(mPi,2) + 6*pow(mPi,4) + 
                 2*pow(q2,2) + pow(s,2) + 2*pow(t,2) + 2*pow(u,2)) + 
              pow(mp,2)*(3*(q2 - 2*t - u)*pow(mPi,4) - (2*t + u)*pow(q2,2) - t*(4*u*(t + u) + s*(t + 4*u) + pow(s,2)) + 
                 q2*(8*t*u + s*(3*t + u) + 2*pow(t,2) + pow(u,2)) + 
                 pow(mPi,2)*(7*s*t + 3*s*u + 18*t*u - q2*(3*s + 14*t + 12*u) + pow(q2,2) + 6*pow(t,2) + 3*pow(u,2))))) + 
        pow(pq,2)*(4*pow(mp,10) + pow(mp,8)*(-2*(q2 + 2*s + 3*(t + u)) + 8*pow(mPi,2)) + 
           2*pow(mp,6)*(3*s*t + 3*s*u + 4*t*u + q2*(2*s + 3*(t + u)) - (8*q2 + 6*s + 15*t - 5*u)*pow(mPi,2) + 2*pow(mPi,4) - 
              2*pow(q2,2)) - pow(mPi,4)*(t*u*(4*s - 3*(t + u)) + 2*q2*(t*u + pow(t,2) + pow(u,2))) + 
           t*u*((t + u)*pow(q2,2) - 2*q2*(t*u + pow(t,2) + pow(u,2)) + (t + u)*(-pow(s,2) + pow(t,2) + pow(u,2))) + 
           pow(mp,4)*(-8*s*t*u - 2*q2*(t + u)*(2*s + 3*(t + u)) - (6*q2 + 4*s + 9*t - 7*u)*pow(mPi,4) + 5*(t + u)*pow(q2,2) - 
              t*pow(s,2) - u*pow(s,2) - 2*s*pow(t,2) + u*pow(t,2) + 3*pow(t,3) + 
              2*pow(mPi,2)*(11*s*t + 3*s*u + 8*t*u + q2*(s + 6*t + 18*u) - 2*pow(q2,2) + 2*pow(s,2) + 5*pow(t,2) - 11*pow(u,2)) - 
              2*s*pow(u,2) + t*pow(u,2) + 3*pow(u,3)) + 
           pow(mp,2)*(2*t*u*pow(s,2) + 2*s*u*pow(t,2) + pow(s,2)*pow(t,2) - 4*u*pow(t,3) - pow(t,4) + 
              pow(mPi,4)*(-2*t*u + 6*q2*(t + u) + 4*s*(t + u) + pow(t,2) - 7*pow(u,2)) + 2*s*t*pow(u,2) + pow(s,2)*pow(u,2) - 
              2*pow(t,2)*pow(u,2) - pow(q2,2)*(6*t*u + pow(t,2) + pow(u,2)) + 
              2*pow(mPi,2)*(2*(t + u)*pow(q2,2) - 2*(t + u)*pow(s,2) + s*(-8*t*u - 3*pow(t,2) + pow(u,2)) + 
                 u*(3*t*u + 3*pow(t,2) + 4*pow(u,2)) - q2*(s*(t + u) + 2*(5*t*u + 2*pow(t,2) + 5*pow(u,2)))) - 4*t*pow(u,3) + 
              2*q2*(2*t*u*(s + 2*u) + 4*u*pow(t,2) + pow(t,3) + pow(u,3)) - pow(u,4)) + 
           2*pow(mPi,2)*(-(pow(q2,2)*(pow(t,2) + pow(u,2))) + t*u*(s*(t + u) + 2*pow(s,2) - 2*(t*u + pow(t,2) + pow(u,2))) + 
              q2*(s*(-(t*u) + pow(t,2) + pow(u,2)) + pow(t + u,3))))))*pow(-t + pow(mp,2),-2)*pow(-u + pow(mp,2),-2);
    EvtComplex _mPsi2S(mPsi2S,wPsi2S/2);
    matr2=matr2/pow(abs(q2-_mPsi2S*_mPsi2S),2);
    
        if(13.4<q2 && q2<13.8 ) matr2=0.;
	 return real(matr2);
}


void EvtPP_PsiEtaPi0::decay(EvtParticle* root) {
    root->initializePhaseSpace(getNDaug(), getDaugs());
    if(first_run) {
        first_run=false;
        s = root->getP4Lab().mass2();
        p2 = EvtVector4R(mp, 0, 0, 0);
        p1 = root->getP4Lab() - p2;     
    };
    
    double prob=Matr2_PsiEtaPi0(root);;
    setProb(prob);
    
}
