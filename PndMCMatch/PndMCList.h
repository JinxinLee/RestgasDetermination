/*
 * PndMCList.h
 *
 *  Created on: Dec 3, 2009
 *      Author: stockman
 */

#ifndef PNDMCLIST_H_
#define PNDMCLIST_H_

#include "TObject.h"
#include "PndDetectorList.h"

#include <vector>



class PndMCList : public TObject {
public:
	PndMCList();
	PndMCList(Int_t type, Int_t entry)
	{
		fType = type;
		fEntry = entry;
	}
	PndMCList(Int_t type, Int_t entry, std::vector<Int_t> list)
	{
		fType = type;
		fEntry = entry;
		fList = list;
	}

	virtual ~PndMCList();

	void SetType(Int_t type){ fType = type;}
	void SetEntry(Int_t entry){ fEntry = entry;}
	void AddElement(Int_t element){fList.push_back(element);}

	Int_t GetType() const {return fType;}
	Int_t GetEntry() const {return fEntry;}
	Int_t GetNElements() const {return fList.size();}
	Int_t GetElement(Int_t index)const {return fList.at(index);}
	std::vector<Int_t> GetElements() const {return fList;}

	void Reset(){fList.clear();}




private:
	std::vector<Int_t> fList;
	Int_t fEntry;
	Int_t fType;

	ClassDef(PndMCList, 1);
};

#endif /* PNDMCLIST_H_ */
