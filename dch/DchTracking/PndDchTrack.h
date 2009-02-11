// -------------------------------------------------------------------------
// -----                      PndDchTrack header file                  -----
// -----                  Created 27.01.2008  by A. Bubak              -----
// -----                      base on CbmStsTrack                      -----
// -------------------------------------------------------------------------

/** \class PndDchTrack
 *  \author A.Bubak <arkadiusz.bubak@gmail.com>
 *  \date 27.01.2008
 *  \brief Class for Dch track representation
 * 
 *   Dch local track. Holds lists of PndDchHits or PndDchDigi and
 *   the fitted track parameters.
 **/

#ifndef PNDDCHTRACK_H
#define PNDDCHTRACK_H 1

// Pnd includes
#include "FairTrackParam.h"

// ROOT includes
#include "TArrayI.h"
#include "TObject.h"

// c++ includes
#include <map>
using std::map;
using std::multimap;


class PndDchHit;
class PndDchCylinderHit;
class PndDchDigi;

class PndDchTrack : public TObject
{
public:

	/** Default constructor **/
	PndDchTrack();

	/** Destructor **/
	virtual ~PndDchTrack();

	/** Public method AddHit
	 * \param hitID - index of hit in global array of hits
	 * \param hit - added new hit
	 **/
	void AddHit(PndDchHit* hit, Int_t hitID);

	/** Public method AddCylinderHit
	 * \param chitID - index of cylinderHit in global array of cylinderHits
	 * \param chit - added new cylinderHit
	 **/
	void AddCylinderHit(PndDchCylinderHit* chit, Int_t chitID);
	
	/** Public method AddDigi
	 * \param digiID - index of digi in global array of digis
	 * \param digi - added new digi
	 **/
	void AddDigi(PndDchDigi* digi, Int_t digiID);

	/** Public method Print **/
	void Print();


	/** Public methods SortHits/SortDigi
	 ** Sorts the hits/digis in each array in downstream direction
	 ** and writes the hit/digi indices into the member TArrayI
	 **/
	void SortHits();          ///<Sort hits according to Z coordinate
	void SortCylinderHits();  ///<Sort cylinderHits according to ... !TODO
	void SortDigis(); //!TODO


	/** Accessors  **/
	Int_t GetNofDchHits()               const { return fDchHits.GetSize(); }          ///<return number of hits in track
	Int_t GetNofDchCylinderHits()       const { return fDchCylinderHits.GetSize(); }  ///<return number of cylinder hits in track
	Int_t GetNofDchDigis()              const { return fDchDigis.GetSize(); }         ///<return number of digis in track
	Int_t GetDchHitIndex(Int_t iHit)          const { return fDchHits.At(iHit); }           ///<return index in global Hit array
	Int_t GetDchCylinderHitIndex(Int_t iChit) const { return fDchCylinderHits.At(iChit); }  ///<return index in global Hit array
	Int_t GetDchDigiIndex(Int_t iDigi)  const { return fDchDigis.At(iDigi); }         ///<return index in global Digi array
	Int_t GetFlag()                     const { return fFlag; }
	Double_t GetChi2()                  const { return fChi2; }
	Int_t    GetNDF()                   const { return fNDF; }
	Double_t GetELoss()                 const { return fELoss;}
	FairTrackParam* GetParamFirst() { return &fParamFirst; }
	FairTrackParam* GetParamLast()  { return &fParamLast ; }


	/** Modifiers  **/
	void SetParamFirst(FairTrackParam& par)    { fParamFirst = par;   }
	void SetParamLast(FairTrackParam& par)     { fParamLast  = par;   }
	void SetFlag(Int_t flag)                  { fFlag       = flag;  }
	void SetChi2(Double_t chi2)               { fChi2       = chi2;  }
	void SetNDF(Int_t ndf)                    { fNDF        = ndf;   }
	void SetELoss(Double_t eLoss)             { fELoss      = eLoss; }


	/** Function for sorting **/
	static Bool_t CompareChi2(const PndDchTrack* a, const PndDchTrack* b)
	{
		return ( a->GetChi2()/(Double_t)a->GetNDF() < b->GetChi2()/(Double_t)b->GetNDF() );
	};

	static Bool_t CompareMomentum(PndDchTrack* a, PndDchTrack* b)
	{
		Double_t mom_a = 1./TMath::Abs(a->GetParamLast()->GetQp());
		Double_t mom_b = 1./TMath::Abs(b->GetParamLast()->GetQp());
		return ( mom_a > mom_b );
	};


private:

	TArrayI      fDchHits;         ///< array of hits
	TArrayI      fDchCylinderHits; ///< array of cylinder hits
	TArrayI		 fDchDigis;        ///< array of digis
	
	/** Track parameters at first and last fitted hit **/
	FairTrackParam fParamFirst;
	FairTrackParam fParamLast;

	/** Quality flag **/
	Int_t fFlag;

	/** RMS deviation of hit coordinates to track **/
	Double32_t fChi2;

	/** Number of degrees of freedom **/
	Int_t fNDF;
	
	/** Total energy loss in TRD **/
	Double_t fELoss;

	/** Maps from hit z position to hit index. STL multimap is used because it
	 *  is automatically sorted. Temporary only; not for storgage.
	 *  The Hit index arrys will be filled by the method SortHits.
	 **/
	std::multimap<Double_t, Int_t> fDchHitMultimap;

	/** Maps from cylinder hit z position to cylinder hit index **/
	std::multimap<Double_t, Int_t> fDchCylinderHitMultimap;
	
	/** Maps from digi plane position to hit index. STL multimap is used because it
	 *  is automatically sorted. Temporary only; not for storgage.
	 *  The Hit index arrys will be filled by the method SortHits.
	 **/
	std::multimap<Double_t, Int_t> fDchDigiMultimap;

	ClassDef(PndDchTrack, 1);
};

#endif

