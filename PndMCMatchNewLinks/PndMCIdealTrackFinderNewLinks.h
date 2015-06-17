/*
 * PndMCIdealTrackFinderNewLinks.h
 *
 *  Created on: Apr 12, 2010
 *      Author: stockman
 */

#ifndef PndMCIdealTrackFinderNewLinks_H_
#define PndMCIdealTrackFinderNewLinks_H_


// framework includes
#include "FairTask.h"
#include "FairMCPoint.h"
#include "PndTrackCand.h"

#include "TClonesArray.h"
#include "TDatabasePDG.h"

class PndMCIdealTrackFinderNewLinks : public FairTask {
public:
	PndMCIdealTrackFinderNewLinks();
	virtual ~PndMCIdealTrackFinderNewLinks();

	  /** Virtual method Init **/
//	  virtual void SetParContainers();
	  virtual InitStatus Init();
	  virtual void AddBranchName(TString name){ fBranchNames.push_back(name);}
	  virtual void SetOutputBranchName(TString name){ fOutBranchName = name; };

	  /** Virtual method Exec **/
	  virtual void Exec(Option_t* opt);

	  //  virtual void Finish();

	  // taken from sttmvdtracking/PndSttMvdGemTrackingIdeal.h
	  void SetMomentumSmearing(Double_t sigmax = -1.,Double_t sigmay = -1.,Double_t sigmaz = -1.) { 
	    fMomSigma.SetXYZ(fabs(sigmax),fabs(sigmay),fabs(sigmaz)); 
	    fRelative=kFALSE; 
	  }; // in GeV
	  void SetRelativeMomentumSmearing(Double_t dpop = -1.) { 
	    fDPoP=fabs(dpop); 
	    fRelative=kTRUE;
	  }; // in GeV
	  void SetVertexSmearing(Double_t sigmax = -1.,Double_t sigmay = -1.,Double_t sigmaz = -1.) { 
	    fVtxSigma.SetXYZ(fabs(sigmax),fabs(sigmay),fabs(sigmaz)); 
	  }; // in cm
	  void SetTrackingEfficiency(Double_t eff = 1.) { 
	    fEfficiency=eff; 
	  };


protected:
	  virtual void CreateTrackCands();
	  // taken from sttmvdtracking/PndSttMvdGemTrackingIdeal.h
	  void SmearVector(TVector3 &vec, const TVector3 &sigma);

private:
	  TString fOutBranchName;
	  TClonesArray* fTrackCand;
	  TClonesArray* fTrack;
	  TClonesArray* fMCTrack;
	  std::map<TString, TClonesArray*> fBranchMap;
	  std::vector<TString> fBranchNames;
	  std::map<FairLink, PndTrackCand> fTrackCandMap;
	  std::map<FairLink, FairMCPoint > fFirstPointMap;
	  std::map<FairLink, FairMCPoint > fLastPointMap;

	  TDatabasePDG *fPdg;            //!<! Particle DB

	  Int_t fHitCount;

	  // Parameters for fake tracking taken from sttmvdtracking/PndSttMvdGemTrackingIdeal.h
	  TVector3 fMomSigma;          ///< Momentum smearing sigma [GeV]
	  Double_t fDPoP;              ///< Relative momentum Smearing
	  Bool_t fRelative;            ///< flag
	  TVector3 fVtxSigma;          ///< Vertex smearing sigma [cm]
	  Double_t fEfficiency;        ///< Tracking efficiency - if (0 <= e < 1), some tracks will be discarded


	  ClassDef(PndMCIdealTrackFinderNewLinks,2);
};

#endif /* PndMCIdealTrackFinderNewLinks_H_ */
