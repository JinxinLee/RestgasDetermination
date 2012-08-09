// -------------------------------------------------------------------------
// -----                 PndGiBuuGenerator header file                  -----
// -----               Created 02/08/12  by T.Stockmanns                -----
// -------------------------------------------------------------------------

/** PndGiBuuGenerator.h
 *@author T. Stockmanns <t.stockmanns@fz-juelich.de>
 *
 The PndGiBuuGenerator reads the GiBuu output file (ASCII format)
 and inserts the tracks into the PndStack via the FairPrimaryGenerator.
 Derived from FairGenerator.
**/


#ifndef PND_GiBuuGENERATOR_H
#define PND_GiBuuGENERATOR_H


#include "FairGenerator.h"
#include "PndGiBuuDecayerAbs.h"

#include "TString.h"
#include <fstream>
#include <map>

class FairPrimaryGenerator;



class PndGiBuuGenerator : public FairGenerator
{

 public: 


  /** Standard constructor
   ** @param fileName The input root file name
   ** @param pidDataFileName Translation between GiBuu particle Id and Pdg particle Id file
   **/
  PndGiBuuGenerator(const Char_t* fileName, const Char_t* pidDataFileName);


  /** Destructor **/
  virtual ~PndGiBuuGenerator();


  /** Reads on event from the input file and pushes the tracks onto
   ** the stack. Abstract method in base class.
   ** @param primGen  pointer to the FairPrimaryGenerator
   **/
  virtual Bool_t ReadEvent(FairPrimaryGenerator* primGen);

  void RegisterDecayer(Int_t pdgCode, PndGiBuuDecayerAbs* decayer){ fDecayerMap[pdgCode] = decayer; }

 protected:
 Bool_t Init();
	
 private:
 /** Default constructor (should not be used) **/
 PndGiBuuGenerator() {};

 void FillPidMap();
 Int_t GetPdgParticleId(Int_t GiBuuId, Int_t charge);

 std::map<std::pair<Int_t, Int_t>, Int_t > fMapGiBuuToPdg; //!
  
  Int_t fEvent;      //! Event number
  Int_t fRunId;			//! Run number in GiBuu
  TString fFileName;   //! Input file name
  TString fPidDataFileName; //! Input Pid data file name
  std::ifstream *fInputAsciiFile; //!

  std::map<Int_t, PndGiBuuDecayerAbs*> fDecayerMap; //!

  Double_t fPx, fPy, fPz;  //! Momentum of particle
  Int_t    fPdg;           //! PDG code of particle

  void CloseInput();


  ClassDef(PndGiBuuGenerator,1);

};

#endif
