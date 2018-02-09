// ******************************************************
// DecayTreeFitter Package
// We thank the original author Wouter Hulsbergen 
// (BaBar, LHCb) for providing the sources. 
// http://arxiv.org/abs/physics/0503191v1 (2005)
// Adaptation & Development for PANDA: Ralf Kliemt (2015)
// ******************************************************
// $Id: Fitter.cpp,v 1.16 2010/06/09 17:55:46 ibelyaev Exp $
// ============================================================================
#include <iomanip>
#include <stdio.h>
#include <sstream>
//#include <boost/foreach.hpp>

//#include "GaudiKernel/PhysicalConstants.h"
//#include "Event/Particle.h"

#include "Fitter.h"

#include "FitParams.h"
#include "DecayChain.h"
#include "ParticleBase.h"
#include "Configuration.h"

#include "RhoDoubleErr.h"
#include "RhoFactory.h"

using namespace DecayTreeFitter;

ClassImp(Fitter);

extern int vtxverbose ;
// ==========================================================================
// constructor from the particle (decay head)
// ==========================================================================
DecayTreeFitter::Fitter::Fitter
( RhoCandidate* bc           ,
 RecoTrackStateProvider*   aExtrapolator     ,
 int verbosity)
: RhoFitterBase(bc)
, m_particle   (bc)
, m_decaychain (0)
, m_fitparams  (0)
, m_status     (UnFitted)
//, m_chiSquare  (-1)
, m_niter      (-1)
, m_errCode    (0)
, m_extrapolator( aExtrapolator) 
{
  vtxverbose = verbosity;
  if(vtxverbose>5) std::cout<<"Fitter::Fitter() ("<<this<<") A - config"<<std::endl;
  Configuration config(aExtrapolator) ;
  // build the tree
  if(vtxverbose>5) std::cout<<"Fitter::Fitter() ("<<this<<") A - tree"<<std::endl;
  m_decaychain = new DecayChain(bc,config) ;
  // allocate the fit parameters
  if(vtxverbose>5) std::cout<<"Fitter::Fitter() ("<<this<<") A - Fitparams"<<std::endl;
  m_fitparams  = new FitParams(m_decaychain->dim()) ;
  if(vtxverbose>5) std::cout<<"Fitter::Fitter() ("<<this<<") A - done"<<std::endl;
}

// ==========================================================================
// constructor from the particle (decay head) and primary vertex
// ==========================================================================
DecayTreeFitter::Fitter::Fitter
( RhoCandidate*   bc           ,
 const RhoVector3Err& pv           ,
 RecoTrackStateProvider* aExtrapolator ,
 int verbosity)
: RhoFitterBase(bc)
, m_particle   ( bc )
, m_decaychain ( 0   )
, m_fitparams  ( 0   )
, m_status     ( UnFitted )
//, m_chiSquare  ( -1  )
, m_niter      ( -1  )
, m_errCode    (  0  )
//
, m_extrapolator ( aExtrapolator) 
//
{
  vtxverbose = verbosity;
  if(vtxverbose>5) std::cout<<"Fitter::Fitter() ("<<this<<") D - config"<<std::endl;
  Configuration config(aExtrapolator) ;
  if(vtxverbose>5) std::cout<<"Fitter::Fitter() ("<<this<<") D - tree  "<<std::flush;
  m_decaychain = new DecayChain(bc,pv,config) ;
  if(vtxverbose>5) std::cout<<"Fitter::Fitter() ("<<this<<") D - fitparams  "<<std::flush;
  m_fitparams  = new FitParams(m_decaychain->dim()) ;
  if(vtxverbose>5) std::cout<<"Fitter::Fitter() ("<<this<<") D - done"<<std::endl;
}
// ==========================================================================
// constructor from the particle (decay head) and beam 
// ==========================================================================
DecayTreeFitter::Fitter::Fitter
( RhoCandidate*   bc           ,
 const RhoLorentzVectorErr& lv           ,
 RecoTrackStateProvider* aExtrapolator ,
 int verbosity)
: RhoFitterBase(bc)
, m_particle   ( bc )
, m_decaychain ( 0   )
, m_fitparams  ( 0   )
, m_status     ( UnFitted )
//, m_chiSquare  ( -1  )
, m_niter      ( -1  )
, m_errCode    (  0  )
//
, m_extrapolator ( aExtrapolator) 
//
{
  vtxverbose = verbosity;
  if(vtxverbose>5) std::cout<<"Fitter::Fitter() ("<<this<<") D - config"<<std::endl;
  Configuration config(aExtrapolator) ;
  if(vtxverbose>5) std::cout<<"Fitter::Fitter() ("<<this<<") D - tree  "<<std::flush;
  m_decaychain = new DecayChain(bc,lv,config) ;
  if(vtxverbose>5) std::cout<<"Fitter::Fitter() ("<<this<<") D - fitparams  "<<std::flush;
  m_fitparams  = new FitParams(m_decaychain->dim()) ;
  if(vtxverbose>5) std::cout<<"Fitter::Fitter() ("<<this<<") D - done"<<std::endl;
}
// ==========================================================================
// constructor from the particle (decay head), beam and primary vertex
// ==========================================================================
DecayTreeFitter::Fitter::Fitter
( RhoCandidate*   bc           ,
 const RhoLorentzVectorErr& lv           ,
 const RhoVector3Err& pv           ,
 RecoTrackStateProvider*     aExtrapolator  ,
 int verbosity)
: RhoFitterBase(bc)
, m_particle   ( bc )
, m_decaychain ( 0   )
, m_fitparams  ( 0   )
, m_status     ( UnFitted )
//, m_chiSquare  ( -1  )
, m_niter      ( -1  )
, m_errCode    (  0  )
//
, m_extrapolator ( aExtrapolator) 
//
{
  vtxverbose = verbosity;
  if(vtxverbose>5) std::cout<<"Fitter::Fitter() ("<<this<<") C - config"<<std::endl;
  Configuration config(aExtrapolator) ;
  if(vtxverbose>5) std::cout<<"Fitter::Fitter() ("<<this<<") C - tree  "<<std::flush;
  m_decaychain = new DecayChain(bc,lv,pv,config) ;
  if(vtxverbose>5) std::cout<<"Fitter::Fitter() ("<<this<<") C - fitparams  "<<std::flush;
 m_fitparams  = new FitParams(m_decaychain->dim()) ;
  if(vtxverbose>5) std::cout<<"Fitter::Fitter() ("<<this<<") C - done"<<std::endl;
}

// ==========================================================================
// set the track extrapolator
// ==========================================================================
void DecayTreeFitter::Fitter::setStateProvider ( RecoTrackStateProvider* aExtrapolator )
{ m_extrapolator = aExtrapolator; }


void DecayTreeFitter::Fitter::setVerbose(int i) { vtxverbose = i ; }

DecayTreeFitter::Fitter::~Fitter()
{
//std::cout<<"Fitter::~Fitter() ("<<this<<") - dc  "<<m_decaychain<<std::endl;
  if(m_decaychain) delete m_decaychain ;
//std::cout<<"Fitter::~Fitter() ("<<this<<") - fp  "<<m_fitparams<<std::endl;
  if(m_fitparams) delete m_fitparams ;
//std::cout<<"Fitter::~Fitter() ("<<this<<") - done"<<std::endl;
}

void
DecayTreeFitter::Fitter::fit(int nitermax, double dChisqConv)
{
if(vtxverbose>5){std::cout<<"Fitter::fit(): - start"<<std::endl;}
//FIXME it's not used?
  //    m_map.clear() ;

  const int maxndiverging=3 ;
  //const double dChisqQuit = nDof() ; // if chi2 increases by more than this --> fit failed

  // initialize
  fChiSquare = -1 ;
  // m_errCode.reset() ;
  m_errCode = 0 ;

  if( m_status == UnFitted )
  { 
    if(vtxverbose>5){std::cout<<"Fitter::fit(): - unfitted: init decaychain"<<std::endl;}
    m_errCode = m_decaychain->init(m_fitparams).flag() ; 
  }
  if(vtxverbose>3){std::cout<<"Fitter:fit(): decaychain init ended with errcode "<<m_errCode<<std::endl;}
  // if(m_errCode.failure()) {
  if ( 0 != m_errCode )
  {
    // the input tracks are too far apart
    m_status = BadInput ;

  } else {
    // reset the status flag
    m_status = UnFitted ;

    int ndiverging=0 ;
    bool finished = false ;

    for(m_niter=0; m_niter<nitermax && !finished; ++m_niter) 
    {
      TVectorD prevpar = m_fitparams->par() ;
      bool firstpass = m_niter==0 ;
      if(vtxverbose>5){std::cout<<"Fitter::fit(): - filtering"<<std::endl;}
      m_errCode = m_decaychain->filter(m_fitparams,firstpass).flag() ;
      if(vtxverbose>5){std::cout<<"Fitter::fit(): - filtering done"<<std::endl;}
      double chisq = m_fitparams->chiSquare() ;
      double deltachisq = chisq - fChiSquare ;
      // if chi2 increases by more than this --> fit failed
      const double dChisqQuit = std::max(double(2*nDof()),2*fChiSquare) ;

      // if(m_errCode.failure()) {
      if( 0 != m_errCode ) {
        finished = true ;
        m_status = Failed ;
        if(vtxverbose>5){std::cout<<"Fitter::fit(): - ow, we got an error: "<<m_errCode<<std::endl;}
      } else {
        if( m_niter>0 ) {
          if( fabs( deltachisq ) < dChisqConv ) {
            fChiSquare = chisq ;
            m_status = Success ;
            finished = true ;
            if(vtxverbose>5){std::cout<<"Fitter::fit(): - yay! we filtered successfully"<<std::endl;}
          } else if( m_niter>1 && deltachisq > dChisqQuit ) {
            m_fitparams->par() = prevpar ;
            m_status  = Failed ;
            m_errCode = ErrCode::fastdivergingfit ;
            finished = true ;
            if(vtxverbose>5){std::cout<<"Fitter::fit(): - fast diverging"<<std::endl;}
          } else if( deltachisq > 0 && ++ndiverging>=maxndiverging) {
            m_fitparams->par() = prevpar ;
            m_status = NonConverged ;
            m_errCode = ErrCode::slowdivergingfit ;
            finished = true ;
            if(vtxverbose>5){std::cout<<"Fitter::fit(): - slow diverging"<<std::endl;}
          } else if( deltachisq > 0 ) {
            if(vtxverbose>5){std::cout<<"Fitter::fit(): - well, smaller steps now"<<std::endl;}
            // make a half step and reduce stepsize
            m_fitparams->par()   += prevpar ;
            m_fitparams->par()   *= 0.5 ;
            //if( m_niter>10) m_fitparams->scale() *= 0.5 ;
          }
        }
        if ( deltachisq < 0 ) ndiverging=0 ; // start over with counting
        if(!finished) fChiSquare = chisq ;
      }
      fNDegreesOfFreedom = m_fitparams->nDof();

      if(vtxverbose>=1) {
        std::cout << "Fitter::fit(): step, stat, errcode, ndf (ncon, dim) , chiSquare, dchisq: "
        << std::setw(3) << m_niter
        << std::setw(3) << m_status<<"\t"
        << std::setw(3) << m_errCode<<"\t"
        << std::setw(3) << nDof() << " ("
        << std::setw(3) << m_fitparams->nConstraints() << ", "
        << std::setw(3) << m_fitparams->dim() << ") "
        << std::setprecision(6)
        << std::setw(12) << chisq
        << std::setw(12) << deltachisq << std::endl ;
      }

      if(vtxverbose>=4) {
        std::cout << print()            << std::endl;
        std::cout << "press a key ...." << std::endl ;
        getchar() ;
      }
    }

    if( m_niter == nitermax && m_status != Success )
    { m_status = NonConverged ; }

    //m_decaychain->mother()->forceP4Sum(m_fitparams) ;

    if(m_status == Success ) {
      if(vtxverbose>5){std::cout<<"Fitter::fit(): - test cov"<<std::endl;}
      if( !(m_fitparams->testCov() ) ) {
        //static int counter(10) ;
        //if( --counter>=0)
        std::cout << "DecayTreeFitterter::Fitter: Error matrix not positive definite. "
          << "Changing status to failed." << std::endl ;
        m_status = Failed ;
        //print() ;
      }
    }
  }
  if(vtxverbose>5){std::cout<<"Fitter::fit(): - done"<<std::endl;}
}

void
DecayTreeFitter::Fitter::fitOneStep()
{
  bool firstpass = m_status==UnFitted ;
  if( firstpass ) m_decaychain->init(m_fitparams) ;
  m_decaychain->filter(m_fitparams,firstpass) ;
  fChiSquare = m_fitparams->chiSquare() ;
  fNDegreesOfFreedom = m_fitparams->nDof();
  if(vtxverbose>=1)
    std::cout << "In Fitter::fitOneStep(): " << m_status << " " << firstpass << " " << fChiSquare << " " << fNDegreesOfFreedom << std::endl ;
  m_status = Success ;
}

std::string
DecayTreeFitter::Fitter::print() const
{
  std::ostringstream s ;
  m_decaychain->mother()->print(m_fitparams) ;
  s << "chisq,ndof,ncontr,niter,status,errcode: "
  << chiSquare() << " "
  << nDof() << " " << m_fitparams->nConstraints() << " "
  << nIter() << " " << status() << " " << m_errCode << std::endl ;
  return s.str() ;
}

int
DecayTreeFitter::Fitter::nDof() const {
  return fNDegreesOfFreedom;
}


double DecayTreeFitter::Fitter::add(RhoCandidate* cand)
{
  // first obtain a map
  //ParticleBase::indexmap indexmap ;
  //m_decaychain->mother()->addToMap( indexmap ) ;
  // add this track
std::cout<<"Fitter::add(RhoCandidate*) - start"<<std::endl;
  ParticleBase* bp = m_decaychain->mother()->addDaughter(cand, Configuration()) ;
  int offset = m_fitparams->dim() ;
  double deltachisq(999) ;
  if( bp ) {
    bp->updateIndex(offset) ;
    // reassign indices
    //int offset(0) ;
    //m_decaychain->mother()->updIndex(offset) ;
    // resize the fitparams
std::cout<<"Fitter::add(RhoCandidate*) - resize"<<std::endl;
    m_fitparams->resize(offset) ;
    // initialize the added track, filter and return the delta chisq
    bp->initPar1(m_fitparams) ;
    bp->initPar2(m_fitparams) ;
    bp->initCov(m_fitparams) ;

std::cout<<"Fitter::add(RhoCandidate*) - constrains"<<std::endl;
    ParticleBase::constraintlist constraints ;
    bp->addToConstraintList(constraints,0) ;
    double chisq = m_fitparams->chiSquare() ;
    ErrCode estatus ;
    for( ParticleBase::constraintlist::const_iterator it = constraints.begin() ;
        it != constraints.end(); ++it)
      estatus |= it->filter(m_fitparams) ;

    deltachisq = m_fitparams->chiSquare() - chisq ;
    fChiSquare = m_fitparams->chiSquare() ;

std::cout<<"Fitter::add(RhoCandidate*) - init dchain"<<std::endl;
    // we want this somewhere else, but too much work now
    decaychain()->initConstraintList() ;

    //    print() ;
  } else {
    std::cout << "cannot add track to this vertex ..."
    << m_decaychain->mother()->type() << std::endl ;
  }
  return deltachisq ;
}

double DecayTreeFitter::Fitter::remove(RhoCandidate* cand)
{
  ParticleBase* pb = const_cast<ParticleBase*>(m_decaychain->locate(cand)) ;
  ErrCode estatus ;
  double dchisq(0) ;
  if(pb) {
    // filter with negative weight
    ParticleBase::constraintlist constraints ;
    pb->addToConstraintList(constraints,0) ;
    double chisq = m_fitparams->chiSquare() ;
    for( ParticleBase::constraintlist::iterator it = constraints.begin() ;
        it != constraints.end(); ++it) {
      it->setWeight(-1) ;
      estatus |= it->filter(m_fitparams) ;
    }
    dchisq = chisq - m_fitparams->chiSquare() ;
    // remove
    ParticleBase* themother = const_cast<ParticleBase*>(pb->mother()) ;
    if(themother) themother->removeDaughter(pb);
  }
  return dchisq ;
}

void DecayTreeFitter::Fitter::setMassConstraint( RhoCandidate* bc, bool badd)
{
  m_decaychain->setMassConstraint(bc,badd) ;
  m_status = UnFitted ;
}

void DecayTreeFitter::Fitter::setMassConstraint( RhoCandidate* bc, double mass)
{
  m_decaychain->setMassConstraint(bc,mass) ;
  m_status = UnFitted ;
}

//TODO
//  void DecayTreeFitter::Fitter::setMassConstraint( RhoCandidateID& pid, bool add)
//  {
//    m_decaychain->setMassConstraint(pid,add) ;
//    m_status = UnFitted ;
//  }
//  void DecayTreeFitter::Fitter::setMassConstraint( RhoCandidateID& pid, double mass)
//  {
//    m_decaychain->setMassConstraint(pid,mass) ;
//    m_status = UnFitted ;
//  }

void DecayTreeFitter::Fitter::updateIndex()
{
 std::cout<<"Fitter::updateIndex() start"<<std::endl;
  int offset=0 ;
  m_decaychain->mother()->updateIndex(offset) ;
 std::cout<<"Fitter::updateIndex() resize"<<std::endl;
  m_fitparams->resize(offset) ;
}

double DecayTreeFitter::Fitter::globalChiSquare() const
{
  return m_decaychain->chiSquare(m_fitparams) ;
}

//  Gaudi::Math::ParticleParams
//  DecayTreeFitter::Fitter::fitParams(const ParticleBase& pb) const
//  {
//    int posindex = pb.posIndex() ;
//    // hack: for tracks and photons, use the production vertex
//    if(posindex<0 && pb.mother()) posindex = pb.mother()->posIndex() ;
//    int momindex = pb.momIndex() ;
//    int lenindex = pb.lenIndex() ;
//    TVector3 pos(m_fitparams->par()(posindex+0),
//                 m_fitparams->par()(posindex+1),
//                 m_fitparams->par()(posindex+2)) ;
//    TLorentzVector p4 ;
//    p4.SetPx( m_fitparams->par()(momindex+0) ) ;
//    p4.SetPy( m_fitparams->par()(momindex+1) ) ;
//    p4.SetPz( m_fitparams->par()(momindex+2) ) ;
//    Gaudi::SymMatrix8x8 cov8 ;
//    double decaylength = lenindex>=0 ? m_fitparams->par()(lenindex) : 0 ;
//
//    if( pb.hasEnergy() ) {
//      // if particle has energy, get full p4 from fitparams
//      p4.SetE( m_fitparams->par()(momindex+3) ) ;
//      int parmap[8] ;
//      for(int i=0; i<3; ++i) parmap[i]   = posindex + i ;
//      for(int i=0; i<4; ++i) parmap[i+3] = momindex + i ;
//      parmap[7] = lenindex ;
//      int maxrow = lenindex >=0 ? 8 : 7 ;
//      for(int row=0; row<maxrow; ++row)
//        for(int col=0; col<=row; ++col)
//          cov8(row,col) = m_fitparams->cov()(parmap[row],parmap[col]) ;
//    } else {
//      // if not, use the pdttable mass
//      Gaudi::SymMatrix7x7 cov7 ;
//      int parmap[8] ;
//      for(int i=0; i<3; ++i) parmap[i]   = posindex + i ;
//      for(int i=0; i<3; ++i) parmap[i+3] = momindex + i ;
//      parmap[6] = lenindex ;
//      int maxrow = lenindex >=0 ? 7 : 6 ;
//      for(int row=0; row<maxrow; ++row)
//        for(int col=0; col<=row; ++col)
//          cov7(row,col) = m_fitparams->cov()(parmap[row],parmap[col]) ;
//
//      // now fill the jacobian
//      double mass = pb.pdtMass() ;
//      double energy2 = mass*mass ;
//      for(int row=0; row<3; ++row) {
//        double px = m_fitparams->par()(momindex+row) ;
//        energy2 += px*px ;
//      }
//      double energy = std::sqrt(energy2) ;
//
//      ROOT::Math::SMatrix<double,8,7> jacobian ;
//      for(int col=0; col<7; ++col)
//        jacobian(col,col) = 1;
//      for(int col=3; col<6; ++col)
//        jacobian(6,col) = m_fitparams->par()(parmap[col])/energy ;
//
//      p4.SetE(energy) ;
//      cov8 = ROOT::Math::Similarity(jacobian,cov7) ;
//    }
//    //
//    // VtxFitParams vtxfitparams(pb.charge(),pos,p4,decaylength,cov8) ;
//    // return vtxfitparams ;
//    return Gaudi::Math::ParticleParams ( pos , p4 , decaylength , cov8 ) ;
//  }
//


//  const Gaudi::Math::ParticleParams*
//  DecayTreeFitter::Fitter::fitParams ( RhoCandidate* cand ) const
//  {
//    Map::const_iterator ifind = m_map.find ( cand ) ;
//    if ( m_map.end() != ifind ) { return &ifind->second ; } // RETURN
//                                                            //
//    const ParticleBase* pb =
//    0 != cand ? m_decaychain->locate(*cand) : m_decaychain->cand() ;
//    //
//    if ( 0 == pb ) { return NULL ; }                       // RETURN
//                                                           //
//    return &(m_map.insert( Map::value_type ( cand , fitParams ( *pb ) ) ).first->second) ;
//  }

RhoDoubleErr
DecayTreeFitter::Fitter::decayLengthSum(RhoCandidate* candA, RhoCandidate* candB) const
{
  // returns the decaylengthsum of two particles (use ful for e.g. B->DD)
  RhoDoubleErr rc(0,0) ;
  const ParticleBase* pbA = m_decaychain->locate(candA) ;
  const ParticleBase* pbB = m_decaychain->locate(candB) ;
  if(pbA && pbB && pbA->mother() && pbB->mother() ) {
    int lenindexA = pbA->lenIndex() ;
    int lenindexB = pbB->lenIndex() ;
    if( lenindexA>=0 && lenindexB>=0) {
      double lenA    = m_fitparams->par()(lenindexA) ;
      double lenB    = m_fitparams->par()(lenindexB) ;
      double cov =
      m_fitparams->cov()(lenindexA, lenindexA) +
      m_fitparams->cov()(lenindexB, lenindexB) +
      2*m_fitparams->cov()(lenindexA, lenindexB) ;

      // rc = VtxDoubleErr(lenA+lenB,std::sqrt(cov)) ;
      rc = RhoDoubleErr( lenA+lenB , cov) ;
    }
  }
  return rc ;
}

std::string
DecayTreeFitter::Fitter::name(RhoCandidate* cand) const
{
  const ParticleBase* pb = m_decaychain->locate(cand) ;
  return pb ? pb->name() : "Not found" ;
}

//  RhoCandidate
//  DecayTreeFitter::Fitter::getFitted() const
//  {
//    RhoCandidate thecand = *particle() ;
//    // const ParticleBase* pb = m_decaychain->locate(thecand) ;
//    const ParticleBase* pb = m_decaychain->locate( *particle() ) ;
//    if( pb ) updateCand(*pb, thecand) ;
//    else std::cout << "Error: cannot find particle in tree" << std::endl ;
//    return thecand ;
//  }
//
//  RhoCandidate
//  DecayTreeFitter::Fitter::getFitted(RhoCandidate* cand) const
//  {
//    // clone the particle
//    RhoCandidate thecand = cand ;
//    // find the ParticleBase corresponding to the original particle.
//    const ParticleBase* pb = m_decaychain->locate(cand) ;
//    if( pb ) updateCand(*pb, thecand) ;
//    else std::cout << "Error: cannot find particle in tree" << std::endl ;
//    return thecand ;
//  }

//   RhoCandidate
//   DecayTreeFitter::Fitter::getFitted(RhoCandidate* cand) const
//   {
//     RhoCandidate thecand = cand ;
//     updateCand(thecand) ;
//     return thecand ;
//   }

//  RhoCandidate*
//  DecayTreeFitter::Fitter::fittedCand(RhoCandidate* /*cand*/,
//                     RhoCandidate* /*headOfTree*/) const
//  {
//    std::cout << "DecayTreeFitter::Fitter::fittedCand: not yet implemented" << std::endl ;
//    return 0 ;
//    // assigns fitted parameters to candidate in tree
//    //RhoCandidate* acand = const_cast<RhoCandidate*>(headOfTree->cloneInTree(cand)) ;
//    //updateCand(*acand) ;
//    //return acand ;
//  }

//  LHCb::DecayTree
//  DecayTreeFitter::Fitter::getFittedTree() const
//  {
//    // clone the decay tree
//    LHCb::DecayTree tree(*m_particle) ;
//    // update the tree.
//    // the easy version will only work once we have a proper 'isCloneOf'.
//    // updateTree( *tree.head() ) ;
//    for ( LHCb::DecayTree::CloneMap::const_iterator it = tree.cloneMap().begin();
//         it != tree.cloneMap().end(); ++it ) {
//      const ParticleBase* pb = m_decaychain->locate(*(it->first)) ;
//      if(pb!=0) updateCand( *pb, *(it->second) ) ;
//    }
//    return tree ;
//  }

/*
 std::string myvtxprint(RhoCandidate & cand,
 const ComIOManip & format) {
 std::ostringstream stream;
 const PdtEntry * pdtEntry = cand.pdtEntry();
 if (0 != pdtEntry){
 stream << pdtEntry->name();
 if(!cand.isComposite())
 stream << "(" << cand.uid() << ")" ;
 stream << std::ends;
 }
 HepString result(stream.str());
 //delete [] stream.str();           // must take ownership here
 return result;
 }
 */

//  void
//  DecayTreeFitter::Fitter::updateCand(const ParticleBase& pb,
//                     RhoCandidate* particle) const
//  {
//    //FIXME HIER MACHEN
//    // assigns fitted parameters to a candidate
//    // VtxFitParams vtxpar = fitParams(pb) ;
//    Gaudi::Math::ParticleParams vtxpar = fitParams(pb) ;
//    int posindex = pb.posIndex() ;
//    // hack: for tracks and photons, use the production vertex
//    if(posindex<0 && pb.mother()) posindex = pb.mother()->posIndex() ;
//    int momindex = pb.momIndex() ;
//    int lenindex = pb.lenIndex() ;
//    TVector3 pos(m_fitparams->par()(posindex+0),
//                 m_fitparams->par()(posindex+1),
//                 m_fitparams->par()(posindex+2)) ;
//    TLorentzVector p4 ;
//    p4.SetPx( m_fitparams->par()(momindex+0) ) ;
//    p4.SetPy( m_fitparams->par()(momindex+1) ) ;
//    p4.SetPz( m_fitparams->par()(momindex+2) ) ;
//    Gaudi::SymMatrix8x8 cov8 ;
//    double decaylength = lenindex>=0 ? m_fitparams->par()(lenindex) : 0 ;
//
//    if( pb.hasEnergy() ) {
//      // if particle has energy, get full p4 from fitparams
//      p4.SetE( m_fitparams->par()(momindex+3) ) ;
//      int parmap[8] ;
//      for(int i=0; i<3; ++i) parmap[i]   = posindex + i ;
//      for(int i=0; i<4; ++i) parmap[i+3] = momindex + i ;
//      parmap[7] = lenindex ;
//      int maxrow = lenindex >=0 ? 8 : 7 ;
//      for(int row=0; row<maxrow; ++row)
//        for(int col=0; col<=row; ++col)
//          cov8(row,col) = m_fitparams->cov()(parmap[row],parmap[col]) ;
//    } else {
//      // if not, use the pdttable mass
//      Gaudi::SymMatrix7x7 cov7 ;
//      int parmap[8] ;
//      for(int i=0; i<3; ++i) parmap[i]   = posindex + i ;
//      for(int i=0; i<3; ++i) parmap[i+3] = momindex + i ;
//      parmap[6] = lenindex ;
//      int maxrow = lenindex >=0 ? 7 : 6 ;
//      for(int row=0; row<maxrow; ++row)
//        for(int col=0; col<=row; ++col)
//          cov7(row,col) = m_fitparams->cov()(parmap[row],parmap[col]) ;
//
//      // now fill the jacobian
//      double mass = pb.pdtMass() ;
//      double energy2 = mass*mass ;
//      for(int row=0; row<3; ++row) {
//        double px = m_fitparams->par()(momindex+row) ;
//        energy2 += px*px ;
//      }
//      double energy = std::sqrt(energy2) ;
//
//      ROOT::Math::SMatrix<double,8,7> jacobian ;
//      for(int col=0; col<7; ++col)
//        jacobian(col,col) = 1;
//      for(int col=3; col<6; ++col)
//        jacobian(6,col) = m_fitparams->par()(parmap[col])/energy ;
//
//      p4.SetE(energy) ;
//      cov8 = ROOT::Math::Similarity(jacobian,cov7) ;
//    }
//    //
//    // VtxFitParams vtxfitparams(pb.charge(),pos,p4,decaylength,cov8) ;
//    // return vtxfitparams ;
//    return Gaudi::Math::ParticleParams ( pos , p4 , decaylength , cov8 ) ;
//
//    // update everything inside the particle. don't update the vertex, for now.
//    particle.setMomentum        ( vtxpar.momentum() ) ;
//    particle.setReferencePoint  ( vtxpar.position() ) ;
//    particle.setMomCovMatrix    ( vtxpar.momCovMatrix () ) ;
//    particle.setPosCovMatrix    ( vtxpar.posCovMatrix () ) ;
//    particle.setPosMomCovMatrix ( vtxpar.momPosCov    () ) ;
//    //
//    // update the chi2 of global fit if this is the head of the tree
//    if( &pb == m_decaychain->cand() )
//    {
//      particle.eraseInfo ( RhoCandidate::Chi2OfParticleReFitter ) ;
//      particle.addInfo   ( RhoCandidate::Chi2OfParticleReFitter ,
//                          chiSquare() ) ;
//    }
//    // update the vertex as well, if this is the head of the tree
//    if( &pb == m_decaychain->cand() )
//    {
//      LHCb::Vertex* vertex = particle.endVertex() ;
//      if( !vertex )
//      {
//        // create a new vertex
//        vertex = new LHCb::Vertex() ;
//        // don't add any daughters. I don't understand why we need them anyway.
//        particle.setEndVertex( vertex ) ;
//      }
//      vertex -> setTechnique  ( LHCb::Vertex::LastGlobal ) ;
//      vertex -> setChi2AndDoF ( chiSquare(), nDof()      ) ;
//      vertex -> setPosition   ( vtxpar.position()        ) ;
//      vertex -> setCovMatrix  ( vtxpar.posCovMatrix()    ) ;
//    }

//  if( pb ==m_decaychain->cand() ) {
//    vtx = new VtxVertex(chiSquare(),nDof(),vtxpar.pos(),vtxpar.posCov(),vtxpar.xp4Cov()) ;
//    vtx->setStatus(FitStatus::VtxStatus(status())) ;
//    vtx->setType(FitStatus::Geometric) ;
//  } else {
//    // all updated daughters are reset to unfitted, but leave the chisquare
//    double chisq = cand.decayVtx() ? cand.decayVtx()->chiSquared() : 0 ;
//    int ndof     = cand.decayVtx() ? cand.decayVtx()->nDof() : 0 ;
//    vtx = new VtxVertex(chisq,ndof,vtxpar.pos(),vtxpar.posCov(),vtxpar.xp4Cov()) ;
//    vtx->setStatus(FitStatus::UnFitted) ;
//  }
//       }
//      cand.setTrajectory(vtxpar,vtx) ;
//}

void DecayTreeFitter::Fitter::updateCand(const ParticleBase& pb, RhoCandidate* aParticle) const
{
  // Update the FittedCand of the RhoCandidate
  // This replaces the previous updateCand() as well as fitParams() functions
  ////int posindex = pb.posIndex() ;
  ////// hack: for tracks and photons, use the production vertex
  ////if(posindex<0 && pb.mother()) posindex = pb.mother()->posIndex() ;
  int posindex = ( pb.mother() ? pb.mother()->posIndex() : pb.posIndex() ) ;
  if(posindex<0) posindex = pb.posIndex() ;
  int momindex = pb.momIndex() ;
  int lenindex = pb.lenIndex() ;
  TVector3 pos(m_fitparams->par()(posindex+0),
               m_fitparams->par()(posindex+1),
               m_fitparams->par()(posindex+2)) ;
  TLorentzVector p4 ;
  p4.SetPx( m_fitparams->par()(momindex+0) ) ;
  p4.SetPy( m_fitparams->par()(momindex+1) ) ;
  p4.SetPz( m_fitparams->par()(momindex+2) ) ;
  // double decaylength = lenindex>=0 ? m_fitparams->par()(lenindex) : 0 ; // Unused
  TMatrixDSym cov8(8) ;
  //
  if( pb.hasEnergy() ) {
    // if particle has energy, get full p4 from fitparams
    p4.SetE( m_fitparams->par()(momindex+3) ) ;
    int parmap[8] ;
    for(int i=0; i<3; ++i) parmap[i]   = posindex + i ;
    for(int i=0; i<4; ++i) parmap[i+3] = momindex + i ;
    parmap[7] = lenindex ;
    int maxrow = lenindex >=0 ? 8 : 7 ;
    for(int row=0; row<maxrow; ++row)
      //for(int col=0; col<=row; ++col) //TMatrixDSym is not really symmetric :-(
      for(int col=0; col<maxrow; ++col)
        cov8(row,col) = m_fitparams->cov()(parmap[row],parmap[col]) ;
  } else {
    // if not, use the pdttable mass
    TMatrixDSym cov7(7) ;
    int parmap[8] ;
    for(int i=0; i<3; ++i) parmap[i]   = posindex + i ;
    for(int i=0; i<3; ++i) parmap[i+3] = momindex + i ;
    parmap[6] = lenindex ;
    int maxrow = lenindex >=0 ? 7 : 6 ;
    for(int row=0; row<maxrow; ++row)
      //for(int col=0; col<=row; ++col)
      for(int col=0; col<maxrow; ++col)
        cov7(row,col) = m_fitparams->cov()(parmap[row],parmap[col]) ;

    // now fill the jacobian
    double mass = pb.pdtMass() ;
    double energy2 = mass*mass ;
    for(int row=0; row<3; ++row) {
      double px = m_fitparams->par()(momindex+row) ;
      energy2 += px*px ;
    }
    double energy = std::sqrt(energy2) ;
    TMatrixD jacobian(8,7);
    for(int col=0; col<7; ++col)
      jacobian(col,col) = 1;
    for(int col=3; col<6; ++col)
      jacobian(6,col) = m_fitparams->par()(parmap[col])/energy ;

    p4.SetE(energy) ;
    cov8 = cov7.Similarity(jacobian);
  }
  //    // VtxFitParams vtxfitparams(pb.charge(),pos,p4,decaylength,cov8) ;
  RhoCandidate* fittedcand=aParticle->GetFit();
  if(!fittedcand)
  {
    fittedcand = RhoFactory::Instance()->NewCandidate ( aParticle );
    fittedcand->RemoveAssociations();
    aParticle->SetFit(fittedcand);//ready to be modified
  }
  fittedcand->SetPos(pos);
  fittedcand->SetP4(p4);
  fittedcand->SetCov7(cov8.GetSub(0,6,0,6));
}
///////////////////////////////////////////
//    int posindex = pb.posIndex() ;
//    // hack: for tracks and photons, use the production vertex
//    if(posindex<0 && pb.mother()) posindex = pb.mother()->posIndex() ;
//    int momindex = pb.momIndex() ;
//    int lenindex = pb.lenIndex() ;
//    TVector3 pos(m_fitparams->par()(posindex+0),
//                 m_fitparams->par()(posindex+1),
//                 m_fitparams->par()(posindex+2)) ;
//    TLorentzVector p4 ;
//    p4.SetPx( m_fitparams->par()(momindex+0) ) ;
//    p4.SetPy( m_fitparams->par()(momindex+1) ) ;
//    p4.SetPz( m_fitparams->par()(momindex+2) ) ;
//    Gaudi::SymMatrix8x8 cov8 ;
//    double decaylength = lenindex>=0 ? m_fitparams->par()(lenindex) : 0 ;
//
//    if( pb.hasEnergy() ) {
//      // if particle has energy, get full p4 from fitparams
//      p4.SetE( m_fitparams->par()(momindex+3) ) ;
//      int parmap[8] ;
//      for(int i=0; i<3; ++i) parmap[i]   = posindex + i ;
//      for(int i=0; i<4; ++i) parmap[i+3] = momindex + i ;
//      parmap[7] = lenindex ;
//      int maxrow = lenindex >=0 ? 8 : 7 ;
//      for(int row=0; row<maxrow; ++row)
//        for(int col=0; col<=row; ++col)
//          cov8(row,col) = m_fitparams->cov()(parmap[row],parmap[col]) ;
//    } else {
//      // if not, use the pdttable mass
//      Gaudi::SymMatrix7x7 cov7 ;
//      int parmap[8] ;
//      for(int i=0; i<3; ++i) parmap[i]   = posindex + i ;
//      for(int i=0; i<3; ++i) parmap[i+3] = momindex + i ;
//      parmap[6] = lenindex ;
//      int maxrow = lenindex >=0 ? 7 : 6 ;
//      for(int row=0; row<maxrow; ++row)
//        for(int col=0; col<=row; ++col)
//          cov7(row,col) = m_fitparams->cov()(parmap[row],parmap[col]) ;
//
//      // now fill the jacobian
//      double mass = pb.pdtMass() ;
//      double energy2 = mass*mass ;
//      for(int row=0; row<3; ++row) {
//        double px = m_fitparams->par()(momindex+row) ;
//        energy2 += px*px ;
//      }
//      double energy = std::sqrt(energy2) ;
//
//      ROOT::Math::SMatrix<double,8,7> jacobian ;
//      for(int col=0; col<7; ++col)
//        jacobian(col,col) = 1;
//      for(int col=3; col<6; ++col)
//        jacobian(6,col) = m_fitparams->par()(parmap[col])/energy ;
//
//      p4.SetE(energy) ;
//      cov8 = ROOT::Math::Similarity(jacobian,cov7) ;
//    }
//    //
//    // VtxFitParams vtxfitparams(pb.charge(),pos,p4,decaylength,cov8) ;
//    // return vtxfitparams ;
//    return Gaudi::Math::ParticleParams ( pos , p4 , decaylength , cov8 ) ;
//////////////////////////////////////

bool
DecayTreeFitter::Fitter::updateCand(RhoCandidate* aParticle) const
{
  // assigns fitted parameters to a candidate
  const ParticleBase* pb = m_decaychain->locate(aParticle) ;
  if(pb) updateCand(*pb,aParticle) ;
  //else {
  // this error message does not make sense, because it is also
  // triggered for daughters that were simply not refitted. we
  // have to do something about that.
  //       VtxPrintTree printer(&myvtxprint) ;
  //       ErrMsg(error) << "cann't find candidate " << std::endl
  //       << printer.print(cand)
  //       << "in tree " << std::endl
  //       << printer.print(*_bc)
  //       << endmsg;
  return pb != 0 ;
}

bool
DecayTreeFitter::Fitter::updateTree(RhoCandidate* p) const
{
  bool rc ;
  if ( (rc = updateCand(p) ) ) {
    //      BOOST_FOREACH( RhoCandidate* daughter, p.daughters() ) {
    //      updateTree( const_cast<RhoCandidate*>(*daughter) ) ;
    for(int iDau=0;iDau<p->NDaughters();iDau++){
      updateTree( const_cast<RhoCandidate*>(p->Daughter(iDau)) ) ;
    }
  }
  return rc ;
}

// Print the result of the fit
std::ostream& DecayTreeFitter::Fitter::fillStream ( std::ostream& s ) const
{ return s << print() ; }

// Get the chisquare of a particular particle in the decay chain
ChiSquare DecayTreeFitter::Fitter::chiSquare( RhoCandidate* aParticle ) const
{
  return m_decaychain->chiSquare(aParticle, m_fitparams) ;
}

// ============================================================================
// The END
// ============================================================================
