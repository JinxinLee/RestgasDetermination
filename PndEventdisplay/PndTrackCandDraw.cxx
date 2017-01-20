// -------------------------------------------------------------------------
// -----                        PndTrackCandDraw source file                  -----
// -----                  Created 03/01/08  by M. Al-Turany            -----
// -------------------------------------------------------------------------

#include "PndTrackCandDraw.h"
#include "FairHit.h"
#include "FairBoxSet.h"
//#include "TClonesArray.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TEveManager.h"
#include "TEveBoxSet.h"
#include "GFTrackCand.h"
#include "PndTrackCand.h"
#include "PndTrackCandHit.h"
#include "FairRootManager.h"

#include "PndDetectorList.h"
#include <iostream>
using std::cout;
using std::endl;

// -----   Standard constructor   ------------------------------------------
PndTrackCandDraw::PndTrackCandDraw(const char* TrackCandName, Int_t iVerbose) :
		FairBoxSetDraw(TrackCandName, iVerbose), fTrackCandList(0){
}
// -------------------------------------------------------------------------
InitStatus PndTrackCandDraw::Init() {
	FairBoxSetDraw::Init();

	if (fVerbose > 1)
		cout << "PndTrackCandDraw::Init()" << endl;
	fManager = FairRootManager::Instance();
	TList* branchNames = fManager->GetBranchNameList();

	TIter next(branchNames);
	TObjString* brName;
	while ((brName = (TObjString*)next())){
		TClonesArray *array = (TClonesArray *) fManager->GetObject(brName->GetString().Data());
		if (array == 0){
			std::cout << "-I- PndTrackCandDraw::Init() branch " << brName->GetString().Data() << " not found!" << std::endl;
		}
	}
	fq = 0;

	return kSUCCESS;
}
// -------------------------------------------------------------------------

void PndTrackCandDraw::AddBoxes(FairBoxSet* set, TObject* obj, Int_t i) {

	if (0 == strcmp(obj->ClassName(), "GFTrackCand")
			|| 0 == strcmp(obj->ClassName(), "PndLhePidTrack"))
		AddBoxesTrackCand(set, obj, i);
	else if (0 == strcmp(obj->ClassName(), "PndTrackCand"))
		AddBoxesPndTrackCand(set, obj, i);
}

void PndTrackCandDraw::AddBoxesTrackCand(FairBoxSet* set, TObject* obj, Int_t i) {
	GFTrackCand *tc;

	if (0 == strcmp(obj->ClassName(), "GFTrackCand")) {
		tc = (GFTrackCand *) obj;
		std::cout << "fTrackCandList is full of GFTrackCands" << std::endl;
	} else {
		std::cout << "fTrackCandList is full of UNKNOWN " << obj->ClassName()
				<< "?" << std::endl;
	}
	if (0 == tc)
		return;

	for (size_t j = 0; j < tc->getNHits(); j++) {
		unsigned int detId, hitId;
		tc->getHit(j, detId, hitId);
		TVector3 point = GetVector(detId, hitId);
		set->AddBox(point.X(), point.Y(), point.Z());
		set->DigitValue(i);
	}
}

void PndTrackCandDraw::AddBoxesPndTrackCand(FairBoxSet* set, TObject* obj, Int_t i) {

	PndTrackCand* pndtc = (PndTrackCand*) obj;
	std::cout << "Hits in TrackCand: " << std::endl;

	((FairMultiLinkedData_Interface*) pndtc)->Print();
	for (size_t j = 0; j < pndtc->GetNHits(); j++) {
		TVector3 point = GetVector(pndtc->GetSortedHit(j));
		set->AddBox(point.X(), point.Y(), point.Z());
		std::cout << " color: " << i << std::endl;
		set->DigitValue(i);
	}
}

TVector3 PndTrackCandDraw::GetVector(Int_t detId, Int_t hitId) {
	FairHit * p;
	FairRootManager* ioman = FairRootManager::Instance();
	TString branchName = ioman->GetBranchName(detId);
	TClonesArray* data = (TClonesArray*) (ioman->GetObject(branchName));
	p = dynamic_cast<FairHit*>(data->At(hitId));
	if (p != 0) {
		std::cout << "Hit in " << branchName << "(" << p->GetX() << "/" << p->GetY() << "/" << p->GetZ() << ")" << std::endl;
		return (TVector3(p->GetX(), p->GetY(), p->GetZ()));
	}
	return TVector3();
}

TVector3 PndTrackCandDraw::GetVector(FairLink link) {
	FairHit *p;
	FairRootManager* ioman = FairRootManager::Instance();
	TString branchName = ioman->GetBranchName(link.GetType());

	p = (FairHit*) ioman->GetCloneOfLinkData(link);

	if (p != 0) {
		TVector3 vec(p->GetX(), p->GetY(), p->GetZ());
		std::cout << "Hit in " << branchName << "Link: " << link << "(" << p->GetX() << "/" << p->GetY() << "/" << p->GetZ() << ")";// << std::endl;

		delete p;
		return (vec);
	}
	return TVector3();

}

TVector3 PndTrackCandDraw::GetVector(TObject* obj){

  return TVector3();

}

// -----   Destructor   ----------------------------------------------------
PndTrackCandDraw::~PndTrackCandDraw() {
}
// -------------------------------------------------------------------------

ClassImp(PndTrackCandDraw)

