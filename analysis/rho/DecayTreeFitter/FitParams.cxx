// ******************************************************
// DecayTreeFitter Package
// We thank the original author Wouter Hulsbergen 
// (BaBar, LHCb) for providing the sources. 
// http://arxiv.org/abs/physics/0503191v1 (2005)
// Adaptation & Development for PANDA: Ralf Kliemt (2015)
// ******************************************************
#include <iostream>
#include <iomanip>
#include "FitParams.h"

using namespace DecayTreeFitter;
ClassImp(FitParams);

DecayTreeFitter::FitParams::FitParams(int adim)
: m_dim(adim),m_par(adim),m_cov(adim),//m_scale(adim),
m_chiSquare(0),m_nConstraints(0),m_nConstraintsVec(adim,0)
{
  ////fill m_scale with "1" at diagonals
  //for(int i=0;i<adim;i++) m_scale[i][i]=1.;
}
DecayTreeFitter::FitParams::FitParams(const FitParams& fpar) 
: m_dim(fpar.dim()),m_par(fpar.dim()),m_cov(fpar.dim()),
m_chiSquare(0),m_nConstraints(0),m_nConstraintsVec(fpar.dim(),0)
{
    m_par = fpar.par();
    m_cov = fpar.cov();
    //FIXME don't copy constraintsvector etc? Those are cleared anyways. I guess
}

DecayTreeFitter::FitParams::~FitParams() {    
  //std::cout<<"FitParams::~FitParams A ("<<&m_nConstraintsVec<<"); n="<<m_nConstraintsVec.size()<<"   c="<<m_nConstraintsVec.capacity() <<std::endl;
  //m_nConstraintsVec.clear();
  //std::cout<<"FitParams::~FitParams B; n="<<m_nConstraintsVec.size()<<"   c="<<m_nConstraintsVec.capacity() <<std::endl;
  //m_chiSquareMap.clear() ;
}

void DecayTreeFitter::FitParams::resetPar() {
  for(int row=0; row<m_dim; row++)
    m_par(row) = 0 ;
}

void DecayTreeFitter::FitParams::resetCov(double aScale) {

  if(aScale > 0)
  {
    for(int row=0; row<m_dim; row++) {
      for(int col=0; col<row; col++) {
        m_cov(row,col) = 0 ;
        m_cov(col,row) = 0 ;
      }
      m_cov(row,row) *= aScale ;
      if(m_cov(row,row) < 0 ) m_cov(row,row)*=-1 ;
      if(m_cov(row,row) == 0 ) m_cov(row,row)=aScale ;
    }
  }
  m_chiSquare=0 ;
  m_nConstraints=0 ;
  for(int row=0; row<m_dim; row++) {
    nConstraintsVec(row) = 0 ;
  }
  m_chiSquareMap.clear() ;
}

bool DecayTreeFitter::FitParams::testCov() const {
  bool okay=true ;
  for(int row=0; row<m_dim && okay; row++)
    okay = m_cov(row,row)>=0 ;
  return okay ;
}

void DecayTreeFitter::FitParams::print() const {
  std::cout << std::setw(3) << "index" << std::setw(15) << "val" << std::setw(15) << "err" << std::endl ;
  std::cout << std::setprecision(5) ;
  for(int row=0; row<m_dim; row++)
    std::cout << std::setw(3) << row
    << std::setw(15) << m_par(row)
    << std::setw(15) << sqrt(m_cov(row,row)) << std::endl ;
  std::cout << std::setprecision(10) ;
} ;

TMatrixDSym DecayTreeFitter::FitParams::cov(const std::vector<int>& indexVec) const {
  int nrow = indexVec.size() ;
  TMatrixDSym thecov(nrow) ;
  for(int row=0; row<nrow; row++)
    //for(int col=0; col<=row ; col++)//fix for TmatrixDSym (which is not really symmetric)
    for(int col=0; col<nrow ; col++)
      thecov(row,col) = m_cov(indexVec[row],indexVec[col]) ;
  return thecov ;
}

TVectorD DecayTreeFitter::FitParams::par(const std::vector<int>& indexVec) const {
  int nrow = indexVec.size() ;
  TVectorD thepar(nrow) ;
  for(int row=0; row<nrow; row++)
    thepar(row) = m_par(indexVec[row]) ;
  return thepar ;
}

void DecayTreeFitter::FitParams::reset(int newdim)
{
  resetPar();
  resetCov();
  if( newdim != m_dim )
  {
    m_par.ResizeTo(newdim);
    m_cov.ResizeTo(newdim,newdim);
    m_nConstraintsVec.resize(newdim,0) ;
  }
}

void DecayTreeFitter::FitParams::resize(int newdim)
{
  if( newdim > m_dim ) {
    m_dim = newdim ;
    // very expensive, but okay ...
    //      TVectorD newpar(newdim) ;
    //      newpar.sub(1,m_par);
    //
    //      TMatrixDSym newcov(newdim) ;
    //      newcov.sub(1,m_cov) ;
    //
    //      //      HepVector newpar(newdim,0) ;
    //      //       HepSymMatrix newcov(newdim,0) ;
    //      //       std::cout << newpar << std::endl ;
    //      //       for(int row=0; row<m_dim ; row++) {
    //      // 	newpar(row) = m_par(row) ;
    //      // 	for(int col=0; col<=row; col++)
    //      // 	// 	  newcov(row,col) = m_cov(row,col) ;
    //      //       }
    //      //      std::cout << m_par << " " << newpar << std::endl ;
    //
    //      m_par = newpar ;
    //      m_cov = newcov ;
    m_par.ResizeTo(newdim);
    m_cov.ResizeTo(newdim,newdim);
    m_nConstraintsVec.resize(newdim,0) ;
  }
}

void DecayTreeFitter::FitParams::addChiSquare( double chisq, int nconstraints, const ParticleBase* p)
{
  m_chiSquare += chisq;
  m_nConstraints += nconstraints ;
  if( p ) m_chiSquareMap[ p ] += ChiSquare(chisq,nconstraints) ;
}

ChiSquare DecayTreeFitter::FitParams::chiSquare( const ParticleBase& p) const
{
  std::map<const ParticleBase*, ChiSquare>::const_iterator it = m_chiSquareMap.find( &p ) ;
  return it != m_chiSquareMap.end() ? it->second : ChiSquare() ;
}

//   void DecayTreeFitter::FitParams::copy(const FitParams& rhs,
// 		       const indexmap& anindexmap)
//   {
//     for(indexmap::const_iterator it = anindexmap.begin() ;
// 	it != anindexmap.end(); ++it) {
//       int idim =     it->first->dim() ;
//       int indexrhs = it->second ;
//       int indexlhs = it->first->index() ;
//       for(int i=0; i<idim; i++)
// 	m_par(indexlhs+i) = rhs.m_par(indexrhs+i) ;
//       for(indexmap::const_iterator it2 = it ;
// 	  it2 != anindexmap.end(); ++it2) {
// 	int jdim     = it2->first->dim() ;
// 	int jndexrhs = it2->second ;
// 	int jndexlhs = it2->first->index() ;
// 	for(int i=0; i<idim; i++)
// 	  for(int j=0; j<jdim ; j++)
// 	    m_cov( indexlhs+i, jndexlhs+j) = rhs.m_cov( indexrhs+i, jndexrhs+j) ;
//       }
//     }
//   }
