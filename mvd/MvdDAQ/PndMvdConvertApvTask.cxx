// -------------------------------------------------------------------------
// -----                PndMvdConvertApvTast source file               -----
// -----                  Created 13/01/09  by L.Ackermann             -----
// -------------------------------------------------------------------------
// libc includes
#include <iostream>

// Root includes
#include "TROOT.h"
#include "TClonesArray.h"


// framework includes
#include "FairRootManager.h"
// PndMvd includes
#include "PndMvdConvertApvTask.h"
#include "PndMvdConvertApv.h"

#include "PndMvdDigiStrip.h"
#include <vector>

using namespace std;

// -----   Default constructor   -------------------------------------------
PndMvdConvertApvTask::PndMvdConvertApvTask(PndMvdConvertApv* Apvconvert,PndMvdMapApv* Apvmapper) : FairTask("Convert Task for PANDA PndMvd")
{
  fApvConvert=Apvconvert;
  fApvMapper=Apvmapper;
}
// -----   Destructor   ----------------------------------------------------
PndMvdConvertApvTask::~PndMvdConvertApvTask()
{
  if(fGeoH) delete fGeoH;
}

// -----   Public method Init   --------------------------------------------
InitStatus PndMvdConvertApvTask::Init()
{
  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman )
    {
      std::cout << "-E- PndMvdStripHitProducer::Init: "
     << "RootManager not instantiated!" << std::endl;
      return kFATAL;
    }
  
  fGeoH = new PndMvdGeoHandling();

  // Create and register output array
  fStripArray = new TClonesArray("PndMvdDigiStrip");
  ioman->Register("MVDStripDigis", "MVD", fStripArray, kTRUE);
  fApvConvert->Init();
  fApvMapper->Init();
  cout<<"Init of Task"<<endl;
  return kSUCCESS;
}

// -----   Public method Exec   --------------------------------------------
void PndMvdConvertApvTask::Exec(Option_t* opt)
{
    // Reset output array
	fStripArray->Delete();
  Int_t rw=-1, sw=-1;
  const char* detpath=""; 
  TString detnameid;
	std::vector<PndMvdDigiStrip> strips = fApvConvert->ReadNext();
	for (std::vector<PndMvdDigiStrip>::iterator strip=strips.begin(); strip!=strips.end(); ++strip)
	{
    rw=strip->GetFE();
    fApvMapper->DoMapping(rw,sw,detpath);
    detnameid=fGeoH->GetID((TString)detpath);
		Int_t stripnum = fStripArray->GetEntriesFast();
		new ((*fStripArray)[stripnum]) PndMvdDigiStrip(strip->GetIndices(), strip->GetDetID(),
                              detnameid, sw, strip->GetChannel(), strip->GetCharge(), strip->GetTimestamp());
	}
}

void PndMvdConvertApvTask::Finish()
{}

ClassImp(PndMvdConvertApvTask);
