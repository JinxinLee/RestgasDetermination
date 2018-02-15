// -------------------------------------------------------------------------
// -----                 PndFlukaGenerator header file                 -----
// -----                                                               -----
// -------------------------------------------------------------------------

/** PndFlukaGenerator.h
 *
 The PndFlukaGenerator reads the FluROOT output file (ROOT format)
 and inserts the tracks into the PndStack via the FairPrimaryGenerator.
 Derived from FairGenerator.
**/


#ifndef PND_FLUKAGENERATOR_H
#define PND_FLUKAGENERATOR_H


#include "FairGenerator.h"
#include "TString.h"
#include <fstream>

class TClonesArray;
class TFile;
class TTree;
class TVirtualMCStack;
class FairPrimaryGenerator;
class TF1;



class PndFlukaGenerator : public FairGenerator
{

 public: 

  /** Default constructor (should not be used) **/
  PndFlukaGenerator();


  /** Standard constructor
   ** @param fileName The input root file name
   **/
  PndFlukaGenerator(const Char_t* fileName);

  /** Destructor **/
  virtual ~PndFlukaGenerator();

  /** Reads on event from the input file and pushes the tracks onto
   ** the stack. Abstract method in base class.
   ** @param primGen  pointer to the FairPrimaryGenerator
   **/
  virtual Bool_t ReadEvent(FairPrimaryGenerator* primGen);

 protected:
 Bool_t ReadRootEvent(FairPrimaryGenerator* primGen);
 Bool_t Init();
	
 private:
  
  Bool_t SetBranchAddresses();

  Int_t iEvent;      //! Event number
  Int_t iEntry;      //! Root tree entry number
  TString fFileName;   //! Input file name
  TFile* fInputRootFile;        //! Pointer to input file
  TTree* fInputTree;        //! Pointer to input tree

  Double_t fpx, fpy, fpz;  //! Momentum of particle
  Double_t fvx, fvy, fvz;  //! Start Vertex of particle
  Int_t    fpid;             //! PDG code of particle
  Int_t    fnpart;             //! number of particles in event

  /** Private method CloseInput. Just for convenience. Closes the 
   ** input file properly. Called from destructor and from ReadEvent. **/
  void CloseInput();


  ClassDef(PndFlukaGenerator,2);

};

#endif
