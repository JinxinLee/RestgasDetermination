#include "PndLumi.h"
#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoNode.h"
#include "FairGeoRootBuilder.h"
#include "FairGeoMedia.h"
using std::cout;
using std::endl;

PndLumi::PndLumi()
    : FairDetector()
{
    fLumiPointCollection = new TClonesArray("PndLumiPoint");
    ResetParameters();

   fListOfSensitives.push_back("lumiSensor");
    cout<<"-I- PndLumi: fListOfSensitives contains:";
    for(Int_t k=0;k<fListOfSensitives.size();k++)
    	cout<<"\n\t"<<fListOfSensitives[k]<<endl;
}

PndLumi::PndLumi(const char* name, Bool_t active)
    : FairDetector(name, active)
{
    fLumiPointCollection = new TClonesArray("PndLumiPoint");

    fListOfSensitives.push_back("lumiSensor");

    cout<<"-I- PndLumi: fListOfSensitives contains:";
    for(Int_t k=0;k<fListOfSensitives.size();k++)
    	cout<<"\n\t"<<fListOfSensitives[k]<<endl;
}

PndLumi::~PndLumi()
{
    if (fLumiPointCollection){
    	fLumiPointCollection->Delete();
    	delete fLumiPointCollection;
    }

}

void PndLumi::Initialize()
{
	// Init function
	FairDetector::Initialize();
//	Double_t BeamMom= par->GetBeamMom();
}

Bool_t PndLumi::ProcessHits(FairVolume* vol)
{
	TParticle* particle =  gMC->GetStack()->GetCurrentTrack();
	Int_t PDGCode = particle->GetPdgCode();

	if (PDGCode <1000010000 ){
		if (gMC->IsTrackEntering()){

    		PndStack* stack = (PndStack*) gMC->GetStack();
    		stack->AddPoint(kLUMI);

    		// Set parameters at entrance of volume. Reset ELoss.
    		fEnergyLoss  = 0.;
    		fLength = gMC->TrackLength();
    		gMC->TrackPosition(fPosIn);
    		gMC->TrackMomentum(fMomIn);
    	}

    	// Sum energy loss for all steps in the active volume
    	fEnergyLoss += gMC->Edep();

    	if (gMC->IsTrackExiting()	||
    			gMC->IsTrackDisappeared()||
    			gMC->IsTrackStop()){
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
    FairRootManager::Instance()->Register("LumiPoint", "PndLumi", fLumiPointCollection, kTRUE);
}

TClonesArray* PndLumi::GetCollection(Int_t iColl) const
{
    if (iColl == 0)
    	return fLumiPointCollection;
    else
    	return NULL;
}

void PndLumi::Print() const
{
    Int_t nPoints = fLumiPointCollection->GetEntriesFast();

    cout << "-I- PndLumi: " << nPoints << " points registered in this event."  << endl <<endl;

    if (fVerboseLevel > 1){
    	for (Int_t i = 0; i < nPoints; i++)	{
    		(*fLumiPointCollection)[i]->Print();
    	}
    }
}

void PndLumi::Reset()
{
    fLumiPointCollection->Clear();
    ResetParameters();
}

void PndLumi::ConstructGeometry()
{
	TString fileName=GetGeometryFileName();
	if(fileName.EndsWith(".geo")){
		ConstructASCIIGeometry();
	}else if(fileName.EndsWith(".root")){
		ConstructRootGeometry();
	}else{
		cout<<"Geometry format not supported "<<endl;
	}
}

bool PndLumi::CheckIfSensitive(std::string name)
{
  for (Int_t i = 0; i < fListOfSensitives.size(); i++){
    if (name.find(fListOfSensitives[i]) != std::string::npos)
    	return true;

  }
  return false;
}
void PndLumi::ConstructASCIIGeometry()
{

    FairGeoLoader *geoLoad = FairGeoLoader::Instance();
    FairGeoInterface *geoFace = geoLoad->getGeoInterface();
    PndLumiGeo *lumiGeo  = new PndLumiGeo();
    lumiGeo->setGeomFile(GetGeometryFileName());
    geoFace->addGeoModule(lumiGeo);

    Bool_t	rc = geoFace->readSet(lumiGeo);
    if (rc) lumiGeo->create(geoLoad->getGeoBuilder());

    TList* volList = lumiGeo->getListOfVolumes();


    FairRun* sim = FairRun::Instance();
    FairRuntimeDb* rtdb=sim->GetRuntimeDb();
    PndLumiGeoPar* par=(PndLumiGeoPar*)(rtdb->findContainer("FairBaseParSet"));

    TObjArray *fSensNodes = par->GetGeoSensitiveNodes();
    TObjArray *fPassNodes = par->GetGeoPassiveNodes();

    TListIter iter(volList);

    FairGeoNode* node = NULL;
    FairGeoVolume* vol= NULL;

    while((node = (FairGeoNode*)iter.Next())){
    	vol = dynamic_cast<FairGeoVolume*>(node);
    	if(node->isSensitive()){
    		fSensNodes->AddLast(vol);
    	}else{
    		fPassNodes->AddLast(vol);
    	}
    }

    par->setChanged();
    par->setInputVersion(sim->GetRunId(),1);


    ProcessNodes(volList);

}

PndLumiPoint* PndLumi:: AddPoint(Int_t trackID, Int_t detID,
		TString detname, TVector3 posIn, TVector3 posOut,
		TVector3 momIn, TVector3 momOut, Double_t time,
		Double_t length, Double_t eLoss) const
{
    // Get a reference to the hit collection
    TClonesArray &points = *fLumiPointCollection;

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

