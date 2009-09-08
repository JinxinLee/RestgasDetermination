#ifndef PNDPIDPROBABILITY_H
#define PNDPIDPROBABILITY_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// PndPidProbability	                                                  //
//                                                                      //
// Definition of the Panda pid probabilities .	                        //
//                                                                      //
// Author: Ralf Kliemt, Dresden/Turin/Bonn, 01.09.09                    //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <iostream>
//#include <assert.h>
#include "TObject.h"

class PndPidProbability : public TObject 
{

 public:

  PndPidProbability();
  PndPidProbability(Float_t e, Float_t mu, Float_t pi, Float_t k, Float_t p);
  ~PndPidProbability();

	Float_t		GetElectronPidProb() const { return fElectronPidProb; }
	Float_t		GetMuonPidProb()     const { return fMuonPidProb; }
	Float_t		GetPionPidProb()     const { return fPionPidProb; } 
	Float_t		GetKaonPidProb()     const { return fKaonPidProb; }
	Float_t		GetProtonPidProb()   const { return fProtonPidProb; }
  
	void	SetElectronPidProb(Double_t val) { fElectronPidProb= (Float_t) val; }
	void	SetMuonPidProb(Double_t val)     { fMuonPidProb=     (Float_t) val; }
	void	SetPionPidProb(Double_t val)     { fPionPidProb=     (Float_t) val; } 
	void	SetKaonPidProb(Double_t val)     { fKaonPidProb=     (Float_t) val; }
	void	SetProtonPidProb(Double_t val)   { fProtonPidProb=   (Float_t) val; }
  
  void Print();
  
protected:

	Float_t		fElectronPidProb;
	Float_t		fMuonPidProb;
	Float_t		fPionPidProb;
	Float_t		fKaonPidProb;
	Float_t		fProtonPidProb;
  
  ClassDef(PndPidProbability,1) // Abstract base class for MicroDST candidates

};

#endif                                           


