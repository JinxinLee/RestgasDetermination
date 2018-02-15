/*
 * PndRingSorterT.cxx
 *
 *  Created on: Jul 30, 2010
 *      Author: stockman
 */

#include "PndRingSorter.h"

void PndRingSorter::AddElement(FairTimeStamp* digi, double timestamp){

	FairTimeStamp* newElement = CreateElement(digi);

	if (timestamp < fLowerBoundPointer.second){
		std::cout << "-E- Timestamp " << timestamp << " below lower bound " << fLowerBoundPointer.second << std::endl;
		newElement->Print();
		return;
	}
	int index = CalcIndex(timestamp);

	if (timestamp >= fLowerBoundPointer.second + (2 * GetBufferSize())){
		if (fVerbose > 0) std::cout << "-I- PndRingSorterT::AddElement : Timestamp " << timestamp << " larger than 2 * bufferspace: " << fLowerBoundPointer.second + GetBufferSize() << " writing out " << index+1 << std::endl;
		WriteOutAll();
		SetLowerBound(timestamp);
	}
	else if (timestamp >= fLowerBoundPointer.second + GetBufferSize()){
		if (fVerbose > 0) std::cout << "-I- PndRingSorterT::AddElement : Timestamp " << timestamp << " larger than bufferspace: " << fLowerBoundPointer.second + GetBufferSize() << " writing out " << index+1 << std::endl;
		WriteOutElements(index+1);
		SetLowerBound(timestamp);
	}
	fRingBuffer[index].insert(std::pair<double, FairTimeStamp*> (timestamp, newElement));
}

 void PndRingSorter::SetLowerBound(double timestampOfHitToWrite){
	int index = CalcIndex(timestampOfHitToWrite + fCellWidth);

	int cellValue = timestampOfHitToWrite / fCellWidth;

	fLowerBoundPointer.second = ((cellValue + 1) * fCellWidth) - GetBufferSize();
	fLowerBoundPointer.first = index;
	if (fVerbose > 0) std::cout << "-I- PndRingSorter::SetLowerBound " << index << " / " << fLowerBoundPointer.second << std::endl;
}

 void PndRingSorter::WriteOutElements(int index){
	if (fLowerBoundPointer.first >= index){
		for (int i = fLowerBoundPointer.first; i < fRingBuffer.size(); i++)
			WriteOutElement(i);
		for (int i = 0; i < index; i++)
			WriteOutElement(i);
	}
	else{
		for (int i = fLowerBoundPointer.first; i < index; i++)
			WriteOutElement(i);
	}
//	if (fVerbose > 1){
		std::cout << "-I- PndRingSorter::WriteOutElements: Size of Output-Array: " << fOutputData.size() << std::endl;
//		for (int i = 0; i < fOutputData.size(); i++)
//			std::cout << fOutputData[i].size() << " | ";
//		std::cout << std::endl;
//	}
}

 void PndRingSorter::WriteOutElement(int index){
	std::multimap<double, FairTimeStamp*>* myDataField = &fRingBuffer.at(index);
	std::multimap<double, FairTimeStamp*>::iterator it;
	if (!myDataField->empty()) {
		if (fVerbose > 1);
		std::cout << "-I- PndRingSorterT:WriteOutElement ";
		myDataField->begin()->second->Print();
		std::cout << std::endl;
		for (it = myDataField->begin(); it != myDataField->end(); it++){
			fOutputData.push_back(it->second);
		}
		myDataField->clear();
	}
}

 int PndRingSorter::CalcIndex(double val){
	int index = val / fCellWidth;
	while (index >= fRingBuffer.size()){
		index -= fRingBuffer.size();
	}
	return index;
}

ClassImp(PndRingSorter);
