/*
 * PndRingSorterT.cxx
 *
 *  Created on: Jul 30, 2010
 *      Author: stockman
 */

#include "PndRingSorterT.h"

template<class T>void PndRingSorterT<T>::AddElement(T& digi, int timestamp){

	if (timestamp < fLowerBoundPointer.second){
		std::cout << "-E- Timestamp " << timestamp << " below lower bound " << fLowerBoundPointer.second << std::endl;
		return;
	}
	int index = CalcIndex(timestamp);
	if (timestamp >= fLowerBoundPointer.second + GetBufferSize()){
		if (fVerbose > 0) std::cout << "-I- PndRingSorterT::AddElement : Timestamp " << timestamp << " larger than bufferspace: " << fLowerBoundPointer.second + GetBufferSize() << " writing out " << index+1 << std::endl;
		WriteOutElements(index+1);
		SetLowerBound(index+1);
	}
	fRingBuffer[index].push(digi);
}

template<class T> void PndRingSorterT<T>::SetLowerBound(int index){
	int offset = index - fLowerBoundPointer.first;
	while (offset < 0){
		offset += fRingBuffer.size();
		fLowerBoundPointer.second += GetBufferSize();
	}
	fLowerBoundPointer.second += offset * fCellWidth;
	fLowerBoundPointer.first = index;
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
		for (int i = 0; i < fOutputData.size(); i++)
			std::cout << fOutputData[i].size() << " | ";
		std::cout << std::endl;
	}
}
template<class T> void PndRingSorterT<T>::WriteOutElement(int index){
	std::stack<T>* myStack = &fRingBuffer.at(index);
	if (!myStack->empty()) {
		if (fVerbose > 1)std::cout << "RingSorter: " << std::endl;
		fOutputData.push_back(*myStack);
		while (!myStack->empty()) {
			if (fVerbose > 1)std::cout << myStack->top() << std::endl;
			myStack->pop();
		}
		std::cout << std::endl;
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

templateClassImp(PndRingSorterT);
