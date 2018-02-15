// -------------------------------------------------------------------------
// -----                 PndDpmDirect header file                  -----
// -----               Created 11/04/08  by M.Al-Turany              -----
// -------------------------------------------------------------------------

/** PndDpmDirect.h
 *@author M.Al-Turany <m.al-turany@gsi.de>
 *
 The PndDpmDirect generates DPM event using the DPM fortran code
 and inserts the tracks into the PndStack via the FairPrimaryGenerator.
 Derived from FairGenerator.
**/

#ifndef PND_DPMDIRECT_H
#define PND_DPMDIRECT_H

#include "FairGenerator.h"
#include "TF1.h"

class TVirtualMCStack;
class FairPrimaryGenerator;



class PndDpmDirect : public FairGenerator
{

 public: 

  /** Default constructor (should not be used) **/
  PndDpmDirect();


  /** Standard constructor
   *  @param Mom in GeV/C
   *  @param Mode = 0. - No elastic scattering, only inelastic
   *  @param Mode = 1. - Elastic and inelastic interactions
   *  @param Mode = 2. - Only elastic scattering, no inelastic one
   **/
  PndDpmDirect(Double_t Mom, Int_t Mode, Long_t Seed = -1);
  PndDpmDirect(Double_t Mom, Int_t Mode, Long_t Seed, Double_t ThtMin);
  PndDpmDirect(Double_t Mom, Int_t Mode, Double_t Rsigma, TF1* DensityFunction, Long_t Seed = -1, Double_t ThtMin=0.001);

  /** Destructor **/
  virtual ~PndDpmDirect();

  /** Generate one event using DPM
   ** @param primGen  pointer to the FairPrimaryGenerator
   **/
  virtual Bool_t ReadEvent(FairPrimaryGenerator* primGen);
	
  void SetUnstable(int pdg);
  void SetStable(int pdg);
  
  
 private:

  /**
   * P_lab(GeV/c)
  */
  double fMom; 
  /** 
  * 0. - No elastic scattering, only inelastic
  * 1. - Elastic and inelastic interactions
  * 2. - Only elastic scattering, no inelastic one
  */
  double  fMode;    
  double fSeed;
  int    fGasmode;
	double fRsigma;
  double fThtMin;
  
  TF1*   fDensityFunction; //!
	
  ClassDef(PndDpmDirect,1);

};

#endif
