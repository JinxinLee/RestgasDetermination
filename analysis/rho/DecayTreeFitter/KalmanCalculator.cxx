// ******************************************************
// DecayTreeFitter Package
// We thank the original author Wouter Hulsbergen 
// (BaBar, LHCb) for providing the sources. 
// http://arxiv.org/abs/physics/0503191v1 (2005)
// Adaptation & Development for PANDA: Ralf Kliemt (2015)
// ******************************************************

#include "KalmanCalculator.h"

#define SLOWBUTSAFE 1
//#undef SLOWBUTSAFE
#undef SKIPHIGHACCURACYCORRECTION
#include "RhoCalculationTools.h"
using namespace DecayTreeFitter;
ClassImp(KalmanCalculator);
extern int vtxverbose;
namespace DecayTreeFitter
{
  inline double fastsymmatrixaccess(double* m, int row, int col)
  {
    return *(m+(row*(row-1))/2+(col-1));
  }

  inline double symmatrixaccess(double* m, int row, int col)
  {
    return (row>=col? fastsymmatrixaccess(m,row,col) : fastsymmatrixaccess(m,col,row)) ;
  }
}
ErrCode
DecayTreeFitter::KalmanCalculator::init(const TVectorD& value, const TMatrixD& G,
                                        const FitParams* fitparams, const TMatrixDSym& V,
                                        int weight)
{
  ErrCode status ;
  if(vtxverbose>=6) { std::cout << "KalmanCalculator::init()"<<std::endl;}
  m_nconstraints = value.GetNrows() ;  // dimension of the constraint
  m_nparameters  = fitparams->dim() ; // dimension of the state

  int valdim  = value.GetNrows() ; // dimension of the constraint
  int statdim = fitparams->par().GetNrows() ; // dimension of the state

#ifdef VTK_BOUNDSCHECKING
  assert( G.GetNrows() == valdim && G.GetNcols() == statdim &&
         (V.GetNrows()==valdim) ) ;
#endif
  m_value = &value ;
  m_matrixG     = &G ;
  TMatrixDSym C(fitparams->cov());
  if(vtxverbose>=6) { printf("KalmanCalculator::init() G.GetNrows()/G.GetNcols() = %i/%i \t valdim/statdim = %i/%i \t V.GetNrows()/C.GetNrows() = %i/%i  V.GetNcols()/C.GetNcols() = %i/%i \n",G.GetNrows(),G.GetNcols(),valdim,statdim,V.GetNrows(),C.GetNrows(),V.GetNcols(),C.GetNcols());}

  // calculate C*G.T()
#ifdef SLOWBUTSAFE
  if(vtxverbose>=7) { std::cout << "KalmanCalculator::init() calc C*GT 'slow'"<<std::endl;}
  //m_matrixCGT = C * G.T() ;
  TMatrixD CGt(C,TMatrixD::kMultTranspose,G);
  if(vtxverbose>=7) { std::cout << "KalmanCalculator::init() CGT:";CGt.Print();}
  m_matrixCGT.ResizeTo(CGt);
  m_matrixCGT = CGt;
#else
  if(vtxverbose>=7) { std::cout << "KalmanCalculator::init() calc C*GT 'fast'"<<std::endl;}
  double tmp ;
  m_matrixCGT = TMatrixD(statdim,valdim) ;
  for(int col=0; col<m_nconstraints; col++)
    for(int k=0; k<m_nparameters; k++)
      if( (tmp=G(col,k)) !=0 ) {
        for(int row=0; row<k; row++)
          m_matrixCGT(row,col) += C(k,row) * tmp ;
        for(int row=k; row<=statdim; row++)
          m_matrixCGT(row,col) += C(row,k) * tmp ;
      }
#endif
  if(vtxverbose>=6) { std::cout << "KalmanCalculator::init() calc R = G*C*GT + V"<<std::endl;}

  // calculate the error in the predicted residual R = G*C*GT + V
  // slow:
#ifdef SLOWBUTSAFE
  TMatrixDSym Rinv=C.Similarity(G);
  m_matrixRinv.ResizeTo(Rinv);
  m_matrixRinv = Rinv;
  TMatrixDSym theV(V);
  theV *= weight;
  m_matrixRinv += theV; 
  if(vtxverbose>=6) { std::cout << "KalmanCalculator::init() V"<<std::endl;}
  if(vtxverbose>=7) { theV.Print();}
#else
  m_matrixRinv = V ;
  if(weight!=1) m_matrixRinv *= weight ;

  for(int row=0; row<m_nconstraints; row++)
  {
    for(int k=0; k<m_nparameters; k++)
      if( (tmp=G(row,k)) != 0 )
        for(int col=0; col<=row; col++)
          m_matrixRinv(row,col) += tmp*m_matrixCGT(k,col) ;
    for(int col=0; col<=row; col++) // fill the other side, too as TMatrixD is not symmetric
      m_matrixRinv(col,row) = m_matrixRinv(row,col)
  }
#endif
  if(vtxverbose>=6) { std::cout << "KalmanCalculator::init() G*C*GT"<<std::endl;}
  if(vtxverbose>=7) { Rinv.Print();}

  if(vtxverbose>=6) { std::cout << "KalmanCalculator::init() invert R"<<std::endl;}
  m_matrixR.ResizeTo(m_matrixRinv);
  m_matrixR = m_matrixRinv ;
  double det=0;
  m_matrixRinv.InvertFast(&det); //If determinant is zero the inversion failed.
  if(!det) {
    status |= ErrCode::inversionerror;
    std::cout << "Error inverting matrix. Vertex fit fails." << std::endl ;
  }
  if(vtxverbose>=7) { m_matrixRinv.Print();}

  if(vtxverbose>=6) { std::cout << "KalmanCalculator::init() calculate gain"<<std::endl;}
  // calculate the gain matrix
  TMatrixD K(m_matrixCGT,TMatrixD::kMult,m_matrixRinv);
  m_matrixK.ResizeTo(K) ;
  m_matrixK = K ;
  if(vtxverbose>=7) { m_matrixK.Print();}
  m_chisq = -1 ;
  //     // let's see if we get same results using sparce matrices
  //     VtkSparseMatrix Gs(G) ;
  //     VtkSparseMatrix CGT = Gs.transposeAndMultiplyRight(fitparams->cov()) ;
  //     HepSymMatrix Rs(value.numrow()) ;
  //     Gs.multiplyLeft(CGT,Rs) ;
  //     if(V) Rs += (*V) ;
  //     Rs.invert(m_ierr) ;
  //     VtkSparseMatrix Ks = CGT*Rs ;
  if(vtxverbose>=6) { std::cout << "KalmanCalculator::init() done"<<std::endl;}
  return status ;
}

void
DecayTreeFitter::KalmanCalculator::updatePar(FitParams* fitparams)
{
  if(vtxverbose>=5) { std::cout << "KalmanCalculator::updatePar(FitParams*):"<<std::endl;}
  //fitparams.par() -= fitparams->cov() * (G.T() * (R * value) ) ;
  fitparams->par() -= m_matrixK * (*m_value) ;
  if(vtxverbose>=7) {(m_matrixK * (*m_value)).Print();}
  m_chisq = m_matrixRinv.Similarity(*m_value) ;
  if(vtxverbose>=5) { std::cout << "KalmanCalculator::updatePar() done"<<std::endl;}
}

void
DecayTreeFitter::KalmanCalculator::updatePar(const TVectorD& pred, FitParams* fitparams)
{
  if(vtxverbose>=5) { std::cout << "KalmanCalculator::updatePar(TVectorD&,FitParams*)"<<std::endl;}
  // this is still very, very slow !
  TVectorD valueprime = (*m_value) + (*m_matrixG) * (pred-fitparams->par()) ;
  fitparams->par() = pred - m_matrixK*valueprime ;
  m_chisq = m_matrixRinv.Similarity( valueprime ) ;
  if(vtxverbose>=5) { std::cout << "KalmanCalculator::updatePar() done"<<std::endl;}
}

void
DecayTreeFitter::KalmanCalculator::updateCov(FitParams* fitparams)
{
  if(vtxverbose>=7) { std::cout << "KalmanCalculator::updateCov(FitParams*)"<<std::endl;}

#ifdef SLOWBUTSAFE
  if(vtxverbose>=8) { std::cout << "KalmanCalculator::updateCov: previous fitpar->cov()" ; RhoCalculationTools::PrintMatrix(fitparams->cov()); }

  // RK: nice try, sadly crappy
  //TMatrixDSym deltaCov = m_matrixRinv.SimilarityT(*m_matrixG).Similarity(fitparams->cov()) ;
  //fitparams->cov() -= deltaCov ;
  
  //TMatrixD dcov1(m_matrixCGT,TMatrixD::kMultTranspose,m_matrixK);
  //const TMatrixDSym C(fitparams->cov());
  TMatrixD HC(*m_matrixG,TMatrixD::kMult,fitparams->cov());
  TMatrixD dcov1(m_matrixK,TMatrixD::kMult,HC);
  TMatrixDSym KRKt = m_matrixR.Similarity(m_matrixK);
  TMatrixDSym dcov3(dcov1.GetNrows());
  for(int row=0; row<dcov1.GetNrows(); row++){
    for(int col=0; col<=row; col++){
      dcov3(row,col) = -2*dcov1(row,col);
      dcov3(col,row) = -2*dcov1(row,col);
    }
  }
  fitparams->cov() += KRKt ;
  fitparams->cov() += dcov3 ;
  //if(vtxverbose>=8) { std::cout << "KalmanCalculator::updateCov: -1*deltacov = V*{R^(-1)*G*R^(-1T)}*V^T" ; RhoCalculationTools::PrintMatrix(deltaCov); }
  if(vtxverbose>=8) { std::cout << "KalmanCalculator::updateCov: KRKt = K*R*KT"; RhoCalculationTools::PrintMatrix(KRKt);}
  if(vtxverbose>=8) { std::cout << "KalmanCalculator::updateCov: dcov3 = -2*C*GT*KT"; RhoCalculationTools::PrintMatrix(dcov3);}
  if(vtxverbose>=7) { std::cout << "KalmanCalculator::updateCov: afterwds fitpar->cov()" ; RhoCalculationTools::PrintMatrix(fitparams->cov()); }
#else

  // There are two expessions for updating the covariance
  // matrix.
  // slow: deltaCov = - 2*C*GT*KT +  K*R*KT
  // fast: deltaCov = - C*GT*KT
  // The fast expression is very sensitive to machine accuracy. The
  // following piece of code effectively invokes the slow
  // expression. I couldn't write it faster than this.

  double tmp ;
#ifndef SKIPHIGHACCURACYCORRECTION
  // substitute C*GT --> 2*C*GT - K*R. of course, this invalidates
  // C*GT, but we do not need it after this routine.

  // we use the fact that _in principle_ C*GT = K*R, such that
  // they have the same zero elements
  for(int row=0; row<m_nparameters; row++)
    for(int col=0; col<m_nconstraints; col++)
      if( (tmp =2*m_matrixCGT(row,col))!=0 ) {
        for(int k=0; k<m_nconstraints; k++)
          tmp -= m_matrixK(row,k) * m_matrixR(k,col) ;
        m_matrixCGT(row,col) = tmp ;
      }
#endif

  //     HepMatrix KR = m_matrixK*m_matrixR ;
  //     double tmp ;
  //     for(int row=1; row<=m_nparameters; ++row)
  //       for(int k=1; k<=m_nconstraints; ++k)
  // 	if( (tmp= (KR(row,k) - 2*m_matrixCGT(row,k))) != 0 )
  // 	  for(int col=1; col<=row; ++col)
  // 	    fitparams->cov().fast(row,col) += tmp * m_matrixK(col,k) ;

  // deltaCov = - C*GT*KT
  for(int row=0; row<m_nparameters; row++)
    for(int k=0; k<m_nconstraints; k++)
      if( (tmp = -(m_matrixCGT(row,k))) != 0 )  // they have same size, and same 'emptiness'
        for(int col=0; col<=row; col++)
          fitparams.cov()(row,col) += tmp * m_matrixK(col,k) ;

#endif
  for(int col=0; col<m_nconstraints; col++)
    for(int k=0; k<m_nparameters; k++)
      if( (*m_matrixG)(col,k) !=0 ) ++(fitparams->nConstraintsVec(k)) ;
  if(vtxverbose>=5) { std::cout << "KalmanCalculator::updateCov() done"<<std::endl;}
}
