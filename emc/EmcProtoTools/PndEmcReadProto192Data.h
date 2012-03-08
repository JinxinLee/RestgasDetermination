/*******************************************
 * PndEmcReadProto192Data is a Fairtask to read
 * Data from the Proto60 and create PndEmcHits
 *
 * author: Christian Hammann <chammann@hiskp.uni-bonn.de>
 * Date: 8.1.2010
 *
 * ******************************************/

#ifndef PNDREADPROTO192DATA_H
#define PNDREADPROTO192DATA_H

#include "FairTask.h"
#include "PndEmcDigiPar.h"
#include "PndEmcGeoPar.h"
#include "PndGeoPassivePar.h"
#include "FairBaseParSet.h"
#include "TFile.h"
#include "TTree.h"
#include "TProtoUnpackEvent.h"
#include "TClonesArray.h"


class PndEmcReadProto192Data : public FairTask{
	public:
		virtual InitStatus Init();
		virtual void Exec(Option_t *opt);
		virtual InitStatus ReInit(){IsInit=kFALSE; return Init();};
		virtual void SetParContainers();
		void SetIndexMap(Long_t *map);
		void SetWaveformMap(Long_t *map);
		PndEmcReadProto192Data(TTree *prototree, TTree *taggertree = NULL, Bool_t StoreHits=kTRUE,Int_t debuglevel=0);
		~PndEmcReadProto192Data(){};
		virtual void SetTaggerTreeOffset(Long_t offset = 0){lTaggerTreeOffset = offset;};

	private:
		TTree *lProto192Tree;
		TTree *lTaggerTree;
		Long_t lProto192EventNum;
		UInt_t lTaggerEventNum;
		Long_t lTaggerTreeOffset;
		Long_t lEventNumOverflow;
		TProtoUnpackEvent *lProto192Event;
		TClonesArray *lWaveArray;
		TClonesArray *lTaggerArray;
		Long_t lIndexMap[64];
		Long_t lWaveformMap[64];


		PndEmcGeoPar *lGeoPar;
		PndEmcDigiPar *lDigiPar;
		PndGeoPassivePar *lGeoPassivePar;
		FairBaseParSet *lBaseParSet;
		Bool_t IsInit;
		Bool_t lStoreHits;
		Int_t lDebug;
		std::vector<Double_t> signal;
		ClassDef(PndEmcReadProto192Data,1);
};


#endif
