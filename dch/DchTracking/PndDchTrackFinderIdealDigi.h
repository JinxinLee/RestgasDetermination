// -------------------------------------------------------------------------
// -----                   PndDchTrackFinderIdealDigi header file           -----
// -----                     Created 8/03/08  by A. Bubak              -----
// -------------------------------------------------------------------------


/** \class PndDchTrackFinderIdealDigi
 *  \author A.Bubak <arkadiusz.bubak@gmail.com>
 *  \date 8/03/08
 *  \brief Ideal track finding algorithm
 *  **/

#ifndef PNDDCHTRACKFINDERIDEALDIGI_H
#define PNDDCHTRACKFINDERIDEALDIGI_H

#include "FairMCPoint.h"
#include "CbmMCTrack.h"

#include "PndDchHit.h"
#include "PndDchTrack.h"
#include "PndDchStructure.h"
#include "PndDchTrackFinder.h"

class PndDchTrackFinderIdealDigi : public PndDchTrackFinder
{

public:

	/** Default constructor **/
	PndDchTrackFinderIdealDigi();

	/** Destructor **/
	virtual ~PndDchTrackFinderIdealDigi();

	/** Initialisation **/
	virtual void Init();

	/** DoFind method
	 * 	\param digiArray   Array of Dch digis
	 *  \param trackArray  Array of PndDchTracs
	 *  \return Number of created tracks
	 **/
	virtual Int_t DoFind(TClonesArray* digiArray,
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

	/** Track candidate **/
	PndDchTrack *fDchTrack;

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

	ClassDef(PndDchTrackFinderIdealDigi,1);

};

#endif
