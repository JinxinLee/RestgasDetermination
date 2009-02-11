// -------------------------------------------------------------------------
// -----                  PndDchMatchTracks header file                -----
// -------------------------------------------------------------------------

/** \class PndDchMatchTracks
 *  \author A.Bubak <arkadiusz.bubak@gmail.com>
 *  \date 15/03/08
 *  \brief Task class for matching a reconstructed PndDchTrack with a simulated one
 * 
 *  Task class for matching a reconstructed PndDchTrack with a simulated
 *  CbmMCTrack. The matching criterion is a maximal number of common
 *  hits/digis/chits//points. The task fills the data class PndDchTrackMatch for
 *  each PndDchTrack.
 **/

#ifndef PNDDCHMATCHTRACKS_H
#define PNDDCHMATCHTRACKS_H 1

// Pnd includes
#include "FairTask.h"

// C++ includes
#include <map>

class TClonesArray;



class PndDchMatchTracks : public FairTask
{

public:

	/** Default constructor **/
	PndDchMatchTracks();


	/** Constructor with verbosity level **/
	PndDchMatchTracks(Int_t verbose);


	/** Constructor with name, title, digi/hits usage and verbosity
	 *  \param name       Name of taks
	 *  \param title      Title of task   (default FairTask)
	 *  \param hitOrDigi  Choose: Use digi, hit, chit
	 *  \param verbose    Verbosity level (default 1)
	 **/
	PndDchMatchTracks(const char* name, const char* title = "FairTask",
			TString hitOrDigi = "hit", Int_t verbose = 1);


	/** Destructor **/
	virtual ~PndDchMatchTracks();


	/** Intialisation at beginning of each event **/
	virtual InitStatus Init();


	/** Execution **/
	virtual void Exec(Option_t* opt);


	/** Finishing **/
	virtual void Finish();

	
	/** Set use Digi or hits
	 ** Option: "hit" or "digi" or "chit", Default: "hit"
	 **/
	void SetUseHitOrDigi(TString useHitOrDigi = "hit") { fUseHitOrDigi = useHitOrDigi; };

	
	/** Accessors **/
	TString GetUseHitOrDigi() const { return fUseHitOrDigi; };

		
private:

	TClonesArray* fTracks;       ///< Array of PndDchTracks
	TClonesArray* fPoints;       ///< Array of FairMCPoints
	TClonesArray* fHits;         ///< Array of PndDchHits
	TClonesArray* fCylinderHits; ///< Array of PndDchCylinderHits
	TClonesArray* fDigis;		 ///< Array of PndDchDigis
	TClonesArray* fHorDs;		 ///< Array of PndDchHits/PndDchDigis/PndDchCylinderHits
	TClonesArray* fMatches;      ///< Array of PndDchTrackMatch

	/** Map from MCTrackID to number of common hits **/
	std::map<Int_t, Int_t> fMatchMap;


	/** Hits or Digi Use **/
	TString fUseHitOrDigi;            ///< Choose use digi/hit/chit, default: hit	


	ClassDef(PndDchMatchTracks,1);

};

#endif
