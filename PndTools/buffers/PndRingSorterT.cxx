/*
 * PndRingSorterT.cxx
 *
 *  Created on: Jul 30, 2010
 *      Author: stockman
 */

#include "PndRingSorterT.h"

template<class T>void PndRingSorterT<T>::AddElement(T& digi, double timestamp){

	if (timestamp < fLowerBoundPointer.second){
		std::cout << "-E- Timestamp " << timestamp << " below lower bound " << fLowerBoundPointer.second << std::endl;
		std::cout << digi << std::endl;
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
	fRingBuffer[index].insert(std::pair<double, T> (timestamp, digi));
}

template<class T> void PndRingSorterT<T>::SetLowerBound(double timestampOfHitToWrite){
	int index = CalcIndex(timestampOfHitToWrite + fCellWidth);

	int cellValue = timestampOfHitToWrite / fCellWidth;

	fLowerBoundPointer.second = ((cellValue + 1) * fCellWidth) - GetBufferSize();
	fLowerBoundPointer.first = index;
	if (fVerbose > 0) std::cout << "-I- PndRingSorterT<T>::SetLowerBound " << index << " / " << fLowerBoundPointer.second << std::endl;
}
template<class T> void PndRingSorterT<T>::WriteOutElements(int index){
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
	if (fVerbose > 1){
		std::cout << "-I- PndRingSorter::WriteOutElements: Size of Output-Array: " << fOutputData.size() << std::endl;
//		for (int i = 0; i < fOutputData.size(); i++)
//			std::cout << fOutputData[i].size() << " | ";
//		std::cout << std::endl;
	}
}
template<class T> void PndRingSorterT<T>::WriteOutElement(int index){
	std::multimap<double, T>* myDataField = &fRingBuffer.at(index);
	typename std::multimap<double, T>::iterator it;
	if (!myDataField->empty()) {
		if (fVerbose > 1)std::cout << "-I- PndRingSorterT:WriteOutElement " << myDataField->begin()->second << std::endl;
		for (it = myDataField->begin(); it != myDataField->end(); it++){
			fOutputData.push_back(it->second);
		}
		myDataField->clear();
	}
}

template<class T> int PndRingSorterT<T>::CalcIndex(double val){
	int index = val / fCellWidth;
	while (index >= fRingBuffer.size()){
		index -= fRingBuffer.size();
	}
	return index;
}

template class PndRingSorterT<PndSdsDigiPixel>;
template class PndRingSorterT<PndSdsDigiStrip>;

templateClassImp(PndRingSorterT);
