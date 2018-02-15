// -------------------------------------------------------------------------
// -----                 PndEvtGenGenerator header file                  -----
// -----               Created 05/02/07  by S.Spataro                 -----
// -------------------------------------------------------------------------

/** PndEvtGenGenerator.h
 *@author K. Goetzen <k.goetzen@gsi.de>
 *
 The PndEvtGenGenerator reads the simpleEvtGenRO output file (ROOT format)
 and inserts the tracks into the PndStack via the FairPrimaryGenerator.
 Derived from FairGenerator.
**/


#ifndef PND_EVTGENGENERATOR_H
#define PND_EVTGENGENERATOR_H


#include "FairGenerator.h"
#include "TString.h"
#include <fstream>

class TClonesArray;
class TFile;
class TTree;
class TVirtualMCStack;
class FairPrimaryGenerator;
class TF1;



class PndEvtGenGenerator : public FairGenerator
{

 public: 

  /** Default constructor (should not be used) **/
  PndEvtGenGenerator();


  /** Standard constructor
   ** @param fileName The input root file name
   **/
  PndEvtGenGenerator(const Char_t* fileName);
  PndEvtGenGenerator(const Char_t* fileName, Double_t Rsigma, TF1 * DensityFunction);


  /** Destructor **/
  virtual ~PndEvtGenGenerator();


  /** Reads on event from the input file and pushes the tracks onto
   ** the stack. Abstract method in base class.
   ** @param primGen  pointer to the FairPrimaryGenerator
   **/
  virtual Bool_t ReadEvent(FairPrimaryGenerator* primGen);

 protected:
 Bool_t ReadAsciiEvent(FairPrimaryGenerator* primGen);
 Bool_t ReadRootEvent(FairPrimaryGenerator* primGen);
 Bool_t Init();
	
 private:
  
  Bool_t SetBranchAddresses();

  Int_t iEvent;      //! Event number
  TString fFileName;   //! Input file name
  TFile* fInputRootFile;        //! Pointer to input file
  TTree* fInputTree;        //! Pointer to input tree
  FILE *fInputAsciiFile;
  
  Double_t *fRPx, *fRPy, *fRPz;  //! Momentum of particle
  Double_t *fRVx, *fRVy, *fRVz;  //! Start Vertex of particle
  Int_t    *fRPdg;             //! PDG code of particle
  Int_t    *fRDF, *fRDL;       //! Number of daughters
  Int_t    fRNTrk;             //! number of particles in event
  
  Bool_t fFileType;         //! 0: ASCII, 1:ROOT
  
  int    fGasmode;          //! Gas mode (vertex smearing)
  double fRsigma;           //! sigma for vertex smearing
  
  TF1*   fDensityFunction;  //!

  /** Private method CloseInput. Just for convenience. Closes the 
   ** input file properly. Called from destructor and from ReadEvent. **/
  void CloseInput();


  ClassDef(PndEvtGenGenerator,1);

};

#endif
