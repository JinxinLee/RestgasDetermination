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

#ifndef PNDDCHCONVERTTRACKCANDTASK_H
#define PNDDCHCONVERTTRACKCANDTASK_H


#include "FairTask.h"

class PndDchTrackFinder;
class TClonesArray;

class PndDchConvertTrackCandTask : public FairTask
{

 public:

  /** Default constructor **/
  PndDchConvertTrackCandTask();


  /** Destructor **/
  virtual ~PndDchConvertTrackCandTask();


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

  TClonesArray* fMCTrackArr;   
  TClonesArray* fCylHitArr;   
  TClonesArray* fDchTrackArr;   
  TClonesArray* fDchTrackMatchArr;
  TClonesArray* fPndTrackCandArr; 

  ClassDef(PndDchConvertTrackCandTask,1);

};

#endif
