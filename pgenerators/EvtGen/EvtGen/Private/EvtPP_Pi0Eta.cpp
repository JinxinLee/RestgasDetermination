/* 
 * File:   EvtPP_Pi0Eta.cpp
 * Author: luchinsky
 * 
 * Created on October 9, 2014, 2:21 PM
 */

#include "EvtGenModels/EvtPP_Pi0Eta.hh"
#define diag1 1
#define diag2 1
#define diag3 1
#define diag4 1



EvtPP_Pi0Eta::EvtPP_Pi0Eta(const EvtPP_Pi0Eta& orig) :
  EvtDecayProb(orig),
  p1(orig.p1), p2(orig.p2), mp(orig.mp), mPi(orig.mPi), mEta(orig.mEta),s(orig.s),first_run(orig.first_run)
{
}

EvtPP_Pi0Eta::~EvtPP_Pi0Eta() {}

string EvtPP_Pi0Eta::getName() {return "PP_PI0ETA";}

EvtDecayBase *EvtPP_Pi0Eta::clone() {return new EvtPP_Pi0Eta;}

void EvtPP_Pi0Eta::init() {
    checkNArg(0);
};

void EvtPP_Pi0Eta::decay(EvtParticle* root) {
    root->initializePhaseSpace(getNDaug(), getDaugs());
    if(first_run) {
        first_run=false;
        s = root->getP4Lab().mass2();
        p2 = EvtVector4R(mp, 0, 0, 0);
        p1 = root->getP4Lab() - p2;     
    };
    
    EvtVector4R kPi=root->getDaug(0)->getP4Lab(), kEta=root->getDaug(1)->getP4Lab();
    double t=(p1-kPi).mass2(), u=(p1-kEta).mass2();
    double matr2 = -8*(-((s + 8*(t + u))*pow(mp,8)) + 4*pow(mp,10) + pow(mp,2)*
      (t*u*(t + u)*(2*s + t + u) + (s + t + u)*pow(mPi,2)*pow(t - u,2) - pow(mPi,4)*pow(t - u,2)) - s*pow(t,2)*pow(u,2) + 
     pow(mp,6)*(14*t*u + 2*s*(t + u) + 5*pow(t,2) + 5*pow(u,2)) - 
     pow(mp,4)*(7*u*pow(t,2) + pow(t,3) + 2*pow(mPi,2)*pow(t - u,2) + 7*t*pow(u,2) + s*(4*t*u + pow(t,2) + pow(u,2)) + pow(u,3)))*
   pow(-t + pow(mp,2),-2)*pow(-u + pow(mp,2),-2);

    setProb(matr2);

}


