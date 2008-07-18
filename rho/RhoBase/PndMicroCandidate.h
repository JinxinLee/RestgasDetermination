#ifndef PNDMICROCANDIDATE_H
#define PNDMICROCANDIDATE_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// PndMicroCandidate	                                                //
//                                                                      //
// Definition of the Panda micro candidate.	                            //
//                                                                      //
// Author: Klaus Goetzen, GSI, 12.06.08		                            //
// Copyright (C) 2008, GSI Darmstadt.									//
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <iostream>

#include <assert.h>

#include "RhoBase/VAbsMicroCandidate.h"
#include "TArrayI.h"
#include "TMatrixD.h"

//class VAbsPidInfo;

//  ========================================================================
//  ===== PndMicroCandidate - Class definig the AOD interface           ====
//  ========================================================================

class PndMicroCandidate : public VAbsMicroCandidate
{

  public:

    PndMicroCandidate();
    PndMicroCandidate(Int_t charge, TVector3& pos, TLorentzVector& p4, TMatrixD& cov7 );
    PndMicroCandidate(Int_t charge, TVector3& pos, TLorentzVector& p4);
	
    ~PndMicroCandidate();

	// ************************
	// ACCESSOR METHODS
	// ************************
	
	// ************************
    // basic properties:
	// ************************

    Int_t     GetCharge() const {return fCharge;}
    TVector3  GetPosition() const{return TVector3(fXposition,fYposition,fZposition);}
    TVector3  GetMomentum() const{return TVector3(fXmomentum,fYmomentum,fZmomentum);}
	Float_t   GetEnergy() const{return fEnergy;}
    TLorentzVector GetLorentzVector() const{return TLorentzVector(fXmomentum,fYmomentum,fZmomentum,fEnergy);}
	
    TVector3  GetFirstHit() const{return TVector3(fFirstHitX,fFirstHitX,fFirstHitX);}
    TVector3  GetLastHit() const{return TVector3(fLastHitX,fLastHitY,fLastHitZ);}
	
    const Float_t* GetErrorP7() const{return fErrP7;}
	const Float_t*  GetParams() const{return fParams;}
    const Float_t* GetCov() const{return fCov;}
	TMatrixD& Cov7() const;
	TMatrixD& P4Cov() const ;
    Int_t     GetMcIndex() const{return fMcIndex;}
	
	// ************************
	// detector specific stuff
	// ************************
	
	// MVD
    Float_t		GetMvdMeanDEdx() const{return fMvdMeanDEdx;}
	Float_t		GetMvdDEdxErr() const{return fMvdDEdxErr;}
	Int_t		GetMvdHits() const {return fMvdHits;}
	const Int_t* GetMvdHitIndexArray()  {return fMvdHitIndexArray.GetArray();}
	
	// STT
    Float_t		GetSttMeanDEdx() const {return fSttMeanDEdx;}
	Float_t		GetSttDEdxErr()const {return fSttDEdxErr;} 
	Int_t		GetSttHits() const{return fSttHits;}
	const Int_t* GetSttHitIndexArray()  {return fSttHitIndexArray.GetArray();}
	
	// TPC
    Float_t		GetTpcMeanDEdx()const {return fTpcMeanDEdx;}
	Float_t		GetTpcDEdxErr()const{return fTpcDEdxErr;}
	Int_t		GetTpcHits() const{return fTpcHits;}
	const Int_t* GetTpcHitIndexArray() {return fTpcHitIndexArray.GetArray();}
	
	// TOF
    Float_t		GetTofM2() const{return fTofM2;}
	Float_t		GetTofM2Err() const{return fTofM2Err;}
	
	// Barrel DIRC
    Float_t		GetBarrelDrcThetaC() const{return fBarrelDrcThetaC;}
    Float_t		GetBarrelDrcThetaCErr() const{return fBarrelDrcThetaCErr;}
    Int_t		GetBarrelDrcNumberOfPhotons() const{return fBarrelDrcNumberOfPhotons;}
	
	// Disc DIRC
    Float_t		GetDiscDrcThetaC() const{return fDiscDrcThetaC;}
    Float_t		GetDiscDrcThetaCErr() const{return fDiscDrcThetaCErr;}
    Int_t		GetDiscDrcNumberOfPhotons() const{return fDiscDrcNumberOfPhotons;}
	
	// RICH
    Float_t		GetRichThetaC() const{return fRichThetaC;}
    Float_t		GetRichThetaCErr() const{return fRichThetaCErr;}
    Int_t		GetRichNumberOfPhotons()const {return fRichNumberOfPhotons;}
	
	// EMC
    Float_t		GetEmcRawEnergy() const{return fEmcRawEnergy;}
    Float_t		GetEmcCalEnergy() const{return fEmcCalEnergy;}
    Int_t		GetEmcNumberOfCrystals() const{return fEmcNumberOfCrystals;}
    Int_t		GetEmcNumberOfBumps() const{return fEmcNumberOfBumps;}
	
	// MUO
	Int_t		GetMuoNumberOfLayers()const {return fMuoNumberOfLayers;}
	Float_t		GetMuoProbability() const{return fMuoProbability;}
	
	// Tracking
    Float_t		GetTrackLength() const{return fTrackLength;}
    Int_t    	GetDegreesOfFreedom() const{return fDegreesOfFreedom;}
    Int_t    	GetFitStatus() const{return fFitStatus;}
    Float_t  	GetProbability()const{return fProbability;}
    Float_t  	GetChiSquared() const{return fChiSquared;}
	    
	//PID
	Float_t		GetElectronPidLH() const { return fElectronPidLH; }
	Float_t		GetMuonPidLH() const { return fMuonPidLH; }
	Float_t		GetPionPidLH() const { return fPionPidLH; } 
	Float_t		GetKaonPidLH() const { return fKaonPidLH; }
	Float_t		GetProtonPidLH() const { return fProtonPidLH; }
	
	// ************************
	// MODIFIER METHODS
	// ************************
	
	void Lock() {fLocked=true;}
	void Unlock() {fLocked=false;}
	bool IsLocked() {return fLocked;}
	
	// ************************
    // basic properties:
	// ************************

    void	SetCharge(int charge) {fCharge=charge;}
    void	SetPosition(TVector3 &pos) { fXposition=pos.X();fYposition=pos.Y();fZposition=pos.Z(); }
    void	SetMomentum(TVector3 &mom) { fXmomentum=mom.X();fYmomentum=mom.Y();fZmomentum=mom.Z(); }
	void	SetEnergy(Double_t en) {fEnergy=(Float_t) en;}
	void	SetLorentzVector(TLorentzVector p4);
    
	void	SetFirstHit(TVector3 &pos) { fFirstHitX=pos.X();fFirstHitY=pos.Y();fFirstHitZ=pos.Z(); }
	void	SetLastHit(TVector3 &pos) { fLastHitX=pos.X();fLastHitY=pos.Y();fLastHitZ=pos.Z(); }
    
	void	SetCov7(const TMatrixD& cov7 );
	void	SetP4Cov(const TMatrixD& covP4 );
    void	SetMcIndex(int idx) {fMcIndex=idx; }
	
	// ************************
	// detector specific stuff
	// ************************
	
	// MVD
    void 	SetMvdMeanDEdx(Double_t val) { fMvdMeanDEdx=(Float_t) val;}
	void	SetMvdDEdxErr(Double_t val) { fMvdDEdxErr=(Float_t) val;}
	//void	SetMvdHits(Int_t val)  { fMvdHits=val;}
	void	SetMvdHitIndexArray(Int_t n, Int_t* arr)  { fMvdHits=n; fMvdHitIndexArray.Set(n,arr);}
	
	// STT
    void	SetSttMeanDEdx(Double_t val)  { fSttMeanDEdx=(Float_t) val;}
	void	SetSttDEdxErr(Double_t val) { fSttDEdxErr=(Float_t) val;} 
	//void	SetSttHits(Int_t val) { fSttHits= val;}
	void	SetSttHitIndexArray(Int_t n, Int_t* arr)  { fSttHits=n; fSttHitIndexArray.Set(n,arr);}
	
	// TPC
    void	SetTpcMeanDEdx(Double_t val) { fTpcMeanDEdx=(Float_t) val;}
	void	SetTpcDEdxErr(Double_t val){ fTpcDEdxErr=(Float_t) val;}
	//void	SetTpcHits(Int_t val) { fTpcHits= val;}
	void	SetTpcHitIndexArray(Int_t n, Int_t* arr) { fTpcHits=n; fTpcHitIndexArray.Set(n,arr);}
	
	// TOF
    void	SetTofM2(Double_t val) { fTofM2=(Float_t) val;}
	void	SetTofM2Err(Double_t val) { fTofM2Err=(Float_t) val;}
	
	// Barrel DIRC
    void	SetBarrelDrcThetaC(Double_t val) { fBarrelDrcThetaC=(Float_t) val;}
    void	SetBarrelDrcThetaCErr(Double_t val) { fBarrelDrcThetaCErr=(Float_t) val;}
    void	SetBarrelDrcNumberOfPhotons(Int_t val) { fBarrelDrcNumberOfPhotons=val;}
	
	// Disc DIRC
    void	SetDiscDrcThetaC(Double_t val) { fDiscDrcThetaC=(Float_t) val;}
    void	SetDiscDrcThetaCErr(Double_t val) { fDiscDrcThetaCErr=(Float_t) val;}
    void	SetDiscDrcNumberOfPhotons(Int_t val) { fDiscDrcNumberOfPhotons=val;}
	
	// RICH
    void	SetRichThetaC(Double_t val) { fRichThetaC=(Float_t) val;}
    void	SetRichThetaCErr(Double_t val) { fRichThetaCErr=(Float_t) val;}
    void	SetRichNumberOfPhotons(Int_t val) { fRichNumberOfPhotons= val;}
	
	// EMC
    void	SetEmcRawEnergy(Double_t val) { fEmcRawEnergy=(Float_t) val;}
    void	SetEmcCalEnergy(Double_t val) { fEmcCalEnergy=(Float_t) val;}
    void	SetEmcNumberOfCrystals(Int_t val) { fEmcNumberOfCrystals= val;}
    void	SetEmcNumberOfBumps(Int_t val) { fEmcNumberOfBumps= val;}
	
	// MUO
	void	SetMuoNumberOfLayers(Int_t val) { fMuoNumberOfLayers= val;}
	void	SetMuoProbability(Double_t val) { fMuoProbability=(Float_t) val;}
	
	// Tracking
    void	SetTrackLength(Double_t val) { fTrackLength=(Float_t) val;}
    void   	SetDegreesOfFreedom(Int_t val) { fDegreesOfFreedom=val;}
    void    SetFitStatus(Int_t val) { fFitStatus= val;}
    void  	SetProbability(Double_t val){ fProbability=(Float_t) val;}
    void  	SetChiSquared(Double_t val) { fChiSquared=(Float_t) val;}
	
	//PID
	void	SetElectronPidLH(Double_t val)  { fElectronPidLH=(Float_t) val; }
	void	SetMuonPidLH(Double_t val)  { fMuonPidLH=(Float_t) val; }
	void	SetPionPidLH(Double_t val)  { fPionPidLH=(Float_t) val; } 
	void	SetKaonPidLH(Double_t val)  { fKaonPidLH=(Float_t) val; }
	void	SetProtonPidLH(Double_t val)  { fProtonPidLH=(Float_t) val; }
	
protected:
    // Candidate lock
	Bool_t	fLocked;
	
    // the basic params
    Char_t  fCharge;		// The electrical charge
    Float_t fXposition,		// The origin in x
	    fYposition,		// The origin in y
	    fZposition,		// The origin in z
	    fXmomentum,		// The momentum in x
	    fYmomentum,		// The momentum in y
	    fZmomentum,		// The momentum in z
	    fEnergy;		// The total energy
		
	Float_t fFirstHitX,
	        fFirstHitY,
			fFirstHitZ;
			
	Float_t fLastHitX,
	        fLastHitY,
			fLastHitZ;
			
	Int_t		fMcIndex;	// MC truth index
	Float_t		fErrP7[28];  	// The symmetric 7*7 error matrix
	Float_t		fParams[5];		// The helix fit parameters
	Float_t		fCov[15];		// The helix error matrix
		
	// detector quantities
	// MVD
    Float_t 	fMvdMeanDEdx;
	Float_t 	fMvdDEdxErr;
	Int_t   	fMvdHits;
	TArrayI		fMvdHitIndexArray;
	
	// STT
    Float_t 	fSttMeanDEdx;
	Float_t 	fSttDEdxErr;
	Int_t   	fSttHits;
	TArrayI		fSttHitIndexArray;
	
	// TPC
    Float_t 	fTpcMeanDEdx;
	Float_t 	fTpcDEdxErr;
	Int_t   	fTpcHits;
	TArrayI		fTpcHitIndexArray;
	
	// TOF
    Float_t 	fTofM2;
	Float_t 	fTofM2Err;
	
	// Barrel DIRC
    Float_t 	fBarrelDrcThetaC;
    Float_t 	fBarrelDrcThetaCErr;
    Int_t		fBarrelDrcNumberOfPhotons;

	// Disc DIRC
    Float_t 	fDiscDrcThetaC;
    Float_t 	fDiscDrcThetaCErr;
    Int_t		fDiscDrcNumberOfPhotons;

	// Rich DIRC
    Float_t 	fRichThetaC;
    Float_t 	fRichThetaCErr;
    Int_t		fRichNumberOfPhotons;

	// EMC
    Float_t 	fEmcRawEnergy;
    Float_t 	fEmcCalEnergy;
    Int_t		fEmcNumberOfCrystals;
    Int_t		fEmcNumberOfBumps;
	
	// MUO
	Int_t		fMuoNumberOfLayers;
	Float_t 	fMuoProbability;
	
	// Tracking
    Float_t 	fTrackLength;
    Int_t		fDegreesOfFreedom;
    Int_t		fFitStatus;
    Float_t 	fProbability;
    Float_t 	fChiSquared;
	
	//PID
	Float_t		fElectronPidLH;
	Float_t		fMuonPidLH;
	Float_t		fPionPidLH;
	Float_t		fKaonPidLH;
	Float_t		fProtonPidLH;
		
    ClassDef(PndMicroCandidate,1) // Abstract base class for MicroDST candidates
};

std::ostream&  operator << (std::ostream& o, const VAbsMicroCandidate&);

#endif                                           


