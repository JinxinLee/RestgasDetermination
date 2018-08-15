// ******************************************************
// DecayTreeFitter Package
// We thank the original author Wouter Hulsbergen 
// (BaBar, LHCb) for providing the sources. 
// http://arxiv.org/abs/physics/0503191v1 (2005)
// Adaptation & Development for PANDA: Ralf Kliemt (2015)
// ******************************************************
#include <iomanip>
#include <float.h>

//#include "Event/Particle.h"
//#include "LoKi/ParticleProperties.h"

#include "ParticleBase.h"
#include "InternalParticle.h"
#include "RecoComposite.h"
#include "RecoResonance.h"
#include "RecoTrack.h"
#include "RecoPhoton.h"
#include "Resonance.h"
#include "MissingParticle.h"
#include "FitParams.h"
#include "Configuration.h"

#include "TParticlePDG.h"
#include "TVectorD.h"
#include "TMatrixDSym.h"
#include "TMath.h"

#include "RhoCalculationTools.h"

#include "PndPidCandidate.h"

//#include "Kernel/IParticlePropertySvc.h"
//#include "Kernel/ParticleProperty.h"
//#include "GaudiKernel/Service.h"

using namespace DecayTreeFitter;

ClassImp(ParticleBase);

//template<class T>
//inline T sqr(T x) { return x*x ; }

int vtxverbose=0 ;

DecayTreeFitter::ParticleBase::ParticleBase(RhoCandidate* aParticle, const ParticleBase* aMother)
: m_particle(aParticle),m_mother(aMother),
m_prop(aParticle->PdtEntry()),
m_index(0),m_pdtMass(0),m_pdtWidth(0),m_pdtCLifeTime(0),m_charge(0),
m_name("Unknown"), m_hasMassConstraint(false)
{
  if( m_prop ) {
    m_pdtMass      = m_prop->Mass() ;
    m_pdtWidth     = m_prop->Width() ;
    m_pdtCLifeTime = TMath::C() * m_prop->Lifetime() * 100; // Units: ctau[cm] = C[m/s] * tau[s]
    double fltcharge = m_prop->Charge()/3. ;// TParticlePDG holds charg in |e|/3
    m_charge = fltcharge < 0 ? int(fltcharge-0.5) : int(fltcharge+0.5) ;
    //m_charge = aParticle->Charge()>0 ? 1 : (aParticle->Charge()<0 ? -1 : 0) ;
    m_name = m_prop->GetName() ;//FIXME Does root object name do the job?
  } else {
    m_charge = aParticle->Charge()>0 ? 1 : (aParticle->Charge()<0 ? -1 : 0) ;
  }
}


DecayTreeFitter::ParticleBase::ParticleBase(const std::string& aname)
: m_particle(0),m_mother(0),
m_prop(0),
m_index(0),m_pdtMass(0),m_pdtWidth(0),m_pdtCLifeTime(0),m_charge(0),
m_name(aname), m_hasMassConstraint(false)
{
}

DecayTreeFitter::ParticleBase::~ParticleBase()
{
  for(daucontainer::iterator it = m_daughters.begin() ;
      it != m_daughters.end() ; ++it)
    if(*it) delete *it ;
  m_daughters.clear() ;
}

void DecayTreeFitter::ParticleBase::updateIndex(int& offset)
{
  if(vtxverbose>=5){std::cout<<"ParticleBase::updateIndex() start"<<std::endl;}
  // first the daughters
  for(const_iterator it = begin(); it!= end(); ++it) (*it)->updateIndex(offset) ;
  // now the real work
  m_index = offset ;
  offset += dim() ;
  if(vtxverbose>=5){std::cout<<"ParticleBase::updateIndex() end"<<std::endl;}
}

ParticleBase* DecayTreeFitter::ParticleBase::addDaughter(RhoCandidate* cand, const Configuration& config)
{
  m_daughters.push_back( DecayTreeFitter::ParticleBase::createParticle(cand,this,config) ) ;
  return m_daughters.back() ;
}

void DecayTreeFitter::ParticleBase::removeDaughter(const ParticleBase* pb)
{
  daucontainer::iterator it = std::find(m_daughters.begin(),m_daughters.end(),pb) ;
  if(it != m_daughters.end() ) {
    ParticleBase* _dp = *it ;
    m_daughters.erase(it) ;
    if(_dp) delete _dp ;
  } else {
    std::cout << "ERROR: cannot remove particle, because not found ..." << std::endl ;
  }
}

ParticleBase*
DecayTreeFitter::ParticleBase::createParticle(RhoCandidate* particle,
                            const ParticleBase* mother,
                            const Configuration& config)
{
  // This routine interpretes a beta candidate as one of the
  // 'Particles' used by the fitter.

  const TParticlePDG* prop = particle->PdtEntry();

  if(vtxverbose>=5)
    std::cout << "DecayTreeFitter::ParticleBase::createParticle from     " <<particle->PdgCode() << " | " << particle->Uid() << std::endl ;
  ParticleBase* rc=0 ;
  //bool bsconstraint = false ;

  // We refit invalid fits, kinematic fits and composites with beamspot
  // constraint if not at head of tree.
  bool validfit = particle->IsLocked();//TODO needed? && particle.endVertex() != 0 ;
  bool iscomposite = (particle->NDaughters()>0) ;
  bool isresonance = iscomposite && prop && isAResonance(prop) ;

  if(!mother) { // 'head of tree' particles
                //if ( bsconstraint )
                //rc = new InteractionPoint(particle) ;
                //else

    if( iscomposite )
    { if(vtxverbose>=2) std::cout<<" H "; if(vtxverbose>=5) std::cout<<std::endl;
      rc = new InternalParticle(particle,0,config) ; // still need proper head-of-tree class
    }
    else {
      std::cout << "DecayTreeFitter::ParticleBase::createParticle: You are fitting a decay tree that exists of "
        << "a single, non-composite particle and which does not have a beamconstraint."
        << "I do not understand what you want me to do with this." << std::endl ;
      std::cout<<" X ";
      rc = new InternalParticle(particle,0,config) ; // still need proper head-of-tree class
    }
  } else if( !iscomposite ) { // external particles (i.e. tracks/neutrals)
    bool hasrecocand = ( NULL !=particle->GetRecoCandidate() );
    bool ischarged = ( fabs(particle->Charge())>0 );
    //bool hastrack = proto && (proto->GetTrackIndex() >= 0) ;
    //bool hascalo  = proto && (proto->GetEmcNumberOfCrystals() > 0) ;
    //std::cout<<" particle="<<particle<<" proto="<<proto<<" ntrk="<<proto->GetTrackIndex()<<" ncry"<<proto->GetEmcNumberOfCrystals()<<std::endl;
    if( hasrecocand )
    {
      if( ischarged )
      { if(vtxverbose>=2) std::cout<<" T "; if(vtxverbose>=5) std::cout<<std::endl;
        rc = new RecoTrack(particle,mother,config) ;  // reconstructed track
      } else
      { if(vtxverbose>=2) std::cout<<" P "; if(vtxverbose>=5) std::cout<<std::endl;
        rc = new RecoPhoton(particle,mother) ; // reconstructed photon
      }
    } else if( validfit ) {  // fitted composites w/o daughters?
      if( isresonance )
      { if(vtxverbose>=2) std::cout<<" RF "; if(vtxverbose>=5) std::cout<<std::endl;
        rc = new RecoResonance(particle,mother) ;
      } else
      { if(vtxverbose>=2) std::cout<<" CF "; if(vtxverbose>=5) std::cout<<std::endl;
        rc = new RecoComposite(particle,mother) ;
      }
    } else { // missing particle!
      if(vtxverbose>=2) std::cout<<" M "; if(vtxverbose>=5) std::cout<<std::endl;
      rc = new MissingParticle(particle,mother) ;
    }
  } else { // 'internal' particles
    if( validfit /*|| isconversion*/ ) {  // fitted composites
      if( isresonance ) { 
        if(vtxverbose>=2) std::cout<<" Rf "; if(vtxverbose>=5) std::cout<<std::endl;
        rc = new RecoResonance(particle,mother) ;
      } else { 
        if(vtxverbose>=2) std::cout<<" Cf "; if(vtxverbose>=5) std::cout<<std::endl;
        rc = new RecoComposite(particle,mother) ;
      }
    } else {         // unfited composites
      if( isresonance ) { 
        if(vtxverbose>=2) std::cout<<" R "; if(vtxverbose>=5) std::cout<<std::endl;
        rc = new Resonance(particle,mother,config) ;
      } else { 
        if(vtxverbose>=2) std::cout<<" I "; if(vtxverbose>=5) std::cout<<std::endl;
        rc = new InternalParticle(particle,mother,config) ;
      }
    }
  }
  if(vtxverbose>=5)
    std::cout << "DecayTreeFitter::ParticleBase::createParticle finished " <<particle->PdgCode() << " | " << particle->Uid() << std::endl ;

  if(vtxverbose>=2)
    std::cout << "DecayTreeFitter::ParticleBase::createParticle returns type=" << rc->type()
    << " index=" << rc->index() << " with name \""<< rc->name() << "\""<<std::endl ;
  return rc ;
}

bool
DecayTreeFitter::ParticleBase::isAResonance(const TParticlePDG* prop) {
  bool rc = false ;
  switch(prop->PdgCode()) {
    case 11: // bremstrahlung is treated as a resonance
    case -11:
      rc = true ;
      break ;
    //case 22: // conversions are not treated as a resonance
    //case 310: // K shorts count as "stable" in PDT Table
      //rc = false;
      //break ;
    default: // this should take care of the pi0
      rc = (prop->Lifetime()>0) && (100.*TMath::C()*prop->Lifetime() < 0.0001); //[cm]
      if(rc) break;
      if(vtxverbose>4)std::cout<<"ParticleBase::isAResonance l."<<__LINE__<<": ctau="<<100.*TMath::C()*prop->Lifetime()<<" cm"<<std::endl;
      if(prop->Stable()) return false;
      //rc = prop.ctau() < 0.001*Gaudi::Units::mm ;
      break;
  }
  return rc ;
}

void DecayTreeFitter::ParticleBase::collectVertexDaughters( daucontainer& particles, int posindex )
{
  // collect all particles emitted from vertex with position posindex
  if(vtxverbose>=3) {
    std::cout << "DecayTreeFitter::ParticleBase::collectVertexDaughters " << posindex << std::endl ;
  }
  //skip: head of tree, particles from different vertex, resonances
  if( mother() && mother()->posIndex() == posindex && type()!=kRecoResonance && type()!=kResonance )
  {
    particles.push_back( this ) ;
    if(vtxverbose>=3) {
      std::cout << "DecayTreeFitter::ParticleBase::collectVertexDaughters - added a particle "<<name()<<" to vertex " << posindex << std::endl ;
    }
  }
  for( daucontainer::const_iterator idau = daughters().begin() ;
      idau != daughters().end() ; ++idau )
    (*idau)->collectVertexDaughters(particles,posindex ) ; //FIXME: RK Caution here!!
    //collectVertexDaughters(particles,posindex ) ;
}

ErrCode
DecayTreeFitter::ParticleBase::initCov(FitParams* fitparams) const
{
  ErrCode status ;

  if(vtxverbose>=2) {
    std::cout << "DecayTreeFitter::ParticleBase::initCov for " << name() << std::endl ;
  }

  // position
  int posindex = posIndex() ;
  if( posindex>=0 ) {
    const double sigx   = 10; // * Gaudi::Units::cm ;
    const double sigy   = 10; // * Gaudi::Units::cm ;
    const double sigz   = 50; // * Gaudi::Units::cm ;
    fitparams->cov()(posindex+0,posindex+0) = sigx*sigx ;
    fitparams->cov()(posindex+1,posindex+1) = sigy*sigy ;
    fitparams->cov()(posindex+2,posindex+2) = sigz*sigz ;
  }

  // momentum
  int momindex = momIndex() ;
  if(momindex>=0) {
    // TODO: calo at high energy?
    const double sigmom = 10; // * Gaudi::Units::GeV ; // GeV
    int maxrow = hasEnergy() ? 4 : 3 ;
    for(int row=momindex; row<momindex+maxrow; row++)
      fitparams->cov()(row,row) = sigmom*sigmom ;
  }

  // lifetime
  int lenindex = lenIndex() ;
  if(lenindex>=0) {
    const double sigz   = 50; // * Gaudi::Units::cm ;
    fitparams->cov()(lenindex,lenindex) = sigz*sigz ;
  }

  for(daucontainer::const_iterator it = m_daughters.begin() ;
      it != m_daughters.end() ; ++it)
    status |= (*it)->initCov(fitparams) ;
  return status ;
}


std::string DecayTreeFitter::ParticleBase::parname(int thisindex) const
{
  std::string rc = name() ;
  switch(thisindex) {
    case 0: rc += " x  " ; break ;
    case 1: rc += " y  " ; break ;
    case 2: rc += " z  " ; break ;
    case 3: rc += " len" ; break ;
    case 4: rc += " px " ; break ;
    case 5: rc += " py " ; break ;
    case 6: rc += " pz " ; break ;
    case 7: rc += " E  " ; break ;
    default: ;
  }
  return rc ;
}

void
DecayTreeFitter::ParticleBase::print(const FitParams* fitpar) const
{
  std::cout << std::setw(5) << "[" << type() << "]" << std::setw(15) << std::flush << name().c_str()
  << std::setw(15)<< " val"  << std::setw(15)<< "err" << std::setw(15) << "sigma^2"<< std::endl ;
  std::cout << std::setprecision(5) ;
  for(int i=0; i<dim(); i++) {
    int theindex = index()+i ;
    std::cout << std::setw(2) << theindex << " "
    << std::setw(20) << parname(i).c_str()
    << std::setw(15) << fitpar->par()(theindex)
    << std::setw(15) << sqrt(fitpar->cov()(theindex,theindex))
    << std::setw(15) << fitpar->cov()(theindex,theindex) <<std::endl ;
  }
  if( hasEnergy() ) {
    int momindex = momIndex() ;
    double E  = fitpar->par()(momindex+3) ;
    double px = fitpar->par()(momindex+0) ;
    double py = fitpar->par()(momindex+1) ;
    double pz = fitpar->par()(momindex+2) ;
    double mass2 = E*E-px*px-py*py-pz*pz ;
    double mass = mass2>0 ? sqrt(mass2) : -sqrt(-mass2) ;

    // TODO this does not work? fitpar->cov()).GetSub(momindex+0,momindex+3)
    TMatrixDSym cov = fitpar->cov().GetSub(momindex+0,momindex+3,momindex+0,momindex+3);
    //HepSymMatrix cov = fitpar->cov().sub(momindex+0,momindex+3) ;
    TVectorD G(4) ; //was at G(4,0) .. ??why
    G(0) = -px/mass ;
    G(1) = -py/mass ;
    G(2) = -pz/mass ;
    G(3) =   E/mass ;
    double massvar = cov.Similarity(G) ;
    std::cout << std::setw(2)<< "-"<<" " << std::setw(20) << "mass: "
    << std::setw(15) << mass
    << std::setw(15) << sqrt(massvar)
    << std::setw(15) << massvar << std::endl ;
  }

  for(daucontainer::const_iterator it = m_daughters.begin() ;
      it != m_daughters.end() ; ++it)
    (*it)->print(fitpar) ;

}

const
ParticleBase* DecayTreeFitter::ParticleBase::locate(RhoCandidate* abc) const
{
  // does there exist an 'iscloneof' in lhcb?
  const ParticleBase* rc = m_particle == abc ? this : 0 ;
  for(daucontainer::const_iterator it = m_daughters.begin() ;
      !rc && it != m_daughters.end(); ++it)
    rc = (*it)->locate(abc) ;
  return rc ;
}

//  void DecayTreeFitter::ParticleBase::locate( RhoCandidateID& pid,
//                             ParticleContainer& result )
//  {
//    /// @attention Comparison by ABSPID!
//    if ( m_particle &&
//         m_particle->particleID().abspid() ==pid.abspid() )
//    {  result.push_back(this) ; } ;
//    //
//    for( daucontainer::iterator it = m_daughters.begin() ;
//         it != m_daughters.end(); ++it)
//    { (*it)-> locate( pid, result ) ; }
//    //
//  }

void DecayTreeFitter::ParticleBase::retrieveIndexMap(indexmap& anindexmap) const
{

  anindexmap.push_back(std::pair<const ParticleBase*,int>(this,index())) ;
  for(daucontainer::const_iterator it = m_daughters.begin() ;
      it != m_daughters.end() ; ++it)
    (*it)->retrieveIndexMap(anindexmap) ;
}


ErrCode DecayTreeFitter::ParticleBase::projectGeoConstraint(const FitParams* fitparams,
                                          Projection& p) const
{
  // implements the constraint

  //  vec{x}_decay = vec{x}_production + decaylength * vec{p} / p
  int posindexmother = mother()->posIndex() ;
  int posindex = posIndex();
  int lenindex = lenIndex() ;
  int momindex = momIndex() ;
  assert(posindexmother>=0 && posindex>=0 && lenindex>=0 && momindex>=0) ;


  // size of momentum
  double px = fitparams->par()(momindex+0) ;
  double py = fitparams->par()(momindex+1) ;
  double pz = fitparams->par()(momindex+2) ;
  double p2 = px*px+py*py+pz*pz ;
  double mom  = std::sqrt(p2) ;
  
  double dvx = fitparams->par()(posindexmother+0) - fitparams->par()(posindex+0);
  double dvy = fitparams->par()(posindexmother+1) - fitparams->par()(posindex+1);
  double dvz = fitparams->par()(posindexmother+2) - fitparams->par()(posindex+2);
  double len=dvx*dvx+dvy*dvy+dvz*dvz;
  len=std::sqrt(len);
  
  // lineair approximation is fine for now
  for(int row=0; row<3; row++) {
    double posxmother = fitparams->par()(posindexmother+row) ;
    double posx       = fitparams->par()(posindex+row) ;
    double momx       = fitparams->par()(momindex+row) ;
    p.r(row) = posxmother - posx + len*momx/mom ;
    p.H(row,posindexmother+row) = 1 ;
    p.H(row,posindex+row)       = -1 ;
    p.H(row,lenindex)         = momx/mom ;
  }
  // still need these as well
  p.H(0,momindex+0)  = len/mom*( 1 - px*px/p2 ) ;
  p.H(0,momindex+1)  = len/mom*( 0 - px*py/p2 ) ;
  p.H(0,momindex+2)  = len/mom*( 0 - px*pz/p2 ) ;
  p.H(1,momindex+0)  = len/mom*( 0 - py*px/p2 ) ;
  p.H(1,momindex+1)  = len/mom*( 1 - py*py/p2 ) ;
  p.H(1,momindex+2)  = len/mom*( 0 - py*pz/p2 ) ;
  p.H(2,momindex+0)  = len/mom*( 0 - pz*px/p2 ) ;
  p.H(2,momindex+1)  = len/mom*( 0 - pz*py/p2 ) ;
  p.H(2,momindex+2)  = len/mom*( 1 - pz*pz/p2 ) ;
///##################################
  //if( charge()!=0 ) 
  //{
    //double lambda = bFieldOverC() * charge() ;
    //double tau = fitparams->par(lenIndex());
    //double pt = sqrt(px*px+py*py) ;
    //const double posprecision = 1e-4 ; // 1mu
    //if( fabs(pt*lambda*tau*tau) > posprecision ) 
    //{
      //// use the helix, but as if it were a 'correction'
      //double sinlt = sin(lambda*tau) ;
      //double coslt = cos(lambda*tau) ;
      //double px1 = px*coslt - py*sinlt ;
      //double py1 = py*coslt + px*sinlt ;
     
      //p.r(0) += -tau*px + (py1-py)/lambda ;
      //p.r(1) += -tau*py - (px1-px)/lambda ;
     
      //p.H(0,lenindex+0) += -px + px1 ;
      //p.H(0,momindex+0) += -tau + sinlt/lambda ;
      //p.H(0,momindex+1) +=        (coslt-1)/lambda ;
      //p.H(1,lenindex+0) += -py + py1 ;
      //p.H(1,momindex+0) +=      - (coslt-1)/lambda ;
      //p.H(1,momindex+1) += -tau + sinlt/lambda ;

      ////if(vtxverbose>=2)
      //std::cout << "Using helix for position of particle: " << name().c_str() << " "
           //<< lambda << " " << lambda*tau
           //<< "  delta-x,y: " << -tau*px + (py1-py)/lambda << "  "
           //<< -tau*py - (px1-px)/lambda << std::endl ;
    //}
  //}
///##################################

  //p.setParticle( *mother() ) ;  // adds geoconstraint chi2 to the balance of the mother particle. Why?
  if(vtxverbose>6){
    std::cout<<"ParticleBase::projectConstraint(): projection is:"<<std::endl;
    std::cout<<"r "; p.r().Print();
    std::cout<<"V "; p.V().Print();
    std::cout<<"H "; RhoCalculationTools::PrintMatrix(p.H());
    }
  return ErrCode::success ;
}

ErrCode DecayTreeFitter::ParticleBase::projectMassConstraint(const FitParams* fitparams,
                                            Projection& p) const
{
  if(vtxverbose>6){std::cout<<"ParticleBase::projectMassConstraint():"<<std::endl;}
  double mass  = pdtMass() ;
  double mass2 = mass*mass ;
  int momindex = momIndex() ;

  // initialize the value
  double px = fitparams->par()(momindex+0) ;
  double py = fitparams->par()(momindex+1) ;
  double pz = fitparams->par()(momindex+2) ;
  double E  = fitparams->par()(momindex+3) ;
  if(vtxverbose>6){std::cout<<"px="<<px<<", py="<<py<<", pz="<<pz<<", E="<<E<<", pdtmass="<<mass<<std::endl;}
  p.r(0) = E*E-px*px-py*py-pz*pz-mass2 ;

  // calculate the projection matrix
  p.H(0,momindex+0) = -2.0*px ;
  p.H(0,momindex+1) = -2.0*py ;
  p.H(0,momindex+2) = -2.0*pz ;
  p.H(0,momindex+3) =  2.0*E ;

  // set the variance in the residual
  double width = pdtWidth() ;
  p.Vfast(0,0) = 4*mass*mass*width*width ;

  return ErrCode::success ;
}

ErrCode
DecayTreeFitter::ParticleBase::projectConstraint(Constraint::Type atype, const FitParams*, Projection&) const
{
  std::cout << "ParticleBase::projectConstraint(): no method to project this constaint: "
  << name() << " " << type() << " " << atype << std::endl ;
  return ErrCode::badsetup ;
}

//   double DecayTreeFitter::ParticleBase::bFieldOverC()
//   {
//     static const BField* bfield =  gblEnv->getTrk()->magneticField();
//     static const double Bz = BField::cmTeslaToGeVc*bfield->bFieldNominal() ;
//     return Bz ;
//   }

ErrCode
DecayTreeFitter::ParticleBase::initTau(FitParams* fitparams) const
{
  int lenindex = lenIndex() ;
  if(lenindex>=0 && hasPosition() ) {
    const ParticleBase* amother = mother() ;
    int momposindex = amother ? amother->posIndex() : -1 ;
    int posindex = posIndex() ;
    int momindex = momIndex() ;
    assert(momposindex>=0) ; // check code logic: no mother -> no tau
                            //assert(fitparams->par(momposindex+0)!=0 ||fitparams->par(momposindex+1)!=0
                            //      ||fitparams->par(momposindex+2)!=0) ; // mother must be initialized

    TVector3 dX,mom ;
    double mom2(0) ;
    for(int irow=0; irow<3; irow++) {
      dX(irow)  = fitparams->par(posindex+irow) - fitparams->par(momposindex+irow) ;
      double px = fitparams->par(momindex+irow) ;
      mom(irow) = px ;
      mom2 += px*px ;
    }
    double tau = dX.Dot(mom)/mom2 ;
    // we don't like 0 and we don't like very negative values
    if( tau==0 ) tau=pdtTau() ;
    //tau = tau==0 ? pdtTau() : std::max(tau,-pdtTau()) ;
    fitparams->par(lenindex) = tau ;
  }
  return ErrCode::success ;
}

double DecayTreeFitter::ParticleBase::chiSquareD(const FitParams* fitparams) const
{
  double rc = 0;
  for(daucontainer::const_iterator it = m_daughters.begin() ;
      it != m_daughters.end(); ++it)
    rc += (*it)->chiSquareD(fitparams) ;
  return rc ;
}

int DecayTreeFitter::ParticleBase::nFinalChargedCandidates() const {
  int rc=0;
  for(daucontainer::const_iterator it = m_daughters.begin() ;
      it != m_daughters.end() ; ++it)
    rc +=(*it)->nFinalChargedCandidates() ;
  return rc ;
}

ChiSquare DecayTreeFitter::ParticleBase::chiSquare( const FitParams* fitparams ) const
{
  ChiSquare chi2 ;
  // add contribution from daughters
  for(daucontainer::const_iterator it = m_daughters.begin() ;
      it != m_daughters.end() ; ++it) {
    chi2 += (*it)->chiSquare(fitparams) ;
  }
  // add own chisquare, adjust for number of parameters
  chi2 += fitparams->chiSquare( *this ) ;
  chi2 += ChiSquare( 0, -dim() ) ;
  return chi2 ;
}

double DecayTreeFitter::ParticleBase::bFieldOverC() 
{ 
  TVector3 pos(0.,0.,0.); //TODO get a sensible position, but let's assume zero first...
  return RhoCalculationTools::GetBz ( pos ) / TMath::C() ; 
}


//backup "old" representation
////ErrCode DecayTreeFitter::ParticleBase::projectGeoConstraint(const FitParams* fitparams,
                                          ////Projection& p) const
////{
  ////// implements the constraint

  //////  vec{x}_decay = vec{x}_production + decaylength * vec{p} / p
  ////int posindexmother = mother()->posIndex() ;
  ////int posindex = posIndex();
  ////int lenindex = lenIndex() ;
  ////int momindex = momIndex() ;
  ////assert(posindexmother>=0 && posindex>=0 && lenindex>=0 && momindex>=0) ;

  ////// decay length
  ////double len =  fitparams->par()(lenindex) ;

  ////// size of momentum
  ////double px = fitparams->par()(momindex+0) ;
  ////double py = fitparams->par()(momindex+1) ;
  ////double pz = fitparams->par()(momindex+2) ;
  ////double p2 = px*px+py*py+pz*pz ;
  ////double mom  = std::sqrt(p2) ;

  ////// lineair approximation is fine for now
  ////for(int row=0; row<3; row++) {
    ////double posxmother = fitparams->par()(posindexmother+row) ;
    ////double posx       = fitparams->par()(posindex+row) ;
    ////double momx       = fitparams->par()(momindex+row) ;
    ////p.r(row) = posxmother - posx + len*momx/mom ;
    ////p.H(row,posindexmother+row) = 1 ;
    ////p.H(row,posindex+row)       = -1 ;
    ////p.H(row,lenindex)         = momx/mom ;
  ////}
  ////// still need these as well
  ////p.H(0,momindex+0)  = len/mom*( 1 - px*px/p2 ) ;
  ////p.H(0,momindex+1)  = len/mom*( 0 - px*py/p2 ) ;
  ////p.H(0,momindex+2)  = len/mom*( 0 - px*pz/p2 ) ;
  ////p.H(1,momindex+0)  = len/mom*( 0 - py*px/p2 ) ;
  ////p.H(1,momindex+1)  = len/mom*( 1 - py*py/p2 ) ;
  ////p.H(1,momindex+2)  = len/mom*( 0 - py*pz/p2 ) ;
  ////p.H(2,momindex+0)  = len/mom*( 0 - pz*px/p2 ) ;
  ////p.H(2,momindex+1)  = len/mom*( 0 - pz*py/p2 ) ;
  ////p.H(2,momindex+2)  = len/mom*( 1 - pz*pz/p2 ) ;
///////##################################
  //////if( charge()!=0 ) 
  //////{
    //////double lambda = bFieldOverC() * charge() ;
    //////double tau = fitparams->par(lenIndex());
    //////double pt = sqrt(px*px+py*py) ;
    //////const double posprecision = 1e-4 ; // 1mu
    //////if( fabs(pt*lambda*tau*tau) > posprecision ) 
    //////{
      //////// use the helix, but as if it were a 'correction'
      //////double sinlt = sin(lambda*tau) ;
      //////double coslt = cos(lambda*tau) ;
      //////double px1 = px*coslt - py*sinlt ;
      //////double py1 = py*coslt + px*sinlt ;
     
      //////p.r(0) += -tau*px + (py1-py)/lambda ;
      //////p.r(1) += -tau*py - (px1-px)/lambda ;
     
      //////p.H(0,lenindex+0) += -px + px1 ;
      //////p.H(0,momindex+0) += -tau + sinlt/lambda ;
      //////p.H(0,momindex+1) +=        (coslt-1)/lambda ;
      //////p.H(1,lenindex+0) += -py + py1 ;
      //////p.H(1,momindex+0) +=      - (coslt-1)/lambda ;
      //////p.H(1,momindex+1) += -tau + sinlt/lambda ;

      ////////if(vtxverbose>=2)
      //////std::cout << "Using helix for position of particle: " << name().c_str() << " "
           //////<< lambda << " " << lambda*tau
           //////<< "  delta-x,y: " << -tau*px + (py1-py)/lambda << "  "
           //////<< -tau*py - (px1-px)/lambda << std::endl ;
    //////}
  //////}
///////##################################

  //////p.setParticle( *mother() ) ;  // adds geoconstraint chi2 to the balance of the mother particle. Why?
  ////if(vtxverbose>6){
    ////std::cout<<"ParticleBase::projectConstraint(): projection is:"<<std::endl;
    ////std::cout<<"r "; p.r().Print();
    ////std::cout<<"V "; p.V().Print();
    ////std::cout<<"H "; RhoCalculationTools::PrintMatrix(p.H());
    ////}
  ////return ErrCode::success ;
////}
