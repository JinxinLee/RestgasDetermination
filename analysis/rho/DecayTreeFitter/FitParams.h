// ******************************************************
// DecayTreeFitter Package
// We thank the original author Wouter Hulsbergen 
// (BaBar, LHCb) for providing the sources. 
// http://arxiv.org/abs/physics/0503191v1 (2005)
// Adaptation & Development for PANDA: Ralf Kliemt (2015)
// ******************************************************
#ifndef FITPARAMS_H
#define FITPARAMS_H 1

#include <iostream>
#include <vector>
#include <cmath>
#include <map>
#include "TVectorD.h"
#include "TMatrixDSym.h"
#include "ChiSquare.h"
#include "Rtypes.h"

namespace DecayTreeFitter
{
  class ParticleBase ;

  class FitParams
  {
  public:
    // Class that contains the parameters and covariance for the
    // vertex fit.
    FitParams(int dim) ;
    
    FitParams(const FitParams& par) ;
    virtual ~FitParams() ;

    TMatrixDSym& cov() { return m_cov ; }
    TVectorD& par() { return m_par ; }
    double& par(int row) { return m_par(row) ; }

    TMatrixDSym cov(const std::vector<int>& indexVec) const ;
    TVectorD par(const std::vector<int>& indexVec) const ;

    const TMatrixDSym& cov() const { return m_cov ; }
    const TVectorD& par() const { return m_par ; }
    const double& par(int row) const { return m_par(row) ; }

    //TMatrixDSym& scale() { return m_scale ; }

    int& nConstraintsVec(int row) { return m_nConstraintsVec[row/*-1*/] ; }

    //int dim() const { return m_par.num_row() ; }
    int dim() const { return m_dim ; }
    double chiSquare() const { return m_chiSquare ; }

    int nConstraints() const { return m_nConstraints ; }
    int nDof() const { return nConstraints() - dim() ; }
    double err(int row) const { return sqrt(m_cov(row,row)) ; }

    void resize(int newdim) ;
    void reset(int newdim) ;
    void resetPar() ;
    void resetCov(double scale=100) ;
    void print() const ;
    bool testCov() const ;
    void addChiSquare( double chi2, int nconstraints, const ParticleBase* p) ;
    ChiSquare chiSquare( const ParticleBase& p) const ;

  protected:
    FitParams() {};
  private:
    int m_dim ;
    TVectorD    m_par ;
    TMatrixDSym m_cov ;
    //TMatrixDSym m_scale ;
    double m_chiSquare ;
    int m_nConstraints ;
    std::vector<int> m_nConstraintsVec ; // vector with number of constraints per parameter
    std::map<const ParticleBase*, ChiSquare> m_chiSquareMap ;
    ClassDef ( FitParams,1 )
  } ;
}

#endif
