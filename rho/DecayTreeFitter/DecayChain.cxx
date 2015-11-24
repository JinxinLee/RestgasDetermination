// ******************************************************
// DecayTreeFitter Package
// We thank the original author Wouter Hulsbergen 
// (BaBar, LHCb) for providing the sources. 
// http://arxiv.org/abs/physics/0503191v1 (2005)
// Adaptation & Development for PANDA: Ralf Kliemt (2015)
// ******************************************************
#include <algorithm>
#include "FitParams.h"
#include "ParticleBase.h"
#include "InteractionPoint.h"
#include "DecayChain.h"

using namespace DecayTreeFitter;

ClassImp(DecayChain);

extern int vtxverbose ;

void
DecayTreeFitter::DecayChain::printConstraints(std::ostream& os) const
{
  os << "Constraints in decay tree: " << m_constraintlist.size() << std::endl ;
  for( ParticleBase::constraintlist::const_iterator
      it = m_constraintlist.begin() ;
      it != m_constraintlist.end(); ++it)
    it->print(os) ;
}

DecayTreeFitter::DecayChain::DecayChain(RhoCandidate* bc, const Configuration& config)
: m_dim(0),m_mother(0),m_isOwner(true)
{
  //std::cout<<"DecayTreeFitter::DecayChain::DecayChain(RhoCandidate* bc="<<bc<<")"<<std::endl;
  m_mother = (ParticleBase*) new InteractionPoint(bc,config) ;
  //m_mother = ParticleBase::createParticle(bc,0,config) ;
  //std::cout<<"DecayTreeFitter::DecayChain::DecayChain(RhoCandidate*) - updateIndex() now"<<std::endl;
  m_mother->updateIndex(m_dim) ;
  //std::cout<<"DecayTreeFitter::DecayChain::DecayChain(RhoCandidate*) - locate() now"<<std::endl;
  m_cand   = locate(bc) ;
  //std::cout<<"DecayTreeFitter::DecayChain::DecayChain(RhoCandidate*) - done"<<std::endl;
}

DecayTreeFitter::DecayChain::DecayChain(RhoCandidate* bc, const RhoVector3Err& pv, const Configuration& config)
: m_dim(0),m_mother(0),m_isOwner(true)
{
  //std::cout<<"DecayTreeFitter::DecayChain::DecayChain(RhoCandidate* bc="<<bc<<",RhoVector3Err v("<<pv.x()<<","<<pv.y()<<","<<pv.z()<<"))"<<std::endl;
  m_mother = (ParticleBase*) new InteractionPoint(pv,bc,config) ;
  //std::cout<<"DecayTreeFitter::DecayChain::DecayChain(RhoCandidate*,RhoVector3Err) - updateIndex() now"<<std::endl;
  m_mother->updateIndex(m_dim) ;
  //std::cout<<"DecayTreeFitter::DecayChain::DecayChain(RhoCandidate*,RhoVector3Err) - locate() now"<<std::endl;
  m_cand   = locate(bc) ;
  //std::cout<<"DecayTreeFitter::DecayChain::DecayChain(RhoCandidate*,RhoVector3Err) - done"<<std::endl;
}

DecayTreeFitter::DecayChain::DecayChain(RhoCandidate* bc, const RhoLorentzVectorErr& lv, const Configuration& config)
: m_dim(0),m_mother(0),m_isOwner(true)
{
  //std::cout<<"DecayTreeFitter::DecayChain::DecayChain(RhoCandidate* bc="<<bc<<",RhoLorentzVectorErr p("<<lv.X()<<","<<lv.Y()<<","<<lv.Z()<<","<<lv.E()<<"))"<<std::endl;
  m_mother = (ParticleBase*) new InteractionPoint(lv,bc,config) ;
  //std::cout<<"DecayTreeFitter::DecayChain::DecayChain(RhoCandidate*,RhoVector3Err) - updateIndex() now"<<std::endl;
  m_mother->updateIndex(m_dim) ;
  //std::cout<<"DecayTreeFitter::DecayChain::DecayChain(RhoCandidate*,RhoVector3Err) - locate() now"<<std::endl;
  m_cand   = locate(bc) ;
  //std::cout<<"DecayTreeFitter::DecayChain::DecayChain(RhoCandidate*,RhoVector3Err) - done"<<std::endl;
}

DecayTreeFitter::DecayChain::DecayChain(RhoCandidate* bc, const RhoLorentzVectorErr& lv, const RhoVector3Err& pv, const Configuration& config)
: m_dim(0),m_mother(0),m_isOwner(true)
{
  //std::cout<<"DecayTreeFitter::DecayChain::DecayChain(RhoCandidate* bc="<<bc<<",RhoLorentzVectorErr p("<<lv.X()<<","<<lv.Y()<<","<<lv.Z()<<","<<lv.E()<<")"<<",RhoVector3Err v("<<pv.x()<<","<<pv.y()<<","<<pv.z()<<"))"<<std::endl;
  m_mother = (ParticleBase*) new InteractionPoint(lv,pv,bc,config) ;
  //std::cout<<"DecayTreeFitter::DecayChain::DecayChain(RhoCandidate*,RhoVector3Err) - updateIndex() now"<<std::endl;
  m_mother->updateIndex(m_dim) ;
  //std::cout<<"DecayTreeFitter::DecayChain::DecayChain(RhoCandidate*,RhoVector3Err) - locate() now"<<std::endl;
  m_cand   = locate(bc) ;
  //std::cout<<"DecayTreeFitter::DecayChain::DecayChain(RhoCandidate*,RhoVector3Err) - done"<<std::endl;
}

DecayTreeFitter::DecayChain::~DecayChain()
{
  if(m_mother && m_isOwner) delete m_mother ;
}

void
DecayTreeFitter::DecayChain::initConstraintList()
{
  if( m_dim==0 ) m_mother->updateIndex(m_dim) ;
  m_constraintlist.clear() ;
  m_mother->addToConstraintList(m_constraintlist,0) ;

  // the order of the constraints is a rather delicate thing
  std::sort(m_constraintlist.begin(),m_constraintlist.end()) ;

  // merge all non-lineair constraints
  m_mergedconstraintlist.clear() ;
  if(false) {
    m_mergedconstraint = MergedConstraint() ;
    for( ParticleBase::constraintlist::iterator it =  m_constraintlist.begin() ;
        it != m_constraintlist.end(); ++it) {
      if( it->isLineair() ) m_mergedconstraintlist.push_back(&(*it)) ;
      else  m_mergedconstraint.push_back(&(*it)) ;
    }
    if( m_mergedconstraint.dim()>0 )
      m_mergedconstraintlist.push_back(&m_mergedconstraint) ;
  }
  if(vtxverbose>=2) {
    std::cout << "DecayTreeFitter::DecayChain::initConstraintList(): " << m_constraintlist.size() << " "
    << m_mergedconstraintlist.size()<< std::endl ;
    printConstraints() ;
  }
}

ErrCode
DecayTreeFitter::DecayChain::init(FitParams* par)
{
  if(vtxverbose>5){std::cout<<"DecayChain::init(): - start"<<std::endl;}
  ErrCode status ;
///FIXME Here we have some bad behavior: We're using automatic copy constructor, implicit copied objects etc. How to do cleanly? Pointers directly with delete/new?
  if( m_dim==0 ){
  if(vtxverbose>5){std::cout<<"DecayChain::init(): - update index"<<std::endl;}
     m_mother->updateIndex(m_dim) ;
  }
  //if( par.dim() != m_dim ) par = FitParams(m_dim) ;
  if( par->dim() != m_dim ){
    if(vtxverbose>5){std::cout<<"DecayChain::init(): - reset 1"<<std::endl;}
     par->reset(m_dim);
  }  else {
    // set everything to 0
    if(vtxverbose>5){std::cout<<"DecayChain::init(): - reset 2"<<std::endl;}
    par->resetPar() ;
    par->resetCov() ;
  }
  // let the mother do it
  if(vtxverbose>5){std::cout<<"DecayChain::init(): - initpar"<<std::endl;}
  status |= m_mother->initPar1(par) ;
  if(vtxverbose>5){std::cout<<"DecayChain::init(): - initcov"<<std::endl;}
  status |= m_mother->initCov(par) ;

  if(vtxverbose>=2){ std::cout << "status after init: "; status.Print(std::cout); std::cout<< std::endl ;}

  return status ;
}

ErrCode
DecayTreeFitter::DecayChain::filter(FitParams* par, bool firstpass)
{
  if(vtxverbose>=4){std::cout<<"DecayChain::filter(): - ok, let's do this! Btw. we're "<<( firstpass ? "" : "not ")<<"on first pass."<<std::endl;}
  ErrCode status ;
  if(m_constraintlist.empty()){ 
    if(vtxverbose>=4){std::cout<<"DecayChain::filter(): - init constrains"<<std::endl;}
    initConstraintList() ;
  }
  
  par->resetCov(1000) ; //FIXME: this scales the cov with a factor 1000. WHY???
  //par->resetCov(100) ; //FIXME: this scales the cov with a factor 1000. WHY???
  if(firstpass || !par->testCov()){
    status |= m_mother->initCov(par) ;
    if(vtxverbose>=3){std::cout<<"DecayChain::filter(): - initcov done"<<std::endl;}
  }

  if( vtxverbose>=4 || (vtxverbose>=2&&firstpass) ) {
    std::cout << "DecayTreeFitter::DecayChain::filter, after initialization: "  << std::endl ;
    m_mother->print(par) ;
  }

  FitParams* reference = new FitParams(*par) ;

  if(m_mergedconstraintlist.empty() ) {
    for( ParticleBase::constraintlist::const_iterator it = m_constraintlist.begin() ;
        it != m_constraintlist.end(); ++it) {
      status |= it->filter(par,reference) ;
      if( vtxverbose>=2 && status.failure() ) {
        std::cout << "DecayChain::filter(): status is failure after parsing constraint: " ;
        it->print() ;
      }
    }
  } else {
    for( std::vector<Constraint*>::const_iterator it = m_mergedconstraintlist.begin() ;
        it != m_mergedconstraintlist.end(); ++it) {
      status |= (*it)->filter(par,reference) ;
      if( vtxverbose>=2 && status.failure() ) {
        std::cout << "DecayChain::filter(): status is failure after parsing constraint: " ;
        (*it)->print() ;
      }
    }
  }

  if(vtxverbose>=3) {std::cout << "DecayChain::filter: status = "; status.Print(std::cout); std::cout<< std::endl ;}
  return status ;
}

double
DecayTreeFitter::DecayChain::chiSquare(const FitParams* par) const
{
  return m_mother->chiSquareD(par) ;
}

const ParticleBase*
DecayTreeFitter::DecayChain::locate(RhoCandidate* bc) const {
  const ParticleBase* rc(0) ;
  // return m_mother->locate(bc) ;
  ParticleMap::const_iterator it = m_particleMap.find(bc) ;
  if( it == m_particleMap.end() ) {
    rc = m_mother->locate(bc) ;
    // used to add every candidate here, but something goes wrong
    // somewhere. now only cache pointers that we internally reference.
    if(rc) m_particleMap[rc->particle()] = rc ;
  } else {
    rc = it->second ;
  }
  return rc ;
}

void
DecayTreeFitter::DecayChain::setMassConstraint(RhoCandidate* bc, bool add)
{
  ParticleBase* part = const_cast<ParticleBase*>(locate(bc)) ;
  if(part && part->setMassConstraint(add)) {
    m_dim = 0 ;
    m_constraintlist.clear() ;
  }
}

void
DecayTreeFitter::DecayChain::setMassConstraint(RhoCandidate* bc, double mass)
{
  ParticleBase* part = const_cast<ParticleBase*>(locate(bc)) ;
  if(part) {
    part->setMassConstraint(mass) ;
    m_dim = 0 ;
    m_constraintlist.clear() ;
  }
}

// TODO is it a problem to not identify by "ID"?
//  void
//  DecayTreeFitter::DecayChain::setMassConstraint(RhoCandidateID& pid, bool add)
//  {
//    ParticleBase::ParticleContainer particles ;
//    m_mother->locate( pid, particles ) ;
//    bool changed(false) ;
//    for(ParticleBase::ParticleContainer::iterator it = particles.begin() ;
//        it != particles.end() ; ++it)
//    { changed |= (*it)->setMassConstraint(add) ; }
//    //
//    if (changed) {
//      m_dim = 0 ;
//      m_constraintlist.clear() ;
//    }
//  }
//
//  void
//  DecayTreeFitter::DecayChain::setMassConstraint(RhoCandidateID& pid , double mass )
//  {
//    ParticleBase::ParticleContainer particles ;
//    m_mother->locate( pid, particles ) ;
//    bool changed(false) ;
//    for(ParticleBase::ParticleContainer::iterator it = particles.begin() ;
//        it != particles.end() ; ++it)
//    { (*it)->setMassConstraint(mass) ;  changed = true ; }
//    //
//    if (changed) {
//      m_dim = 0 ;
//      m_constraintlist.clear() ;
//    }
//  }


int
DecayTreeFitter::DecayChain::index(RhoCandidate* bc) const {
  int rc = -1 ;
  const ParticleBase* base = locate(bc) ;
  if(base) rc = base->index() ;
  return rc ;
}

int
DecayTreeFitter::DecayChain::posIndex(RhoCandidate* bc) const {
  const ParticleBase* base = locate(bc) ;
  return base ? base->posIndex() : -1 ;
}

int
DecayTreeFitter::DecayChain::momIndex(RhoCandidate* bc) const {
  const ParticleBase* base = locate(bc) ;
  return base ? base->momIndex() : -1 ;
}

int
DecayTreeFitter::DecayChain::lenIndex(RhoCandidate* bc) const {
  const ParticleBase* base = locate(bc) ;
  return base ? base->lenIndex() : 0 ;
}

ChiSquare
DecayTreeFitter::DecayChain::chiSquare(RhoCandidate* bc, const FitParams* fitpars ) const {
  const ParticleBase* base = locate(bc) ;
  ChiSquare chisq ;
  if( base ) {
    chisq = base->chiSquare(fitpars) ;
    // uhh: need to subtract the lifetime dof if this particle has a
    // mother with a different vertex
    if( base->mother() && base->posIndex()>=0 &&
       base->posIndex() != base->mother()->posIndex() )
      chisq += ChiSquare(0,1) ;
  }
  return chisq ;
}
