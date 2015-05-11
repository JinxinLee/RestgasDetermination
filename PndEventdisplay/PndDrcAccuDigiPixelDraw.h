// -----------------------------------------
// PndDrcAccuDigiPixelDraw.h
//
// Created on: 03.06.2013
// Author: R.Dzhygadlo
// ------------------------------------------

// Class allows to plot DIRC digitized hits in accumulative mode using Event Display

#ifndef PNDDRCACCUDIGIPIXELDRAW_H_
#define PNDDRCACCUDIGIPIXELDRAW_H_

#include "FairBoxSetDraw.h"
#include "PndGeoHandling.h"
#include "PndGeoDrc.h"

#include "TVector3.h"
#include "TEveBoxSet.h"

#include <map>
#include <vector>

typedef std::map<Int_t, TEveBoxSet*>::iterator boxSetMapIter;
typedef std::map<Int_t, Int_t>::iterator IntIter;

class PndDrcAccuDigiPixelDraw : public FairBoxSetDraw
{
public:
        PndDrcAccuDigiPixelDraw();
	PndDrcAccuDigiPixelDraw(const char* name, TString digifile, Double_t weight =0, Int_t ndigievents = 0, Int_t iVerbose = 1);
	virtual ~PndDrcAccuDigiPixelDraw();

	void Exec(Option_t* option);

protected:
	TVector3 GetVector(TObject* obj){return TVector3();};

	InitStatus Init();

	void ReadAllHits();
	void SortDigis(TClonesArray* digis);
	TEveBoxSet* CreateNewBoxSet(TString& name);

	std::map<Int_t, TEveBoxSet* > fHitsArr; //!
	std::map<Int_t, Int_t > fHitsN;         //!
	PndGeoHandling* fGeoH;
	TClonesArray* fClusterCands;
	TClonesArray* fRecoHits;
	
	TString fDigiFile;
	Int_t fNdigiEvents;
	Double_t fBoxHeight;
	Bool_t fFirstEvent;
	
	Double_t fPixelSize;
	Double_t fBoxSize;

	Bool_t fUseCluster;
	PndGeoDrc* fGeo;


	ClassDef(PndDrcAccuDigiPixelDraw,1);
};

#endif /* PNDDRCACCUDIGIPIXELDRAW_H_ */
