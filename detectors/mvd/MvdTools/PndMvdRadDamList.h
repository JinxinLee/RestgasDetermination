/*
 * PndMvdRadDamList.h
 *
 *  Created on: Dec 16, 2008
 *      Author: stockman
 */

#ifndef PNDMVDRADDAMLIST_H
#define PNDMVDRADDAMLIST_H

#include <vector>
#include <utility>

#include "TString.h"

class PndMvdRadDamList
{
public:
	PndMvdRadDamList(TString fileName);
	virtual ~PndMvdRadDamList();

	Double_t GetWeight(Double_t energy);

private:
	std::vector<std::pair<Double_t, Double_t> > fList;

	Int_t FindClosestEnergyIndex(Double_t energy);
	Double_t Interpolate(Double_t energy, Int_t index);

	ClassDef(PndMvdRadDamList,1);

};

#endif /* PNDMVDRADDAMLIST_H */
