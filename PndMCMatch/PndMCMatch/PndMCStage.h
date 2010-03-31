/*
 * PndMCStage.h
 *
 *  Created on: Dec 1, 2009
 *      Author: stockman
 */

#ifndef PNDMCSTAGE_H_
#define PNDMCSTAGE_H_

#include "PndDetectorList.h"
#include "PndMCObject.h"

#include <TObject.h>
#include <vector>
#include <string>

#include<iostream>

class PndMCStage: public PndMCObject {
public:
	PndMCStage();
	PndMCStage(fDetectorType id, std::string fileName, std::string branchName, Double_t weight = 1.0);
	PndMCStage(const PndMCStage& mcStage):
		PndMCObject((PndMCObject)mcStage),
		fBranchName(mcStage.GetBranchName()),
		fFileName(mcStage.GetFileName()),
		fWeight(mcStage.GetWeight())
	{}

	virtual ~PndMCStage();

	void SetBranchName(std::string branchName)	{ fBranchName = branchName; }
	void SetFileName(std::string fileName)			{ fFileName = fileName; }
	void SetWeight(Double_t weight)				{ fWeight = weight; }

	std::string	GetBranchName(void) 	const {return fBranchName;}
	std::string GetFileName(void) 		const {return fFileName;}
	Double_t 	GetWeight(void) 		const {return fWeight;}

	void operator=(const PndMCStage& result){
		PndMCObject::operator=(result);
		fBranchName = result.GetBranchName();
		fFileName = result.GetFileName();
		fWeight = result.GetWeight();
		SetStage(result.GetEntryVector());
	}

	virtual void Print(std::ostream& out){out << *this;}

	friend std::ostream& operator<< (std::ostream& out, const PndMCStage& stage){
		out << stage.GetStageId() << ": " << stage.GetBranchName() << " // " <<  stage.GetFileName() << std::endl; //" with weight: " << stage.GetWeight() << std::endl;
		((PndMCObject)stage).Print(out);
		return out;
	}

private:
	std::string fBranchName;
	std::string fFileName;
	Double_t fWeight;


	ClassDef(PndMCStage, 1);
};

#endif /* PNDMCSTAGE_H_ */
