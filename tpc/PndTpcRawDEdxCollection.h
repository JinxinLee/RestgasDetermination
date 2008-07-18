//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
// 	Stores DEdx for one track and some information to cut on in draw commands	     
//   has some procedures to do truncated mean stuff on the stored data
//   
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Viola Michael    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef PNDTPCRAWDEDXCOLLECTION_HH
#define PNDTPCRAWDEDXCOLLECTION_HH

// Base Class Headers ----------------
#include "TObject.h"

// Collaborating Class Headers -------
#include "DEDX.h"

// C/C++ Headers ----------------------
#include <vector>

// Collaborating Class Declarations --


class PndTpcRawDEdxCollection : public TObject
{ 
public:
  	// Constructors/Destructors ---------
	PndTpcRawDEdxCollection():fTrackNr(-1),fMotherID(-200),fPDGCode(0),fSorted(false),fP(-10.),fMCP(-10.),fMinHits(1),Pi(-1.),e(-1.),p(-1.),Mu(-1.),K(-1.){}
	virtual ~PndTpcRawDEdxCollection()						{}	//Close() here?
	
	  // Accessors -----------------------
/*
	Int_t GetTrackNr() 					const 	{ return fTrackNr;				}
	Int_t GetPDG() 					const 	{ return fPDGCode;				}
	Double_t GetDE(int i) 				const	{ return fRawDEDx[i].fDE;		}	
	Double_t GetDX(int i) 				const 	{ return fRawDEDx[i].fDx;		}
	bool IsPdg(Int_t nPDG) 				const 	{ return (nPDG == fPDGCode); 	}
	bool IsTrackNr(Int_t nTrackNr) 		const 	{ return (nTrackNr == fPDGCode); 	}
*/	
	// Modifiers -----------------------
	void SetRawDEdx(Double_t dE, Double_t dx);
	void SetP(Double_t P)						{ fP=P;						}
	void SetMCP(Double_t P)						{ fMCP=P;						}
	void SetPDG(Int_t nPDG)						{ fPDGCode=nPDG;				}		
	void SetTrackNr(Int_t nTrackNr)				{ fTrackNr=nTrackNr;			}
	void SetMotherId(Int_t nMotherID)				{ fMotherID=nMotherID;			}
	void SetMCTrackId(std::vector<Int_t> MCTrackID)	{ fMCTrackID=MCTrackID;			}
	void SetSorted(bool isSorted)					{ fSorted=isSorted;				}
	
	void SetP_e(double p_e) 		{ e=p_e; 	}
	void SetP_Pi(double p_Pi)	{ Pi=p_Pi;}
	void SetP_p(double p_p)		{ p=p_p; 	}
	void SetP_Mu(double p_Mu)	{ Mu=p_Mu;}
	void SetP_K(double p_K)		{ K=p_K;	}
	
	void SetMinNumberOfConsideredHits(int nConsideredHits){fMinHits=nConsideredHits;	}
	
	// Operations ----------------------
	void Close();	// has to be called, when all dedx data has been inserted!
	Double_t TruncateAndMean(double minTruncFactor=0., double maxTruncFactor=1., int dbgLevel=0) const;
	Double_t TruncateAndMeanAlternative(double minTruncFactor=0., double maxTruncFactor=1., int dbgLevel=0) const;
	void Truncate(std::vector<DEDx> &Truncated, double minTruncFactor=0., double maxTruncFactor=1.) const;
	Double_t GetSumDE(double minTruncFactor=0., double maxTruncFactor=1.) const;
	Double_t GetSumDx(double minTruncFactor=0., double maxTruncFactor=1.) const;
	Double_t GetNumberOfMCTrackIDs() const 	{ return fMCTrackID.size();	}
	Double_t GetNumberOfHits() const		{ return fRawDEDx.size();	}
	//std::vector<DEDx> Truncate(double minTruncFactor=0., double maxTruncFactor=1.) const;
private:
	//for cuts ---------------
	Int_t fTrackNr;	
	Int_t fMotherID;
	std::vector<Int_t> fMCTrackID;
	Int_t fPDGCode;
	bool fSorted;
	
	//dedx ------------------
	Double_t fP;
	Double_t fMCP;
	std::vector< DEDx > fRawDEDx;
	
	//for analysis ---------
	int fMinHits;
	
	//PID probability of being a Particle
	double Pi;
	double e;
	double p;
	double Mu;
	double K;
	
 	 // Private Methods -----------------	
	bool CheckTruncation(double minTruncFactor, double maxTruncFactor) const;
	bool ChecknConsideredHits(int nConsideredHits, int dbgLevel) const;
	void PrintDEDx(double de, double dx, int dbgLevel) const;
public:
  	ClassDef(PndTpcRawDEdxCollection,3)
};

#endif
