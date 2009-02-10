/*
 * PndMvdRadDamTask.h
 *
 *  Created on: Dec 16, 2008
 *      Author: stockman
 */

#ifndef PNDMVDRADDAMTASK_H_
#define PNDMVDRADDAMTASK_H_

#include "CbmTask.h"
#include "PndMvdRadDamList.h"
#include "PndMvdGeoHandling.h"

#include "TClonesArray.h"
#include "TH2.h"

#include <map>
#include <string>

class PndMvdRadDamTask : public CbmTask
{
public:
	PndMvdRadDamTask();
	~PndMvdRadDamTask();

	  virtual void SetParContainers();
	  virtual InitStatus Init();
	  virtual InitStatus ReInit();

	  /** Virtual method Exec **/
	  virtual void Exec(Option_t* opt);

	  virtual void Finish();

private:
	TClonesArray* fMCTracks;
	TClonesArray* fMCHits;
	TClonesArray* fRadDamHits;

	PndMvdRadDamList* fElectronList;
	PndMvdRadDamList* fProtonList;
	PndMvdRadDamList* fNeutronList;
	PndMvdRadDamList* fPionList;

	PndMvdGeoHandling* fGeoH;

	std::map<std::string, TH2*> fMapDetHistos;
	TH1D* fRadDamHisto;

	std::map<Int_t, PndMvdRadDamList*> fWeightListsMap;
	void InitWeightLists();

	  ClassDef(PndMvdRadDamTask,1);

};

#endif /* PNDMVDRADDAMTASK_H_ */
