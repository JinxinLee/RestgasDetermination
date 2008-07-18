// -------------------------------------------------------------------------
// -----                   PndDchTrackFinder header file               -----
// -----                  Created 27.01.2008 by A. Bubak               -----
// -----                according to the CbmStsTrackFinder             -----
// -------------------------------------------------------------------------


/** \class PndDchTrackFinder
 *  \author A.Bubak <arkadiusz.bubak@gmail.com>
 *  \date 27.01.2008
 *  \brief Abstract base class for concrete Dch track finding algorithm.
 * 
 *  Each derived class must implement the method DoFind. This has
 *  to operate on the TClonesArrays of Pnd hits/digis and to fill
 *  the PndDchTrackArray.
 **/

#ifndef PNDDCHTRACKFINDER
#define PNDDCHTRACKFINDER 1

#include "TObject.h"

class TClonesArray;

class PndDchTrackFinder : public TObject
{

public:

	/** Default constructor **/
	PndDchTrackFinder() { };


	/** Destructor **/
	virtual ~PndDchTrackFinder() { };


	/** Virtual method Init. If needed, to be implemented in the
	 *  concrete class.
	 **/
	virtual void Init() { };


	/** Abstract method DoFind. To be implemented in the concrete class.
	 *  Task: Read the hit/digi array and fill the track array,
	 *  pointers to which are given as arguments
	 * 
	 *  \param hitOrDigiArray   Array of Pnd hits/digis
	 *  \param trackArray       Array of PndDchTrack
	 *  \return Number of tracks created
	 **/
	virtual Int_t DoFind(TClonesArray* hitOrDigiArray,
			TClonesArray* trackArray) = 0;


	ClassDef(PndDchTrackFinder,1);

};

#endif
