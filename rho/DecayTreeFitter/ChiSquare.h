// ******************************************************
// DecayTreeFitter Package
// We thank the original author Wouter Hulsbergen 
// (BaBar, LHCb) for providing the sources. 
// http://arxiv.org/abs/physics/0503191v1 (2005)
// Adaptation & Development for PANDA: Ralf Kliemt (2015)
// ******************************************************
#ifndef DECAYTREEFITTER_ChiSquare_H
#define DECAYTREEFITTER_ChiSquare_H 1
#include "Rtypes.h"
#include "TMath.h"

namespace DecayTreeFitter
{

  class ChiSquare
  {
  public:

    /// Constructor
    ChiSquare(const double achi2, int andof) : m_chi2(achi2),m_nDoF(andof) {}

    /// Default Constructor
    ChiSquare() : m_chi2(0.0),
    m_nDoF(0) {}

    /// Default Destructor
    virtual ~ChiSquare() {}

    /// return chi2/ndof if ndof>0. returns zero otherwise.
    double chi2PerDoF() const { return m_nDoF>0 ? m_chi2/m_nDoF : 0 ; }

    /// addition operator
    ChiSquare& operator+=(const ChiSquare& rhs);

    /// subtraction operator
    ChiSquare& operator-=(const ChiSquare& rhs);

    /// addition operator
    ChiSquare operator+(const ChiSquare& rhs);

    /// subtraction operator
    ChiSquare operator-(const ChiSquare& rhs);

    /// Retrieve const  chi square
    double chi2() const { return m_chi2 ; }

    /// Retrieve const  number of degrees of freedom
    int nDoF() const { return m_nDoF ; }
 
    /// Get Cofidence level
    double prob() const { return TMath::Prob(m_chi2,m_nDoF) ; }


  protected:

  private:

    double m_chi2; ///< chi square
    int    m_nDoF; ///< number of degrees of freedom
    ClassDef ( ChiSquare,1 )

  }; // class ChiSquare


  inline ChiSquare& ChiSquare::operator+=(const ChiSquare& rhs)
  {
    m_chi2 += rhs.m_chi2 ;
    m_nDoF += rhs.m_nDoF ;
    return *this ;
  }

  inline ChiSquare& ChiSquare::operator-=(const ChiSquare& rhs)
  {
    m_chi2 -= rhs.m_chi2 ;
    m_nDoF -= rhs.m_nDoF ;
    return *this ;
  }

  inline ChiSquare ChiSquare::operator+(const ChiSquare& rhs)
  {
    ChiSquare rc = *this ;
    rc += rhs ;
    return rc ;
  }

  inline ChiSquare ChiSquare::operator-(const ChiSquare& rhs)
  {
    ChiSquare rc = *this ;
    rc -= rhs ;
    return rc ;
  }
}

#endif
