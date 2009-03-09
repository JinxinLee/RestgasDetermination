#include "PndLumiDigiProducer.h"

PndLumiDigiProducer::PndLumiDigiProducer()
: FairTask("Lumi Digi Producer")
{
	fVerboseLevel = 0 ;
	fZ0 = 0. ;
	fPitch = 0. ;
	fOrient = 0. ;
	fSensorWidth = 0. ;
	fSensorLength = 0. ;
	fRadialDistance = 0. ;
	fDistancePlan = 0. ;
	fThreshold = 0. ;
	fNoise = 0. ;
	fSide = 0.;
	fSigma = -1.;

}

PndLumiDigiProducer:: PndLumiDigiProducer(Double_t Z0, Double_t pitch, Double_t orient,
		Double_t width, Double_t length, Double_t r, Double_t d, Double_t threshold,
		Double_t noise, Double_t side, Double_t sigma, Int_t verbose)
: FairTask("Lumi Digi Producer")
{
	fVerboseLevel = verbose ;
	fZ0 = Z0 ;
	fPitch = pitch ;
	fOrient =  orient;
	fSensorWidth =  width;
	fSensorLength =  length;
	fRadialDistance =  r;
	fDistancePlan =  d;
	fThreshold =  threshold;
	fNoise =  noise;
	fSide = side;
	fSigma = sigma;
}

PndLumiDigiProducer::~PndLumiDigiProducer()
{
}

InitStatus PndLumiDigiProducer::Init()
{
    // Get RootManager
    FairRootManager* ioman = FairRootManager::Instance();
    if ( ! ioman ){
    	cout << "-E- PndLumiDigiProducer::Init: "
    	<< "RootManager not instantiated!" << endl;
    	return kFATAL;
    }

    // Get input array
    fLumiPointCollection = (TClonesArray*) ioman->GetObject("LumiPoint");
    if (!fLumiPointCollection){
    	cout << "-W- PndLumiDigiProducer::Init: "
    	<< "No LumiPoint collection!" << endl;
    	return kERROR;
    }

    // Create and register output array
    fLumiDigiCollection = new TClonesArray("PndLumiDigi");
    ioman->Register("LumiDigi", "Lumi", fLumiDigiCollection, kTRUE);

    return kSUCCESS;
}

void PndLumiDigiProducer::Exec(Option_t* opt)
{
    // Reset output array
    if (!fLumiDigiCollection)
    	Fatal("Exec", "No Digi collection");

    fLumiDigiCollection->Clear();

    // Declare some variables
    PndLumiPoint *point = NULL;

    Int_t
    	detID = 0,       // Detector ID
    	trackID = 0;     // Track index

    TVector3
		entryPos, pos, dpos;       // Position and error vectors

    TVector3
    	LocEntryPos;

    Double_t nrStrips;

    Int_t iDigi=0 ;
    Int_t digisize, clustsize;
    Double_t xin, yin, zin;
    Double_t xout, yout, zout;
    Double_t inStripId, outStripId, StripId;
    Int_t planId, sensorId;

    std::vector<PndLumiStrip> strip; //collect strip hit
    std::vector<PndLumiStrip> digi;  //collect digi strip

    std::map<Int_t, PndLumiStrip> clust; //put digi array in order
    std::map<Int_t, Double_t> IdEnergy; //need for defining right and left
    std::map <Int_t , Double_t>::iterator it;
	std::vector<Int_t> Id;
	Double_t eLoss;						//energy deposited

    std::vector<PndLumiStrip>::iterator strip_iterator;

    // Loop over LumiPoints
    Int_t nPoints = fLumiPointCollection->GetEntriesFast();

    for (Int_t iPoint = 0; iPoint < nPoints; iPoint++) {
    	point = (PndLumiPoint*) fLumiPointCollection->At(iPoint);

    	Id.clear();

    	if (!point)
    		continue;

    	detID = point->GetDetectorID();
    	trackID = point->GetTrackID();
    	eLoss = (point->GetEnergyLoss()) * 1E9;//GeV

		// Determine hit position
		TVector3
			entryPos = point->GetEntryPoint(),
			exitPos = point->GetExitPoint();

		std::string detname = point->GetDetName().Data();

		cout << endl;
		cout << "Detector Hit : "<< detname << endl;

		FairGeoVector posInL, posOutL;
		FairGeoVector loc;
		TVector3 strip_orient;

		PndLumiTransposition trans(fVerboseLevel);
		trans.GetLocalHitPoints(point, posInL, posOutL);
		xin = posInL.getX(); yin =  posInL.getY(); zin = posInL.getZ();
		xout = posOutL.getX(); yout =  posOutL.getY(); zout = posOutL.getZ();

		loc.setX(xin); loc.setY(yin); loc.setZ(zin);

		//Double_t a = (strip_orient.Y())/fPitch;

		LocEntryPos.SetXYZ(xin, yin, zin);

		planId = static_cast<int>((entryPos.Z()-fZ0)/fDistancePlan);

		if (entryPos.Y() > fRadialDistance)  sensorId = 0;
		if (entryPos.X() > fRadialDistance)  sensorId = 1;
		if (entryPos.Y() < -fRadialDistance) sensorId = 2;
		if (entryPos.X() < -fRadialDistance) sensorId = 3;

		TVector2 stripzeroId;

		if (fSide > 0){
			cout << "Front side: fOrient" <<endl;
			strip_orient = trans.LocalToStripOrientation(fOrient, posInL);
			stripzeroId.Set(fSensorWidth, 0.0);

		}

		if (fSide < 0){
			cout << "Back side : -(90-fOrient) "<<endl;
			strip_orient = trans.LocalToStripOrientation(-((TMath::Pi()/2)-fOrient), posOutL);
			stripzeroId.Set(0.0, 0.0);

		}


		PndLumiCalcStripDigi StripDigi(fPitch, fOrient, fSensorWidth, fSensorLength,
						fThreshold, fNoise, fSigma, stripzeroId);
		digi = StripDigi.GetStripsDigi(posInL, posOutL, eLoss);

		inStripId = StripDigi.CalcStripFromHit(posInL.getX(), posInL.getY());
		outStripId  = StripDigi.CalcStripFromHit(posOutL.getX(), posOutL.getY());
		if (fSide > 0){
			nrStrips = StripDigi.CalcStripFromHit(0.0, fSensorLength);
		}
		if (fSide < 0){
			nrStrips = StripDigi.CalcStripFromHit(fSensorWidth, fSensorLength);
		}
		cout << "# total of strips : " << nrStrips << endl;
		StripId = 0.5 * (inStripId + outStripId);

		clust = GetClusters(digi);

		digisize = digi.size();
		clustsize = clust.size();
		cout << " Number of strips fired : " << clustsize << endl;


		Double_t Q_r = 0.;
		Double_t Q_l = 0.;
		Int_t rId;
		Int_t lId;

		if (clustsize!=0){
			IdEnergy =  GetLeftAndRight(clust, digi, Int_t(StripId));

			for ( it = IdEnergy.begin(); it != IdEnergy.end(); it++){
				Id.push_back((*it).first);
			}
			Int_t min = *min_element(Id.begin(),Id.end());
			Int_t max = *max_element(Id.begin(),Id.end());

			rId = IdEnergy.find(max)->first;
			lId = IdEnergy.find(min)->first;
			Q_r = IdEnergy.find(max)->second;
			Q_l = IdEnergy.find(min)->second;
		}
		for (strip_iterator = digi.begin(); strip_iterator != digi.end(); ++strip_iterator){
			new ((*fLumiDigiCollection)[iDigi]) PndLumiDigi( detID, LocEntryPos, dpos,
					nPoints, planId,  sensorId,  clustsize, *strip_iterator, Q_r, Q_l, inStripId,
					outStripId,	rId,  eLoss, detname );
				iDigi++;
		}
  }  // Loop over MCPoints

    cout <<"-I- PndLumiDigiProducer: " << nPoints << " MCPoints - "<<  iDigi << " Digis created."<< std::endl;
    cout << "--------------------------------------------------------- "<< endl;

   //Print();

}

std::map<Int_t,PndLumiStrip> PndLumiDigiProducer::GetClusters(std::vector<PndLumiStrip> strip)
{
	std::map<Int_t,PndLumiStrip> clust;
	std::map<Int_t,PndLumiStrip>::iterator it;
	clust.clear();

	for (Int_t j = 0; j < strip.size(); j++){
		clust[strip[j].GetIndex()] = strip[j];
	}
	for ( it=clust.begin() ; it != clust.end(); it++ )
		cout << " ** PndLumiCalcStripDigi : "<< (*it).second << endl;
	return clust;

}

//Identification of the left and the right strip in a cluster
//return to a map with size two
std::map<Int_t,Double_t> PndLumiDigiProducer::
GetLeftAndRight(std::map<Int_t,PndLumiStrip> clust,	std::vector<PndLumiStrip> digi, Int_t mean)
{
	std::map<Int_t,Double_t> IdEnergy;
	std::map<Int_t,Double_t>::iterator it;
	IdEnergy.clear();

	std::vector<Int_t> Index;
	Int_t Id[2];
	Double_t Energy[2];

	for (Int_t i = 0; i < digi.size(); i++){
		Index.push_back(digi[i].GetIndex());
	}
	Int_t min = *min_element(Index.begin(),Index.end());
	Int_t max = *max_element(Index.begin(),Index.end());

	Int_t Id_emax =  min;
	for (Int_t id = min ; id < max; id++){
		if ((clust.find(Id_emax)->second).GetCharge() <=(clust.find(id)->second).GetCharge()){
			Id_emax = id ;
		}
	}
	if (clust.size() == 1){
		Energy[0]= (clust.find(min)->second).GetCharge();
		Id[0] = min;
		Energy[1]= 0.0;
		Id[1] = -1;

	} else{
		if (clust.size() == 2){
			Energy[0]=(clust.find(min)->second).GetCharge();
			Id[0] = min;
			Energy[1]=(clust.find(max)->second).GetCharge();
			Id[1]= max;
		}
		if (clust.size() == 3){
			double q_l;
			double q_r;
			if ((clust.find(Id_emax-1)->second).GetCharge() <
					(clust.find(Id_emax+1)->second).GetCharge()){
				Id[1] = Id_emax +1;
				Id[0] = Id_emax;
				for (int i = min; i <= (clust.find(Id_emax)->second).GetIndex(); i++){
					q_l = (clust.find(i)->second).GetCharge();
					Energy[0] += q_l;
				}
				for (int i =(clust.find(Id_emax + 1)->second).GetIndex(); i <= max ; i++){
					q_r = (clust.find(i)->second).GetCharge();
					Energy[1] += q_r;
				}
			}
			if ((clust.find(Id_emax-1)->second).GetCharge() >
			(clust.find(Id_emax+1)->second).GetCharge()){
				Id[1] = Id_emax;
				Id[0] = Id_emax - 1;
				for (int i = min; i <= (clust.find(Id_emax-1)->second).GetIndex(); i++){
					q_l = (clust.find(i)->second).GetCharge();
					Energy[0] += q_l;
				}
				for (int i =(clust.find(Id_emax)->second).GetIndex(); i <= max ; i++){
					q_r = (clust.find(i)->second).GetCharge();
					Energy[1] += q_r;
				}
			}

		}
		if (clust.size() > 3){
			double q_l;
			double q_r;
			Id[1] = mean +1;
			Id[0] = mean;
			for (int i = min; i <= mean; i++){
				q_l = (clust.find(i)->second).GetCharge();
				Energy[0] += q_l;
			}
			for (int i = mean +1 ; i <= max; i++){
				q_r = (clust.find(i)->second).GetCharge();
				Energy[1] += q_r;
			}

		}
	}
	for (Int_t j = 0; j < 2; j++){
		IdEnergy[Id[j]] = Energy[j];
	}
	return IdEnergy;
}


void PndLumiDigiProducer::Print() const
{
	Int_t nPoints = fLumiPointCollection->GetEntriesFast();
	Int_t nStrips = fLumiDigiCollection->GetEntriesFast();
    cout << "-I- PndLumiDigiProducer: " << nPoints << " MCPoints - "
    << nStrips << " digi registered in this event." << endl;
    if (fVerboseLevel > 1){
    	for (Int_t i=0; i < nStrips; i++){
	    (*fLumiDigiCollection)[i]->Print();
    	}
    }
}

ClassImp(PndLumiDigiProducer)
