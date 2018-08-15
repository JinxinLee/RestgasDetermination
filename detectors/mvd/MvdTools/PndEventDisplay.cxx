#include "PndEventDisplay.h"

ClassImp(PndEventDisplay);

PndEventDisplay::PndEventDisplay() : fHitListMap()
{
}

PndEventDisplay::PndEventDisplay(PndEventDisplay& display) :
	fHitListMap(display.GetHitListMap())
{}

PndEventDisplay::~PndEventDisplay()
{
}

void PndEventDisplay::AddNewGroup(TString groupName, PndGeoHitList* newList)
{
	fHitListMap[groupName] = newList;
}

void PndEventDisplay::AddHit(TString groupName, FairHit* hit, Bool_t vis)
{
	fHitListMap[groupName]->AddHit(hit, vis);
}

void PndEventDisplay::AddHit(TString groupName, Double_t x, Double_t y, Double_t z, Bool_t vis)
{
	fHitListMap[groupName]->AddHit(x, y, z, vis);
}

void PndEventDisplay::AddHit(TString groupName, TGeoMatrix* mat, Bool_t vis)
{
	fHitListMap[groupName]->AddHit(mat, vis);
}

void PndEventDisplay::AddHits(TString groupName, TClonesArray* hits, Bool_t vis)
{
	fHitListMap[groupName]->AddHits(hits, vis);
}

void PndEventDisplay::SetHits(TString groupName, TClonesArray* hits, Bool_t vis)
{
	fHitListMap[groupName]->SetHits(hits, vis);
}

void PndEventDisplay::ClearHits(TString groupName)
{
	fHitListMap[groupName]->RemoveGeoHits(0);
}

PndGeoHitList* PndEventDisplay::GetHitList(TString ListName)
{
	if (fHitListMap[ListName] == 0)
		std::cout << "-E- PndEventDisplay: " << ListName << " does not exist!" << std::endl;
	return fHitListMap[ListName];
}

	

