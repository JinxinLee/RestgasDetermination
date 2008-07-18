// -------------------------------------------------------------------------
// -----                  PndDchFindTracksQa header file               -----
// -------------------------------------------------------------------------

/** \class PndDchFindTracksQa
 *  \author A.Bubak <arkadiusz.bubak@gmail.com>
 *  \date 15.03.2008
 *  \brief Quality check task for PndDchFindTracks
 **/

#ifndef PNDDCHFINDTRACKSQA_H
#define PNDDCHFINDTRACKSQA_H 1

// Pnd includes
#include "CbmTask.h"

// C++ includes
#include <set>
#include <map>

// Root declaration
class TVector3;
class TClonesArray;
class TH1;
class TH1F;
class TList;


class PndDchFindTracksQa : public CbmTask
{

public:
	/** Default constructor **/
	PndDchFindTracksQa();

	/** Standard constructor
	 * \param quota        True/all hits for track to be considered reconstructed
	 * \param minPoints    Minimal number of MCPoints for considered MCTracks
	 * \param useHitOrDigi Choose usage of digi, hit, chit: default is "hit"
	 * \param verbose      Verbosity level
	 **/
	PndDchFindTracksQa(Double_t quota, Int_t minPoints, TString useHitOrDigi, Int_t verbose = 1);

	/** Destructor **/
	virtual ~PndDchFindTracksQa();


	/** Initialisation at beginning of each event **/
	virtual InitStatus Init();

	/** Execution **/
	virtual void Exec(Option_t* opt);

	/** Accessors **/
	TString GetUseHitOrDigi() const { return fUseHitOrDigi; };

	/** Set use digi or hits
	 ** Option: "hit", "digi" or "chit", Default: "hit"
	 **/
	void SetUseHitOrDigi(TString useHitOrDigi = "hit") { fUseHitOrDigi = useHitOrDigi; };

private:
	/** Finish **/
	virtual void Finish();

	/** Create histograms **/
	void CreateHistos();

	/** Reset histograms and counters **/
	void Reset();

	/** Divide histograms (reco/all) with correct error for the efficiency
	 * \param histo1  reconstructed tracks
	 * \param histo2  all tracks (normalisation)
	 * \param histo3  efficiency
	 **/
	void DivideHistos(TH1* histo1, TH1* histo2, TH1* histo3);


	/** Map from MCTrack index to matched TrdTrack index **/
	std::map<Int_t, Int_t> fMatchMap;

	/** Map from MCTrack index to percentage of matched hits **/
	std::map<Int_t, Double_t> fQualiMap;

	/** Pointers to data arrays **/
	TClonesArray* fMCTracks;        ///<pointer to MCtrack array
	TClonesArray* fTracks;          ///<pointer to PndDchTrack
	TClonesArray* fMatches;         ///<pointer to DchTrackMatch

	/** Task parameters **/
	Double_t fQuota;  //!< True/all hits for track to be considered reconstructed
	Int_t fMinPoints; //!< Minimal number of MCPoints for considered MCTrack

	/** Histograms **/
	TH1F* fhMomAccAll,  *fhMomRecAll,  *fhMomEffAll;   // eff. vs. p, all
	TH1F* fhMomAccPrim, *fhMomRecPrim, *fhMomEffPrim;  // eff. vs. p, vertex
	TH1F* fhMomAccSec,  *fhMomRecSec,  *fhMomEffSec;   // eff. vs. p, non-vertex
	TH1F* fhNpAccAll,   *fhNpRecAll,   *fhNpEffAll;    // eff. vs. np, all
	TH1F* fhNpAccPrim,  *fhNpRecPrim,  *fhNpEffPrim;   // eff. vs. np, vertex
	TH1F* fhNpAccSec,   *fhNpRecSec,   *fhNpEffSec;    // eff. vs. np, non-vertex
	TH1F* fhZAccSec,    *fhZRecSec,    *fhZEffSec;     // eff. vs. z, non-vertex
	TH1F* fhNhClones,   *fhNhGhosts;                   // # hits of clones and ghosts   

	/** List of histograms **/
	TList* fHistoList;  ///<List of created histograms

	/** Counters **/
	Int_t fNAccAll, fNAccPrim, fNAccRef, fNAccSec;
	Int_t fNRecAll, fNRecPrim, fNRecRef, fNRecSec;
	Int_t fNGhosts, fNClones;
	Int_t fNofEvents;

	/** Usage hits or digi **/
	TString fUseHitOrDigi;            ///< Choose use hits or digis, default: hits


	ClassDef(PndDchFindTracksQa,1);
};


#endif


