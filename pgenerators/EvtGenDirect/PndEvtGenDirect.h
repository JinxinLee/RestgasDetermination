// -------------------------------------------------------------------------
// -----                 PndEvtGenDirect header file                  -----
// -----               Created 11/04/08  by M.Al-Turany              -----
// -------------------------------------------------------------------------

/** PndEvtGenDirect.h
 *@author M.Al-Turany <m.al-turany@gsi.de>
 *
 The PndEvtGenDirect generates EVT event using the EVT fortran code
 and inserts the tracks into the PndStack via the FairPrimaryGenerator.
 Derived from FairGenerator.
**/

#ifndef PND_EVTDIRECT_H
#define PND_EVTDIRECT_H

#include "FairGenerator.h"
#include "TF1.h"

class TVirtualMCStack;
class FairPrimaryGenerator;

#include "EvtGenBase/EvtStdHep.hh"
#include "EvtGen.hh"

class PndEvtGenDirect : public FairGenerator
{

 public: 

  /** Default constructor (should not be used) **/
  PndEvtGenDirect();

  PndEvtGenDirect(TString particle,TString decfile="",Double_t Mom=0, Long_t Seed=-1,TString defDECAY="",TString defPDL=""); // Mom>0 -> pbar Momentum; Mom<0 -> cms Energy
  
  /** Destructor **/
  virtual ~PndEvtGenDirect();

  /** Generate one event using EVT
   ** @param primGen  pointer to the FairPrimaryGenerator
   **/
  virtual Bool_t ReadEvent(FairPrimaryGenerator* primGen);

  void SetStoreTree(Bool_t store=true){ fStoreTree=store;};
  inline void SetVerbose(int v=1){verbose=v;};

 private:

  Bool_t fStoreTree;
  Int_t verbose;
  /**
   * P_lab(GeV/c)
  */
  Double_t fEnergy;	//! Energy of System
  Double_t fMomentum; 	//! Momentum of System
  
  EvtGen *myGenerator;	//! Pointer to the actual EvtGen
  EvtStdHep evtstdhep;  //! The decay tree
  EvtId PART;		    //! The mother particle
	
  ClassDef(PndEvtGenDirect,1);

};

#endif
