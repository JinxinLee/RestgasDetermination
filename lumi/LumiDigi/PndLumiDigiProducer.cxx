#include "PndLumiDigiProducer.h"
#include "TGeoManager.h"

PndLumiDigiProducer::PndLumiDigiProducer()
: FairTask("Lumi Digitization Process")
{
	fVerbose = 0;
	fDiffusion = kFALSE;
}


PndLumiDigiProducer::PndLumiDigiProducer(Int_t verbose)
: FairTask("Lumi Digitization Process")
{
	fVerbose = verbose;
}


PndLumiDigiProducer::~PndLumiDigiProducer()
{
	if (0!=fGeoH) delete fGeoH;
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

    fGeoH = new PndLumiTransposition(fVerbose, gGeoManager);

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
	std::cout<<"PndLumiDigiProducer Start Execution Task"<<std::endl;
    // Reset output array
    if (!fLumiDigi)
    	Fatal("Exec", "No Digi collection");
    fLumiDigi->Clear();

    // Declare some variables
   PndLumiPoint *point = NULL;

    Int_t	detID, trkID;

    Int_t iDigi;

    std::vector<PndLumiStrip> digifront, digiback;  //collect digi strip

    Double_t eLoss;						//energy deposited

    std::vector<PndLumiStrip>::iterator strip_iterator;
    TString detname;
//    TString sensor="lumiSensor";

    // Loop over LumiPoints
    Int_t nPoints = fLumiPoint->GetEntriesFast();

    for (Int_t iPoint = 0; iPoint < nPoints; iPoint++) {

    	point = (PndLumiPoint*) fLumiPoint->At(iPoint);

//    	detID = point->GetDetectorID();
    	trkID = point->GetTrackID();

    	eLoss = (point->GetEnergyLoss()) * 1E9;//eV

    	detname = point->GetDetName();

//    	if(detname.Contains(sensor)){

    	detID = getDetId(detname);

			FairGeoVector posInL, posOutL;

			fGeoH->GetLocalHitPoints(point, posInL, posOutL);

			PndLumiCalcStripDigi StripDigiFront(fDigiPar,kTOP);
			PndLumiCalcStripDigi StripDigiBack(fDigiPar,kBOTTOM);

			digifront = StripDigiFront.GetStrips(posInL, posOutL, eLoss);
			digiback = StripDigiBack.GetStrips(posOutL, posInL, eLoss);


			if (digifront.size()!=0){
				for (strip_iterator = digifront.begin(); strip_iterator != digifront.end(); ++strip_iterator){
					iDigi = fLumiDigi->GetEntriesFast();
					new ((*fLumiDigi)[iDigi]) PndLumiDigi( detID, iPoint, detname, trkID, *strip_iterator,	kTOP);
				}
			}
			if (digiback.size()!=0){
				for (strip_iterator = digiback.begin(); strip_iterator != digiback.end(); ++strip_iterator){
					iDigi = fLumiDigi->GetEntriesFast();
					new ((*fLumiDigi)[iDigi]) PndLumiDigi( detID, iPoint, detname, trkID, *strip_iterator,	kBOTTOM);
				}
			}

//    	}
    }
    Print();

}
Int_t PndLumiDigiProducer::getDetId(TString detname)
{
	string DetName = detname.Data();
	string usc = "_";
	Int_t m = DetName.find_last_of(usc);
	string sensorID = DetName.erase(0,m+1);
	Int_t id = atoi(sensorID.c_str());

	return id;
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
