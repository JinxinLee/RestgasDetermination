/////////////////////////////////////////////////////////////
//  PndDchDigiProducer
//  Filler of PndDchDigi
/////////////////////////////////////////////////////////////////

#include "PndDchDigiProducer.h"

#include "PndDchDigi.h"
#include "PndDchPoint.h"
#include "PndDchMapper.h"
#include "PndDchDrifter.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRun.h"

#include "TClonesArray.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"
#include "TVector3.h"
#include "TSystem.h"

#include <vector>

using std::cout;
using std::endl;
using std::vector;

// Default constructor
PndDchDigiProducer::PndDchDigiProducer() :
  FairTask("Ideal DCH digi Producer") {
}

// Destructor
PndDchDigiProducer::~PndDchDigiProducer() {
  if(fDigiArray){
    fDigiArray->Delete();
    delete fDigiArray;
  }
}

// Public method Init
InitStatus PndDchDigiProducer::Init() {

 // cout << "PndDchDigiProducer::Init()... " << endl;

  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- PndDchDigiProducer::Init():\n\t "
	 << "RootManager not instantiated!" << endl;
    return kFATAL;
  }
  cout<<"Picking up the mapper...";
  fMapper     = PndDchMapper::Instance();
  cout<< fMapper <<"\n Picking up the drifter..."<<endl;

  TString fDriftFile = gSystem->Getenv("VMCWORKDIR");
  fDriftFile += "/macro/params/";
  fDriftFile += "d2t_rtdb.dat";

//  fDrifter    = PndDchDrifter::Instance("d2t_rtdb.dat",1.);
  fDrifter    = PndDchDrifter::Instance(fDriftFile.Data(),1.);
 // cout<<fDrifter<<endl;


  // Get input array
  fPointArray = (TClonesArray*) ioman->GetObject("PndDchPoint");
  if ( ! fPointArray ) {
    cout << "-W- PndDchDigiProducer::Init(): "
	 << "No PndDchPoint array!" << endl;
    return kERROR;
  }

  // Create and register output array
  fDigiArray = new TClonesArray("PndDchDigi");
  ioman->Register("PndDchDigi","Dch",fDigiArray,kTRUE);

  // Create histograms
  CreateHistos();
  Reset();

  cout << "-I- PndDchDigiProducer: Intialization successfull" << endl;

  return kSUCCESS;

}

// Public method Exec
void PndDchDigiProducer::Exec(Option_t* opt) {

  // Reset output array
  if ( ! fDigiArray ) Fatal("Exec", "No DigiArray");
  fDigiArray->Clear();

  // Loop over PndDchPoints...
  PndDchPoint* point  = 0;
  Int_t nPoints = fPointArray->GetEntriesFast();
  Double_t pointTime;
  for (Int_t iPoint=0; iPoint<nPoints; iPoint++) {
    point  = (PndDchPoint*) fPointArray->At(iPoint);
    //... and register the Digis you want
    if (ToBeOrNotToBe(point)) {
      Int_t nAdded = AddDigis(point, iPoint);
      if(fVerbose>1){
	cout<<"PndDchDigiProducer::Exec(...):\n\t";
	cout<< "For this point I added "<< nAdded<< " digis"<<endl;
      }
    }
  }
}

// Private method AddDigi
Int_t PndDchDigiProducer::AddDigis(PndDchPoint* point, Int_t refIndex) {
	TClonesArray& clref = *fDigiArray;
	Int_t size = clref.GetEntriesFast();
	Double_t timeStamp = point->GetTime();
	Int_t     plane     = point->GetPlane();
	Int_t     chamber   = point->GetChamber();
	Double_t driftTime;
	vector<Int_t> wires;
	vector<Double_t> distances;
	Int_t nAdded = 0;
	Int_t nWires = fMapper->GetFiredWires(wires, distances, point);
	for(Int_t i = 0; i < nWires; i++){
	  if(nAdded!=0) break;
	  Int_t wire = wires[i];
	  if(fDrifter->CalculateDriftTime(driftTime,distances[i])){
	    PndDchDigi* digi = new(clref[size]) PndDchDigi( timeStamp, plane, chamber,
							    wire, driftTime, refIndex);
	    nAdded++;
	    if(fVerbose>1) digi->Print("");
	    fhDriftTime[chamber]->Fill(driftTime);
	    size++;
	  } else {
	    cout<< "caution: wrong drift time"<<endl;
	  }
	}
	return nAdded;
}

// Private method ToBeOrNotToBe digitised
Bool_t PndDchDigiProducer:: ToBeOrNotToBe(const PndDchPoint* point) const {
  Double_t  detID   = point->GetDetectorID();
//   if(fMapper->CalculateChamber(detID)>5)
//     return kFALSE;
  Double_t  trackID = point->GetTrackID();
  PndDchDigi* digi = 0;
  PndDchPoint* itspoint = 0;
  Int_t nDigis = fDigiArray->GetEntriesFast();
  for (Int_t i=0; i<nDigis; i++){
    digi = (PndDchDigi*) fDigiArray->At(i);
    itspoint = (PndDchPoint*) fPointArray->At(digi->GetRefIndex());
    if (detID == itspoint->GetDetectorID() &&
	trackID == itspoint->GetTrackID())
      return kFALSE;
  }
  return kTRUE;
}

// -----   Private method CreateHistos   -----------------------------------
void PndDchDigiProducer::CreateHistos() {

	// Histogram list
	fHistoList = new TList();

	for(Int_t ch = 0; ch < 9; ch++){
	    TString name = "fhDriftTime";
	    name +=ch;
	    TString title = "TDC output for chamber ";
	    title+=ch;
	    fhDriftTime[ch] = new TH1F(name, title, 50, -50.,200.);
	    fHistoList->Add(fhDriftTime[ch]);
	  }
}


// -----   Private method Reset   ------------------------------------------
void PndDchDigiProducer::Reset() {

	TIter next(fHistoList);
		while(TH1* histo = ((TH1*)next())) histo->Reset();
}


// -----   Private method Finish   -----------------------------------------
void PndDchDigiProducer::Finish() {

 	TDirectory *current = gDirectory;
	TDirectory *hdir = current->mkdir("DchDigiProducerHistos");
	hdir->cd();
	TIter next(fHistoList);
	while ( TH1* histo = ((TH1*)next()) ) histo->Write();
	current->cd();
}

ClassImp(PndDchDigiProducer)
