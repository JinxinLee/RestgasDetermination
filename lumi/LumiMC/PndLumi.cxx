#include "PndLumi.h"

using std::cout;
using std::endl;

PndLumi::PndLumi()
    : FairDetector()
{
    fLumiPoint = new TClonesArray("PndLumiPoint");
    fVerboseLevel = 1;

    ResetParameters();
}

PndLumi::PndLumi(const char* name, Bool_t active, Int_t verbose)
    : FairDetector(name, active)
{
    fLumiPoint = new TClonesArray("PndLumiPoint");
    fVerboseLevel = verbose;
}

PndLumi::~PndLumi()
{
    if (fLumiPoint){
    	fLumiPoint->Delete();
    	delete fLumiPoint;
    }
}


Bool_t PndLumi::ProcessHits(FairVolume* vol)
{
    //if (gMC->TrackCharge() != 0.){
	TParticle* particle =  gMC->GetStack()->GetCurrentTrack();
	Int_t PDGCode = particle->GetPdgCode();

	if (PDGCode < 1000000000){
		if (gMC->IsTrackEntering()){
    		// Set parameters at entrance of volume. Reset ELoss.
    		fEnergyLoss  = 0.;
    		fLength = gMC->TrackLength();
    		gMC->TrackPosition(fPosIn);
    		gMC->TrackMomentum(fMomIn);

    	}

    	// Sum energy loss for all steps in the active volume
    	fEnergyLoss += gMC->Edep();

    	if (gMC->IsTrackExiting()||
    			gMC->IsTrackDisappeared() ||
    			gMC->IsTrackStop())	{

    		// Create PndLumiPoint at exit of active volume
    		fTime   = gMC->TrackTime() * 1.0e09;
    		fTrackID  = gMC->GetStack()->GetCurrentTrackNumber();
    		fVolumeID = vol->getMCid();
    		gMC->TrackPosition(fPosOut);
    		gMC->TrackPosition(fMomOut);
    		fDetName = gMC->CurrentVolPath();

			AddPoint(fTrackID, fVolumeID, fDetName,
					TVector3 (fPosIn.X(), fPosIn.Y(), fPosIn.Z()),
					TVector3 (fPosOut.X(), fPosOut.Y(), fPosOut.Z()),
					TVector3 (fMomIn.X(), fMomIn.Y(), fMomIn.Z()),
					TVector3 (fMomOut.X(), fMomOut.Y(), fMomOut.Z()),
    				fTime, fLength, fEnergyLoss);


    		PndStack* stack = (PndStack*) gMC->GetStack();
    		stack->AddPoint(kLUMI);

    		ResetParameters();
    	}
    }
    return kTRUE;
}

void PndLumi::EndOfEvent()
{
    if (fVerboseLevel)
    	Print();
    Reset();
}

void PndLumi::Register()
{
    FairRootManager::Instance()->Register("LumiPoint", "PndLumi", fLumiPoint, kTRUE);
}

TClonesArray* PndLumi::GetCollection(Int_t iColl) const
{
    if (iColl == 0)
    	return fLumiPoint;
    else
    	return NULL;
}

void PndLumi::Print() const
{
    Int_t nPoints = fLumiPoint->GetEntriesFast();

    cout << "-I- PndLumi: " << nPoints << " points registered in this event."  << endl <<endl;

    if (fVerboseLevel > 1){
    	for (Int_t i = 0; i < nPoints; i++)	{
    		(*fLumiPoint)[i]->Print();
    	}
    }
}

void PndLumi::Reset()
{
    fLumiPoint->Clear();
    ResetParameters();
}

void PndLumi::ConstructGeometry()
{
    FairGeoLoader *geoLoad = FairGeoLoader::Instance();

    FairGeoInterface *geoFace = geoLoad->getGeoInterface();

    PndLumiGeo *lumiGeo  = new PndLumiGeo();

    lumiGeo->setGeomFile(GetGeometryFileName());
    geoFace->addGeoModule(lumiGeo);

    Bool_t	rc = geoFace->readSet(lumiGeo);

    if (rc)
    	lumiGeo->create(geoLoad->getGeoBuilder());

    TList* volList = lumiGeo->getListOfVolumes();

    ProcessNodes(volList);
}

PndLumiPoint* PndLumi:: AddPoint(Int_t trackID, Int_t detID,
		TString detname, TVector3 posIn, TVector3 posOut,
		TVector3 momIn, TVector3 momOut, Double_t time,
		Double_t length, Double_t eLoss) const
{
    // Get a reference to the hit collection
    TClonesArray &points = *fLumiPoint;

    // get the number of hits already present
    Int_t size = points.GetEntriesFast();

    // create a new hit after the last one present in the collection
    return new(points[size]) PndLumiPoint(trackID, detID, posIn, momIn,
    		time, length, eLoss, posOut, momOut, detname);
}

void PndLumi::ResetParameters()
{
    fTrackID = fVolumeID = 0;
    fTime = fLength = fEnergyLoss = 0.;

    fPosIn.SetXYZT(0., 0., 0., 0.);
    fPosOut.SetXYZT(0., 0., 0., 0.);
    fMomIn.SetXYZT(0., 0., 0., 0.);
    fMomOut.SetXYZT(0., 0., 0., 0.);
};


ClassImp(PndLumi)

