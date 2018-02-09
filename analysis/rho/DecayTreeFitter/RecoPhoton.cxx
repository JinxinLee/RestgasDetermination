// ******************************************************
// DecayTreeFitter Package
// We thank the original author Wouter Hulsbergen 
// (BaBar, LHCb) for providing the sources. 
// http://arxiv.org/abs/physics/0503191v1 (2005)
// Adaptation & Development for PANDA: Ralf Kliemt (2015)
// ******************************************************
#include <stdio.h>
//#include "Event/Particle.h"
//#include "Event/CaloHypo.h"
//#include "Event/CaloPosition.h"

#include "RecoPhoton.h"
#include "FitParams.h"
#include "TVector3.h"
#include "RhoCalculationTools.h"

using namespace DecayTreeFitter;
ClassImp(RecoPhoton);

extern int vtxverbose ;

DecayTreeFitter::RecoPhoton::RecoPhoton(RhoCandidate* bc, const ParticleBase* aMother)
: RecoParticle(bc,aMother), m_V(3)
{
  updCache() ; // read from candidate
}

DecayTreeFitter::RecoPhoton::~RecoPhoton() {}

ErrCode DecayTreeFitter::RecoPhoton::initParPhoton(FitParams* fitparams,
                                  const TVector3& motherpos) const
{
  TVector3 dx( m_m(0) - motherpos.x(),
              m_m(1) - motherpos.y(),
              m_z    - motherpos.z()) ;
  double l = dx.Mag() ;

  // get the energy
  double energy = m_m(2) ;

  // assign the momentum
  int momindex = momIndex() ;
  fitparams->par(momindex+0) = energy*dx.x()/l ;
  fitparams->par(momindex+1) = energy*dx.y()/l ;
  fitparams->par(momindex+2) = energy*dx.z()/l ;

  return ErrCode() ;
}

ErrCode
DecayTreeFitter::RecoPhoton::initPar1(FitParams* fitparams)
{
  if(vtxverbose>5){std::cout<<"RecoPhoton::initPar1: - "<<std::endl;}
  return initParPhoton( fitparams, TVector3(0,0,0) ) ;
}

ErrCode
DecayTreeFitter::RecoPhoton::initPar2(FitParams* fitparams)
{
  if(vtxverbose>5){std::cout<<"RecoPhoton::initPar2: - "<<std::endl;}
  // calculate the direction
  int posindexmother = mother()->posIndex() ;
  TVector3 motherpos(fitparams->par(posindexmother+0),
                     fitparams->par(posindexmother+1),
                     fitparams->par(posindexmother+2)) ;
  return initParPhoton(fitparams, motherpos ) ;
}

ErrCode
DecayTreeFitter::RecoPhoton::initCov(FitParams* fitparams) const
{
  int momindex = momIndex() ;
  double varEnergy =  m_V(2,2) ;
  const double factor = 1000;
  for(int row=0; row<3; row++)
    fitparams->cov()(momindex+row,momindex+row) = factor * varEnergy ;
  return ErrCode() ;
}

ErrCode
DecayTreeFitter::RecoPhoton::updCache()
{
  //    const LHCb::CaloHypo* hypo = particle().proto()->calo()[0] ;
  //    // this works for photons
  //    const LHCb::CaloPosition* pos = hypo->position() ;
  // and this seems to work for merged pi0 ...
  //    if(!pos && hypo->clusters().size()>0)
  //      pos = &(hypo->clusters()[0]->position()) ;
  ErrCode rc ;

  //    if(pos) {
  // FIXME: Welche Parameter hat das?
  // Parameters: X, Y, of Cluster (in crystal plane?), E
  // FIXME: LHCb is a fixed-target experiment, all detectors line up along Z!
  FairRecoCandidate* rec = particle()->GetRecoCandidate();
  m_m(0) = rec->GetLastHit().X(); // x
  m_m(1) = rec->GetLastHit().Y(); // y
  m_z    = rec->GetLastHit().Z(); // z
  m_m(2) = rec->GetEnergy(); // E
  if(vtxverbose>4) std::cout<<"RecoPhoton::updCache() l."<<__LINE__ <<" m_m(0) = "<<m_m(0) <<" cm; m_m(1) = "<<m_m(1) <<" cm; m_z = "<<m_z <<"cm; m_M(2) = "<<m_m(2)<<" GeV" <<std::endl;
  TMatrixD cov7 = rec->Cov7(); //error matrix for
  m_V[0][0]=cov7[0][0]; // x-x
  m_V[0][1]=cov7[0][1]; // x-y
  m_V[0][2]=cov7[0][6]; // x-E
  m_V[1][0]=cov7[1][0]; // y-x
  m_V[1][1]=cov7[1][1]; // y-y
  m_V[1][2]=cov7[1][6]; // y-E
  m_V[2][0]=cov7[6][0]; // E-x
  m_V[2][1]=cov7[6][1]; // E-y
  m_V[2][2]=cov7[6][6]; // E-E
                        //      m_m = pos->parameters() ;
                        //      m_V = pos->covariance() ;
                        //      m_z = pos->z() ;
                        //    } else {
                        //      std::cout << "DecayTreeFitter::RecoPhoton: cannot find position info for cluster" << std::endl ;
                        //      rc = ErrCode::badsetup ;
                        //    }
  return ErrCode() ;
}

ErrCode
DecayTreeFitter::RecoPhoton::projectRecoConstraint(const FitParams* fitparams, Projection& p) const
{
  // residual of photon:
  // r(0-2) = motherpos + mu * photon momentum - cluster position
  // r(3)   = |momentum| - cluster energy
  // mu is calculated from the 'chi2-closest approach' (see below)

  ErrCode status ;

  // calculate the total momentum and energy:
  int momindex  = momIndex() ;
  double px = fitparams->par()(momindex+0) ;
  double py = fitparams->par()(momindex+1) ;
  double pz = fitparams->par()(momindex+2) ;
  double m  = pdtMass() ; // could be non-zero for mergedpi0

  double energy = std::sqrt(px*px+py*py+pz*pz+m*m) ;
  double dedpx  = px/energy ;
  double dedpy  = py/energy ;
  double dedpz  = pz/energy ;

  double tx = px/pz ;
  double ty = py/pz ;
  double dtxdpx = 1/pz ;
  double dtxdpz = -tx/pz ;
  double dtydpy = 1/pz ;
  double dtydpz = -ty/pz ;

  // calculate dX = Xc - Xmother
  int posindex  = mother()->posIndex() ;

  double dz = m_z    - fitparams->par(posindex+2) ;
  double dx = m_m(0) - fitparams->par(posindex+0) - dz * tx ;
  double dy = m_m(1) - fitparams->par(posindex+1) - dz * ty ;
  double dE = m_m(2) - energy ;

  p.r(0) = dx ;
  p.r(1) = dy ;
  p.r(2) = dE ;

  // calculate the projection matrix
  // first the 'position' part

  // derivative of position to position of mother
  p.H(0,posindex+0) = -1 ;
  p.H(1,posindex+1) = -1 ;

  // derivative to position to 3-momentum
  p.H(0,momindex+0) = -dz * dtxdpx ;
  p.H(0,momindex+2) = -dz * dtxdpz ;
  p.H(1,momindex+1) = -dz * dtydpy ;
  p.H(1,momindex+2) = -dz * dtydpz ;

  // derivative of momentum to 3-momentum
  p.H(2,momindex+0) = -dedpx ;
  p.H(2,momindex+1) = -dedpy ;
  p.H(2,momindex+2) = -dedpz ;

  // error
  for(int irow=0; irow<3; irow++)
    for(int icol=0; icol<3/*=irow*/; icol++)
      p.Vfast(irow,icol) = m_V(irow,icol) ;
  if(vtxverbose>6){
    std::cout<<"RecoPhoton::projectRecoConstraint(): projection is:"<<posindex<<std::endl;
    std::cout<<"r "; p.r().Print();
    std::cout<<"V "; p.V().Print();
    std::cout<<"H "; RhoCalculationTools::PrintMatrix(p.H());
    }

  return status ;
}
