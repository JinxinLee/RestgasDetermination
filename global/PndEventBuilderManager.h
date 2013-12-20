//* $Id: */

// -------------------------------------------------------------------------
// -----                     PndEventBuilderManager header file              -----
// -----                  Created 20/09/2013 by R. Karabowicz          -----
// -------------------------------------------------------------------------


/** PndEventBuilderManager
 *@author Radoslaw Karabowicz <r.karabowicz@gsi.de>
 *@since 20/09/2013
 *@version 1.0
 **
 ** FairRoot general task for recreating events in Time-Based reconstruction mode.
 ** Various experiments should implement their own version of Event Builder.
 ** The main member of the task is vector of reconstructed events fRecoEvents.
 ** It also contains a vector of implementations of FairEventBuilders, that are
 ** responsible for feeding fRecoEvents vector via FindEvents() function.
 ** 
 ** The heart of the experiment-specific implemenations is
 ** the AnalyzeAndExtractEvents() function, which should interpret
 ** the experimental data to reconstruct events.
 **/


#ifndef PNDEVENTBUILDERMANAGER_H
#define PNDEVENTBUILDERMANAGER_H 1

#include "FairEventBuilderManager.h"

#include "TStopwatch.h"

#include <map>
#include <set>
#include <vector>

class TClonesArray;

class PndEventBuilderManager : public FairEventBuilderManager
{


 public :

  /** Default constructor **/
  PndEventBuilderManager();


  /** Constructor with task name **/
  PndEventBuilderManager(const char* name, Int_t iVerbose=1);


  /** Destructor **/
  virtual ~PndEventBuilderManager();

 private:

  FairRecoEventHeader*    fOutEvent;

  /** Get parameter containers **/
  virtual void SetParContainers();


  /** Intialisation **/
  virtual InitStatus Init();


  /** Reinitialisation **/
  virtual InitStatus ReInit();


  virtual void AnalyzeAndExtractEvents(Double_t maxEventTimeAllowed);


  ClassDef(PndEventBuilderManager,1);

};

#endif
