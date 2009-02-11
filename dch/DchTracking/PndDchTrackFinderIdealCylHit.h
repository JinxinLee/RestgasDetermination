// -------------------------------------------------------------------------
// -----                   PndDchTrackFinderIdealCylHit header file      -----
// -----                     Created 27/03/08  by A. Bubak             -----
// -------------------------------------------------------------------------


/** \class PndDchTrackFinderIdealCylHit
 *  \author A.Bubak <arkadiusz.bubak@gmail.com>
 *  \date 27/03/08
 *  \brief Ideal track finding algorithm
 *  **/

#ifndef PNDDCHTRACKFINDERIDEALCYLHIT_H
#define PNDDCHTRACKFINDERIDEALCYLHIT_H 1

#include "FairMCPoint.h"
#include "CbmMCTrack.h"

#include "PndDchCylinderHit.h"
#include "PndDchTrack.h"
#include "PndDchStructure.h"
#include "PndDchTrackFinder.h"

class PndDchTrackFinderIdealCylHit : public PndDchTrackFinder
{

public:

	/** Default constructor **/
	PndDchTrackFinderIdealCylHit();

	/** Destructor **/
	virtual ~PndDchTrackFinderIdealCylHit();

	/** Initialisation **/
	virtual void Init();

	/** DoFind method
	 * 	\param cylHitArray  Array of Dch cylinder hits
	 *  \param trackArray   Array of PndDchTracs
	 *  \return Number of created tracks
	 **/
	virtual Int_t DoFind(TClonesArray* cylHitArray,
			TClonesArray* trackArray);


	/** Public modifiers **/
	void SetVerbose(const Int_t& verbose) { fVerbose = verbose; };
	void SetPrimary(const Int_t& primary) { fPrimary = primary; };

	
private:

	/** Arrays of MC information **/
	TClonesArray* fMCTrackArray;
	TClonesArray* fMCPointArray;

	/** Arrays of Digis information **/
	TClonesArray* fDigiArray;
	
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

	ClassDef(PndDchTrackFinderIdealCylHit,1);

};

#endif
