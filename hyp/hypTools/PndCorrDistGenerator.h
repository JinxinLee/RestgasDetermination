// -------------------------------------------------------------------------
// -----               PndCorrDistGenerator header file                -----
// -----               Created 23/05/07  by Aida Galoyan               -----
// -------------------------------------------------------------------------

/** PndCorrDistGenerator.h
 The PndCorrDistGenerator reads the Urqmd+SMM output file (ROOT format)
 and inserts the tracks into the PndStack via the FairPrimaryGenerator.
 Derived from FairGenerator.
**/


#ifndef PNDCORRDISTGENERATOR_H
#define PNDCORRDISTGENERATOR_H 1


#include "FairGenerator.h"
#include "THParticle.h"

// using namespace std;

class TClonesArray;
class TFile;
class TH2F;
class TTree;
class TVirtualMCStack;
class FairPrimaryGenerator;



class PndCorrDistGenerator : public FairGenerator
{

 public: 

  /** Default constructor (should not be used) **/
  PndCorrDistGenerator();


  /** Standard constructor
   ** @param fileName The input (PLUTO) file name
   **/
  PndCorrDistGenerator(const Char_t* fileName);


  /** Destructor **/
  virtual ~PndCorrDistGenerator();


  /** Reads on event from the input file and pushes the tracks onto
   ** the stack. Abstract method in base class.
   ** @param primGen  pointer to the FairPrimaryGenerator
   **/
  virtual Bool_t ReadEvent(FairPrimaryGenerator* primGen);


	
 private:

  Int_t iEvent;              //! Event number
  const Char_t* fFileName;   //! Input file name
  TFile* fInputFile;         //! Pointer to input file
  TH2F* fInputHist;         //! Pointer to input histogramm 2D
  TClonesArray* fParticles;  //! Particle array from PLUTO
  Int_t fPdgType;

  /** Private method CloseInput. Just for convenience. Closes the 
   ** input file properly. Called from destructor and from ReadEvent. **/
  void CloseInput();


  ClassDef(PndCorrDistGenerator,1);

};

#endif
