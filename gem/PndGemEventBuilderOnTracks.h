//* $Id: */

// -------------------------------------------------------------------------
// -----                     PndGemEventBuilderOnTracks header file                -----
// -----                  Created 22/08/2013 by R. Karabowicz          -----
// -------------------------------------------------------------------------


/** PndGemEventBuilderOnTracks
 *@author Radoslaw Karabowicz <r.karabowicz@gsi.de>
 *@since 22/08/2013
 *@version 1.0
 **
 ** PANDA task class for event building basing on reconstructed GEM tracks
 ** Task level RECO
 ** Produces ... - do not know what yet
 **/


#ifndef PNDGEMEVENTBUILDERONTRACKS_H
#define PNDGEMEVENTBUILDERONTRACKS_H 1


#include "FairTask.h"

#include "TStopwatch.h"

#include <map>
#include <set>
#include <vector>

class TClonesArray;
class PndTrack;

struct RecoEvent {
  Int_t    nofTracks;
  Double_t meanTime;
};

class PndGemEventBuilderOnTracks : public FairTask
{


 public :

  /** Default constructor **/
  PndGemEventBuilderOnTracks();


  /** Standard constructor **/
  PndGemEventBuilderOnTracks(Int_t iVerbose);


  /** Constructor with task name **/
  PndGemEventBuilderOnTracks(const char* name, Int_t iVerbose=0);


  /** Destructor **/
  virtual ~PndGemEventBuilderOnTracks();


  /** Execution **/
  virtual void Exec(Option_t* opt);

 private:

  TClonesArray*     fGemTracks;

  Int_t             fTNofEvents;
  Int_t             fTNofTracks;
  Int_t             fTNofRecoEvents;

  TStopwatch fTimer;
  Double_t fExecTime;
  std::vector<RecoEvent> fRecoEvents;

  /** Get parameter containers **/
  virtual void SetParContainers();


  /** Intialisation **/
  virtual InitStatus Init();


  /** Reinitialisation **/
  virtual InitStatus ReInit();


  /** Finish at the end of each event **/
  virtual void Finish();

  Int_t CompareTrackToPreviousEvents(PndTrack* tempTrack);

  ClassDef(PndGemEventBuilderOnTracks,1);

};

#endif
