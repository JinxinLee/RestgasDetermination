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
    PndMicroCandidate(Int_t charge, TVector3 &pos, TLorentzVector &p4, TMatrixD &cov7 );
    PndMicroCandidate(Int_t charge, TVector3 &pos, TLorentzVector &p4);
	
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
	Double_t  GetEnergy() const{return fEnergy;}
    TLorentzVector GetLorentzVector() const{return TLorentzVector(fXmomentum,fYmomentum,fZmomentum,fEnergy);}
	
    TVector3  GetFirstHit() const{return TVector3(fFirstHitX,fFirstHitX,fFirstHitX);}
    TVector3  GetLastHit() const{return TVector3(fLastHitX,fLastHitY,fLastHitZ);}
	
    const Float_t* GetErrorP7() const{return fErrP7;}
	const Float_t*  GetParams() const{return fParams;}
    const Float_t* GetCov() const{return fCov;}
	TMatrixD& Cov7() const;
	TMatrixD& P4Cov() const ;
    Int_t     GetMcIndex() const{return fMcIndex;}
    Int_t     GetTrackIndex() const{return fTrackIndex;}	
	
	// ************************
	// detector specific stuff
	// ************************
	
  // MVD
  Float_t		GetMvdDEDX()     const { return fMvdDEDX; }
  Float_t		GetMvdMeanDEdx()     const { return fMvdDEDX; }
	Float_t		GetMvdDEdxErr() const{return fMvdDEdxErr;}
  Int_t		        GetMvdHits()     const { return fMvdHits; }
  const Int_t* GetMvdHitIndexArray()  {return fMvdHitIndexArray.GetArray();}
	
  // STT
  Float_t		GetSttMeanDEDX() const { return fSttMeanDEDX; }
	Float_t		GetSttDEdxErr()const {return fSttDEdxErr;} 
  Int_t		        GetSttHits()     const { return fSttHits; }
  const Int_t* GetSttHitIndexArray()  {return fSttHitIndexArray.GetArray();}
	
  // TPC
  Float_t		GetTpcMeanDEDX() const { return fTpcMeanDEDX; }
	Float_t		GetTpcDEdxErr()const{return fTpcDEdxErr;}
  Int_t		        GetTpcHits()     const { return fTpcHits;}
  const Int_t* GetTpcHitIndexArray() {return fTpcHitIndexArray.GetArray();}
	
  // TOF
  Float_t		GetTofStopTime()    const { return fTofStopTime; }
  Float_t		GetTofM2()          const { return fTofM2; }
  Float_t		GetTofTrackLength() const { return fTofTrackLength; }
  Float_t		GetTofQuality()     const { return fTofQuality; }
  Int_t                 GetTofIndex()       const { return fTofIndex; }
  //old accessors  
	Float_t		GetTofM2Err() const{return fTofM2Err;}
  
  // Barrel DIRC
  Float_t		GetDrcThetaC()          const { return fDrcThetaC;}
  Float_t		GetDrcThetaCErr()       const { return fDrcThetaCErr;}
  Float_t		GetDrcQuality()         const { return fDrcQuality;}
  Int_t		        GetDrcNumberOfPhotons() const { return fDrcNumberOfPhotons;}
  Int_t                 GetDrcIndex()           const { return fDrcIndex; }
  //old accessors  
  Float_t		GetBarrelDrcThetaC() const{return fDrcThetaC;}
    Float_t		GetBarrelDrcThetaCErr() const{return fDrcThetaCErr;}
    Int_t		GetBarrelDrcNumberOfPhotons() const{return fDrcNumberOfPhotons;}

  // Disc DIRC
  Float_t		GetDiscThetaC()          const { return fDiscThetaC;}
  Float_t		GetDiscThetaCErr()       const { return fDiscThetaCErr;}
  Float_t		GetDiscQuality()         const { return fDiscQuality;}
  Int_t		    GetDiscNumberOfPhotons() const { return fDiscNumberOfPhotons;}
  Int_t         GetDiscIndex()           const { return fDiscIndex; }
  //old accessors  
    Float_t		GetDiscDrcThetaC() const{return fDiscThetaC;}
    Float_t		GetDiscDrcThetaCErr() const{return fDiscThetaCErr;}
    Int_t		GetDiscDrcNumberOfPhotons() const{return fDiscNumberOfPhotons;}
  
	// RICH
  Float_t		GetRichThetaC()          const { return fRichThetaC;}
  Float_t		GetRichThetaCErr()       const { return fRichThetaCErr;}
  Float_t		GetRichQuality()         const { return fRichQuality;}
  Int_t		        GetRichNumberOfPhotons() const { return fRichNumberOfPhotons;}
  Int_t                 GetRichIndex()           const { return fRichIndex; }

  // EMC
  Float_t		GetEmcRawEnergy() const{return fEmcRawEnergy;}
  Float_t		GetEmcCalEnergy() const{return fEmcCalEnergy;}
  Float_t		GetEmcQuality()   const{return fEmcQuality;}
  Int_t		GetEmcNumberOfCrystals() const{return fEmcNumberOfCrystals;}
  Int_t		GetEmcNumberOfBumps() const{return fEmcNumberOfBumps;}
  Int_t                 GetEmcModule()    const { return fEmcModule; }
  Int_t                 GetEmcIndex()     const { return fEmcIndex; }
	
  // MUO
  Int_t		GetMuoNumberOfLayers()const {return fMuoNumberOfLayers;}
  Float_t		GetMuoProbability() const{return fMuoProbability;}
  Float_t		GetMuoQuality() const{return fMuoQuality;}
  Int_t                 GetMuoModule()    const { return fMuoModule; }
  Int_t                 GetMuoIndex()     const { return fMuoIndex; }

  // Tracking
 
  Int_t    	GetDegreesOfFreedom() const{return fDegreesOfFreedom;}
    Float_t		GetTrackLength() const{return fTrackLength;}
  Int_t    	GetFitStatus() const{return fFitStatus;}
  Float_t  	GetChiSquared() const{return fChiSquared;}
    Float_t  	GetProbability()const{return fProbability;}
	    
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
	void	SetLorentzVector(TLorentzVector &p4);
    
	void	SetFirstHit(TVector3 &pos) { fFirstHitX=pos.X();fFirstHitY=pos.Y();fFirstHitZ=pos.Z(); }
	void	SetLastHit(TVector3 &pos) { fLastHitX=pos.X();fLastHitY=pos.Y();fLastHitZ=pos.Z(); }
    
	void	SetCov7(const TMatrixD &cov7 );
	void	SetP4Cov(const TMatrixD &covP4 );
    void	SetMcIndex(int idx) {fMcIndex=idx; }
    void  SetTrackIndex(int idx) {fTrackIndex=idx; }	
	
	// ************************
	// detector specific stuff
	// ************************
	
  // MVD
  void 	SetMvdDEDX(Double_t val)    { fMvdDEDX = (Float_t) val; }
  void	SetMvdHits(Int_t val)       { fMvdHits = val; }
  //old modifiers  
    void 	SetMvdMeanDEdx(Double_t val) { fMvdDEDX=(Float_t) val;}
	void	SetMvdDEdxErr(Double_t val) { fMvdDEdxErr=(Float_t) val;}
  void	SetMvdHitIndexArray(Int_t n, Int_t* arr)  { fMvdHits=n; fMvdHitIndexArray.Set(n,arr);}
	
  // STT
  void	SetSttMeanDEDX(Double_t val)  { fSttMeanDEDX = (Float_t) val; }
  void	SetSttHits(Int_t val)         { fSttHits = val; }
  void	SetSttHitIndexArray(Int_t n, Int_t* arr)  { fSttHits=n; fSttHitIndexArray.Set(n,arr);}
  //old modifiers  
    void	SetSttMeanDEdx(Double_t val)  { fSttMeanDEDX=(Float_t) val;}
	void	SetSttDEdxErr(Double_t val) { fSttDEdxErr=(Float_t) val;} 
	
  // TPC
  void	SetTpcMeanDEDX(Double_t val) { fTpcMeanDEDX =(Float_t) val; }
  void	SetTpcHits(Int_t val)        { fTpcHits = val; }
  void	SetTpcHitIndexArray(Int_t n, Int_t* arr) { fTpcHits=n; fTpcHitIndexArray.Set(n,arr);}
  //old modifiers  
    void	SetTpcMeanDEdx(Double_t val) { fTpcMeanDEDX=(Float_t) val;}
	void	SetTpcDEdxErr(Double_t val){ fTpcDEdxErr=(Float_t) val;}
	
  // TOF
  void	SetTofStopTime(Double_t val)    { fTofStopTime = (Float_t) val;}
  void	SetTofM2(Double_t val)          { fTofM2 = (Float_t) val;}
  void  SetTofTrackLength(Double_t val) { fTofTrackLength = val; }
  void  SetTofQuality(Double_t val)     { fTofQuality = val; }
  void	SetTofIndex(Int_t val)          { fTofIndex = val;}
  //old modifiers  
	void	SetTofM2Err(Double_t val) { fTofM2Err=(Float_t) val;}
	
  // Barrel DIRC
  void	SetDrcThetaC(Double_t val)       { fDrcThetaC= (Float_t) val;}
  void	SetDrcThetaCErr(Double_t val)    { fDrcThetaCErr=(Float_t) val;}
  void	SetDrcQuality(Double_t val)      { fDrcQuality=(Float_t) val;}
  void	SetDrcNumberOfPhotons(Int_t val) { fDrcNumberOfPhotons = val;}
  void  SetDrcIndex(Int_t val)           { fDrcIndex = val; }
  //old modifiers  
    void	SetBarrelDrcThetaC(Double_t val) { fDrcThetaC=(Float_t) val;}
    void	SetBarrelDrcThetaCErr(Double_t val) { fDrcThetaCErr=(Float_t) val;}
    void	SetBarrelDrcNumberOfPhotons(Int_t val) { fDrcNumberOfPhotons=val;}

  // Disc DIRC
  void	SetDiscThetaC(Double_t val)        { fDiscThetaC=(Float_t) val;}
  void	SetDiscThetaCErr(Double_t val)    { fDiscThetaCErr=(Float_t) val;}
  void	SetDiscQuality(Double_t val)      { fDiscQuality=(Float_t) val;}
  void	SetDiscNumberOfPhotons(Int_t val) { fDiscNumberOfPhotons=val;}
  void  SetDiscIndex(Int_t val)           { fDiscIndex = val; }
  //old modifiers  
    void	SetDiscDrcThetaC(Double_t val) { fDiscThetaC=(Float_t) val;}
    void	SetDiscDrcThetaCErr(Double_t val) { fDiscThetaCErr=(Float_t) val;}
    void	SetDiscDrcNumberOfPhotons(Int_t val) { fDiscNumberOfPhotons=val;}

  // RICH
  void	SetRichThetaC(Double_t val)       { fRichThetaC=(Float_t) val;}
  void	SetRichThetaCErr(Double_t val)    { fRichThetaCErr=(Float_t) val;}
  void	SetRichQuality(Double_t val)      { fRichQuality=(Float_t) val;}
  void	SetRichNumberOfPhotons(Int_t val) { fRichNumberOfPhotons= val;}
  void  SetRichIndex(Int_t val)           { fRichIndex = val; }
  
  // EMC
  void	SetEmcRawEnergy(Double_t val)     { fEmcRawEnergy=(Float_t) val;}
  void	SetEmcCalEnergy(Double_t val)     { fEmcCalEnergy=(Float_t) val;}
  void	SetEmcQuality(Double_t val)       { fEmcQuality=(Float_t) val;}
  void	SetEmcNumberOfCrystals(Int_t val) { fEmcNumberOfCrystals= val;}
  void	SetEmcNumberOfBumps(Int_t val)    { fEmcNumberOfBumps= val;}
  void  SetEmcModule(Int_t val)           { fEmcModule = val; }
  void  SetEmcIndex(Int_t val)            { fEmcIndex = val; }
  // MUO
  void	SetMuoNumberOfLayers(Int_t val) { fMuoNumberOfLayers= val;}
  void	SetMuoProbability(Double_t val) { fMuoProbability=(Float_t) val;}
  void	SetMuoQuality(Double_t val)     { fMuoQuality=(Float_t) val;}
  void  SetMuoModule(Int_t val)           { fMuoModule = val; }
  void  SetMuoIndex(Int_t val)            { fMuoIndex = val; }

  // Tracking
  void   	SetDegreesOfFreedom(Int_t val) { fDegreesOfFreedom=val;}
  void    SetFitStatus(Int_t val) { fFitStatus= val;}
  void  	SetChiSquared(Double_t val) { fChiSquared=(Float_t) val;}
    void	SetTrackLength(Double_t val) { fTrackLength=(Float_t) val;}
    void  	SetProbability(Double_t val){ fProbability=(Float_t) val;}
	
	//PID
	void	SetElectronPidLH(Double_t val)  { fElectronPidLH=(Float_t) val; }
	void	SetMuonPidLH(Double_t val)  { fMuonPidLH=(Float_t) val; }
	void	SetPionPidLH(Double_t val)  { fPionPidLH=(Float_t) val; } 
	void	SetKaonPidLH(Double_t val)  { fKaonPidLH=(Float_t) val; }
	void	SetProtonPidLH(Double_t val)  { fProtonPidLH=(Float_t) val; }
  
	//Initialize the Members
  void SetDefault();
	
protected:
    // Candidate lock
	Bool_t	fLocked;
	
    // the basic params
    Char_t  fCharge;		// The electrical charge
    Float_t fXposition,		// The origin in x
	    fYposition,		// The origin in y
	    fZposition;		// The origin in z
	Double_t
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
    Int_t         fTrackIndex; // PndTrack index
	Float_t		fErrP7[28];  	// The symmetric 7*7 error matrix
	Float_t		fParams[5];		// The helix fit parameters
	Float_t		fCov[15];		// The helix error matrix
		
  // detector quantities
	
  // ******** forcompatibility
  // old members variables	
	Float_t 	fMvdDEdxErr;
	Float_t 	fSttDEdxErr;
	Float_t 	fTpcDEdxErr;
	Float_t 	fTofM2Err;
    
	Float_t 	fTrackLength;
    Float_t 	fProbability;
	
	TArrayI		fMvdHitIndexArray;
	TArrayI		fSttHitIndexArray;
	TArrayI		fTpcHitIndexArray;
  // ******** forcompatibility		
	
	
  // MVD
  Float_t 	fMvdDEDX;
  Int_t   	fMvdHits;
  //TArrayI		fMvdHitIndexArray;
	
  // STT
  Float_t 	fSttMeanDEDX;
  Int_t   	fSttHits;
  //TArrayI		fSttHitIndexArray;
	
  // TPC
  Float_t 	fTpcMeanDEDX;
  Int_t   	fTpcHits;
  //TArrayI		fTpcHitIndexArray;
	
  // TOF
  Float_t       fTofStopTime;
  Float_t 	fTofM2;
  Float_t       fTofTrackLength;
  Float_t       fTofQuality;
  Int_t         fTofIndex;
	
  // Barrel DIRC
  Float_t 	fDrcThetaC;
  Float_t 	fDrcThetaCErr;
  Float_t       fDrcQuality;
  Int_t		fDrcNumberOfPhotons;
  Int_t         fDrcIndex;

  // Disc DIRC
  Float_t 	fDiscThetaC;
  Float_t 	fDiscThetaCErr;
  Float_t       fDiscQuality;
  Int_t		fDiscNumberOfPhotons;
  Int_t         fDiscIndex;
  
  // RICH
  Float_t 	fRichThetaC;
  Float_t 	fRichThetaCErr;
  Float_t       fRichQuality;
  Int_t		fRichNumberOfPhotons;
  Int_t         fRichIndex;
  
  // EMC
  Float_t 	fEmcRawEnergy;
  Float_t 	fEmcCalEnergy;
  Float_t       fEmcQuality;
  Int_t		fEmcNumberOfCrystals;
  Int_t		fEmcNumberOfBumps;
  Int_t         fEmcModule;
  Int_t         fEmcIndex;
  
  // MUO
  Int_t		fMuoNumberOfLayers;
  Float_t 	fMuoProbability;
  Float_t       fMuoQuality;
  Int_t         fMuoModule;
  Int_t         fMuoIndex;
  
  // Tracking
  Int_t	        fDegreesOfFreedom;
  Int_t	        fFitStatus;
  Float_t 	fChiSquared;
  
  //PID (workaround)
  Float_t		fElectronPidLH;
  Float_t		fMuonPidLH;
  Float_t		fPionPidLH;
  Float_t		fKaonPidLH;
  Float_t		fProtonPidLH;
		
    ClassDef(PndMicroCandidate,1) // Abstract base class for MicroDST candidates
};

std::ostream&  operator << (std::ostream& o, const VAbsMicroCandidate&);

#endif                                           


