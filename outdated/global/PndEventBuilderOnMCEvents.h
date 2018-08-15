//* $Id: */

// -------------------------------------------------------------------------
// -----                     PndEventBuilderOnMCEvents header file                -----
// -----                  Created 22/08/2013 by R. Karabowicz          -----
// -------------------------------------------------------------------------


/** PndEventBuilderOnMCEvents
 *@author Radoslaw Karabowicz <r.karabowicz@gsi.de>
 *@since 22/08/2013
 *@version 1.0
 **
 ** PANDA ideal event builder from ideal event
 ** information from FairEventHeader.
 ** Task level RECO
 **/


#ifndef PNDEVENTBUILDERONMCEVENTS_H
#define PNDEVENTBUILDERONMCEVENTS_H 1

#include "FairEventBuilder.h"

#include "TStopwatch.h"

#include <map>
#include <set>
#include <vector>

class TClonesArray;

class FairRecoEventHeader;

class PndEventBuilderOnMCEvents : public FairEventBuilder
{


 public :

  /** Default constructor **/
  PndEventBuilderOnMCEvents();

  /** Constructor with task name **/
  PndEventBuilderOnMCEvents(const char* name, Int_t iVerbose=0);


  /** Destructor **/
  virtual ~PndEventBuilderOnMCEvents();


  virtual void          StoreEventData(FairRecoEventHeader* recoEvent);

  std::vector<std::pair<double, FairRecoEventHeader*> > FindEvents();

  std::vector<std::pair<double, FairRecoEventHeader*> > Modify(std::pair<double, FairRecoEventHeader*> oldData, std::pair<double, FairRecoEventHeader*> newData);
  void AddNewDataToTClonesArray(FairTimeStamp* data);
  double FindTimeForData(FairTimeStamp* data);
  void FillDataMap(FairTimeStamp* data, double activeTime);
  void EraseDataFromDataMap(FairTimeStamp* data);

 private:

  FairEventHeader*  fEventHeader;

  Int_t             fTNofEvents;
  Int_t             fTNofRecoEvents;

  TStopwatch fTimer;
  Double_t   fExecTime;

  std::map<FairRecoEventHeader,double> fEvent_map;
  std::map<FairRecoEventHeader,double> fData_map;

  /** Get parameter containers **/
  virtual void SetParContainers();


  /** Intialisation **/
  virtual Bool_t Init();


  /** Reinitialisation **/
  virtual Bool_t ReInit();

  virtual void Print();

  /** Finish at the end of each event **/
  virtual void Finish();

  ClassDef(PndEventBuilderOnMCEvents,1);

};

#endif
