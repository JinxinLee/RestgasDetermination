// -------------------------------------------------------------------------
// -----                    PndMvdDigiTask source file                 -----
// -----                  Created 07/10/08  by R. Kliemt               -----
// -----                  Wrapper for Mvd digitizing tasks             -----
// -------------------------------------------------------------------------


#include "PndMvdDigiTask.h"
#include "PndMvdHybridHitProducer.h"
#include "PndMvdStripHitProducer.h"
#include "PndMvdNoiseProducer.h"
#include "PndSdsDigiPixelSorterTask.h"
#include "PndSdsDigiStripSorterTask.h"

#include "PndMvdTimeWalkCorrTask.h"

// -----   Default constructor   -------------------------------------------
PndMvdDigiTask::PndMvdDigiTask() :
  PndBlackBoxTask("MVD Digitization BlacBox Task"),
  fPersistance(kTRUE)
{
  this->Add(new PndMvdHybridHitProducer());
  this->Add(new PndMvdStripHitProducer());
  this->Add(new PndMvdNoiseProducer());
     SetPersistance(fPersistance);
}
// -------------------------------------------------------------------------

void PndMvdDigiTask::SetPersistance(Bool_t pers)
{
  fPersistance = pers;
  TList* thistasks = this->GetListOfTasks();
  ((PndMvdHybridHitProducer*)thistasks->At(0))->SetPersistance(fPersistance);
  ((PndMvdStripHitProducer*)thistasks->At(1))->SetPersistance(fPersistance);
  ((PndMvdNoiseProducer*)thistasks->At(2))->SetPersistance(fPersistance);
  return;
}

void PndMvdDigiTask::RunTimeBased()
{
	 TList* thistasks = this->GetListOfTasks();
	((PndMvdHybridHitProducer*)thistasks->At(0))->RunTimeBased();
	((PndMvdStripHitProducer*)thistasks->At(1))->RunTimeBased();
	((PndMvdNoiseProducer*)thistasks->At(2))->RunTimeBased();

	this->Add(new PndSdsDigiPixelSorterTask(10000, 10, "MVDPixelDigis", "MVDSortedPixelDigis", "PndMvd")); //"MVDDigisCorr"
	this->Add(new PndSdsDigiStripSorterTask(10000, 10, "MVDStripDigis", "MVDSortedStripDigis", "PndMvd"));

	std::cout << "PndMvdDigiTask: PndMvdPixelDigiSorterTask added" << std::endl;
}

// -----   Destructor   ----------------------------------------------------
PndMvdDigiTask::~PndMvdDigiTask()
{
}
// -------------------------------------------------------------------------


ClassImp(PndMvdDigiTask);
