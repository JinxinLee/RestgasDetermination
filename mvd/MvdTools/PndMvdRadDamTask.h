/*
 * PndMvdRadDamTask.h
 *
 *  Created on: Dec 16, 2008
 *      Author: stockman
 */

#ifndef PNDMVDRADDAMTASK_H
#define PNDMVDRADDAMTASK_H

#include "FairTask.h"
#include "PndMvdRadDamList.h"
#include "PndGeoHandling.h"

#include "TClonesArray.h"
#include "TH2.h"

#include <map>
#include <string>

class PndMvdRadDamTask : public FairTask
{
public:
	PndMvdRadDamTask();
	~PndMvdRadDamTask();
  PndMvdRadDamTask(const PndMvdRadDamTask& o) :
    fPersistance(o.fPersistance),
	  fMCTracks(o.fMCTracks),
	  fMCHits(o.fMCHits),
	  fRadDamHits(o.fRadDamHits),
 	  fElectronList(o.fElectronList),
	  fProtonList(o.fProtonList),
	  fNeutronList(o.fNeutronList),
	  fPionList(o.fPionList),
	  fGeoH(o.fGeoH),
	  fMapDetHistos(o.fMapDetHistos),
	  fRadDamHisto(o.fRadDamHisto),
	  fWeightListsMap(o.fWeightListsMap)
  {};
  PndMvdRadDamTask& operator=(const PndMvdRadDamTask& o)
  {
    fPersistance=o.fPersistance;
	  fMCTracks=o.fMCTracks;
	  fMCHits=o.fMCHits;
	  fRadDamHits=o.fRadDamHits;
 	  fElectronList=o.fElectronList;
	  fProtonList=o.fProtonList;
	  fNeutronList=o.fNeutronList;
	  fPionList=o.fPionList;
	  fGeoH=o.fGeoH;
	  fMapDetHistos=o.fMapDetHistos;
	  fRadDamHisto=o.fRadDamHisto;
	  fWeightListsMap=o.fWeightListsMap;
    return *this;  
  };

	  virtual void SetParContainers();
	  virtual InitStatus Init();
	  virtual InitStatus ReInit();

	  /** Virtual method Exec **/
	  virtual void Exec(Option_t* opt);

	  virtual void Finish();

  void SetPersistance(Bool_t p = kTRUE){fPersistance=p;};
  Bool_t GetPersistance() {return fPersistance;};
  
private:
  Bool_t fPersistance; // switch to turn on/off storing the arrays to a file
	TClonesArray* fMCTracks;
	TClonesArray* fMCHits;
	TClonesArray* fRadDamHits;

	PndMvdRadDamList* fElectronList;
	PndMvdRadDamList* fProtonList;
	PndMvdRadDamList* fNeutronList;
	PndMvdRadDamList* fPionList;

	PndGeoHandling* fGeoH;

	std::map<std::string, TH2*> fMapDetHistos;
	TH1D* fRadDamHisto;

	std::map<Int_t, PndMvdRadDamList*> fWeightListsMap;
	void InitWeightLists();

	  ClassDef(PndMvdRadDamTask,1);

};

#endif /* PNDMVDRADDAMTASK_H */
