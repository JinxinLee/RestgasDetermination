//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      An ideal pattern recognition
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef PNDHYPDPATTERNRECOTASK_HH
#define PNDHYPDPATTERNRECOTASK_HH

// Base Class Headers ----------------
#include "FairTask.h"
#include<map>

// Collaborating Class Headers -------


// Collaborating Class Declarations --
class TClonesArray;
class GFRecoHitFactory;
class AbsBFieldIfc;
class FairField;
class FairGeanePro;

class PndHypDPatternRecoTask : public FairTask {
public:

  // Constructors/Destructors ---------
  PndHypDPatternRecoTask();
  ~PndHypDPatternRecoTask();

  // Operators
  

  // Accessors -----------------------


  // Modifiers -----------------------
  //void SetPointBranchName(const TString& name) {_pointBranchName=name;}
  void AddHitBranch(unsigned int detId, const TString& m){_hitBranchNameMap[detId]=m;};
  void SetPersistence(Bool_t opt=kTRUE) {_persistence=opt;}
  void SetField(FairField* f){_field=f;}
  void UseGeane(bool f=true){_useGeane=f;}
  Int_t GetChargeIon(Int_t ion);
  // Operations ----------------------
  
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);


private:

  // Private Data Members ------------
  //TString _pointBranchName;
  TClonesArray* pointArray;
  TClonesArray* _trackArray;
  TClonesArray* _mcArray;

  std::map<unsigned int,TString> _hitBranchNameMap;
  std::map<unsigned int,TClonesArray*> _hitBranchMap;
   int fEventNr;
  Bool_t _persistence;
  Bool_t _useGeane;

  GFRecoHitFactory* _theRecoHitFactory;
 
  FairField* _field;
  FairGeanePro* _geanePro;

  // Private Methods -----------------

public:
  ClassDef(PndHypDPatternRecoTask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
