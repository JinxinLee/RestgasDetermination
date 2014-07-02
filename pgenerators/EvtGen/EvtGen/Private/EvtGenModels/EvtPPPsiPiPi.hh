/* 
 * File:   EvtPPPsiPiPi.h
 * Author: luchinsky
 *
 * Created on April 28, 2014, 10:57 AM
 */

#ifndef EVTPPPSIPIPI_H
#define	EVTPPPSIPIPI_H

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

//double const _ECM=3.518;

class EvtParticle;


class EvtPPPsiPiPi:public  EvtDecayProb {
public:
    EvtPPPsiPiPi() { E=4.26;};
    EvtPPPsiPiPi(double _E) { E=_E;};
    EvtPPPsiPiPi(const EvtPPPsiPiPi& orig);
    virtual ~EvtPPPsiPiPi();

  std::string getName();
  EvtDecayBase* clone();
  void init();
  void initProbMax() {};
  void decay(EvtParticle *p);
  int nCall;

private:
    double E,s;
    double diag1, diag2, diag3, diag4;
//    double diag1, g_pp_Psi2S, g_Psi2S_Psi_f;
//    double diag2, g_pp_Hc, g_Hc_Psi_f;
//    double diag3, g_pp_Psi, g_pp_f;
//    double diag4;
    // momenta
    EvtVector4R p1,p2,p,k1,k2,q;
    
    EvtDiracParticle proton, antiproton; 
    
    // scalar products
    double pp1, pp2, q2, k1p1, k1p2, k2p1, k2p2, k1p, k2p,k1k2,k1q,k2q;
    
    // masses
    double mp, mp2, mp4,
        mpi, mpi2, mpi4, 
        mPsi, mPsi2, mPsi4, 
        mPsi2S,mHc, mmu, GammaPsi2S, GammaHc;

    double Matr2_pi();
    double Matr2_2();
//    void Matr2_3();
};

#endif	/* EVTPPPSIPIPI_H */

