// ******************************************************
// DecayTreeFitter Package
// We thank the original author Wouter Hulsbergen 
// (BaBar, LHCb) for providing the sources. 
// http://arxiv.org/abs/physics/0503191v1 (2005)
// Adaptation & Development for PANDA: Ralf Kliemt (2015)
// ******************************************************
#include <stdio.h>

//#include "Event/Particle.h"
#include "RecoTrack.h"
#include "FitParams.h"
//#include "TrackKernel/TrackTraj.h"
//#include "TrackInterfaces/ITrackStateProvider.h"
#include "TVector3.h"
#include <assert.h>
#include <cmath>
#include "LineTool.h"
#include "RhoCalculationTools.h"


//[R.Kliemt, Dec.2014]
// TODO: How do we want to treat Bremsstrahlung? Certainly not inside the Fitter!
//namespace {
//  void applyBremCorrection( RhoCandidate state,
//			    double bremEnergy,
//			    double bremEnergyCov )
//  {
//    double momentum       = state.P();
//    double momentumCov = state.covariance()(4,4) * std::pow(momentum,4) ;
//    momentum += bremEnergy ;
//    momentumCov += bremEnergyCov ;
//    state.SetMomentum( momentum ) ;
//    state.covariance()(4,4) = momentumCov / std::pow(momentum,4) ;
//  }
//}

using namespace DecayTreeFitter;

ClassImp(RecoTrack);

extern int vtxverbose ;

DecayTreeFitter::RecoTrack::RecoTrack(RhoCandidate* cand,
                     const ParticleBase* aMother,
                     const Configuration& config)
: RecoParticle(cand,aMother),
//      m_track(cand.proto()->track()),
m_candidate(cand),
m_stateprovider( config.stateProvider() ),
m_useTrackTraj( config.useTrackTraj() ),
//      m_tracktraj(0),
m_cached(false),
m_flt(0)
//      m_bremEnergy(0),
//      m_bremEnergyCov(0)
{
  //set it to something such that we have a momentum estimate
     //m_state = m_track->firstState() ;
     m_stateprovider->state( m_state,const_cast<RhoCandidate*>(m_candidate));
  // get the bremenergy correction
  //[R.Kliemt, Dec.2014]
  // TODO: How do we want to treat Bremsstrahlung? Certainly not inside the Fitter!
  //    if( particle().particleID().abspid() == 11 ) {
  //      // FIXME: this is a hack. it will go wrong if you fit the
  //      // updated decay tree twice. to do this properly we need to use
  //      // the bremadding tools, but there are other problems with that.
  //      double momentumFromTrack    = 1/std::abs(m_state.qOverP() ) ;
  //      double momentumError2FromTrack = m_state.covariance()(4,4) * std::pow(momentumFromTrack,4) ;
  //      double momentumFromParticle = particle().momentum().P() ;
  //      double momentumError2FromParticle =
  //	particle().momCovMatrix()(3,3) * std::pow(particle().momentum().E()/momentumFromParticle,2) ;
  //      m_bremEnergyCov = momentumError2FromParticle - momentumError2FromTrack ;
  //      m_bremEnergy = momentumFromParticle - momentumFromTrack ;
  //      // if the correction is too small or unphysical, ignore it. the units are MeV.
  //      if( !(m_bremEnergyCov >1*Gaudi::Units::MeV*Gaudi::Units::MeV &&
  //	    m_bremEnergy>1*Gaudi::Units::MeV ) )
  //	m_bremEnergy = m_bremEnergyCov = 0 ;
  //      if(vtxverbose>=1) {
  //	std::cout << "Fitting an electron. ("
  //		  << momentumFromTrack << " +/- " << std::sqrt( momentumError2FromTrack ) << ") --> ("
  //		  << momentumFromParticle << " +/- " << std::sqrt( momentumError2FromParticle ) << ")" << std::endl ;
  //	std::cout << "brem correction: " << m_bremEnergy << " +/- " << std::sqrt( m_bremEnergyCov ) << std::endl ;
  //      }
  //      applyBremCorrection(m_state,m_bremEnergy,m_bremEnergyCov) ;
  //    }
}

ErrCode
DecayTreeFitter::RecoTrack::initPar2(FitParams* fitparams)
{
  if(vtxverbose>5){std::cout<<"RecoTrack::initPar2: - updcache"<<std::endl;}
  ErrCode rc = updCache(fitparams) ;
  const TVector3& recoP = particle()->P3() ;
  int momindex = momIndex() ;
  if(vtxverbose>5){std::cout<<"RecoTrack::initPar2: - write p3 to fitparams at momindex "<<momindex<<std::endl;}
  fitparams->par(momindex+0) = recoP.x() ;
  fitparams->par(momindex+1) = recoP.y() ;
  fitparams->par(momindex+2) = recoP.z() ;
  return rc ;
}

ErrCode
DecayTreeFitter::RecoTrack::initCov(FitParams* fitparams)
{
  // we only need a rough estimate of the covariance
  assert(m_cached) ;
  TMatrixD covP = particle()->P3Cov() ;
  int momindex = momIndex() ;
  for(int row=0; row<3; row++)
    fitparams->cov()(momindex+row,momindex+row) = 1000*covP(row,row) ;
  return ErrCode() ;
}

//[R.Kliemt, Dec.2014]
//TODO We don't cache a state right now. Let's get the fitter
//     running at all in the first place.
// Goal: Update the momentum direction from the real mesured track
// BUT in Panda most stuff will decay inside the beam pipe anyways
// We can't get better mesurements than the FirtstPar from tracking!
// Later we should ask PndAnaCalcTools to fetch a nice state, maybe employing GEANE/Runge-Kutta
// Since we have no State per se, we could update the FittedCand P3/P3Cov
ErrCode
DecayTreeFitter::RecoTrack::updCache(const FitParams* fitparams)
{
  //if(vtxverbose>5){std::cout<<"RecoTrack::updCache() - TODO: fix Stateprovider with real to-Z-propagation! "<<std::endl;}
  ErrCode rc ;
  //m_cached = true ; // DIRTY
  //return rc;  // Somewhat dirty
  
  // declare some constants
  const double ztolerance = m_stateprovider->ToleranceZ();//0.05; //*Gaudi::Units::cm ;
  //const double ztolerance = m_stateprovider->ToleranceZ(); 
  const double maxR = 0.05; //* Gaudi::Units::cm ; Beampipe radius in PANDA is 2cm

	//A don't do anything if too few of a change
	//B try cache
	//C ask stateprovider
  // TODO really from Rhocand?
  ////double prevstatez = particle()->Pos().Z() ;
  if(vtxverbose>5){std::cout<<"RecoTrack::updCache() - ask for fitparam z at posindex+2 = "<<mother()->posIndex()+2<<std::endl;}
  double vx = fitparams->par()(mother()->posIndex()+0) ;
  double vy = fitparams->par()(mother()->posIndex()+1) ;
  double vz = fitparams->par()(mother()->posIndex()+2) ;
  double dz = vz - m_state.z() ;
  double x = m_state.x() + dz * m_state.tx() ;
  double y = m_state.y() + dz * m_state.ty() ;
  double r = std::sqrt( x*x + y*y ) ;
  if(vtxverbose>5){std::cout<<"RecoTrack::updCache() - fitpar z="<<vz<<"  current state z="<<m_state.z()<<"  dz="<<dz<<"  ztolerance="<<ztolerance<<std::endl;}
  // If we stay close to the existing state, don't change anything.
  if( std::abs( dz ) > ztolerance )
  {
	  // first just look for the closest cached state
    if(vtxverbose>5){std::cout<<"RecoTrack::updCache() - find cached state"<<std::endl;}
    const DecayTreeFitter::State& aState = closestCachedState ( vz ) ;
    if( std::abs( aState.z() - vz ) < std::abs( dz ) )
	  {
	    if(vtxverbose>5)std::cout << "RecoTrack::updCache(): Found a closer state! "  << name() << " fitpar z=" << vz << "  current state z="  << m_state.z() << "  better state z="  << aState.z()  << std::endl ;
	    m_state = aState ;
	    dz = vz - m_state.z() ;
	  }
  }

  if( std::abs( dz ) > ztolerance || r > maxR )
  {
	  if(vtxverbose>5)std::cout << "RecoTrack::updCache(): calculate a new state."<< std::endl ;
	  // now that there is no cached state closeby, we'll fetch one
	  if( !m_stateprovider ) {
	   std::cerr<<"ERROR: DecayTreeFitter::RecoTrack::updCache() has no StateProvider"<<std::endl;
	   m_cached=false;
	   rc = ErrCode::badsetup;
	   return rc;
	  }
    if(vtxverbose>5){std::cout<<"RecoTrack::updCache() - call stateprovider"<<std::endl;}
	  m_stateprovider->state(m_state,const_cast<RhoCandidate*>(m_candidate),vx,vy,vz) ;
	  m_StateCache.push_back(m_state);
  }

  if(vtxverbose>5){std::cout<<"RecoTrack::updCache() - done"<<std::endl;}
  m_cached = true ;
  return rc ;
}
// *** Version of LHCB
//ErrCode
//DecayTreeFitter::RecoTrack::updCache(const FitParams* fitparams)
//{
//   ErrCode rc ;
//   // declare some constants
//   const double ztolerance = 1.; //*Gaudi::Units::cm ;
//   const double maxR = 1.; //* Gaudi::Units::cm ;
//    // The logic is a bit non-trivial. Above all, we need to make sure
//    // we don't apply the bremcorrection more than once on an existing
//    // state. States that we get from the track do not have brem
//    // correction applied.
//
//    // If we stay close to the existing state, don't change anything.
//    double prevstatez = particle().z() ;
//    double z = fitparams->par()(mother()->posIndex() + 3 ) ;
//    double dz = z - m_state.z() ;
//    if( std::abs( dz ) > ztolerance ) {
//      // first just look for the closest state on the track
//      const DecayTreeFitter::State& state = m_track->closestState ( z ) ;
//      if( std::abs( state.z() - z ) < std::abs( dz ) ) {
// 	//std::cout << "Found a closer state! "
// 	//	  << name() << " " << z << " "
// 	//	  << m_state.z() << " " << m_state.location() << " "
// 	//	  << state.z() << " " << state.location() << std::endl ;
// 	m_state = state ;
// 	dz = z - m_state.z() ;
//      }
//
//      // if that didn't work, try something else
//      if( std::abs( dz ) > ztolerance ) {
// 	// if the existing state is closest to beam, and if we stay
// 	// inside the beampipe, then don't change anything as well.
// 	double x = m_state.x() + dz * m_state.tx() ;
// 	double y = m_state.y() + dz * m_state.ty() ;
// 	double r = std::sqrt( x*x + y*y ) ;
// 	if( m_state.location() != LHCb::State::ClosestToBeam || r > maxR ) {
// 	  //std::cout << "Updating state for: "
// 	  //	    << name() << " " << z << " " << prevstatez << " " << m_state.z() << " "
// 	  //	    << m_stateprovider << " " << m_tracktraj << std::endl ;
// 	  if( m_stateprovider ) {
// 	    if( !m_useTrackTraj ) {
// 	      StatusCode sc = m_stateprovider->state( m_state,*m_track,z,ztolerance) ;
// 	      if( !sc.isSuccess() ) rc = ErrCode::badsetup ;
// 	    } else {
// 	      // cache the tracktraj
// 	      if( m_tracktraj==0 ) m_tracktraj = m_stateprovider->trajectory(*m_track) ;
// 	      // if nothing failed, use it.
// 	      if(m_tracktraj)
// 		m_state = m_stateprovider->trajectory(*m_track)->state(z) ;
// 	      else rc = ErrCode::badsetup ;
// 	    }
// 	  } else {
// 	    // create a trajectory on the fly
// 	    LHCb::TrackTraj traj( *m_track ) ;
// 	    m_state = traj.state( z ) ;
// 	  }
// 	}
//      }
//
//      //[R.Kliemt, Dec.2014]
//      // TODO: How do we want to treat Bremsstrahlung? Certainly not inside the Fitter!
// //      // deal with bremstrahlung
// //      if( m_state.z() != prevstatez && m_bremEnergyCov > 0 )
// //	applyBremCorrection(m_state,m_bremEnergy,m_bremEnergyCov) ;
// //    }
//   m_cached = true ;
//   return rc ;
//}

const DecayTreeFitter::State&
DecayTreeFitter::RecoTrack::closestCachedState( double z )
{
	// give the state with the best matching z coordinate, for an empty cache vector, we'll return the "current state"
	int beststate=-1;
	double bestdz=9999; //[cm] initially stupidly large
	for (unsigned int i=0;i<m_StateCache.size();i++){
		double dz=std::abs(m_StateCache[i].z() - z);
		if(dz < bestdz)
		{
			bestdz=dz;
			beststate=i;
		}
	}
	if(beststate>=0) return m_StateCache[beststate];
	else return m_state;
}



TVectorD symdiag(const TMatrixDSym& m) {
  TVectorD rc(m.GetNrows()) ;
  for(int i=0; i<m.GetNrows(); i++)
    rc(i) = std::sqrt(m(i,i)) ;
  return rc ;
}

ErrCode
DecayTreeFitter::RecoTrack::projectRecoConstraint(const FitParams* fitparams, Projection& p) const
{
  ErrCode status ;
  assert(m_cached) ;
  if(vtxverbose>=5)std::cout<<"RecoTrack::projectRecoConstraint()"<<std::endl;
  (const_cast<RecoTrack*>(this))->updCache(fitparams) ;
  if(vtxverbose>=5)std::cout<<"RecoTrack::projectRecoConstraint() cache updated"<<std::endl;
   
  int posindex = mother()->posIndex() ;
  TVector3 motherpos(fitparams->par()(posindex+0),
                     fitparams->par()(posindex+1),
                     fitparams->par()(posindex+2)) ;

  int momindex = momIndex() ;
  double px = fitparams->par()(momindex+0) ;
  double py = fitparams->par()(momindex+1) ;
  double pz = fitparams->par()(momindex+2) ;
  double mom2 = px*px+py*py+pz*pz ;
  double mom = std::sqrt(mom2) ;
  double tx  = px/pz ;
  double ty  = py/pz ;
  double qop = charge() / mom ;

  double dtxdpx = 1./pz ;
  double dtxdpz = -tx/pz ;
  double dtydpy = 1./pz ;
  double dtydpz = -ty/pz ;

  double dqopdpx = - qop * px/mom2 ;
  double dqopdpy = - qop * py/mom2 ;
  double dqopdpz = - qop * pz/mom2 ;
  
  if(vtxverbose>=5)std::cout<<"RecoTrack::projectRecoConstraint() residual"<<std::endl;
  // fill the residual
  const TVectorD& m_m = m_state.stateVector() ;
  const TMatrixD& m_V = m_state.covariance() ;
  double dz  = m_state.z() - motherpos.z() ;

  if(vtxverbose>=5) {
    std::cout<<"RecoTrack::projectRecoConstraint() "<<std::endl;
    std::cout<<"----------------------"<<std::endl;
    std::cout<<"       state position:  ("      <<m_state.x()<<","<<m_state.y()<<","<<m_state.z()<<")"<<std::endl; 
    std::cout<<"           mother pos:  ("      <<motherpos.x()<<","<<motherpos.y()<<","<<motherpos.z()<<")"<<std::endl; 
    std::cout<<"   state    tx,ty,q/p:  ("      <<m_state.tx()<<","<<m_state.ty()<<","<<m_state.qOverP()<<")"<<std::endl;
    std::cout<<"   fitpar tx, ty, q/p:  ("      <<tx<<","<<ty<<","<<qop<<")"<<std::endl;
    std::cout<<"   drift dz*(tx,ty,1):  ("      <<dz*tx<<","<<dz*ty<<","<<dz<<")" <<std::endl;
    std::cout<<"           fitpar mom:  ("      <<px<<","<<py<<","<<pz<<")"<<std::endl; 
    std::cout<<"               q, mom:  ("      <<charge()<<","<<mom<<")"<<std::endl;
    std::cout<<"----------------------"<<std::endl;
  }
  ///// Indeces of m_m??
  p.r(0) = m_m(0) - motherpos.x() - dz*tx ;
  p.r(1) = m_m(1) - motherpos.y() - dz*ty ;
  p.r(2) = m_m(2) - tx ;
  p.r(3) = m_m(3) - ty ;
  p.r(4) = m_m(4) - qop ;

  if(vtxverbose>=5)std::cout<<"RecoTrack::projectRecoConstraint() projection"<<std::endl;
  // compute the projection matrix
  // derivative of position to position of mother
  p.H(0,posindex+0) = -1 ;
  p.H(1,posindex+1) = -1 ;
  p.H(0,posindex+2) = tx ;
  p.H(1,posindex+2) = ty ;

  // derivative of position to 3-momentum
  p.H(0,momindex+0) = -dz * dtxdpx ;
  p.H(0,momindex+2) = -dz * dtxdpz ;
  p.H(1,momindex+1) = -dz * dtydpy ;
  p.H(1,momindex+2) = -dz * dtydpz ;

  // derivative of slopes/qop to 3-momentum
  p.H(2,momindex+0) = -dtxdpx ;
  p.H(2,momindex+2) = -dtxdpz ;
  p.H(3,momindex+1) = -dtydpy ;
  p.H(3,momindex+2) = -dtydpz ;
  p.H(4,momindex+0) = -dqopdpx ;
  p.H(4,momindex+1) = -dqopdpy ;
  p.H(4,momindex+2) = -dqopdpz ;

  if(vtxverbose>=5)std::cout<<"RecoTrack::projectRecoConstraint() covariance"<<std::endl;
  // copy the covariance matrix
  for(int row=0; row<dimM(); row++)
    for(int col=0; col<dimM()/*=row*/; col++)
      p.Vfast(row,col) = m_V(row,col) ;

  if(vtxverbose>6){
    std::cout<<"RecoTrack::projectRecoConstraint(): projection is:"<<posindex<<std::endl;
    std::cout<<"r "; p.r().Print();
    std::cout<<"V "; p.V().Print();
    std::cout<<"H "; RhoCalculationTools::PrintMatrix(p.H());
    }
  if(vtxverbose>=5)std::cout<<"RecoTrack::projectRecoConstraint() finished"<<std::endl;
  return status ;

}

//  ErrCode
//  DecayTreeFitter::RecoTrack::projectRecoConstraint(const FitParams& fitparams, Projection& p) const
//  {
//    ErrCode status ;
//    assert(m_cached) ;
//    (const_cast<RecoTrack*>(this))->updCache(fitparams) ;
//
//    int posindex = mother()->posIndex() ;
//    TVector3 motherpos(fitparams->par()(posindex+1),
//			      fitparams->par()(posindex+2),
//			      fitparams->par()(posindex+3)) ;
//
//    int momindex = momIndex() ;
//    double px = fitparams->par()(momindex+1) ;
//    double py = fitparams->par()(momindex+2) ;
//    double pz = fitparams->par()(momindex+3) ;motherpos.z()
//    double mom2 = px*px+py*py+pz*pz ;
//    double mom = std::sqrt(mom2) ;
//    double tx  = px/pz ;
//    double ty  = py/pz ;
//    double qop = charge() / mom ;
//
//    double dtxdpx = 1/pz ;
//    double dtxdpz = -tx/pz ;
//    double dtydpy = 1/pz ;
//    double dtydpz = -ty/pz ;
//
//    double dqopdpx = - qop * px/mom2 ;
//    double dqopdpy = - qop * py/mom2 ;
//    double dqopdpz = - qop * pz/mom2 ;
//
//    // fill the residual
//    double dz  = m_state.z() - motherpos.z() ;
//    const Gaudi::Vector5& m_m = m_state.stateVector() ;
//    const Gaudi::SymMatrix5x5& m_V = m_state.covariance() ;
//    p.r(0) = m_m(0) - motherpos.x() - dz*tx ;
//    p.r(1) = m_m(1) - motherpos.y() - dz*ty ;
//    p.r(2) = m_m(2) - tx ;
//    p.r(3) = m_m(3) - ty ;
//    p.r(4) = m_m(4) - qop ;
//
//    // compute the projection matrix
//    // derivative of position to position of mother
//    p.H(0,posindex+0) = -1 ;
//    p.H(1,posindex+1) = -1 ;
//    p.H(0,posindex+2) = tx ;
//    p.H(1,posindex+2) = ty ;
//
//    // derivative of position to 3-momentum
//    p.H(0,momindex+0) = -dz * dtxdpx ;
//    p.H(0,momindex+2) = -dz * dtxdpz ;
//    p.H(1,momindex+1) = -dz * dtydpy ;
//    p.H(1,momindex+2) = -dz * dtydpz ;
//
//    // derivative of slopes/qop to 3-momentum
//    p.H(2,momindex+0) = -dtxdpx ;
//    p.H(2,momindex+2) = -dtxdpz ;
//    p.H(3,momindex+1) = -dtydpy ;
//    p.H(3,momindex+2) = -dtydpz ;
//    p.H(4,momindex+0) = -dqopdpx ;
//    p.H(4,momindex+1) = -dqopdpy ;
//    p.H(4,momindex+2) = -dqopdpz ;
//
//    // copy the covariance matrix
//    for(int row=0; row<5; row++)
//      for(int col=0; col<5/*=row*/; col++)
//	      p.Vfast(row,col) = m_V(row,col) ;
//
//    return status ;
//  }
