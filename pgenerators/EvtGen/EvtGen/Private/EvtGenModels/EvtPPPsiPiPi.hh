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

// #define diag1 1
// #define diag2 1
// #define diag3 1
// #define diag4 1

//double const _ECM=3.518;

class EvtParticle;


// EvtPPPsiPiPi::EvtPPPsiPiPi() {
//   diag1=1; diag2=1; diag3=1; diag4=1;
//   mp=0.938;  mpi=0.130;  mPsi=3.1; mmu=0; mPsi2S=3.7; mHc=3.7;
//   mp2=pow(mp,2); mp4=pow(mp,4);
//   mpi2=pow(mpi,2); mpi4=pow(mpi,4);
//   mPsi2=pow(mPsi,2); mPsi4=pow(mPsi,4);
// }


class EvtPPPsiPiPi:public  EvtDecayProb {
public:
// mp=0.938; mpi=0.135; mPsi=3.097; mmu=0.001; mPsi2S=3.686; mHc=3.526;
  
    EvtPPPsiPiPi();
    EvtPPPsiPiPi(const EvtPPPsiPiPi& orig);
    virtual ~EvtPPPsiPiPi();

  std::string getName();
  EvtDecayBase* clone();
  void init();
   void initProbMax();
  void decay(EvtParticle *p);

private:
    //double E,s;
    // momenta
    EvtVector4R p1,p2;//p,k1,k2,q;
    
//     EvtDiracParticle proton, antiproton; 
    
    // scalar products
//    double pp1, pp2, q2, k1p1, k1p2, k2p1, k2p2, k1p, k2p,k1k2,k1q,k2q;
    
    // masses
    double mp, mp2, mp4,
        mpi, mpi2, mpi4, 
        mPsi, mPsi2, mPsi4, 
        mPsi2S,mHc, mmu, GammaPsi2S, GammaHc;
    double diag1, diag2, diag3, diag4;

    double Matr2_pi();
    double Matr2_2(double s, EvtVector4R p, EvtVector4R k1, EvtVector4R k2);
//    void Matr2_3();
};

#endif	/* EVTPPPSIPIPI_H */

