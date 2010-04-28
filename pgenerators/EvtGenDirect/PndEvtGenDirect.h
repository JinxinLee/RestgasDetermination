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

  PndEvtGenDirect(TString particle,TString decfile="",Double_t Mom=0, Long_t Seed=-1);
  
  /** Destructor **/
  virtual ~PndEvtGenDirect();

  /** Generate one event using EVT
   ** @param primGen  pointer to the FairPrimaryGenerator
   **/
  virtual Bool_t ReadEvent(FairPrimaryGenerator* primGen);
	
 private:

  /**
   * P_lab(GeV/c)
  */
  double E, P; 
  
  EvtGen *myGenerator;
  EvtStdHep evtstdhep;
  EvtId PART;
	
  ClassDef(PndEvtGenDirect,1);

};

#endif
