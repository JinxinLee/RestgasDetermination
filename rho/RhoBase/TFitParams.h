#ifndef TFITPARAMS_H
#define TFITPARAMS_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TFitParams								//
//                                                                      //
// Kinematics base class for TCandidates				//
//									//
// Author List:								//
// Marcel Kunze,  RUB, Feb. 00						//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//									//
//////////////////////////////////////////////////////////////////////////

#include "RhoMath/TError.h"
#include "RhoMath/TPointErr.h"
#include "RhoMath/TVectorErr.h"
#include "RhoMath/TLorentzVectorErr.h"
#include "TVector3.h"
#include "TLorentzVector.h"

#define MATRIXSIZE 28

class TCandidate;

//		---------------------
// 		-- Class Interface --
//		---------------------

class TFitParams : public TObject
{
protected:
    
    // the params
    Char_t  fCharge;		// The electrical charge
    Float_t 
		fXposition,		// The origin in x
	    fYposition,		// The origin in y
		fZposition;		// The origin in z
	Double_t
	    fXmomentum,		// The momentum in x
	    fYmomentum,		// The momentum in y
	    fZmomentum,		// The momentum in z
		fEnergy;		// The total energy
	Float_t 
		fErrP7[MATRIXSIZE],	// The symmetric 7*7 error matrix
	    fParams[5],		// The helix fit parameters
	    fCov[15];		// The helix error matrix
    
    // Fitter interface
public:
    enum Status{unlocked=0,locked,nailed,unknown};
private:
    Double_t fChi2;
    Status   fStatus;
    TCandidate* fFit; //!owned pointer
public:
    
    //
    // Public interface
    //
    
    //  default
    TFitParams();
    
    //
    //  from the quantities themselves
    //
    
    // this one takes the full 7X7 covariance
    TFitParams( Double_t charge,
	const TVector3& pos,
	const TLorentzVector& p4,
	const TMatrixD& cov7 );
    
    // copy constructor
    TFitParams( const TFitParams& );
    
    // Use helix parameters (for fitting applications)
    TFitParams( Double_t charge, Float_t* par, Float_t* cov );
    
    // destructor
    ~TFitParams();

    TFitParams&  GetFitParams() const { return (TFitParams&) *this; }

    // modifiers
    void SetCharge( Double_t charge ) { fCharge = (Char_t) charge; }
    void SetMass( Double_t mass );
    void SetEnergy( Double_t energy ) { fEnergy =  energy; }
    void SetE( Double_t energy ) { fEnergy = energy; }
    void SetMassAndEnergy( Double_t mass, Double_t energy );
    void SetPosition( const TVector3& pos );
    void SetPos( const TVector3& pos ) { SetPosition(pos); }
    void SetP3( const TVector3& p3 );
    void SetVect( const TVector3& p3 ) { SetP3(p3); }
    void SetP4( Double_t mass, const TVector3& p3 );
    void SetP4( const TLorentzVector& p4 );
    void SetP7( const TVector3& pos, const TLorentzVector& p4 );
    void SetCovP4( const TMatrixD& covP4 );
    void SetCov7( const TMatrixD& cov7 );
    void SetCov7( const TMatrixD& covPos, const TMatrixD& covP4 );
    void SetCov7( const TMatrixD& covPos, const TMatrixD& covP4,const TMatrixD& covPosP4 );
    void Set( const TVector3& pos, const TLorentzVector& p4, const TMatrixD& cov7 );
    void Set( Double_t mass,const TPointErr& posErr, const TVectorErr& p3Err, const TMatrixD& xpErr ); 
    void Set( Double_t charge, Float_t* par, Float_t* cov=0); // From helix parms
    void SetErr(Float_t *err) {if (err!=0) for (int i=0; i<MATRIXSIZE;i++) fErrP7[i] = err[i];}
    void SetPar(Float_t *par) {if (par!=0) if (par!=0) for (int i=0; i<5;i++) fParams[i] = par[i];}

    // operators
    Bool_t  operator == (const TFitParams& c) const;
    
    // accessors
    Double_t	GetCharge()   const { return fCharge; }
    Double_t	Charge()   const { return fCharge; }
    TVector3	GetPosition()   const { return TVector3(fXposition,fYposition,fZposition); }
    TVector3	Pos()   const { return GetPosition(); }
    TVector3	GetMomentum()   const { return TVector3(fXmomentum,fYmomentum,fZmomentum); }
    TVector3	GetVect()   const { return TVector3(fXmomentum,fYmomentum,fZmomentum); }
    Double_t	Px() const { return fXmomentum; }
    Double_t	Py() const { return fYmomentum; }
    Double_t	Pz() const { return fZmomentum; }
    TPointErr	PosWCov()   const;
    TError&	PosCov()   const;
    void        SetPosCov(const TMatrixD&);  //Dipak
    TLorentzVector   P4()   const { return TLorentzVector(fXmomentum,fYmomentum,fZmomentum,fEnergy); }
    TError&	P4Cov()   const;
    TError&	P4Err()   const { return P4Cov(); }
    TLorentzVectorErr P4WErr()  const;
    TVector3	P3()   const { return GetMomentum(); }
    TError&	P3Cov()   const;
    TVectorErr  P3WErr()  const;
    Double_t	E()   const { return fEnergy; }
    Double_t	Energy() const  { return fEnergy; }
    Double_t	GetEnergy() const  { return fEnergy; }
    Float_t*	GetErrP7() { return fErrP7; }
    Double_t	EVar()   const;
    Double_t	M()   const;
    Double_t	P()   const;
    TMatrixD&	Cov7()   const;
    TMatrixD&	XPCov()   const;
    Double_t	Pt()  const { return TMath::Sqrt(fXmomentum*fXmomentum+fYmomentum*fYmomentum); }
    
    // Fitter interface (Parameters are stored in TrkExchangePar format)
    
    Double_t Chi2() const {return fChi2;}
    void SetChi2(Double_t chi2) {fChi2=chi2;}
    Status  GetStatus()  const {return fStatus;};
    void SetStatus(Status s) {fStatus=s;};
    void SetFit(TCandidate* b) {fFit=b;}
    const TCandidate* GetFit() const { return fFit;}
    Float_t *GetHelixParams() { return fParams; }
    Float_t *GetHelixCov() { return fCov; }
    Double_t GetHelixD0() const {return fParams[0];}
    Double_t GetHelixPhi0() const    {return fParams[1];}
    Double_t GetHelixOmega() const {return fParams[2];}
    Double_t GetHelixZ0() const	{return fParams[3];}
    Double_t GetHelixTanDip() const	{return fParams[4];}
    void SetHelixParms(Float_t *p);
    void SetHelixCov(Float_t *p);
    TError& ErrorMatrixOfEmcClusters(Double_t clEnergy,Double_t clTheta,Double_t clPhi) const;
    const TMatrixD& DerivsParWrtCoord(TVector3 mom, TVector3 pos, Int_t charge) const;
    const TMatrixD& DerivsCoordWrtPar(Float_t *params, Int_t charge, Double_t fltlen) const;
    
    void Boost(Double_t bx, Double_t by, Double_t bz);
    void Boost(const TVector3 &p)    { Boost(p.X(),p.Y(),p.Z()); }
    
    //
    // Prints
    //
   void PrintOn( std::ostream& o=std::cout ) const;

private:
    Double_t Correlation(Int_t x1,Int_t x2,const TMatrixD& m,const TMatrixD &cov) const;
    
public:
    ClassDef(TFitParams,1) // Class to store candidate fit parameters	
};
#endif




