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
#include "TObject.h"

class PndPidProbability : public TObject 
{

 public:

  PndPidProbability();
  PndPidProbability(Float_t e, Float_t mu, Float_t pi, Float_t k, Float_t p, Int_t idx = -1);
  ~PndPidProbability();

  Float_t		GetElectronPdf() const { return fElectronPdf; }
  Float_t		GetMuonPdf()     const { return fMuonPdf; }
  Float_t		GetPionPdf()     const { return fPionPdf; } 
  Float_t		GetKaonPdf()     const { return fKaonPdf; }
  Float_t		GetProtonPdf()   const { return fProtonPdf; }
  Int_t                 GetIndex()       const { return fIndex;}

  Float_t   GetElectronPidProb(PndPidProbability* flux = new PndPidProbability(0.2,0.2,0.2,0.2,0.2)) const { return fElectronPdf * flux->GetElectronPdf() / GetSumProb(flux); }
  Float_t   GetMuonPidProb    (PndPidProbability* flux = new PndPidProbability(0.2,0.2,0.2,0.2,0.2)) const { return fMuonPdf     * flux->GetMuonPdf() / GetSumProb(flux); }
  Float_t   GetPionPidProb    (PndPidProbability* flux = new PndPidProbability(0.2,0.2,0.2,0.2,0.2)) const { return fPionPdf     * flux->GetPionPdf() / GetSumProb(flux); } 
  Float_t   GetKaonPidProb    (PndPidProbability* flux = new PndPidProbability(0.2,0.2,0.2,0.2,0.2)) const { return fKaonPdf     * flux->GetKaonPdf() / GetSumProb(flux); }
  Float_t   GetProtonPidProb  (PndPidProbability* flux = new PndPidProbability(0.2,0.2,0.2,0.2,0.2)) const { return fProtonPdf   * flux->GetProtonPdf() / GetSumProb(flux); }

  Float_t   GetSumProb        (PndPidProbability* flux = new PndPidProbability(0.2,0.2,0.2,0.2,0.2)) const 
  { 
    return 
      fElectronPdf * flux->GetElectronPdf() + 
      fMuonPdf     * flux->GetMuonPdf()     +
      fPionPdf     * flux->GetPionPdf()     +
      fKaonPdf     * flux->GetKaonPdf()     +
      fProtonPdf   * flux->GetProtonPdf(); 
  }
  
  void	SetElectronPdf(Double_t val) { fElectronPdf= (Float_t) val; }
  void	SetMuonPdf(Double_t val)     { fMuonPdf=     (Float_t) val; }
  void	SetPionPdf(Double_t val)     { fPionPdf=     (Float_t) val; } 
  void	SetKaonPdf(Double_t val)     { fKaonPdf=     (Float_t) val; }
  void	SetProtonPdf(Double_t val)   { fProtonPdf=   (Float_t) val; }
  void  SetIndex(Int_t idx)          { fIndex = idx; }

  PndPidProbability& operator*=(const PndPidProbability& a);
  PndPidProbability operator*(const PndPidProbability& a);
  
  void Print();
  void Reset();
 protected:
  
  Float_t		fElectronPdf; // e  Probability density function
  Float_t		fMuonPdf;     // mu Probability density function
  Float_t		fPionPdf;     // pi Probability density function
  Float_t		fKaonPdf;     // k  Probability density function
  Float_t		fProtonPdf;   // p  Probability density function
  Int_t                 fIndex;       // Candidate Index
  ClassDef(PndPidProbability,2) // 
    
    };



#endif                                           


