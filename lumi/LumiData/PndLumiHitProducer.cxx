#include "PndLumiHitProducer.h"

PndLumiHitProducer::PndLumiHitProducer()
    :  FairTask("Lumi Hit Producer")
{
    fdPos = 0.;
    fEfficiency = 1.;
    fThreshold = 0.;
    fVerboseLevel = 0;
}

PndLumiHitProducer::PndLumiHitProducer(Double_t dpos, Double_t eff,
		Double_t thr,Int_t ver)
    :  FairTask("Lumi Hit Producer")
{
    fdPos = dpos;
    fEfficiency = eff;
    fThreshold = thr;
    fVerboseLevel = ver;
}

PndLumiHitProducer::~PndLumiHitProducer()
{
}

InitStatus PndLumiHitProducer::Init()
{
    // Get RootManager
    FairRootManager* ioman = FairRootManager::Instance();

    if ( ! ioman ){
        cout << "-E- PndLumiHitProducer::Init: "
             << "RootManager not instantiated!" << endl;
        return kFATAL;
    }

    // Get input array
    fLumiPointCollection = (TClonesArray*) ioman->GetObject("LumiPoint");
    if (!fLumiPointCollection){
    	cout << "-W- PndLumiHitProducer::Init: "
             << "No LumiPoint collection!" << endl;
        return kERROR;
    }

    // Create and register output array
    fLumiHitCollection = new TClonesArray("PndLumiHit");
    ioman->Register("LumiHit", "Lumi", fLumiHitCollection, kTRUE);

    return kSUCCESS;
}

void PndLumiHitProducer::Exec(Option_t* opt)
{
    // Reset output array
    if (!fLumiHitCollection)
        Fatal("Exec", "No hit collection");

    fLumiHitCollection->Clear();

    // Declare some variables
    PndLumiPoint *point = NULL;

    Int_t
        detID = 0,       	// Detector ID
        trackID = 0;     	// Track index

    TVector3 pos, dpos;     // Global position and corresponding error vectors

    Double_t eloss; // energy loss

    // Loop over LumiPoints
    Int_t nPoints = fLumiPointCollection->GetEntriesFast();

    for (Int_t iPoint = 0; iPoint < nPoints; iPoint++){
            point = (PndLumiPoint*) fLumiPointCollection->At(iPoint);

            if ( ! point) continue;

            detID = point->GetDetectorID();
            trackID = point->GetTrackID();
            eloss = point->GetEnergyLoss();

            TVector3
                entryPos = point->GetEntryPoint(),
                exitPos = point->GetExitPoint();

            dpos.SetXYZ(fdPos, fdPos, fdPos);
            pos.SetXYZ(((entryPos.X() + exitPos.X()) / 2.) + gRandom->Gaus(0., fdPos),
            		((entryPos.Y() + exitPos.Y()) / 2.) + gRandom->Gaus(0., fdPos),
                    ((entryPos.Z() + exitPos.Z()) / 2.) + gRandom->Gaus(0., fdPos));

            if ((eloss/fThreshold) < fEfficiency){
            	new ((*fLumiHitCollection)[iPoint]) PndLumiHit(detID, pos, dpos, iPoint, eloss, 0);
            } else{
                new ((*fLumiHitCollection)[iPoint]) PndLumiHit(detID, pos, dpos, iPoint, eloss, 1);
            }
    }   // Loop over MCPoints

    if (fVerboseLevel) Print();
}

void PndLumiHitProducer::Print() const
{
	Int_t nHits = fLumiHitCollection->GetEntriesFast();
	cout << "-I- PndLumiHitProducer: " << nHits << " hits registered in this event."  << endl;
	if (fVerboseLevel > 1){
		for (Int_t i=0; i < nHits; i++){
			(*fLumiHitCollection)[i]->Print();
		}
	}
}
ClassImp(PndLumiHitProducer)
