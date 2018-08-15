//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id:$
//
// Description:
//	EMC Digi.
//
//	 Software developed for the BaBar Detector at the SLAC B-Factory.
// Adapted for the PANDA experiment at GSI		
//
// Author List:
//	Xiaorong Shi            Lawrence Livermore National Lab
//	Steve Playfer           University of Edinburgh
//	Stephen Gowdy           University of Edinburgh
// Dima Melnichuk - adaption for PANDA
//
// Copyright Information:
//	Copyright (C) 1994	Lawrence Livermore National Lab
//
/////////////////////////////////////////////////////////////// 
//#pragma once
#ifndef PNDEMCDIGI_H
#define PNDEMCDIGI_H

#include "TObject.h"
#include "PndEmcTwoCoordIndex.h"
#include "PndEmcHit.h"
#include "FairTimeStamp.h"
#include "TVector3.h"
#include "TClonesArray.h"

class TVector3;
class PndEmcXtal;
class PndEmcSharedDigi;

/**
 * @brief represents the reconstructed hit of one emc crystal
 * @ingroup PndEmc
 */
class PndEmcDigi : public FairTimeStamp
{

	friend std::ostream& operator<< (std::ostream& out, PndEmcDigi& digi){
		out << "PndEmc Digi in crystal: " << digi.GetDetectorId() 
			<< " energy: " << digi.GetEnergy()
			<< " timestamp: "<<digi.GetTimeStamp();
		return out;
	}

	public:    
	//for time based simulation
	static TClonesArray* fDigiArrayTBD;
	static void InitDigiArrayTBD();

	/** Default constructor **/
	PndEmcDigi();

	PndEmcDigi(Int_t trackid, Int_t id, Float_t energy, Float_t time, Int_t hitIndex=-1);
	PndEmcDigi(const PndEmcDigi&);

	/** Destructor **/
	virtual ~PndEmcDigi();    
	// Operators

	virtual bool operator==(const PndEmcDigi & otherDigi) const;
	virtual bool operator!=(const PndEmcDigi & otherDigi) const;
	virtual bool operator<( const PndEmcDigi & otherDigi) const;

	virtual bool equal(FairTimeStamp* data){
		PndEmcDigi* myDigi = dynamic_cast <PndEmcDigi*> (data);
		if (myDigi != 0){
			if (fDetectorId == myDigi->GetDetectorId())
				return true;
		}
		return false;
	}

	/** Output to screen **/
	virtual void Print(const Option_t* opt ="") const;

	enum PositionMethod { surface, depth };

	/** Modifiers **/
	void SetEnergy(Double32_t energy) { fEnergy     = energy ;};
	void SetTrackId(Int_t id)         { fTrackId    = id     ;};
	void SetDetectorId(Int_t id);

	virtual void AbsorbEnergy(PndEmcDigi& otherDigi);

	//check if 2 digis are neigbour or not (prelim for testing)
	bool isNeighbour(const PndEmcDigi* theDigi) const;


	/** Accessors **/
	virtual Double_t GetEnergy()  const;
	Int_t GetTrackId()    const { return fTrackId    ;};
	Int_t GetDetectorId() const { return fDetectorId ;};
	PndEmcTwoCoordIndex* GetTCI() const;
	Int_t GetThetaInt() const {return fThetaInd;};
	Int_t GetPhiInt() const {return fPhiInd;};
	Double_t GetTheta() const {return fTheta;};
	Double_t GetPhi() const {return fPhi;};
	Short_t GetModule()      const { return (fDetectorId/100000000);};
	Short_t GetRow()         const { return ((fDetectorId/1000000)%100);};
	Short_t GetCrystal()     const { return (fDetectorId%10000);};
	Short_t GetCopy()        const { return ((fDetectorId/10000)%100);};
	Short_t GetXPad()        const;
	Short_t GetYPad()        const;

	Int_t GetHitIndex() {return fHitIndex;}
	const TVector3 &where() const {return fWhere;};

	virtual PndEmcSharedDigi* dynamic_cast_PndEmcSharedDigi();
	virtual const PndEmcSharedDigi* dynamic_cast_PndEmcSharedDigi() const;

	static Double_t getRescaleFactor(){ return fRescaleFactor; };
	static Double_t getPositionDepthPWO(){ return fPositionDepthPWO; };
	static Double_t getPositionDepthShashlyk(){ return fPositionDepthShashlyk; };


	Int_t fEvtNo;

	protected:
	Double_t fEnergy;    // digi amplitude
	Int_t fTrackId;
	Int_t fDetectorId;
	Int_t fHitIndex; //  Index of hit which is converted to digi
	TVector3 fWhere;
	Int_t fThetaInd;
	Int_t fPhiInd;
	Double_t fTheta;
	Double_t fPhi;


	private:

	friend class PndEmcWaveformToDigi;
	friend class PndEmcWaveformToDigiFPGA;
	friend class PndEmcWaveformToCalibratedDigi;
	friend class PndEmcMultiWaveformToCalibratedDigi;
	friend class PndEmcFWEndcapDigi;
	friend class PndEmcTmpWaveformToDigi;
	friend class PndEmcMakeDigi;
	friend class PndEmcCorrBump;

	static void selectDigiPositionMethod( PositionMethod , 
			double positionDepthPWO = 0., 
			double positionDepthShahslyk = 0.,
			double rescaleFactor = 1.);

	static TVector3 surfacePosition( const PndEmcXtal* xtal );
	static TVector3 depthPosition( const PndEmcXtal* xtal );

	static TVector3 ( *&algPointer() ) ( const PndEmcXtal* );

	static double fRescaleFactor;
	static double fPositionDepthPWO;
	static double fPositionDepthShashlyk;


	ClassDef(PndEmcDigi,5);
};

#endif //PndEmcDigi_H
