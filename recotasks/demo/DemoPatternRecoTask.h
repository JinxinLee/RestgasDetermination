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

#ifndef DEMOPRTASK_HH
#define DEMOPRTASK_HH

// Base Class Headers ----------------
#include "CbmTask.h"
#include<map>

// Collaborating Class Headers -------


// Collaborating Class Declarations --
class TClonesArray;
class RecoHitFactory;
class AbsBFieldIfc;
class CbmField;
class CbmGeanePro;

class DemoPatternRecoTask : public CbmTask {
public:

  // Constructors/Destructors ---------
  DemoPatternRecoTask();
  ~DemoPatternRecoTask();

  // Operators
  

  // Accessors -----------------------


  // Modifiers -----------------------
  //void SetPointBranchName(const TString& name) {_pointBranchName=name;}
  void AddHitBranch(unsigned int detId, const TString& m){_hitBranchNameMap[detId]=m;};
  void SetPersistence(Bool_t opt=kTRUE) {_persistence=opt;}
  void SetField(CbmField* f){_field=f;}
  void UseGeane(bool f=true){_useGeane=f;}

  // Operations ----------------------
  
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);


private:

  // Private Data Members ------------
  //TString _pointBranchName;
  //TClonesArray* _pointArray;
  TClonesArray* _trackArray;
  TClonesArray* _mcArray;

  std::map<unsigned int,TString> _hitBranchNameMap;
  std::map<unsigned int,TClonesArray*> _hitBranchMap;
  
  Bool_t _persistence;
  Bool_t _useGeane;

  RecoHitFactory* _theRecoHitFactory;
 
  CbmField* _field;
  CbmGeanePro* _geanePro;

  // Private Methods -----------------

public:
  ClassDef(DemoPatternRecoTask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
