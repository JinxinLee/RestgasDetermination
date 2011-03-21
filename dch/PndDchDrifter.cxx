//-----------------------------------------------------------
//-----------------------------------------------------------
#include <iostream>
#include <fstream>
#include  "TMath.h"
#include "PndDchDrifter.h"
#include "TMath.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "PndDchDigiPar.h"
#include "PndDchDigiInf.h"

#include <map>

using std::ifstream;
using std::cout;
using std::endl;
using std::map;
using std::pair;

PndDchDrifter* PndDchDrifter::fgDrifterInstance = 0;

PndDchDrifter* PndDchDrifter::Instance() {
	if (0 == fgDrifterInstance) {
		cout << "First call of PndDchDrifter::Instance(...)"
				<< " must be with filename and cellsize!!" << endl;
		abort();
	} else
		return fgDrifterInstance;
}

PndDchDrifter* PndDchDrifter::Instance(TString filename, Double_t cellsize) {
	if (0 != fgDrifterInstance) {
		cout << "An instance of PndDchDrifter already existed, "
				<< "I replace it with a new one" << endl;
		delete fgDrifterInstance;
	}
	fgDrifterInstance = new PndDchDrifter(filename, cellsize);
	//	fgDrifterInstance->Print();
	return 	fgDrifterInstance;
}
PndDchDrifter::PndDchDrifter(TString fileName, Double_t splaneWidth) :
	fCellSize(splaneWidth) {
	fInf = PndDchDigiInf::Instance(fileName);
	SetParContainers();

	//fNumberOfPoints  = (Int_t)fInf->GetVariableStrict("numberofpoints");
	//fTimeUnit2ns     = fInf->GetVariableStrict("timeunit2ns");
	//fDistanceUnit2cm = fInf->GetVariableStrict("distanceunit2cm");

	fNumberOfPoints = fInf->GetNumberOfPointsForDigi();
	fTimeUnit2ns = fInf->GetTimeUnit2ns();
	fDistanceUnit2cm = fInf->GetfDistanceUnit2cm();

	TObjString* key;
	Double_t driftTime;
	Double_t distance;

	map<Double_t, Double_t> digiMap;
	map<Double_t, Double_t>::iterator iDigiMap;

	TMap *parVariables = fPar->GetVariables();
	TIterator* iter = parVariables->MakeIterator();

	while ((key = (TObjString*) iter->Next()) != NULL) {

		TObjString* value = (TObjString*) parVariables->GetValue(key->String());

		//distance   = strtod(key->GetString(),NULL);
		//driftTime  = strtod(value->GetString(),NULL);
		distance = key->GetString().Atof();
		driftTime = value->GetString().Atof();

		if (!key->GetString().CompareTo("numberofpoints"))
			continue;
		if (!key->GetString().CompareTo("timeunit2ns"))
			continue;
		if (!key->GetString().CompareTo("distanceunit2cm"))
			continue;
		// TMap is not sorted automatically
		digiMap.insert(pair<Double_t, Double_t> (distance, driftTime));
	}

	for (iDigiMap = digiMap.begin(); iDigiMap != digiMap.end(); iDigiMap++) {
		//cout << (*iDigiMap).first <<" "<< (*iDigiMap).second << endl;
		fDistance.push_back((*iDigiMap).first * fDistanceUnit2cm);
		fDriftTime.push_back((*iDigiMap).second * fTimeUnit2ns);
	}
	}

/*

 PndDchDrifter::PndDchDrifter(TString fileName, Double_t splaneWidth)
 : fCellSize(splaneWidth)
 {
 Double_t timeUnit2ns = 1.  ; //read from the file to recalculate tim and distance
 // in  proper units. I assume to have time in [ns]
 Double_t distanceUnit2cm = 1.; //I assume to have distance in [cm]
 //reading the calibration distance to drift time file
 fNumberOfPoints = 0;
 ifstream infile(fileName.Data(),std::fstream::in);
 if(!infile.good()){
 cout<<"The drift time parametrization file "<<fileName<<" is not found"<<endl;
 abort();
 }
 infile>> fNumberOfPoints;
 infile>> distanceUnit2cm >> timeUnit2ns;
 fDistance.reserve(fNumberOfPoints);
 fDriftTime.reserve(fNumberOfPoints);
 for (Int_t i=0; i<fNumberOfPoints; i++) {
 infile >> fDistance[i] >> fDriftTime[i];
 fDriftTime[i] *= timeUnit2ns;
 fDistance[i]  *= distanceUnit2cm;
 }
 infile.close();
 }
 
*/
void PndDchDrifter::SetParContainers() {
	FairRun* ana = FairRun::Instance();
	FairRuntimeDb* rtdb = ana->GetRuntimeDb();
	fPar = (PndDchDigiPar*) (rtdb->getContainer("PndDchDigiParGarfield"));
	fInf->FillDigiPar(fPar);
}

Bool_t PndDchDrifter::CalculateDriftTime(Double_t &time, Double_t distance) const {
	Double_t bin = 0.5 * fCellSize / fNumberOfPoints;
	Int_t index1 = (Int_t) TMath::Floor(distance / bin);
	Int_t index2 = (Int_t) TMath::Ceil(distance / bin);
	if (index1 < 0 || index2 >= fNumberOfPoints)
		return kFALSE;
	Double_t weight1 = 0.;
	Double_t weight2 = 0.;
	if (index1 == index2)
		time = fDriftTime[index1];
	else {
		weight1 = TMath::Abs(distance - index1 * bin) / bin;
		weight2 = TMath::Abs(index2 * bin - distance) / bin;
		time = weight1 * fDriftTime[index1] + weight2 * fDriftTime[index2];
	}
	return kTRUE;
}

Bool_t PndDchDrifter::CalculateDistance(Double_t time, Double_t &distance) const {
	distance = 0;
	if (time < 10e-9) {
		return kFALSE;
	}
	Double_t bin = 0.5 * fCellSize / fNumberOfPoints;
	Int_t i = 0;
	for (i = 0; i < fNumberOfPoints && fDriftTime[i] < time; i++) {
		;
	}

	if (i > 0 && i < fNumberOfPoints) {
		Double_t w1 = TMath::Abs(time - fDriftTime[i - 1]);
		Double_t w2 = TMath::Abs(time - fDriftTime[i]);
		distance = (i - 1 + w1 / (w2 + w1)) * bin;
		if (distance < fCellSize / 2.)
			return kTRUE;
		else
			return kFALSE;
	} else {
		return kFALSE;
	}
}

void PndDchDrifter::Print(const Option_t* option) const {
	cout << "#########    PndDchDrifter: ##########\n";
	cout<< "  fNumberOfPoints= "<< fNumberOfPoints<<endl;
	cout<< "  fCellSize = "<< fCellSize<<endl;
	for(Int_t i = 0; i<fNumberOfPoints; i++){
	  cout<<i<<"\t driftTime= "<< fDriftTime[i]<<"  fDistance= "<<fDistance[i]<<endl;
	}
}

ClassImp(PndDchDrifter)
