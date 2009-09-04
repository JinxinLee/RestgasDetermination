//////////////////////////////////////////////////////////////////////////
//                                                                      //
// PndPidProbability	                                                  //
//                                                                      //
// Definition of the Panda pid probabilities .	                        //
//                                                                      //
// Author: Ralf Kliemt, Dresden/Turin/Bonn, 01.09.09                    //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "PndPidProbability.h"


PndPidProbability::PndPidProbability()		
{
  fElectronPidProb = 0.;
	fMuonPidProb = 0.;
	fPionPidProb = 0.;
	fKaonPidProb = 0.;
	fProtonPidProb = 0.;  
}

PndPidProbability::PndPidProbability(Float_t e, Float_t mu, Float_t pi, Float_t k, Float_t p)
{
  fElectronPidProb = e;
	fMuonPidProb = mu;
	fPionPidProb = pi;
	fKaonPidProb = k;
	fProtonPidProb = p;  
}
	
PndPidProbability::~PndPidProbability()
{
}

void PndPidProbability::Print()
{
  std::cout <<"PndPidProbability:   "
  <<"p(e)="<<fElectronPidProb<<"  "
  <<"p(mu)="<<fMuonPidProb<<"  "
  <<"p(pi)="<<fPionPidProb<<"  "
  <<"p(K)="<<fKaonPidProb<<"  "
  <<"p(P)="<<fProtonPidProb
  <<std::endl;
}

ClassImp(PndPidProbability)

