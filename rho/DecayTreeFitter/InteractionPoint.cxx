// ******************************************************
// DecayTreeFitter Package
// We thank the original author Wouter Hulsbergen 
// (BaBar, LHCb) for providing the sources. 
// http://arxiv.org/abs/physics/0503191v1 (2005)
// Adaptation & Development for PANDA: Ralf Kliemt (2015)
// ******************************************************
#include "FitParams.h"
#include "InteractionPoint.h"
#include "RecoTrack.h"
//#include "Event/Particle.h"
//#include "Event/VertexBase.h"
#include <assert.h>
//#include <vector>
#include "TVector3.h"
#include "TMath.h"
#include "RhoVector3Err.h"
#include "RhoCalculationTools.h"
#include "SortTool.h"

using namespace DecayTreeFitter;

extern int vtxverbose ;
ClassImp(InteractionPoint);

DecayTreeFitter::InteractionPoint::InteractionPoint(RhoCandidate* cand, const Configuration& config)
: InternalParticle(cand,0,config), m_ipPosCov(3), m_ipPosCovInv(3), m_ipMomCov(4), m_ipMomCovInv(4), m_hasposcon(false), m_hasmomcon(false)
{
  if(vtxverbose>=2) std::cout << "InteractionPoint: start"<<std::endl;
  //addDaughter( cand, config ) ;
  //if(vtxverbose>=2) std::cout << "InteractionPoint: daughters done"<<std::endl;
 
  m_hasmomcov = false;
  m_hasposcov = false;
 
  if(vtxverbose>=2) {
    std::cout << "InteractionPoint: without initial beam spot" <<std::endl ;
    std::cout << "daughter: " << daughters().front()->name() << std::endl ;
  }
}

DecayTreeFitter::InteractionPoint::InteractionPoint(const RhoVector3Err& ipvertex, RhoCandidate* cand, const Configuration& config)
: InternalParticle(cand,0,config), m_ipPosCov(3), m_ipPosCovInv(3), m_ipMomCov(4), m_ipMomCovInv(4), m_hasposcon(true), m_hasmomcon(false)
{
  if(vtxverbose>=2) std::cout << "InteractionPoint: start"<<std::endl;
  //addDaughter( cand, config ) ;
  //if(vtxverbose>=2) std::cout << "InteractionPoint: daughters done"<<std::endl;
  
  m_ipPos(0) = ipvertex.x() ;
  m_ipPos(1) = ipvertex.y() ;
  m_ipPos(2) = ipvertex.z() ;
  for(int ii=0;ii<3;ii++) for(int jj=0;jj<3;jj++){
    m_ipPosCov(ii,jj) = ipvertex.CovMatrix()(ii,jj);
  }
  m_hasmomcov = false;
  m_hasposcov = (0!=m_ipPosCov.NonZeros()); // a zero protection flag
  if(m_hasposcov)
  {
    double ierr;
    if(vtxverbose>=2) std::cout << "InteractionPoint: inverting matrix"<<std::endl;
    m_ipPosCovInv = m_ipPosCov ;
    m_ipPosCovInv.Invert(&ierr) ;
  }
  if(vtxverbose>=2) {
    std::cout << "InteractionPoint: initial beam spot = ("
    <<m_ipPos(0) << "," << m_ipPos(1) << "," << m_ipPos(2) << ")       " <<std::endl ;
    if(vtxverbose>=5){
      std::cout<<"m_ipPosCov"; RhoCalculationTools::PrintMatrix(m_ipPosCov);
    }
    std::cout << "daughter: " << daughters().front()->name() << std::endl ;
  }
}

DecayTreeFitter::InteractionPoint::InteractionPoint(const RhoLorentzVectorErr& ipmom, RhoCandidate* cand, const Configuration& config)
: InternalParticle(cand,0,config), m_ipPosCov(3), m_ipPosCovInv(3), m_ipMomCov(4), m_ipMomCovInv(4), m_hasposcon(false), m_hasmomcon(true)
{
  if(vtxverbose>=2) std::cout << "InteractionPoint: start"<<std::endl;
  //addDaughter( cand, config ) ;
  //if(vtxverbose>=2) std::cout << "InteractionPoint: daughters done"<<std::endl;
  
  m_ipMom(0) = ipmom.X();
  m_ipMom(1) = ipmom.Y();
  m_ipMom(2) = ipmom.Z();
  m_ipMom(3) = ipmom.E();
  for(int ii=0;ii<4;ii++) for(int jj=0;jj<4;jj++){
    m_ipMomCov(ii,jj) = ipmom.CovMatrix()(ii,jj);
  }
  
  m_hasposcov = false;
  m_hasmomcov = (0!=m_ipMomCov.NonZeros()); // a zero protection flag
  if(m_hasmomcov)
  {
    double ierr;
    if(vtxverbose>=2) std::cout << "InteractionPoint: inverting matrix"<<std::endl;
    m_ipMomCovInv = m_ipMomCov ;
    m_ipMomCovInv.Invert(&ierr) ;
  }

  if (particle())
  {
    RhoVector3Err vtx = particle()->PosWCov();
    if(vtx.Mag()>0){
      if(vtxverbose>=3)    std::cout << "InteractionPoint: Found a vertex in RhoCandidate"<<std::endl;
      m_ipPos(0) = vtx.x() ;
      m_ipPos(1) = vtx.y() ;
      m_ipPos(2) = vtx.z() ;
      m_hasposcon=true;
      for(int ii=0;ii<3;ii++) for(int jj=0;jj<3;jj++){
        m_ipPosCov(ii,jj) = vtx.CovMatrix()(ii,jj);
      }
      m_hasposcov = (0!=m_ipPosCov.NonZeros()); // a zero protection flag
    }
  }

  if(vtxverbose>=2) {
    std::cout << "InteractionPoint: initial mom = (" <<m_ipMom(0) << "," << m_ipMom(1) << "," << m_ipMom(2)<< "," << m_ipMom(3) << ")" <<std::endl ;
    if(vtxverbose>=5){
      std::cout<<"m_ipMomCov"; RhoCalculationTools::PrintMatrix(m_ipMomCov);
    }
    std::cout << "daughter: " << daughters().front()->name() << std::endl ;
  }
}

DecayTreeFitter::InteractionPoint::InteractionPoint(const RhoLorentzVectorErr& ipmom, const RhoVector3Err& ipvertex, RhoCandidate* cand, const Configuration& config)
: InternalParticle(cand,0,config), m_ipPosCov(3), m_ipPosCovInv(3), m_ipMomCov(4), m_ipMomCovInv(4), m_hasposcon(true), m_hasmomcon(true)
{
  if(vtxverbose>=2) std::cout << "InteractionPoint: start"<<std::endl;
  //addDaughter( cand, config ) ;
  //if(vtxverbose>=2) std::cout << "InteractionPoint: daughters done"<<std::endl;
  
  m_ipPos(0) = ipvertex.x() ;
  m_ipPos(1) = ipvertex.y() ;
  m_ipPos(2) = ipvertex.z() ;
  for(int ii=0;ii<3;ii++) for(int jj=0;jj<3;jj++){
    m_ipPosCov(ii,jj) = ipvertex.CovMatrix()(ii,jj);
  }
  
  m_ipMom(0) = ipmom.X();
  m_ipMom(1) = ipmom.Y();
  m_ipMom(2) = ipmom.Z();
  m_ipMom(3) = ipmom.E();
  for(int ii=0;ii<4;ii++) for(int jj=0;jj<4;jj++){
    m_ipMomCov(ii,jj) = ipmom.CovMatrix()(ii,jj);
  }
  
  m_hasposcov = (0!=m_ipPosCov.NonZeros()); // a zero protection flag
  if(m_hasposcov)
  {
    double ierr;
    if(vtxverbose>=2) std::cout << "InteractionPoint: inverting matrix"<<std::endl;
    m_ipPosCovInv = m_ipPosCov ;
    m_ipPosCovInv.Invert(&ierr) ;
  }
  m_hasmomcov = (0!=m_ipMomCov.NonZeros()); // a zero protection flag
  if(m_hasmomcov)
  {
    double ierr;
    if(vtxverbose>=2) std::cout << "InteractionPoint: inverting matrix"<<std::endl;
    m_ipMomCovInv = m_ipMomCov ;
    m_ipMomCovInv.Invert(&ierr) ;
  }
  if(vtxverbose>=2) {
    std::cout << "InteractionPoint: initial beam spot = ("
    <<m_ipPos(0) << "," << m_ipPos(1) << "," << m_ipPos(2) << ")       " 
    << "mom = (" <<m_ipMom(0) << "," << m_ipMom(1) << "," << m_ipMom(2)<< "," << m_ipMom(3) << ")" <<std::endl ;
    if(vtxverbose>=5){
      std::cout<<"m_ipPosCov"; RhoCalculationTools::PrintMatrix(m_ipPosCov);
      std::cout<<"m_ipMomCov"; RhoCalculationTools::PrintMatrix(m_ipMomCov);
    }
    std::cout << "daughter: " << daughters().front()->name() << std::endl ;
  }
}

ErrCode
DecayTreeFitter::InteractionPoint::initPar1(FitParams* fitparams)
{
  ErrCode status ;
  if(vtxverbose>5){std::cout<<"InteractionPoint::initPar1: - start"<<std::endl;}
  
  // initialize our IP measurement if there
  int posindex = posIndex() ;
  if(m_hasposcon)
  {
    for(int row=0; row<3; row++)
      fitparams->par()(posindex+row) = m_ipPos(row) ;
  } 
  
  InternalParticle::initPar1(fitparams);
  
  return status ;
}

ErrCode
DecayTreeFitter::InteractionPoint::initCov(FitParams* fitpar) const
{
  ErrCode status = InternalParticle::initCov(fitpar);
  int posindex = posIndex();
  for(int row=0; row<3; row++)
  {
    if(m_hasposcov) 
    { // when we know an external IP we put it here as initial values
      for(int col=0;col<3;col++) 
        fitpar->cov()(posindex+row,posindex+col) = m_ipPosCov(row,col) ;
    }
  }
return status;

}

ErrCode
DecayTreeFitter::InteractionPoint::projectIPConstraint(const FitParams* fitparams,
                                      Projection& p) const
{
  int posindex = posIndex() ;
  if(vtxverbose>6){std::cout<<"InteractionPoint::projectIPConstraint(): posindex="<<posindex<<std::endl;}
  if(m_hasposcon){
    for(int row=0; row<3; ++row) {
      p.r(row) =  fitparams->par()(posindex+row) - m_ipPos(row); 
      p.H(row,posindex+row) = 1 ;
      if(m_hasposcov)
        for(int col=0; col<3; ++col)
          p.Vfast(row,col) = m_ipPosCov(row,col) ;
    }
  }
  if(vtxverbose>6){
    std::cout<<"InteractionPoint::projectIPConstraint(): projection is:"<<posindex<<std::endl;
    std::cout<<"r "; p.r().Print();
    std::cout<<"V "; p.V().Print();
    std::cout<<"H "; RhoCalculationTools::PrintMatrix(p.H());
    }
  return ErrCode::success ;
}

ErrCode
DecayTreeFitter::InteractionPoint::projectBeamConstraint(const FitParams* fitparams,
                                      Projection& p) const
{
  int momindex = momIndex() ; // get momentum part of the only daughter (head of tree)
  if(vtxverbose>6){std::cout<<"InteractionPoint::projectIPConstraint(): daumomindex="<<momindex<<std::endl;}
  if(m_hasmomcon){
    for(int row=0; row<4; ++row) {
      p.r(row) = fitparams->par()(momindex+row) - m_ipMom(row);
      p.H(row,momindex+row) = 1 ;
      if(m_hasmomcov)
        for(int col=0; col<4; ++col)
          p.Vfast(row,col) = m_ipMomCov(row,col) ;
    }
  }
  if(vtxverbose>6){
    std::cout<<"InteractionPoint::projectBeamConstraint(): projection is:"<<momindex<<std::endl;
    std::cout<<"r "; p.r().Print();
    std::cout<<"V "; p.V().Print();
    std::cout<<"H "; RhoCalculationTools::PrintMatrix(p.H());
    }
  return ErrCode::success ;
}

ErrCode
DecayTreeFitter::InteractionPoint::projectConstraint(Constraint::Type aType,
                                    const FitParams* fitparams,
                                    Projection& p) const
{
  ErrCode status;
  switch(aType) {
    case Constraint::beamspot:
      status |= projectIPConstraint(fitparams,p) ;
      break ;
    case Constraint::beamenergy:
      status |= projectBeamConstraint(fitparams,p) ;
      break ;
    default:
      status |= InternalParticle::projectConstraint(aType,fitparams,p) ;
  }
  return status ;
}

double
DecayTreeFitter::InteractionPoint::chiSquare(const FitParams* fitparams) const
{
  std::cout<<" Marke 2" <<std::endl;
  double chisq=0.;
  //if(m_hasposcon&&m_hasposcov) 
  //{ // no covariance, no chi2 contribution!
    //Projection pPos(fitparams->dim(),3) ;
    //projectIPConstraint(fitparams,pPos) ;
    //chisq+=pPos.chiSquare(); 
  //}
  if(m_hasmomcon&&m_hasmomcov) 
  { // no covariance, no chi2 contribution!
    Projection pMom(fitparams->dim(),4) ;
    projectBeamConstraint(fitparams,pMom) ;
    chisq+=pMom.chiSquare(); 
  }
  return chisq ;
}

//double
//DecayTreeFitter::InteractionPoint::chiSquare(const FitParams* fitparams) const
//{
  //double chisq=0.;
  //if(m_hasposcon&&m_hasposcov) 
  //{ // no covariance, no chi2 contribution!
    //int posindex = posIndex() ;
    //TVectorD residualpos(3) ;
    //for(int row=0; row<3; ++row)
      //residualpos(row) = fitparams->par()(posindex+row) - m_ipPos(row) ;
    //chisq += m_ipPosCovInv.Similarity(residualpos);
  //}
  //if(m_hasmomcon&&m_hasmomcov) 
  //{ // no covariance, no chi2 contribution!
    //int momindex = momIndex() ;
    //TVectorD residualmom(4) ;
    //for(int row=0; row<4; ++row)
      //residualmom(row) = m_ipMom(row) - fitparams->par()(momindex+row) ;
    //chisq += m_ipMomCovInv.Similarity(residualmom);
  //}
  //// add the daughters' chi2
  //chisq += ParticleBase::chiSquareD(fitparams) ;

  //return chisq ;
//}

void DecayTreeFitter::InteractionPoint::addToConstraintList(constraintlist& alist, int depth) const
{
  InternalParticle::addToConstraintList(alist,depth);

  //the beamspot
  if(m_hasposcon && m_hasposcov) alist.push_back(Constraint(this,Constraint::beamspot,depth,3)) ;
  if(m_hasmomcon) alist.push_back(Constraint(this,Constraint::beamenergy,depth,4)) ;
}


//std::string DecayTreeFitter::InteractionPoint::parname(int thisindex) const
//{
  //int id = thisindex ;
  //// skip the lifetime parameter name
  //if(id>=3) ++id ;
  //return ParticleBase::parname(id) ;
//}
