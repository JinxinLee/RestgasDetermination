// -------------------------------------------------------------------------
// -----                PndDchDigiProducer header file                 -----
// -----                Created 31/07/07  by A. Wronska                -----
// -------------------------------------------------------------------------

/** \class PndDchDigiProducer
 *  \author A.Wronska <wronska@if.uj.edu.pl>
 *  \date 31/07/07
 *  \brief Class for conversion points to digi
 *
 * 	Class performs digitization of MCpoints stored in PndDchPoint array
 * 	and produces an array of digis PndDchDigi
 **/

#ifndef PNDDCHDIGIPRODUCER_H
#define PNDDCHDIGIPRODUCER_H


// Pnd includes
#include "CbmTask.h"
#include "PndDchDigi.h"
// ROOT includes
#include "TVector3.h"
#include "TH1F.h"
//#include "TList.h"

class TClonesArray;
class TObjectArray;
class PndDchPoint;
class PndDchMapper;
class PndDchDrifter;
class TDirectory;
class TList;

class PndDchDigiProducer : public CbmTask {

public:

	/** Default constructor **/
	PndDchDigiProducer();

	/** Destructor **/
	virtual ~PndDchDigiProducer();

	/** Virtual method Init **/
	virtual InitStatus Init();

	/** Virtual method Exec **/
	virtual void Exec(Option_t* opt);

	/** Public method AddDigis(...) **/
	Int_t AddDigis(PndDchPoint* point, Int_t refIndex);


private:

	/** Private method ToBeOrNotToBe(...)
	 ** checks if a point should be digitized
	 **/
	Bool_t ToBeOrNotToBe(const PndDchPoint* point) const;

	TClonesArray* fPointArray; ///< Input array of PndDchPoints
	TClonesArray* fDigiArray;  ///< Output array of PndDchDigis
	PndDchMapper* fMapper;	   ///< pointer to a dch mapper
	PndDchDrifter* fDrifter;   ///< pointer to a dch drifter

	/** Finish **/
	virtual void Finish();

	/** Create histograms **/
	void CreateHistos();

	/** Private method Reset()
	** reset histograms and counters
	**/
	void Reset();

	TH1F* fhDriftTime[9]; ///< histograms of drift time

	/** List of histograms **/
	TList* fHistoList;    ///< List of created histograms


	ClassDef(PndDchDigiProducer,1)

};

#endif
