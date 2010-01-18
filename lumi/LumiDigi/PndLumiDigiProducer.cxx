#include "PndLumiDigiProducer.h"
#include "TGeoManager.h"
#include "TH1I.h"

PndLumiDigiProducer::PndLumiDigiProducer()
: FairTask("Lumi Digitization Process")
{

}


PndLumiDigiProducer::PndLumiDigiProducer(Int_t verbose)
: FairTask("Lumi Digitization Process")
{
	fVerbose = verbose;
}


PndLumiDigiProducer::~PndLumiDigiProducer()
{
	delete fGeoH;
}


void PndLumiDigiProducer::SetParContainers()
{
	FairRun* ana = FairRun::Instance();
	FairRuntimeDb* rtdb = ana->GetRuntimeDb();
	fDigiPar = (PndLumiDigiPara*)(rtdb->getContainer("LumiStripDigiPara"));
}


InitStatus PndLumiDigiProducer::ReInit()
{
   SetParContainers();
   return kSUCCESS;
}


InitStatus PndLumiDigiProducer::Init()
{
    // Get RootManager
	FairRun* ana = FairRun::Instance();
    FairRootManager* ioman = FairRootManager::Instance();

    fGeoH = new PndLumiTransposition(fVerbose);

    if ( ! ioman ){
    	cout << "-E- PndLumiDigiProducer::Init: "
    	<< "RootManager not instantiated!" << endl;
    	return kFATAL;
    }

    // Get input array
    fLumiPoint = (TClonesArray*) ioman->GetObject("LumiPoint");
    if (!fLumiPoint){
    	cout << "-W- PndLumiDigiProducer::Init: "
    	<< "No LumiPoint collection!" << endl;
    	return kERROR;
    }

    // Create and register output array
    fLumiDigi = new TClonesArray("PndLumiDigi");
    ioman->Register("LumiDigi", "Lumi", fLumiDigi, kTRUE);

    if(! fDigiPar){
    	std::cout<<"-E- PndLumiDigiProducer::DigiPar Container doesn't exist!"<<std::endl;
    	return kERROR;
    }

    fDigiPar->Print();

    std::cout<<"PndLumiDigiProducer initialized successfully!"<<std::endl;

    return kSUCCESS;
}


void PndLumiDigiProducer::Exec(Option_t* opt)
{
    // Reset output array
    if (!fLumiDigi)
    	Fatal("Exec", "No Digi collection");
    fLumiDigi->Clear();

    // Declare some variables
    PndLumiPoint *point = NULL;

    Int_t
    	detID = 0,       // Detector ID
    	trackID = 0;     // Track index

    TVector3
		entry, exit,  pos, dpos;       // Position and error vectors

    Double_t time;

    Int_t iDigi=0 ;

    std::vector<PndLumiStrip> digifront, digiback;  //collect digi strip

    Double_t eLoss;						//energy deposited

    std::vector<PndLumiStrip>::iterator strip_iterator;

    std::vector<Int_t> sensorID;
    std::vector<Int_t> multiback;


    // Loop over LumiPoints
    Int_t nPoints = fLumiPoint->GetEntriesFast();

    for (Int_t iPoint = 0; iPoint < nPoints; iPoint++) {

    	point = (PndLumiPoint*) fLumiPoint->At(iPoint);

    	detID = point->GetDetectorID();
    	trackID = point->GetTrackID();
    	eLoss = (point->GetEnergyLoss()) * 1E9;//eV
    	time = point->GetTime();

    	// Determine hit position
    	entry = point->GetEntryPoint();
    	exit = point->GetExitPoint();

    	pos=0.5*(entry+exit);
    	dpos.SetXYZ(0.0,0.0,0.0);


    	TString detname = point->GetDetName();

    	FairGeoVector posInL, posOutL;

    	fGeoH->GetLocalHitPoints(point, posInL, posOutL);

    	PndLumiCalcStripDigi StripDigiFront(fDigiPar,kTOP);
    	PndLumiCalcStripDigi StripDigiBack(fDigiPar,kBOTTOM);

    	digifront = StripDigiFront.GetStripsDigi(posInL, posOutL, eLoss);

    	digiback = StripDigiBack.GetStripsDigi(posOutL, posInL, eLoss);

    	if (digifront.size()!=0){
    		for (strip_iterator = digifront.begin(); strip_iterator != digifront.end(); ++strip_iterator){
    			new ((*fLumiDigi)[iDigi]) PndLumiDigi( detID, pos, dpos, iPoint,*strip_iterator, detname,
    					kTOP,digifront.size(), IsActive(detname, kTOP));
    			iDigi++;
    		}
    	}
    	if (digiback.size()!=0){
    		for (strip_iterator = digiback.begin(); strip_iterator != digiback.end(); ++strip_iterator){
    			new ((*fLumiDigi)[iDigi]) PndLumiDigi( detID, pos, dpos, iPoint,*strip_iterator, detname,
    					kBOTTOM,digiback.size(), IsActive(detname, kBOTTOM));
    			iDigi++;
    		}
    	}

    }
    Print();

}

Bool_t PndLumiDigiProducer::IsActive(TString detname, SensorSide side)
{
	string DetName = detname.Data();
	string usc = "_";
	Int_t m = DetName.find_last_of(usc);
	string sensorID = DetName.erase(0,m+1);
	Int_t i = atoi(sensorID.c_str());

	TString sensor = fDigiPar->GetSensType();

	TString bothside = "2"; //two sides are active
	TString topside = "0";  //only front side is active
	TString botside = "1";  //only back side is active

	if(sensor[i-1]==bothside){
		if (side==kTOP) return kTRUE;
		if (side==kBOTTOM) return kTRUE;
	}
	if(sensor[i-1]==topside){
		if (side==kTOP) return kTRUE;
		if (side==kBOTTOM) return kFALSE;
	}
	if(sensor[i-1]==botside){
		if (side==kTOP) return kFALSE;
		if (side==kBOTTOM) return kTRUE;
	}

}


void PndLumiDigiProducer::Print() const
{
	Int_t nPoints = fLumiPoint->GetEntriesFast();
	Int_t nDigis = fLumiDigi->GetEntriesFast();

	cout << "-I- PndLumiDigiProducer: " << nPoints << " MCPoints - "<< nDigis << " Digis registered for this event." << endl;

    if (fVerbose > 1){
    	for (Int_t i=0; i < nDigis; i++){
	    (*fLumiDigi)[i]->Print();
    	}
    }
}

ClassImp(PndLumiDigiProducer)
