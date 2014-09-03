// -------------------------------------------------------------------------
// -----             PndFtfGenerator header file                  ----
// -----            edited  29/08/14 by Aida Galoyan 
//----       from the file PndDpmGenerator.h created 05/02/07 by S.Spataro     

// -------------------------------------------------------------------------

/** PndFtfGenerator.h
 *
 The PndFtfGenerator reads the FTF output file (ROOT format)
 and inserts the tracks into the PndStack via the FairPrimaryGenerator.
 Derived from FairGenerator.
**/


#ifndef PND_FTFGENERATOR_H
#define PND_FTFGENERATOR_H


#include "FairGenerator.h"

class TClonesArray;
class TFile;
class TTree;
class TVirtualMCStack;
class FairPrimaryGenerator;



class PndFtfGenerator : public FairGenerator
{

 public: 

  /** Default constructor (should not be used) **/
  PndFtfGenerator();


  /** Standard constructor
   ** @param fileName The input (PLUTO) file name
   **/
  PndFtfGenerator(const Char_t* fileName);


  /** Destructor **/
  virtual ~PndFtfGenerator();


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


  ClassDef(PndFtfGenerator,1);

};

#endif
