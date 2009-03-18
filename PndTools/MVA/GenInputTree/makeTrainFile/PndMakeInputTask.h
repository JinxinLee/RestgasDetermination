#pragma once
#ifndef PNDGPIDTASKLHE_H
#define PNDGPIDTASKLHE_H

//C++ includes
#include <iostream>
#include <vector>

//ROOT and PandaRoot includes
#include "TTree.h"

#include "FairTask.h"
#include "FairRootManager.h"
#include "PndLhePidTrack.h"
#include "PndTpcPoint.h"
#include "PndPidCand.h"
//#include "PndMvdMCPoint.h"

class TClonesArray;

class PndMakeInputTask : public FairTask
{
 public:

  /*
   * Default constructor 
  */
  PndMakeInputTask();
  PndMakeInputTask(std::string treeName);
  /*
   * Destructor 
  */
  virtual ~PndMakeInputTask();
  
  /*
   * Virtual method Init
   */
  virtual InitStatus Init();
  
  /*
   * Virtual method Exec
   */
  virtual void Exec(Option_t* opt);
  
  void SetTreeName(const std::string& name){treeName = name;};
  
  // PRIVATE FUNCTIONS AND VARS
 private:
  float p, tof, emc, stt, mvd, thetaC;
  std::string treeName;
  TTree* dataTree;
  
  TClonesArray* fPidTrackCand;
  ClassDef(PndMakeInputTask,1);
};
#endif
