// -------------------------------------------------------------------------
// -----                   PndDchTrackFinderIdeal header file          -----
// -----                  Created 27/01/08  by A. Bubak                -----
// -----               according to the CbmStsTrackFinderIdeal         -----
// -------------------------------------------------------------------------

/** \class PndDchTrackFinderIdeal
 *  \author A.Bubak <arkadiusz.bubak@gmail.com>
 *  \date 27/01/08
 *  \brief Ideal track finding algorithm
 * 
 *  Track finding procedure operates on mc data.
 *  Reads MC truth (MCTracks and MCPoints), creates
 *  one PndTrack for each MCTrack and attaches the hits according
 *	to the MCTrack of the corresponding MCPoint
 **/


#ifndef PNDDCHTRACKFINDERIDEAL_H
#define PNDDCHTRACKFINDERIDEAL_H

#include "FairMCPoint.h"
#include "PndMCTrack.h"

#include "PndDchHit.h"
#include "PndTrackCand.h"
#include "PndTrackCandHit.h"
#include "PndDchStructure.h"
#include "PndDchTrackFinder.h"

class PndDchTrackFinderIdeal : public PndDchTrackFinder
{

 public:

  /** Default constructor **/
  PndDchTrackFinderIdeal();

  /** Destructor **/
  virtual ~PndDchTrackFinderIdeal();

  /** Initialisation **/
  virtual void Init();

  /** DoFind method
   * \param hitArray    Array of Dch hits
   * \param trackArray  Array of PndDchTrack
   * \return  Number of created tracks
   **/
  virtual Int_t DoFind(TClonesArray* hitArray,
		       TClonesArray* trackArray);

  /** Public modifiers **/
  void SetVerbose(const Int_t& verbose) { fVerbose = verbose; };
  void SetPrimary(const Int_t& primary) { fPrimary = primary; };

  
 private:

  /** Arrays of MC information **/
  TClonesArray* fMCTrackArray;
  TClonesArray* fMCPointArray;

  /** fDchStructure **/
  PndDchStructure *fDchStructure;

  /** Verbosity level.
   ** 0 - quit
   ** 1 - event level
   ** 2 - track level
   ** 3 - debug (maximal output)
   **/
  Int_t fVerbose;

  /** Primaries level
   ** 0 - All MC tracks are considered
   ** 1 - Only primary MC tracks
  **/
  Int_t fPrimary;
  
  /** Event counter **/
   Int_t fNofEvents;  ///< event counter


  ClassDef(PndDchTrackFinderIdeal,1);

};

#endif
