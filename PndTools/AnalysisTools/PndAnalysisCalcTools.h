#ifndef PNDANALYSISCALCTOOLS_H
#define PNDANALYSISCALCTOOLS_H 1

#include <string>
#include <vector>

#include "TString.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "TMatrixD.h"

#include "TCandList.h"
#include "FairRootManager.h"
#include "FairTrackParP.h"

class TDatabasePDG;

class TGeant3;

class PndAnalysisCalcTools {
public:

  static void SetVerbose(Int_t level){ fVerbose = level; }

  // Calculator functions
  static Bool_t FillHelixParams(TCandidate* cand, Bool_t skipcov=kFALSE);
  static Bool_t P7toHelix(const TVector3 &pos, const TLorentzVector &p4, const Double_t Q, 
                          const TMatrixD &cov77, Float_t *helixparams, TMatrixD &helixCov, Bool_t skipcov=kFALSE);
  static Bool_t P7toPRG(const TVector3 &pos, const TLorentzVector &p4, const Double_t Q, const TMatrixD &cov77,
                        const TVector3 &expPoint, Float_t *helixparams, TMatrixD &helixCov, TMatrixD &jacobian, Bool_t skipcov=kFALSE);
  static Bool_t SDtoHelix(FairTrackParH* par, TCandidate* cand, Bool_t skipcov=kFALSE);
  static Double_t GetBz(const TVector3 &position);
  
  static void ForceConstantBz(Double_t bz=0.){fBz=bz;fBzSet=kTRUE;};
  static void UnForceConstantBz(){fBzSet=kFALSE;};
  
private:
  PndAnalysisCalcTools();
  ~PndAnalysisCalcTools();
  
  static Double_t fBz;
  static Int_t fVerbose;
  static Bool_t fBzSet;
  
};


#endif

