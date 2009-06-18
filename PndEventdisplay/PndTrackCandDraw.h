// -------------------------------------------------------------------------
// -----                      PndTrackCandDraw header file                    -----
// -----          Created 10/12/07  by M. Al-Turany                    -----
// -------------------------------------------------------------------------


/** PndTrackCandDraw
 * @author T. Stockmanns
 * @since 18.06.09
 *   Task to display track candidates
 **
 **/

#ifndef PNDTRACKCANDDRAW_H
#define PNDTRACKCANDDRAW_H


#include "FairTask.h"
#include "TClonesArray.h"

#include <string>

class FairEventManager;
class TEveQuadSet;
class PndTrackCandDraw : public FairTask {

 public:

  /** Default constructor **/
  PndTrackCandDraw();


  /** Standard constructor
  *@param name        Name of task
  *@param iVerbose    Verbosity level
  **/
  PndTrackCandDraw(const char* TrackCandName, Int_t iVerbose = 1);

  /** Destructor **/
  virtual ~PndTrackCandDraw();

  /** Set verbosity level. For this task and all of the subtasks. **/
  void SetVerbose(Int_t iVerbose);
   /** Executed task **/
  virtual void Exec(Option_t* option);
  void Reset();
protected:

  Int_t   fVerbose;       //  Verbosity level
  virtual void SetParContainers() ;
  virtual InitStatus Init();
  /** Action after each event**/
  virtual void Finish() ;
  TClonesArray *fPixPointList; //!
  TClonesArray *fStripPointList; //!
  TClonesArray *fTrackCandList; //!
  TClonesArray *fTpcClusterList;
  FairEventManager *fEventManager;   //!
  TEveQuadSet* fq;    //!
  ClassDef(PndTrackCandDraw,1);

};


#endif
