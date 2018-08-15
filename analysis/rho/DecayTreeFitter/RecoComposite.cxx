// ******************************************************
// DecayTreeFitter Package
// We thank the original author Wouter Hulsbergen 
// (BaBar, LHCb) for providing the sources. 
// http://arxiv.org/abs/physics/0503191v1 (2005)
// Adaptation & Development for PANDA: Ralf Kliemt (2015)
// ******************************************************

//#include "Event/Particle.h"
#include "RecoComposite.h"
#include "FitParams.h"
#include "TMatrixD.h"
#include "RhoCalculationTools.h"

using namespace DecayTreeFitter;
ClassImp(RecoComposite);

extern int vtxverbose ;

DecayTreeFitter::RecoComposite::RecoComposite(RhoCandidate* bc, const ParticleBase* amother)
: ParticleBase(bc,amother),m_m(),m_matrixV(),m_hasEnergy(true)
{
  //bool massconstraint = bc && bc->constraint(RecoConstraint::Mass) ;
  //m_hasEnergy = !massconstraint ;
  updCache() ;
}

TMatrixDSym convertToHepSymMatrix(const TMatrixD& M)
{ //FIXME does that go with the RhoCandidate?
  int dim = M.GetNrows() ;
  TMatrixDSym rc(dim) ;
  for(int irow=0; irow<dim; ++irow)
    for(int icol=0; icol<=irow; ++icol)
    {
      rc(irow,icol) = M(irow,icol);
      if(icol!=irow) rc(icol,irow) = M(irow,icol);
    }
  return rc ;
}

void DecayTreeFitter::RecoComposite::updCache()
{
  // cache par7 (x,y,z,px,py,pz,E) cov7
  TVector3 pos = particle()->Pos() ;
  TLorentzVector mom = particle()->P4() ;
  m_m = TVectorD(dimM()) ;
  m_m(0) = pos.x() ;
  m_m(1) = pos.y() ;
  m_m(2) = pos.z() ;
  m_m(3) = mom.Px() ;
  m_m(4) = mom.Py() ;
  m_m(5) = mom.Pz() ;
  if(hasEnergy()) m_m(6) = mom.E() ;
  TMatrixDSym cov7 = convertToHepSymMatrix( particle()->Cov7()) ;//FIXME The conversion shall be checked
  cov7.GetSub(0,dimM(),m_matrixV) ; // so either 7 or 6, depending on mass constraint
  if(vtxverbose>=4) {
    std::cout << "cov matrix of external candidate: " << name().c_str()
    << " " << dimM() << std::endl;
    m_matrixV.Print();
  }
}

DecayTreeFitter::RecoComposite::~RecoComposite() {}

ErrCode
DecayTreeFitter::RecoComposite::initPar1(FitParams* fitparams)
{
  if(vtxverbose>5){std::cout<<"RecoComposite::initPar1: - "<<std::endl;}
  int posindex = posIndex() ;
  int momindex = momIndex() ;

  //quick map for parameters
  int indmap[7]  ;
  for(int i=0; i<3; ++i) indmap[i]   = posindex+i ;
  for(int i=0; i<4; ++i) indmap[i+3] = momindex+i ;
  // copy the 'measurement'
  for(int row=0; row<dimM(); ++row)
    fitparams->par()(indmap[row]) = m_m(row) ;

  return ErrCode::success ;
}

ErrCode
DecayTreeFitter::RecoComposite::initPar2(FitParams* fitparams)
{
  if(vtxverbose>5){std::cout<<"RecoComposite::initPar2: - "<<std::endl;}
  // call default lifetime initialization
  return initTau(fitparams) ;
}

ErrCode
DecayTreeFitter::RecoComposite::projectRecoComposite(const FitParams* fitparams,
                                    Projection& p) const
{
  int posindex = posIndex() ;
  int momindex = momIndex() ;

  // quick map for parameters
  int indmap[7]  ;
  for(int i=0; i<3; i++) indmap[i]   = posindex+i ;
  for(int i=0; i<4; i++) indmap[i+3] = momindex+i ;
  for(int row=0; row<dimM(); row++) {
    //p.r(row) = fitparams->par()(indmap[row]) - m_m(row) ;
    p.r(row) = m_m(row) - fitparams->par()(indmap[row]) ; //RK turned sign. It's "measurement - step before".
    p.H(row,indmap[row]) = 1 ;
    for(int col=0; col<dimM()/*=row*/; col++)
      p.Vfast(row,col) = m_matrixV(row,col) ;
  }
  if(vtxverbose>6){
    std::cout<<"RecoComposite::projectRecoComposite(): projection is:"<<posindex<<std::endl;
    std::cout<<"r "; p.r().Print();
    std::cout<<"V "; p.V().Print();
    std::cout<<"H "; RhoCalculationTools::PrintMatrix(p.H());
    }

  return ErrCode::success ;
}

ErrCode
DecayTreeFitter::RecoComposite::projectConstraint(Constraint::Type aType,
                                 const FitParams* fitparams,
                                 Projection& p) const
{
  ErrCode aStatus ;
  switch(aType) {
    case Constraint::composite:
      aStatus |= projectRecoComposite(fitparams,p) ;
      break ;
    case Constraint::geometric:
      aStatus |= projectGeoConstraint(fitparams,p) ;
      break ;
    default:
      aStatus |= ParticleBase::projectConstraint(aType,fitparams,p) ;
  }
  return aStatus ;
}

double
DecayTreeFitter::RecoComposite::chiSquare(const FitParams* fitparams) const
{
  Projection p(fitparams->dim(),dimM()) ;
  projectRecoComposite(fitparams,p) ;
  return p.chiSquare() ;
}
