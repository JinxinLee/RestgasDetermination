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
PndMvdConvertApvTask::PndMvdConvertApvTask(PndMvdConvertApv* Apvconvert) : FairTask("Convert Task for PANDA PndMvd")
{
  fApvConvert=Apvconvert;
}
// -----   Destructor   ----------------------------------------------------
PndMvdConvertApvTask::~PndMvdConvertApvTask()
{
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

  // Create and register output array
  fStripArray = new TClonesArray("PndMvdDigiStrip");
  ioman->Register("MVDStripDigis", "MVD", fStripArray, kTRUE);
  fApvConvert->Init();
  cout<<"Init of Task"<<endl;
  return kSUCCESS;
}

// -----   Public method Exec   --------------------------------------------
void PndMvdConvertApvTask::Exec(Option_t* opt)
{
    // Reset output array
	fStripArray->Delete();
	std::vector<PndMvdDigiStrip> strips = fApvConvert->ReadNext();
	for (std::vector<PndMvdDigiStrip>::iterator strip=strips.begin(); strip!=strips.end(); ++strip)
	{
		Int_t iStrip = fStripArray->GetEntriesFast();
// 		std::cout<<" -I- PndMvdConvertApvTask::Exec(): Adding a strip digi at position "<<iStrip
// 				 <<"\n in sensor "<<(*strip).GetDetName().Data() <<std::endl;
		new ((*fStripArray)[iStrip]) PndMvdDigiStrip(*strip);
// 		new ((*fStripArray)[iStrip]) PndMvdDigiStrip((*strip).GetIndex(), (*strip).GetDetID(), (*strip).GetDetName(), (*strip).GetFE(), (*strip).GetChannel(), (*strip).GetCharge(), (*strip).GetTimestamp()) ;
	}
// std::cout<<" -I- fStripArray size: "<<fStripArray->GetEntriesFast()<<endl;;
}

void PndMvdConvertApvTask::Finish()
{
// std::cout<<" -I- final fStripArray size: "<<fStripArray->GetEntriesFast()<<endl;;

}

ClassImp(PndMvdConvertApvTask);
