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
 *	 \return TClonesArray of PndTrackCand
 **/

#ifndef PNDDCHFINDTRACKS_H
#define PNDDCHFINDTRACKS_H 1


#include "FairTask.h"

#include "TStopwatch.h"

class PndDchTrackFinder;
class TClonesArray;

class PndDchFindTracks : public FairTask
{

 public:

  /** Default constructor **/
  PndDchFindTracks();


  /** Standard constructor 
   *  \param name   Name of class
   *  \param title  Task title
   *  \param finder Finder algorithm
   **/
  PndDchFindTracks(const char* name, const char* title = "PndTask", 
		   PndDchTrackFinder* finder = NULL);


  /** Destructor **/
  virtual ~PndDchFindTracks();


  /** Initialisation at beginning of each event **/
  virtual InitStatus Init();

  /** Task execution **/
  virtual void Exec(Option_t* opt);


  /** Finish at the end of each event **/
  virtual void Finish();


  /** SetParContainers **/
  virtual void SetParContainers();


  /** Accessors **/
  PndDchTrackFinder* GetFinder() 	  { return fFinder; };
  Int_t GetNofTracks()          const { return fNofTracks; };
  TString GetUseHitOrDigi()     const { return fUseHitOrDigi; };


  /** Set concrete track finder **/
  void UseFinder(PndDchTrackFinder* finder) { fFinder = finder; };

  /** Set use digi or hits
   ** Option: "hit/chit/digi", Default: "hit"
   **/
  void SetUseHitOrDigi(TString useHitOrDigi = "hit") { fUseHitOrDigi = useHitOrDigi; };
  

 private:

  PndDchTrackFinder* fFinder;    	    ///< Pointer to TrackFinder concrete class
  TClonesArray* fDchHitArray;           ///< Input array of DCH hits
  TClonesArray* fDchCylinderHitArray;   ///< Input array of Dch cylinder hits
  TClonesArray* fDchDigiArray;          ///< Input array of Dch digis
  TClonesArray* fDchHitOrDigiArray;     //
  TClonesArray* fTrackArray;     	    ///< Output array of PndTrackCand
  Int_t fNofTracks;              	    ///< Number of created tracks
  TString fUseHitOrDigi;                ///< Choose use hits or digis, default: hits
  
  TStopwatch fTimer;
  Int_t fTNofEvents;
  Int_t fTNofTracks;
  Double_t fTTime;

  ClassDef(PndDchFindTracks,1);

};

#endif
