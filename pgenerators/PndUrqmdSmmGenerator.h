// -------------------------------------------------------------------------
// -----               PndUrqmdSmmGenerator header file                -----
// -----               Created 23/05/07  by Aida Galoyan               -----
// -------------------------------------------------------------------------

/** PndUrqmdSmmGenerator.h
 The PndUrqmdSmmGenerator reads the Urqmd+SMM output file (ROOT format)
 and inserts the tracks into the CbmStack via the CbmPrimaryGenerator.
 Derived from CbmGenerator.
**/


#ifndef PND_URQMDSMMGENERATOR_H
#define PND_URQMDSMMGENERATOR_H


#include "CbmGenerator.h"

// using namespace std;

class TClonesArray;
class TFile;
class TTree;
class TVirtualMCStack;
class CbmPrimaryGenerator;



class PndUrqmdSmmGenerator : public CbmGenerator
{

 public: 

  /** Default constructor (should not be used) **/
  PndUrqmdSmmGenerator();


  /** Standard constructor
   ** @param fileName The input (PLUTO) file name
   **/
  PndUrqmdSmmGenerator(const Char_t* fileName);


  /** Destructor **/
  virtual ~PndUrqmdSmmGenerator();


  /** Reads on event from the input file and pushes the tracks onto
   ** the stack. Abstract method in base class.
   ** @param primGen  pointer to the CbmPrimaryGenerator
   **/
  virtual Bool_t ReadEvent(CbmPrimaryGenerator* primGen);


	
 private:

  Int_t iEvent;              //! Event number
  const Char_t* fFileName;   //! Input file name
  TFile* fInputFile;         //! Pointer to input file
  TTree* fInputTree;         //! Pointer to input tree
  TClonesArray* fParticles;  //! Particle array from PLUTO

  /** Private method CloseInput. Just for convenience. Closes the 
   ** input file properly. Called from destructor and from ReadEvent. **/
  void CloseInput();


  ClassDef(PndUrqmdSmmGenerator,1);

};

#endif
