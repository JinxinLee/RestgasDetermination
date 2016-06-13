/*
 * PndMCEntry.h
 *
 *  Created on: Dec 22, 2009
 *      Author: stockman
 */

#ifndef PNDMCENTRY_H_
#define PNDMCENTRY_H_

#include "PndMCList.h"
#include "PndDetectorList.h"
#include "FairLink.h"
#include "FairMultiLinkedData.h"

#include <utility>
#include <iostream>
#include <sstream>

class PndMCEntry : public FairMultiLinkedData {
public:
	PndMCEntry();
PndMCEntry(std::set<FairLink> links, Int_t source = -1, Int_t pos = -1)
		:FairMultiLinkedData(links), fSource(source), fPos(pos){
		SetPersistanceCheck(kFALSE);
	}
	PndMCEntry(FairMultiLinkedData links, Int_t source = -1, Int_t pos = -1)
		:FairMultiLinkedData(links), fSource(source), fPos(pos){
		SetPersistanceCheck(kFALSE);
	}

//	PndMCEntry(std::vector<std::pair<int, int> > links, Int_t source = -1, Int_t pos = -1)
//			:FairMultiLinkedData(links), fSource(source), fPos(pos){}

	void SetSource(Int_t source){fSource = source;}
	void SetPos(Int_t pos){fPos = pos;}

	Int_t GetSource() const {return fSource;}
	Int_t GetPos() const {return fPos;}

	void RemoveType(Int_t type);

	virtual ~PndMCEntry();

	virtual void Print(std::ostream& out){
		out << *this;
	}

	friend std::ostream& operator<< (std::ostream& out, const PndMCEntry& link){
		//out << "Source: " << link.GetSource() << " Position: " << link.GetPos() << std::endl;
#ifdef _HavePrintLinkInfo
                ((FairMultiLinkedData)link).PrintLinkInfo(out);
#else
//  		std::stringstream ss;
//  		ss << out.rdbuf();
//  		std::string myString = ss.str();
//              ((FairMultiLinkedData)link).Print(myString.c_str());
#endif
		return out;
	}


private:
	Int_t fSource;
	Int_t fPos;

	ClassDef(PndMCEntry, 1);
};

#endif
