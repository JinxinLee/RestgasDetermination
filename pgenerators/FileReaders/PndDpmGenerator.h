// -------------------------------------------------------------------------
// -----                 PndDpmGenerator header file                  -----
// -----               Created 05/02/07  by S.Spataro                 -----
// -------------------------------------------------------------------------

/** PndDpmGenerator.h
 *@author S.Spataro <stefano.spataro@exp2.physik.uni-giessen.de>
 *
 The PndDpmGenerator reads the DPM output file (ROOT format)
 and inserts the tracks into the PndStack via the FairPrimaryGenerator.
 Derived from FairGenerator.
**/


#ifndef PND_DPMGENERATOR_H
#define PND_DPMGENERATOR_H


#include "FairGenerator.h"

class TClonesArray;
class TFile;
class TTree;
class TVirtualMCStack;
class FairPrimaryGenerator;



class PndDpmGenerator : public FairGenerator
{

 public: 

  /** Default constructor (should not be used) **/
  PndDpmGenerator();


  /** Standard constructor
   ** @param fileName The input (PLUTO) file name
   **/
  PndDpmGenerator(const Char_t* fileName);


  /** Destructor **/
  virtual ~PndDpmGenerator();


  /** Reads on event from the input file and pushes the tracks onto
   ** the stack. Abstract method in base class.
   ** @param primGen  pointer to the FairPrimaryGenerator
   **/
  virtual Bool_t ReadEvent(FairPrimaryGenerator* primGen);

  void SetStartEvent(Int_t start) { iEvent = start; };
	
 private:

  Int_t iEvent;      //! Event number
  const Char_t* fFileName;   //! Input file name
  TFile* fInputFile;        //! Pointer to input file
  TTree* fInputTree;        //! Pointer to input tree
  TClonesArray* fParticles;  //! Particle array from PLUTO

  /** Private method CloseInput. Just for convenience. Closes the 
   ** input file properly. Called from destructor and from ReadEvent. **/
  void CloseInput();


  ClassDef(PndDpmGenerator,1);

};

#endif
