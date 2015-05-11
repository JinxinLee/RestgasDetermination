/*
 * PndMvdDigiPixelDraw.h
 *
 *  Created on: Apr 20, 2009
 *      Author: stockman
 */

#ifndef PNDMVDDIGIPIXELDRAW_H_
#define PNDMVDDIGIPIXELDRAW_H_

#include "FairBoxSetDraw.h"
#include "PndSdsDigiPixel.h"
#include "PndGeoHandling.h"

#include "TVector3.h"
#include "TEveBoxSet.h"

#include <map>
#include <vector>

typedef std::map<Int_t, TEveBoxSet*>::iterator boxSetMapIter;

class PndMvdDigiPixelDraw : public FairBoxSetDraw
{
public:
	PndMvdDigiPixelDraw();
	PndMvdDigiPixelDraw(const char* name, Int_t iVerbose = 1);
	virtual ~PndMvdDigiPixelDraw();

	void Exec(Option_t* option);

	void Reset();

protected:
	TVector3 GetVector(TObject* obj){return TVector3();};

	InitStatus Init();

	void SortDigis(TClonesArray* digis);
	TEveBoxSet* CreateNewBoxSet(TString& name);

	std::map<Int_t, TEveBoxSet* > fModules; //!
	PndGeoHandling* fGeoH;
	TClonesArray* fClusterCands;
	TClonesArray* fRecoHits;

	Double_t fPixelSize;
	Double_t fBoxSize;

	Bool_t fUseCluster;

	ClassDef(PndMvdDigiPixelDraw,1);
};

#endif /* PNDMVDDIGIPIXELDRAW_H_ */
