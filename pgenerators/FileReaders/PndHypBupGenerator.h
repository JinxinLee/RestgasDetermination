// -------------------------------------------------------------------------
// -----                PndHypBupGenerator header file                 -----
// -----                Created  by Alicia Sanchez                 -----
// -------------------------------------------------------------------------

/** PndHypBupGenerator
 *@author Alicia Sanchez  <a.sanchez@gsi.de>
 *
 *@version 1.0
 *
 ** The PndHypBupGenerator  uses the
 ** root output of the RAZHYP_GEN as input for simulation.
 ** The format of the event header is a THParticle ttree:

**/


#ifndef PNDHYPBUPGENERATOR_H
#define PNDHYPBUPGENERATOR_H 1

#include "THParticle.h" //detectors/hyp


#include "FairGenerator.h"

class TClonesArray;
class TFile;
class TTree;
class TVirtualMCStack;

class PndPrimaryGenerator;
//class CbmParticle;



class PndHypBupGenerator : public FairGenerator  
{

 public: 

  /** Default constructor without arguments should not be used. **/
  PndHypBupGenerator();


  /** Standard constructor. 
   ** @param fileName The input file name
   **/
  PndHypBupGenerator(const char* fileName);


  /** Destructor. **/
  virtual ~PndHypBupGenerator();

	
  /** Reads on event from the input file and pushes the tracks onto
   ** the stack. Abstract method in base class.
   ** @param primGen  pointer to the CbmPrimaryGenerator
   **/
  virtual Bool_t ReadEvent(FairPrimaryGenerator* primGen);



 private:

               //!  PDG database
  Int_t iEvent;      //! Event number
  const Char_t* fFileName;   //! Input file name
  TFile* fInputFile;        //! Pointer to input file
  TTree* fInputTree;        //! Pointer to input tree
  TClonesArray* fParticles;  //! Particle array from PLUTO

	
  /** Private method CloseInput. Just for convenience. Closes the 
   ** input file properly. Called from destructor and from ReadEvent. **/
  void CloseInput();


  /** Private method RegisterIons. Goes through the input file and registers
   ** any ion needed. **/
  
  //Int_t RegisterIons();



  /** STL map from ion name to CbmIon **/
  //std::map<TString,CbmParticle*> fIonMap;       //!
	
	
  ClassDef(PndHypBupGenerator,1);

};

#endif
