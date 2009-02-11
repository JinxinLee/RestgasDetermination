// -------------------------------------------------------------------------
// -----                PndDchCylinderHitProducer header file          -----
// -----                Created 26/03/08  by A. Wronska                -----
// -------------------------------------------------------------------------

/** \class PndDchCylinderHitProducer
 *  \author A.Wronska <aleksandra.wronska@uj.edu.pl>
 *  \date 26/03/08
 *  \brief Class for conversion digis to spatial cylinder hits
 * 
 * This task converts electronic information from the detector
 * (i.e. digis) into spatial information in form of a a cylinder
 * (PndDchCylinderHit): wire position in the plane, plane's z-position and 
 * rotation angle, distance to the wire (=radius of the cylinder).
 **/

#ifndef PNDDCHCYLINDERHITPRODUCER_H
#define PNDDCHCYLINDERHITPRODUCER_H 1

// Pnd includes
#include "FairTask.h"

class TClonesArray;
class PndDchStructure;
class PndDchCylinderHit;
class TVector3;
class TVector2;

class PndDchCylinderHitProducer : public FairTask{

 public:
	
	/** Default constructor **/  
	PndDchCylinderHitProducer();
	
	/** Destructor **/
	virtual ~PndDchCylinderHitProducer();
	
	/** Virtual method Init **/
	virtual InitStatus Init();

	/** Virtual method Exec **/
	virtual void Exec(Option_t* opt);

	/** Public method AddCylinderHit **/
	PndDchCylinderHit* AddCylinderHit(Int_t digiidx, Double_t xLoc,
					  Double_t zGlo, Double_t dist, 
					  Double_t distErr, Double_t alpha, 
					  TVector2 end1, TVector2 end2) const;
	
 private: 
   
	TClonesArray* fDigiArray;       ///< input array of PndDchDigis
	TClonesArray* fPointArray;       
	TClonesArray* fCylHitArray;     ///< output array of PndDchCylinderHits
	PndDchStructure* fDchStructure; ///< structure of dch setup
	PndDchMapper* fDchMapper;       ///< mapper of dch
	PndDchDrifter* fDchDrifter;     ///< drifter of dch

        /** Private method with parametrised detection efficiency **/
	Double_t Efficiency(Double_t fracDist);

        /** Private method with parametrised position resolution **/
	Double_t Resolution(Double_t fracDist);

        /** Private method deciding if to convert into a chit **/
	Bool_t Accept(Double_t fracDist);

	/** Private method to smear the distance from wire **/
	Double_t SmearDistance(Double_t dist, Double_t cellsize);
	  
    ClassDef(PndDchCylinderHitProducer,1);
    
};

#endif
