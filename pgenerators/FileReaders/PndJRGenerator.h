// -------------------------------------------------------------------------
// -----                 PndJRGenerator header file                  -----
// -----               Created 05/02/07  by S.Spataro                 -----
// -----              Modified 21/06/07 by A.Galoyan                 -----      
// -------------------------------------------------------------------------

/** PndJRGenerator.h
 *@author S.Spataro <stefano.spataro@exp2.physik.uni-giessen.de>
 The PndJRGenerator reads the PndJRGen  output file (ROOT format)
 and inserts the tracks into the PndStack via the FairPrimaryGenerator.
 Derived from FairGenerator.
**/


#ifndef PND_JRGENERATOR_H
#define PND_JRGENERATOR_H


#include "FairGenerator.h"

class TClonesArray;
class TFile;
class TTree;
class TVirtualMCStack;
class FairPrimaryGenerator;



class PndJRGenerator : public FairGenerator
{

 public: 

  /** Default constructor (should not be used) **/
  PndJRGenerator();


  /** Standard constructor
   ** @param fileName The input (PLUTO) file name
   **/
  PndJRGenerator(const Char_t* fileName);


  /** Destructor **/
  virtual ~PndJRGenerator();


  /** Reads on event from the input file and pushes the tracks onto
   ** the stack. Abstract method in base class.
   ** @param primGen  pointer to the FairPrimaryGenerator
   **/
  virtual Bool_t ReadEvent(FairPrimaryGenerator* primGen);


	
 private:

  Int_t iEvent;      //! Event number
  const Char_t* fFileName;   //! Input file name
  TFile* fInputFile;        //! Pointer to input file
  TTree* fInputTree;        //! Pointer to input tree
  TClonesArray* fParticles;  //! Particle array from PLUTO

  /** Private method CloseInput. Just for convenience. Closes the 
   ** input file properly. Called from destructor and from ReadEvent. **/
  void CloseInput();


  ClassDef(PndJRGenerator,1);

};

#endif
