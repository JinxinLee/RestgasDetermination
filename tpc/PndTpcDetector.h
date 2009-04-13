//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Detector class - sensitive detector for VMC
//
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef TPCDETECTOR_HH
#define TPCDETECTOR_HH

// Base Class Headers ----------------
#include "FairDetector.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op
#include "TVector3.h"

// Collaborating Class Declarations --
class TClonesArray;
class FairVolume;
class PndTpcPoint;

class PndTpcDetector : public FairDetector{
public:	
  // Constructors/Destructors ---------
  /**      Name :  Detector Name
	   Active: kTRUE for active detectors  
	   (ProcessHits() will be called)
	   kFALSE for inactive detectors     */
  PndTpcDetector(const char * Name, Bool_t Active);
  
  // Modifiers -----------------------
  /**   use Alice Style MC    
	make one hit per collision with atom
	use other straggling
	
	WARNING:	
	   1. geant3 has to be used!
	   2. LOSS = 5 has to be set!
	   3. DCUTE und DCUTM should be set to 10 keV.	
	   4. For Digitaization: PndTpcClusterizerTask
	      tpcClusterizer->SetMereChargeConversion() has to be set!
	   5. if you do not use this option make sure 2., 4. are not set!
	      :-(	
	   6. SetMaxNStep should be set to a high value
  */ 
  void SetAliMC(Bool_t opt=kTRUE) {fAliMC=opt;}
  

  //NOT YET READY
  //if you let GEANT produce delta rays explicitely and you want to treat them in a 
  //reconstruction that is based on MC trackID's, turn on this option
  
  //void SetDeltaAttach(Bool_t opt=kTRUE) {fDeltaAttach=opt;}
  
  
  
  /**      default constructor    */
  PndTpcDetector();
  /**       destructor     */
  virtual ~PndTpcDetector();
  
  /**      Initialization of the detector is done here    */
  //virtual void   Initialize();
  
  /**       this method is called for each step 
	    during simulation (see FairMCApplication::Stepping())    */
  virtual Bool_t ProcessHits( FairVolume *v=0);
  
  /**       Registers the produced collections in CBMRootManager.     */
  virtual void   Register();
  
  /** Gets the produced collections */
  virtual TClonesArray* GetCollection(Int_t iColl) const ;
  
  /**      has to be called after each event 
	   to reset the containers        */
  virtual void   Reset();
  
  /**      Create the detector geometry        */
  void ConstructGeometry();
  
  void EndOfEvent();
  void BeginEvent();
  
  PndTpcPoint* AddHit(Int_t trackID, Int_t detID, TVector3 pos,
		   TVector3 mom, Double_t time, Double_t length,
		   Double_t eLoss);
  // Operators
  
  
  // Accessors -----------------------
  
  
  // Modifiers -----------------------
  
  
  // Operations ----------------------
  // the ALICE TPC MC


  // __________________________________________________________________________________
  // found at http://www-aix.gsi.de/~miranov/AliRoot/roothtml/TPC/src/AliTPCv3.cxx.html
  Float_t AliTPCv3_InitDetector();
  Float_t AliTPCv3_BetheBloch(Float_t bg);
  void AliTPCv3_SetStepToNextCollision();
  
private:
  
  // Private Data Members ------------
  TClonesArray  *fPndTpcPointCollection;
  bool fAliMC; //use Alice Monte Carlo, ELOSS=5 has to be set!
  bool fDeltaAttach; //assign delta MC points to the mother track
  // Private Methods -----------------
  
public:
  
  ClassDef(PndTpcDetector,1)
    
};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
