#include "PndLumiStripHitProducer.h"

PndLumiStripHitProducer::PndLumiStripHitProducer()
: FairTask("LumiStrip Hit Producer")
{
	fVerboseLevel = 0;
}

PndLumiStripHitProducer::PndLumiStripHitProducer(Double_t pitch, Double_t orient,
		Double_t width,	Double_t length, Double_t threshold, Double_t noise, Int_t verbose)
:  FairTask("LumiStrip Hit Producer")
{
	fVerboseLevel = verbose;
	fPitch = pitch;
	fOrient = orient;
	fSensorWidth = width;
	fSensorLength = length;
	fThreshold = threshold;
	fNoise = noise;
}

PndLumiStripHitProducer::~PndLumiStripHitProducer()
{
}

InitStatus PndLumiStripHitProducer::Init()
{
 // Get RootManager
    FairRootManager* ioman = FairRootManager::Instance();

    if ( ! ioman )
    {
	cout << "-E- PndLumiStripHitProducer::Init: "
	     << "RootManager not instantiated!" << endl;
	return kFATAL;
    }

    // Get input array
    fLumiPointCollection = (TClonesArray*) ioman->GetObject("LumiPoint");
    if (!fLumiPointCollection)
    {
	cout << "-W- PndLumiStripHitProducer::Init: "
	     << "No LUMIPoint collection!" << endl;
	return kERROR;
    }

    // Create and register output array
    //fLumiHitCollection = new TClonesArray("PndLumiHit");
    fLumiStripHitCollection = new TClonesArray("PndLumiStripHit");
    ioman->Register("LumiStripHit", "Lumi", fLumiStripHitCollection, kTRUE);

    return kSUCCESS;
}

void PndLumiStripHitProducer::Exec(Option_t* opt)
{
	// Reset output array
    if (!fLumiStripHitCollection)
    	Fatal("Exec", "No hit collection");

    fLumiStripHitCollection->Clear();

    PndLumiPoint *point = NULL;

	Int_t
       	detID = 0,       // Detector ID
       	trackID = 0;     // Track index

	TVector3
		entryPos, exitPos, dpos;

    //Strip paramaters
    std::vector<PndLumiStrip> strip;
    std::vector<PndLumiStrip>::iterator strip_iterator;

    Double_t eLoss;

    Int_t nPoints = fLumiPointCollection->GetEntriesFast();

    Int_t iStrip = 0;

    for (Int_t iPoint = 0; iPoint < nPoints; iPoint++){
    	point = (PndLumiPoint*) fLumiPointCollection->At(iPoint);

    	if (!point) continue;

    	detID = point->GetDetectorID();
    	trackID = point->GetTrackID();
    	eLoss = (point->GetEnergyLoss()) * 1E9; //GeV
    	std::string detname = point->GetDetName().Data();

    	entryPos = point->GetEntryPoint();
    	exitPos = point->GetExitPoint();
    	dpos.SetXYZ(0., 0., 0.);

    	FairGeoVector posInL, posOutL;
    	PndLumiTransposition trans(fVerboseLevel);
    	trans.GetLocalHitPoints(point, posInL, posOutL);

    	//nrPlan = static_cast<int>((entryPos.Z()-fZ0)/fDistancePlan);
    	//if (entryPos.Y() > fDetectorDistance) nrSensor = 0;
    	//if (entryPos.X() > fDetectorDistance) nrSensor = 1;
    	//if (entryPos.Y() < -fDetectorDistance) nrSensor = 2;
    	//if (entryPos.X() < -fDetectorDistance) nrSensor = 3;

    	//Identification of strips hit

    	PndLumiCalcStrip StripHit(fPitch, fOrient, fSensorWidth, fSensorLength,
    					fThreshold, fNoise);

		std::vector<PndLumiStrip> strips =
			StripHit.GetStrips(posInL, posOutL, eLoss);

		Int_t size = strips.size();
		cout << " " << size << " Strip(s) is(are) involved in this Hit. " << endl;

    	for (strip_iterator = strips.begin(); strip_iterator != strips.end(); ++strip_iterator){
    		new ((*fLumiStripHitCollection)[iStrip]) PndLumiStripHit( detID, entryPos, dpos , iPoint,
    				exitPos, size, *strip_iterator, eLoss);
      		iStrip++;
    	}

    }
}

ClassImp(PndLumiStripHitProducer)
