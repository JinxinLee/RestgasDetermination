// -------------------------------------------------------------------------
// -----                PndMvdConvertApvTast source file               -----
// -----                  Created 13/01/09  by L.Ackermann             -----
// -------------------------------------------------------------------------
// libc includes
#include <iostream>
#include <map>

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
  fPersistance = kTRUE;
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
  ioman->Register("MVDStripDigis", "MVD", fStripArray, fPersistance);
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

	std::map<TString,Double_t> OurMap;

  Int_t rw=-1, sw=-1;
  TString detpath=""; 
  TString detnameid;
  Int_t buffIndex = -1;

	std::vector<PndMvdDigiStrip> strips = fApvConvert->ReadNext();
	
	Int_t stripnum;

	for (std::vector<PndMvdDigiStrip>::iterator strip=strips.begin(); strip!=strips.end(); ++strip)
	{
    rw=strip->GetFE();
    fApvMapper->DoMapping(rw,sw,detpath);
    detnameid=fGeoH->GetID(detpath);


     
    if(fVerbose>1) Info("Exec","Write a Digi from detector %s %s",detpath.Data(),detnameid.Data());
	
    stripnum = fStripArray->GetEntriesFast();
    //cout << "stripnum: " << stripnum << endl;
		new ((*fStripArray)[stripnum]) PndMvdDigiStrip(strip->GetIndices(), strip->GetDetID(),
                              detnameid, sw, strip->GetChannel(), strip->GetCharge(), strip->GetTimestamp());
	


		buffIndex = strip->GetIndex();

		if (detpath.Contains("StripActiveTS"))

		  {

		    //	    cout << "Filling Bottom Side of single sided modules" << endl;
		    //stripnum = fStripArray->GetEntriesFast();		    
		    //cout << "stripnum: " << stripnum << endl;
		    
		    OurMap[detnameid]+= strip->GetCharge();

		  }


	}

	for (std::map<TString,Double_t>::iterator it=OurMap.begin();it!=OurMap.end();++it)
	  {

	    stripnum = fStripArray->GetEntriesFast();
	    
	new ((*fStripArray)[stripnum]) PndMvdDigiStrip(buffIndex, 0,
				it->first, 3, 0, it->second, 0);
	  }
	
}

void PndMvdConvertApvTask::Finish()
{}

ClassImp(PndMvdConvertApvTask);
