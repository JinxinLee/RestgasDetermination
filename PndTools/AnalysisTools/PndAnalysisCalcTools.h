#ifndef PNDANALYSISCALCTOOLS_H
#define PNDANALYSISCALCTOOLS_H 1

#include <string>
#include <vector>

#include "TString.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "TMatrixD.h"

#include "RhoCandList.h"
#include "FairRootManager.h"
#include "FairTrackParP.h"
#include "RhoCalculationTools.h"
class TDatabasePDG;

class TGeant3;

class PndAnalysisCalcTools
{
  public:

    static void SetVerbose(Int_t level) { fVerbose = level; }

    // Calculator functions
//    static Bool_t FillHelixParams(RhoCandidate* cand, Bool_t skipcov=kFALSE);
    static Bool_t P7toHelix(const TVector3& pos, const TLorentzVector& p4, const Double_t Q,
                            const TMatrixD& cov77, Float_t* helixparams, TMatrixD& helixCov, Bool_t skipcov=kFALSE);
    static Bool_t P7toPRG(const TVector3& pos, const TLorentzVector& p4, const Double_t Q, const TMatrixD& cov77,
                          const TVector3& expPoint, Float_t* helixparams, TMatrixD& helixCov, TMatrixD& jacobian, Bool_t skipcov=kFALSE);
//    static Bool_t SDtoHelix(FairTrackParH* par, RhoCandidate* cand, Bool_t skipcov=kFALSE);
  static Bool_t StateFromTrajectory( TVectorD& state, const RhoCandidate* track, Double_t z ) {std::cerr<<"PndAnalysisCalcTools::StateFromTrajectory not implemented!!"<<std::endl;return kFALSE;};

    static Double_t GetBz(const TVector3& position) { return RhoCalculationTools::GetBz(position);};

  private:
    PndAnalysisCalcTools();
    ~PndAnalysisCalcTools();

    static Int_t fVerbose;

};


#endif

