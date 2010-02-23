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
  fElectronPdf = 0.;
	fMuonPdf = 0.;
	fPionPdf = 0.;
	fKaonPdf = 0.;
	fProtonPdf = 0.;  
}

PndPidProbability::PndPidProbability(Float_t e, Float_t mu, Float_t pi, Float_t k, Float_t p)
{
  fElectronPdf = e;
	fMuonPdf = mu;
	fPionPdf = pi;
	fKaonPdf = k;
	fProtonPdf = p;  
}
	
PndPidProbability::~PndPidProbability()
{
}

PndPidProbability* PndPidProbability::operator*(const PndPidProbability& a)
{ 
  PndPidProbability *result = new PndPidProbability(
						    a.GetElectronPdf() * this->GetElectronPdf(), 
						    a.GetMuonPdf()     * this->GetMuonPdf(),
						    a.GetPionPdf()     * this->GetPionPdf(),
						    a.GetKaonPdf()     * this->GetKaonPdf(),
						    a.GetProtonPdf()   * this->GetProtonPdf()
						    );
  return result;
}

void PndPidProbability::Print()
{
  std::cout <<"PndPidProbability:   "
  <<"p(e)="<<fElectronPdf<<"  "
  <<"p(mu)="<<fMuonPdf<<"  "
  <<"p(pi)="<<fPionPdf<<"  "
  <<"p(K)="<<fKaonPdf<<"  "
  <<"p(P)="<<fProtonPdf
  <<std::endl;
}

ClassImp(PndPidProbability)

