
// -------------------------------------------------------------------------
// -----                PndLmdStripHitProducer source file             -----
// -------------------------------------------------------------------------

/*
 * Updated by h.xu@fz-juelich.de on Jun30.2010
 * To match the base class SdsStripHitProducer,the virtual functions SetParContainers() and
 * SetCalculators() were reloaded.
 */


//LUMI
#include "PndLmdStripHitProducer.h"
//PANDA
//#include "PndSdsContFact.h"
#include "PndLmdContFact.h"
#include "PndSdsStripDigiPar.h"
//SDS
#include "PndSdsIdealChargeConversion.h"
#include "PndSdsTotChargeConversion.h"
#include "PndSdsTotDigiPar.h"
//FAIR
#include "FairRun.h"
#include "FairRuntimeDb.h"
//ROOT
#include "TList.h"

// -----   Default constructor   -------------------------------------------
PndLmdStripHitProducer::PndLmdStripHitProducer() :
  PndSdsStripHitProducer("PndLmdStripHitProducer")
{
//	fDigiParameterList = new TList();
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndLmdStripHitProducer::~PndLmdStripHitProducer()
{
  delete fGeoH;
}
// -------------------------------------------------------------------------

void PndLmdStripHitProducer::SetBranchNames(TString inBranchname, TString outBranchname, TString folderName)
{
  fBranchName = inBranchname;
  fOutBranchName = outBranchname;
  fFolderName = folderName;
}

void PndLmdStripHitProducer::SetBranchNames()
{
  fBranchName = "LMDPoint";
  fOutBranchName = "LMDStripDigis";
  fFolderName = "PndLmd";
}

void PndLmdStripHitProducer::SetParContainers()
{
  // called from the FairRun::Init()
  // Caution: The Parameter Set is not filled from the DB IO, yet.
  // This will be done just before this Tasks Init() is called.

  FairRun* ana = FairRun::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();

  PndLmdContFact* thelmdcontfact = (PndLmdContFact*)rtdb->getContFactory("PndLmdContFact");
  TList* theContNames = thelmdcontfact->GetDigiParNames();
  Info("SetParContainers()","The container names list contains %i entries",theContNames->GetEntries());
  TIter cfIter(theContNames);
  while (TObjString* contname = (TObjString*)cfIter()) {
    TString parsetname = contname->String();
    Info("SetParContainers()",parsetname.Data());
    if(parsetname.BeginsWith("LmdStripDigiPar")){
      PndSdsStripDigiPar* digipar = (PndSdsStripDigiPar*)(rtdb->getContainer(parsetname.Data()));
      if ( ! digipar ) Fatal("SetParContainers","No DIGI parameter found: %s",parsetname.Data());
      fDigiParameterList->Add(digipar);
     // digipar->Print();
    }
  }
  PndSdsStripHitProducer::SetParContainers();
}

void PndLmdStripHitProducer::SetCalculators()
{
	PndSdsStripHitProducer::SetCalculators();
  //calculator for TOT charge conversion:
	TIter params(fDigiParameterList);
	TIter totparams(fChargeDigiParameterList);
	while( PndSdsStripDigiPar* digipar = (PndSdsStripDigiPar*)params() ){
		PndSdsTotDigiPar* totdigipar = (PndSdsTotDigiPar*) totparams();
		if ( 0==digipar ) continue;
		const char* senstype = digipar->GetSensType();
		if(fVerbose>2)cout<<"The senstype is"<<senstype<<endl;
		if ( digipar->GetChargeConvMethod() == 0 ){
			if(fVerbose>0)	Info("SetCalculators()","Use Ideal charge conversion for %s sensors",senstype);
			fChargeConverter[senstype] = new PndSdsIdealChargeConversion();
		}
		else if (digipar->GetChargeConvMethod() == 1){
			if(fVerbose>0)	Info("SetCalculators()","Use Tot charge conversion for %s sensors",senstype);
			      fChargeConverter[senstype] = new PndSdsTotChargeConversion(
			                                                                 totdigipar->GetChargingTime(),
			                                                                 totdigipar->GetConstCurrent(),
			                                                                 digipar->GetThreshold(),
			                                                                 totdigipar->GetClockFrequency(),
			                                                                 fVerbose);
		}
		else Fatal ("SetCalculators()","charge conversion method not defined!");
	}
}


ClassImp(PndLmdStripHitProducer);

