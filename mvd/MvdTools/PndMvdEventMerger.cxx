#include "PndMvdEventMerger.h"
#include "TFile.h"
#include "TTree.h"

#include <iostream>
#include "PndSdsMCPoint.h"
#include "PndSdsDigiStrip.h"
#include "PndSdsDigiPixel.h"
using std::cout;
using std::endl;

PndMvdEventMerger::PndMvdEventMerger()
{
	//fBuffer     = 0;
	//fBuffer = new TObjArray();	
}
PndMvdEventMerger::PndMvdEventMerger(TString fileName, TString branchName, Int_t nEvents, Int_t nMerged)
{
//	fBuffer = new TObjArray();
	if ( ! FillBuffer(fileName, branchName, nEvents, nMerged) )
		cout << "-E- PndMvdEventMerger: Error in filling buffer";
}
PndMvdEventMerger::~PndMvdEventMerger()
{
	//fBuffer->Delete();
	//delete fBuffer;
	//fBuffer = 0;
}

TClonesArray* PndMvdEventMerger::GetEvent(Int_t iEvent)
{
	//if ( !fBuffer)
	//	std::cout << "-E- CbmMvdPileupManager::GetEvent: No event buffer!";

	if ((unsigned int)iEvent > fBuffer.size())
	{
		cout << "-W- CbmMvdPileupManager::GetEvent: Event " << iEvent
				<< " not present in buffer! " << endl;
		cout << "                                   Returning NULL pointer! "
				<< endl;
		return NULL;
	}

	TClonesArray* pArray = (TClonesArray*) fBuffer[iEvent];

	if ( !pArray)
	{
		cout << "-W CbmMvdPileupManager::GetEvent: Returning NULL pointer!"
				<< endl;
		return NULL;
	}

	return pArray;
}

Int_t PndMvdEventMerger::FillBuffer(TString fileName, TString branchName, Int_t nEvents, Int_t nMerged)
{
	//if ( !fBuffer)
	//	cout << "-E- Fill Buffer: No event buffer!";

	//fBuffer->Delete();


	TClonesArray* pointArray= NULL;
	TClonesArray* mergedPointsArray = NULL;
	//TFile* saveGFile = gFile;

	TFile* bgfile = new TFile(fileName);
	if ( !bgfile)
	{
		cout << "-W- CbmMvdPileupManager::FillBuffer:  Background file "
				<< fileName << " could noy be opened! " << endl;
		return 0;
	}
	cout << "-I- CbmMvdPileupManager::FillBuffer: Opening file " << endl;
	cout << fileName << endl;

	TTree* bgtree = (TTree*) bgfile->Get("cbmsim");
	if ( !bgtree)
	{
		cout << "-W- CbmMvdPileupManager::FillBuffer:  "
				<< "Could not find cbmsim tree in background file " << endl;
		return 0;
	}

	Int_t nEventsInFile = bgtree->GetEntries();
	cout << "-I- CbmMvdPileupManager::FillBuffer: " << nEventsInFile
			<< " events in file" << endl;
	Int_t nBuffer = TMath::Min(nEvents, nEventsInFile);
	cout << "-I- CbmMvdPileupManager::FillBuffer: Buffering " << nBuffer
			<< " events" << endl;
	
	Int_t nMergedBuffer = TMath::Min(nMerged, nEventsInFile);
	

	bgtree->SetBranchAddress(branchName, &pointArray);

	for (Int_t iEvent=0; iEvent<nMergedBuffer; iEvent++)
	{
		bgtree->GetEntry(iEvent);
		if (iEvent == 0)
			mergedPointsArray = (TClonesArray*)pointArray->Clone();
		else {
			std::cout << "New size of mergedPointsArray: " << AddTClonesArray(mergedPointsArray, pointArray) << std::endl;
		}
	}
	

	//fBuffer = new TObjArray(nBuffer,0);
	for (Int_t iEvent=0; iEvent<nBuffer; iEvent++)
	{
		std::cout << "Adding Event: " << iEvent << " to buffer!" << std::endl;
		std::cout << "Merged PointsArray Size: " << mergedPointsArray->GetEntriesFast() << std::endl;
		//std::cout << fBuffer << std::endl;
		fBuffer.push_back((TClonesArray*)mergedPointsArray);
	}

//	delete bgtree;
	bgfile->Close();
//	delete bgfile;
//	saveGFile->cd();

	return nBuffer;
}

Int_t PndMvdEventMerger::AddTClonesArray(TClonesArray* target, TClonesArray* source)
{
	Int_t origSize = target->GetEntriesFast();
	for (int i = 0; i < source->GetEntriesFast(); i++){
		std::cout << source->GetClass()->GetName() << std::endl;
		TString myClassName(source->GetClass()->GetName());
		if(myClassName.Contains("PndSdsMCPoint"))
			new ((*target)[origSize + i])PndSdsMCPoint(*(PndSdsMCPoint*)(source->At(i)));
		else if (myClassName.Contains("PndSdsDigiPixel"))
			new ((*target)[origSize + i])PndSdsDigiPixel(*(PndSdsDigiPixel*)(source->At(i)));
		else if (myClassName.Contains("PndSdsDigiStrip"))
			new ((*target)[origSize + i])PndSdsDigiStrip(*(PndSdsDigiStrip*)(source->At(i)));
//		else if (myClassName.Contains("PndTpcPoint"))
//			new ((*target)[origSize + i])PndTpcPoint(*(PndTpcPoint*)(source->At(i)));
//		else if (myClassName.Contains("PndTpcCluster"))
//			new ((*target)[origSize + i])PndTpcCluster(*(PndTpcCluster*)(source->At(i)));
		else std::cout << "The Class: " << myClassName << " is unknown!" << std::endl;
				
	}
	return target->GetEntriesFast();
}

ClassImp(PndMvdEventMerger);
