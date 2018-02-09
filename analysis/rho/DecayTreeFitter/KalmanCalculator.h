// ******************************************************
// DecayTreeFitter Package
// We thank the original author Wouter Hulsbergen 
// (BaBar, LHCb) for providing the sources. 
// http://arxiv.org/abs/physics/0503191v1 (2005)
// Adaptation & Development for PANDA: Ralf Kliemt (2015)
// ******************************************************
#ifndef KALMANCALCULATOR_H
#define KALMANCALCULATOR_H 1

#include "TVectorD.h"
#include "TMatrixD.h"
#include "TMatrixDSym.h"
#include "FitParams.h"
#include "ErrCode.h"
#include "Rtypes.h"

namespace DecayTreeFitter
{

  class KalmanCalculator
  {
  public:
    KalmanCalculator(){};
    virtual ~KalmanCalculator(){};

    ErrCode init(const TVectorD& value, const TMatrixD& G,
                 const FitParams* fitparams, const TMatrixDSym& V, int weight=1) ;
    void updatePar(FitParams* fitparams) ;
    void updatePar(const TVectorD& prediction, FitParams* fitparams) ;
    void updateCov(FitParams* fitparams) ;
    double chisq() const { return m_chisq ; }
  private:
    int m_nconstraints ; // dimension of the constraint
    int m_nparameters  ; // dimension of the state
    const TVectorD* m_value ;
    const TMatrixD* m_matrixG ;
    TMatrixDSym m_matrixR;    // cov of residual
    TMatrixDSym m_matrixRinv; // inverse of cov of residual
    TMatrixD m_matrixK   ;    // kalman gain matrix
    double m_chisq ;
    int m_ierr ;
    // some temporary results
    TMatrixD m_matrixCGT ;
    ClassDef ( KalmanCalculator,1 )
  } ;
}

#endif
