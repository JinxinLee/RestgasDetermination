/*
 * PndRingSorter.h
 *
 *  Created on: Jul 15, 2010
 *      Author: stockman
 */

#ifndef PndRingSorterT_H_
#define PndRingSorterT_H_

#include "TObject.h"
#include "PndSdsDigiPixel.h"
#include "PndSdsDigiStrip.h"

#include <vector>
#include <stack>
#include <map>

#include <iostream>

template <class T> class PndRingSorterT : public TObject{
public:
	PndRingSorterT(int size = 100, double width = 10):fLowerBoundPointer(0,0), fCellWidth(width), fVerbose(2)
	{
		fRingBuffer.resize(size);
	}

	virtual ~PndRingSorterT(){};

	virtual void AddElement(T& digi, double timestamp);
	virtual void WriteOutElements(int index);				///< writes out the entries from LowerBoundPointer up to index
	virtual void WriteOutElement(int index);					///< writes out the entry at the index and clears it
	virtual void WriteOutAll(){
		WriteOutElements(fLowerBoundPointer.first);
	}
	virtual double GetBufferSize(){return fCellWidth * fRingBuffer.size();}
	virtual std::vector<T> GetOutputData(){
		return fOutputData;
	}

	virtual void DeleteOutputData(){fOutputData.clear(); }
	virtual void SetLowerBound(int index);

	virtual void Print(std::ostream& out = std::cout){
		out << "RingSorter: Size " << fRingBuffer.size() << " CellWidth: " << fCellWidth << std::endl;
		out << "| ";
		for (int i = 0; i < fRingBuffer.size(); i++){
			out << fRingBuffer[i].size() << " |";
		}
		out << std::endl;
	}


private:
	int CalcIndex(double val);
	std::vector<std::multimap<double, T> > fRingBuffer;
	std::vector<T> fOutputData;
	std::pair<int, double> fLowerBoundPointer;
	double fCellWidth;
	int fVerbose;

	ClassDef(PndRingSorterT,1)

};

#endif /* PndRingSorterT_H_ */
