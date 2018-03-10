/**
 * PndIdealTrackFinder.h
 *
 * \brief Ideal track finder for all types of tracking detectors
 * \detailed The PndIdealTrackFinder combines all hits in given branches into PndTrackCand and calculates the PndTrack based on MC information
 * \author Tobias Stockmanns <t.stockmanns@fz-juelich.de>
 * \date Apr 12, 2010
 *
 */

#ifndef PndIdealTrackFinder_H_
#define PndIdealTrackFinder_H_


// framework includes
#include "PndPersistencyTask.h"
#include "FairMCPoint.h"
#include "PndTrackCand.h"
#include "PndTrackFunctor.h"

#include "TClonesArray.h"
#include "TDatabasePDG.h"
#include <math.h>

class PndIdealTrackFinder : public PndPersistencyTask {
public:
	PndIdealTrackFinder();
	virtual ~PndIdealTrackFinder();

	  /** Virtual method Init **/
//	  virtual void SetParContainers();
	  virtual InitStatus Init();
	  virtual void AddBranchName(TString name){ fBranchNames.push_back(name);} ///< Search for tracks only in given branches. If no BranchName is given all tracking detectors are taken
	  virtual void AddBranchName(TString name, TString mcName){ fBranchNames.push_back(name);	fPointBranchMap[name] = mcName;} ///< Search for tracks only in given branches. If no BranchName is given all tracking detectors are taken

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

	  void SetTrackSelector(TString selector){			///< The track selector decides if a track is taken or discarded. You can set them via a string. Valid strings can be found in PndTrackFunctor.h
		  fTrackSelector = PndTrackFunctor::make_PndTrackFunctor(selector.Data());
	  }

	  void SetPersistence(Bool_t persistence) { SetPersistency(persistence); }


protected:
	  virtual void CreateTrackCands();
	  virtual void CreateTracks();
	  virtual void FilterTrackCands();
	  virtual FairMCPoint* GetFairMCPoint(TString hitBranch, FairMultiLinkedData_Interface* links, FairMultiLinkedData& array);
	  // taken from sttmvdtracking/PndSttMvdGemTrackingIdeal.h
	  virtual void SmearVector(TVector3 &vec, const TVector3 &sigma);

protected:
	  TString fOutBranchName;
	  TClonesArray* fTrackCand;
	  TClonesArray* fTrack;
	  TClonesArray* fMCTrack;
	  PndTrackFunctor* fTrackSelector;
	  std::map<TString, TClonesArray*> fBranchMap;
	  std::vector<TString> fBranchNames;
	  std::map<FairLink, PndTrackCand> fTrackCandMap;
	  std::map<FairLink, FairMCPoint > fFirstPointMap;
	  std::map<FairLink, FairMCPoint > fLastPointMap;
	  std::map<TString, TString> fPointBranchMap;

	  TDatabasePDG *fPdg;            //!<! Particle DB

	  Int_t fHitCount;

	  // Parameters for fake tracking taken from sttmvdtracking/PndSttMvdGemTrackingIdeal.h
	  TVector3 fMomSigma;          ///< Momentum smearing sigma [GeV]
	  Double_t fDPoP;              ///< Relative momentum Smearing
	  Bool_t fRelative;            ///< flag
	  TVector3 fVtxSigma;          ///< Vertex smearing sigma [cm]
	  Double_t fEfficiency;        ///< Tracking efficiency - if (0 <= e < 1), some tracks will be discarded


	  ClassDef(PndIdealTrackFinder,2);
};

#endif /* PndIdealTrackFinder_H_ */
