// ******************************************************
// DecayTreeFitter Package
// We thank the original author Wouter Hulsbergen 
// (BaBar, LHCb) for providing the sources. 
// http://arxiv.org/abs/physics/0503191v1 (2005)
// Adaptation & Development for PANDA: Ralf Kliemt (2015)
// ******************************************************
#ifndef PROJECTION_H
#define PROJECTION_H 1
#include "Rtypes.h"

#include "TVectorD.h"
#include "TMatrixDSym.h"
#include "TMatrixD.h"
#include <string.h> // for memset
#include "ParticleBase.h"
namespace DecayTreeFitter
{

  class Projection
  {
  public:
    // constructor
    Projection(int dimP, int dimC) : m_matrixH(dimC,dimP),m_r(dimC),m_matrixV(dimC),m_offset(0),m_particle(0),m_nHidden(0) {}
    virtual ~Projection(){};

    // accessors to the projection matrix
    const TMatrixD& H() const { return m_matrixH ; }
    //HepMatrix& H() { return m_matrixH ; }
    double& H(int row, int col) {
#ifdef VTK_BOUNDSCHECKING
      assert( m_offset+row >0 && col>0 && m_offset+row <= m_matrixH.GetNrows() && col <= m_matrixH.GetNcols() ) ;
#endif
      return m_matrixH(m_offset+row,col) ; }

    // accessors to the residual (or the 'value' of the constraint)
    const TVectorD& r() const { return m_r ; }
    TVectorD& r() { return m_r ; }
    //HepVector& r() { return m_r ; }
    double& r(int row) {
#ifdef VTK_BOUNDSCHECKING
      assert( m_offset+row >0  && m_offset+row <= m_r.GetNrows() ) ;
#endif
      return m_r(m_offset+row) ; }

    // accessors to the covariance matrix
    const TMatrixDSym& V() const { return m_matrixV ; }
    //HepSymMatrix& V() { return m_matrixV ; }
    double& V(int row, int col) { return m_matrixV(m_offset+row,m_offset+col) ; }
    double& Vfast(int row, int col) {
#ifdef VTK_BOUNDSCHECKING
      assert( m_offset+row >0 && m_offset+col>0 && m_offset+row <= m_matrixV.GetNrows() && m_offset+col <= m_matrixV.GetNcols() && row>=col ) ;
#endif
      return m_matrixV(m_offset+row,m_offset+col) ; }

    // reset
    //FIXME: Will this work for TMatixD, too?
    void reset() {
      // fill everything with '0'.  this implementation is kind of
      // tricky, but it is fast.
      //      int dimC = m_matrixH.GetNrows() ;
      //      int dimP = m_matrixH.GetNcols() ;
      //      memset(&(m_matrixH(1,1)),0,dimP*dimC*sizeof(double));
      //      memset(&(m_r(1))  ,0,dimC*sizeof(double));
      //      memset(&(m_matrixV(1,1)),0,dimC*(dimC+1)/2*sizeof(double));
      m_matrixH.Zero();
      m_r.Zero();
      m_matrixV.Zero();
      m_offset = 0 ;
    }

    // globalChisq
    double chiSquare() const {
      TMatrixDSym W=m_matrixV ;
      double det=0;
      W.InvertFast(&det);
      return W.Similarity(m_r) ;
    }

    void incrementOffset(unsigned int i) { m_offset+=i ; }
    unsigned int offset() const { return m_offset ; }

    void setParticle( const ParticleBase& p ) { m_particle = &p ; }
    const ParticleBase* particle() const { return m_particle ; }

  private:
    TMatrixD m_matrixH ;    // projection matrix
    TVectorD m_r ;    // constraint residual
    TMatrixDSym m_matrixV ; // constraint variance (zero for lagrange constraints)
    unsigned int m_offset ; // offset for constraint index. only non-zero for merged constraints.
    const ParticleBase* m_particle ; // particle where chi2 should be added
                                     // the number of hidden 'degrees of freedom'. always zero except for the 'photon' constraint
    unsigned int m_nHidden ;
    ClassDef ( Projection,1 )
  } ;
}
#endif
