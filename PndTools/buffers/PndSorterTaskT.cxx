// -------------------------------------------------------------------------
// -----                PndSorterTaskT source file             -----
// -------------------------------------------------------------------------

#include "PndSorterTaskT.h"
#include "TClonesArray.h"

#include "FairRootManager.h"




template<class T> InitStatus PndSorterTaskT<T>::ReInit()
{
  return kSUCCESS;
}

// -----   Public method Init   --------------------------------------------
template<class T> InitStatus PndSorterTaskT<T>::Init()
{

  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman )
  {
    std::cout << "-E- PndSorterTaskT::Init: "
    << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  fSorter = new PndRingSorterT<T>(fNumberOfCells, fWidthOfCells);

  // Create and register output array
  fInputArray = FairRootManager::Instance()->GetTClonesArray(fInputBranch);

  if(fVerbose>1) Info("Init","Registering this branch: %s/%s","PndMvd",fOutputBranch.Data());
  fOutputArray = ioman->Register(fOutputBranch, fInputArray->GetClass()->GetName(), fFolder, fPersistance);
  
  
  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
template<class T> void PndSorterTaskT<T>::Exec(Option_t* opt)
{


  
  fInputArray = FairRootManager::Instance()->GetTClonesArray(fInputBranch);
  std::cout << "-I- PndSorterTaskT: Size PixelArray: " << fInputArray->GetEntriesFast() << std::endl;
  for (int i = 0; i < fInputArray->GetEntriesFast(); i++){
	  T* myData = (T*)fInputArray->At(i);
	  myData->SetEntryNr(FairLink(0, fEntryNr, fInputBranch, i));
	  if (fVerbose > 2) std::cout << "Sorter filled with: " << *myData << std::endl;
	  fSorter->AddElement(*myData, ((FairTimeStamp*)myData)->GetTimeStamp());
  }
  if (fVerbose > 2)fSorter->Print();

  std::vector<T> sortedData = fSorter->GetOutputData();
  fSorter->DeleteOutputData();

  fOutputArray = FairRootManager::Instance()->GetEmptyTClonesArray(fOutputBranch);
  for (int i = 0; i < sortedData.size(); i++){
	  T* mySortedData = new ((*fOutputArray)[i])T(sortedData[i]);
	  std::cout << "SortedData written: " << *mySortedData << std::endl;
  }
  fEntryNr++;
}

// -------------------------------------------------------------------------

template<class T> void PndSorterTaskT<T>::FinishEvent(){
	  fOutputArray->Delete();
}

template<class T> void PndSorterTaskT<T>::FinishTask()
{
	fInputArray = FairRootManager::Instance()->GetTClonesArray(fInputBranch);
	if (fVerbose > 2) std::cout << "-I- PndSorterTaskT::FinishTask Size InputArray: " << fInputArray->GetEntriesFast() << std::endl;
	for (int i = 0; i < fInputArray->GetEntriesFast(); i++) {
		T* myDigi = (T*) fInputArray->At(i);
		fSorter->AddElement(*myDigi, ((FairTimeStamp*)myDigi)->GetTimeStamp());
	}
	fSorter->WriteOutAll();
	std::vector<T> sortedData = fSorter->GetOutputData();
	fSorter->DeleteOutputData();

	FairRootManager* ioman = FairRootManager::Instance();
	fOutputArray = ioman->GetEmptyTClonesArray(fOutputBranch);
	for (int i = 0; i < sortedData.size(); i++) {
		T* mySortedData = new ((*fOutputArray)[i]) T(	sortedData[i]);
		if (fVerbose > 2)std::cout << fOutputBranch << " written: " << *mySortedData << std::endl;
	}

	//ioman->SaveAllContainers();

}

template class PndSorterTaskT<PndSdsDigiPixel>;
template class PndSorterTaskT<PndSdsDigiStrip>;

templateClassImp(PndSorterTaskT);
