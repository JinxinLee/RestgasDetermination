// -------------------------------------------------------------------------
// -----            PndDchIdealHitProducer header file                 -----
// -----            Created 26/10/07  by A. Wronska                    -----
// -------------------------------------------------------------------------

/** \class PndDchIdealHitProducer
 *  \author A.Wronska <aleksandra.wronska@uj.edu.pl>
 *  \date 26/10/07
 *  \brief Class for conversion points to hits
 * 
 * is a dummy hit reconstruction task, which converts each point	
 * into a hit (1:1 conversion) with a resolution defined per chamber 
 * in the initialization of the task	
 **/

#ifndef PNDDCHIDEALHITPRODUCER_H
#define PNDDCHIDEALHITPRODUCER_H 1

// Pnd includes
#include "CbmTask.h"

// Root includes
#include "TH1F.h"

class TClonesArray;
class PndDchHit;
class PndDchPoint;
class PndDchStructure;
class TVector3;
class TDirectory;
class TList;

class PndDchIdealHitProducer : public CbmTask{

 public:
	
	/** Default constructor **/  
	PndDchIdealHitProducer();
	
	/** Destructor **/
	virtual ~PndDchIdealHitProducer();
	
	/** Virtual method Init **/
	virtual InitStatus Init();

	/** Virtual method Exec **/
	virtual void Exec(Option_t* opt);
	
	/** Public method AddHit(...)
	 ** adds another hit to the hit array
	 **/
	PndDchHit* AddHit(const PndDchPoint* point, Int_t idx);

 private: 
   
	TClonesArray* fPointArray;///< Input array of PndDchPoints 
	TClonesArray* fHitArray;  ///< Output array of PndDchHits
	PndDchStructure* fDchStructure;	///< pointer to structure of dch setup
	TH1F* fhDx, *fhDy, *fhDz; ///< histograms with pull distributions
	TList* fHistoList;        ///< List of created histograms

	/** Private method Smear(...)
	 **    smears the position vector
	 **/
	void Smear(TVector3& pos, const TVector3 dpos) const;
	
	/** Virtual method Finish **/
	virtual void Finish();

	/** Private method CreateHistos()
	 ** create control histograms 
	 **/
	void CreateHistos();
	
	/** Private method Reset()
	 ** resets histograms and counters 
	 **/
	void Reset();
	
  ClassDef(PndDchIdealHitProducer,1);
  
};

#endif
