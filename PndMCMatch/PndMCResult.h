/*
 * PndMCResult.h
 *
 *  Created on: Dec 1, 2009
 *      Author: stockman
 */

#ifndef PNDMCRESULT_H_
#define PNDMCRESULT_H_

#include "PndDetectorList.h"
#include "PndMCObject.h"

#include <TObject.h>
#include <vector>
#include <string>

#include<iostream>

class PndMCResult: public PndMCObject {
public:
	PndMCResult();
	PndMCResult(Int_t start, Int_t stop);
	PndMCResult(const PndMCResult& result):
		PndMCObject(result.GetStartType(), result.GetEntryVector()),
		fStartType(result.GetStartType()),
		fStopType(result.GetStopType())
		{}

	virtual ~PndMCResult();

	void SetStartType(Int_t start){fStartType = start;}
	void SetStopType(Int_t stop){fStopType = stop;}

	Int_t GetStartType(void) const {return fStartType;}
	Int_t GetStopType(void) const {return fStopType;}

	void operator=(const PndMCResult& result){
		fStartType = result.GetStartType();
		fStopType = result.GetStopType();
		SetStage(result.GetEntryVector());
	}

	virtual void Print(std::ostream& out = std::cout){ out << *this;}

	friend std::ostream& operator<< (std::ostream& out, const PndMCResult& result){
		out << "MC Link from: " << result.GetStartType() << " to "  << result.GetStopType() << ":" << std::endl;
		((PndMCObject)result).Print(out);
		return out;
	}


private:
	Int_t fStartType;
	Int_t fStopType;

	ClassDef(PndMCResult, 2);
};

#endif /* PndMCResult_H_ */
