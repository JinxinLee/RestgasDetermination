// -------------------------------------------------------------------------
// -----                   PndGemTrackFinderOnHits header file          -----
// -----                  Created 02.06.2009 by R. Karabowicz          -----
// -------------------------------------------------------------------------

/** \class PndGemTrackFinderOnHits
 *  \author R. Karabowicz <r.karabowicz@gsi.de>
 *  \date 19.03.2009
 *  \brief OnHits track finding algorithm
 * 
 *  Track finding procedure operates on mc data.
 *  Reads MC truth (MCTracks and MCPoints), creates
 *  one PndTrack for each MCTrack and attaches the hits according
 *	to the MCTrack of the corresponding MCPoint
 **/


#ifndef PNDGEMTRACKFINDERONHITS_H
#define PNDGEMTRACKFINDERONHITS_H

#include "FairMCPoint.h"
#include "PndMCTrack.h"

#include "PndGemHit.h"
#include "PndGemTrack.h"
#include "PndGemTrackFinder.h"

class PndGemTrackFinderOnHits : public PndGemTrackFinder
{

 public:

  /** Default constructor **/
  PndGemTrackFinderOnHits();

  /** Destructor **/
  virtual ~PndGemTrackFinderOnHits();

  /** Initialisation **/
  virtual void Init();

  /** DoFind method
   * \param hitArray    Array of Gem hits
   * \param trackArray  Array of PndGemTrack
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

  /** fGemStructure **/
  //  PndGemStructure *fGemStructure;

  /** Track candidate **/
  PndGemTrack *fGemTrack; 

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
  
  Int_t fNofRecoTracks;
  Int_t fGoodRecoTracks;
  Int_t fGhostRecoTracks;
  Int_t fCloneRecoTracks;


  ClassDef(PndGemTrackFinderOnHits,1);

};

#endif
