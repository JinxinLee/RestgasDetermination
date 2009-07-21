// -------------------------------------------------------------------------
// -----                   PndDchFindTracks header file                -----
// -----                  Created 27.01.2008  by A. Bubak              -----
// -----                 according to the CbmStsFindTracks             -----
// -------------------------------------------------------------------------


/** \class PndDchFindTracks
 *  \author A.Bubak <arkadiusz.bubak@gmail.com>
 *  \date 27.01.2008
 *  \brief Task class for track finding in the Dch
 * 
 *   Uses as track finding algorithm classes derived from PndDchTrackFinder. 
 *	 Input:  TClonesArray of PndDchHit or
 *	 Input:  TClonesArray of PndCylinderHit
 *	 Input:  TClonesArray of PndDchDigi
 *	 \return TClonesArray of PndDchTrack
 **/

#ifndef PNDGEMIDEALPATTERNRECOTASK_H
#define PNDGEMIDEALPATTERNRECOTASK_H


#include "FairTask.h"

class PndDchTrackFinder;
class TClonesArray;

class PndGemIdealPatternRecoTask : public FairTask
{

 public:

  /** Default constructor **/
  PndGemIdealPatternRecoTask();


  /** Destructor **/
  virtual ~PndGemIdealPatternRecoTask();


  /** Initialisation at beginning of each event **/
  virtual InitStatus Init();

  /** Task execution **/
  virtual void Exec(Option_t* opt);


  /** SetParContainers **/
  virtual void SetParContainers();


  /** Accessors **/


  /** Set concrete track finder **/

  /** Set use digi or hits
   ** Option: "hit/chit/digi", Default: "hit"
   **/
  

 private:

  TClonesArray* fGemHitArr;   
  TClonesArray* fMCTrackArr;   
  TClonesArray* fGemPointArr;
  TClonesArray* fPndTrackCandArr; 

  ClassDef(PndGemIdealPatternRecoTask,1);

};

#endif
