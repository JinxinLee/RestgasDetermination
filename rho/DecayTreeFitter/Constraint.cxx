// ******************************************************
// DecayTreeFitter Package
// We thank the original author Wouter Hulsbergen 
// (BaBar, LHCb) for providing the sources. 
// http://arxiv.org/abs/physics/0503191v1 (2005)
// Adaptation & Development for PANDA: Ralf Kliemt (2015)
// ******************************************************
#include <iomanip>
#include "FitParams.h"
#include "ParticleBase.h"
#include "Constraint.h"
#include "KalmanCalculator.h"

using namespace DecayTreeFitter;

ClassImp(Constraint);

extern int vtxverbose ;

bool DecayTreeFitter::Constraint::operator<(const Constraint& rhs) const
{
  // the simple way
  return m_type < rhs.m_type ||
  (m_type == rhs.m_type && m_depth < rhs.m_depth) ;

  // this is probably the second most complicated routine: how do we
  // order the constraints. there is one very special case:
  // Ks->pipi0 requires the pi0 mass constraints at the very
  // end. otherwise, it just doesn't work. in all other cases, we
  // prefer to fit 'down' the tree'. the 'external' constraints must
  // be filtered first, but soft pions must be fitted after the
  // geometric constraints of the D. You see, this is horrible.

  // if either of the two is external, or either of the two is a
  // mass constraint, we order by m_typem_

  //  if( (m_type <= Constraint::composite ||
  //      rhs.m_type <= Constraint::composite ) ||
  //    (m_type >= Constraint::mass ||
  //     rhs.m_type >= Constraint::mass ) ) {
  //      return m_type < rhs.m_type ||
  //      (m_type == rhs.m_type && m_depth < rhs.m_depth) ;
  //   }
  //// if not, we order by depth
  //return m_depth < rhs.m_depth  ||
  //(m_depth == rhs.m_depth && m_type < rhs.m_type ) ;

}

ErrCode
DecayTreeFitter::Constraint::project(const FitParams* fitpar, Projection& p) const
{
  // this one will be overruled by the MergedConstraint
  p.setParticle( *m_node ) ;
  return m_node->projectConstraint(m_type,fitpar,p) ;
}

ErrCode
DecayTreeFitter::Constraint::filter(FitParams* fitpar) const
{
  ErrCode status ;
  if(m_type<=Constraint::unknown || m_type>=Constraint::ntypes) {
    std::cout << "Constraint::filter(FitParams*): unknown constraint: " << m_type
    << std::endl ;
    status |= ErrCode::badsetup ;
  } else if (m_type!=merged && !m_node) {
    std::cout << "Constraint::filter(FitParams*): constraint without a node"
    << std::endl ;
    status |= ErrCode::badsetup ;
  } else {
    if(vtxverbose>=3) { std::cout << "Constraint::filter(FitParams*): filtering "  ; print() ;}
    // save the unfiltered ('predicted') parameters. we need to
    // store them if we want to iterate constraints.
    const TVectorD* pred(0) ;
    if(m_maxNIter>1) pred = new TVectorD(fitpar->par()) ;

    Projection p(fitpar->dim(),m_dim) ;
    KalmanCalculator kalman ;
    double chisq(0) ;
    int iter(0) ;
    bool finished(false) ;
    if(vtxverbose>=5) { std::cout << "Constraint::filter(FitParams*): starting loop "  ; print() ;}
    while (!finished && !status.failure()) {
      p.reset() ;
      status |= project(fitpar,p) ;
      if(!status.failure()) {
        status |= kalman.init( p.r(), p.H(), fitpar, p.V() ) ;
        //status |= kalman.init( p.r(), p.H(), fitpar, (&p.V()) ? ( p.V().NonZeros()>0 ? &p.V() : 0 ) : 0  ) ;
        if( !status.failure()) {
          if(iter==0 || !pred) {
            kalman.updatePar( fitpar ) ;
          } else {
            kalman.updatePar( *pred, fitpar ) ;
          }
          const double dchisqconverged = 0.001 ;
          double newchisq = kalman.chisq() ;
          double dchisq = newchisq - chisq ;
          bool diverging = iter > 0 && dchisq>0  ;
          bool converged = fabs(dchisq) < dchisqconverged ;
          finished  = ++iter >= m_maxNIter || diverging || converged ;

          if(vtxverbose>=3) {
            std::cout << "chi2,niter: "
            << iter << " "<< std::setprecision(7)
            << std::setw(12) << chisq << " "
            << std::setw(12)<< newchisq << " "
            << std::setw(12)<< dchisq << " "
            << diverging << " "
            << converged << " ";
            status.Print(std::cout);
            std::cout << std::endl ;
          }
          chisq = newchisq ;
        }
      }
    }
    if(!status.failure()) {
      kalman.updateCov( fitpar ) ;
      fitpar->addChiSquare( kalman.chisq(), m_dim, p.particle() ) ;
    }
    if(pred) delete pred ;
    if(vtxverbose>=4 &&m_node&&
       m_node->mother() ) { m_node->mother()->print(fitpar) ; }
  }
  if(vtxverbose>=5) { std::cout << "Constraint::filter(FitParams*): done"  ; print() ;}
  return status ;
}


ErrCode
DecayTreeFitter::Constraint::filter(FitParams* fitpar, const FitParams* reference) const
{
  // filter but linearize around reference
  ErrCode status ;
  if(m_type<=Constraint::unknown || m_type>=Constraint::ntypes) {
    std::cout << "Constraint::filter(FitParams*,FitParams*): unknown constraint: " << m_type
    << std::endl ;
    status |= ErrCode::badsetup ;
  } else if (m_type!=merged && !m_node) {
    std::cout << "Constraint::filter(FitParams*,FitParams*): filter constraint without a node"
    << std::endl ;
    status |= ErrCode::badsetup ;
  } else {
    if(vtxverbose>=3) { std::cout << "Constraint::filter(FitParams*,FitParams*): filtering (par&refpar) "  ; print() ;}

    // project using the reference
    Projection p(fitpar->dim(),m_dim) ;
    if(vtxverbose>=5) { std::cout << "Constraint::filter(FitParams*,FitParams*): project"<<std::endl;}
    status = project(reference,p) ;
    if(vtxverbose>=5) { std::cout << "Constraint::filter(FitParams*,FitParams*): residual"<<std::endl;}
    // now update the residual
    p.r() += p.H() * (fitpar->par() - reference->par()) ;
    if(vtxverbose>=5) { std::cout << "Constraint::filter(FitParams*,FitParams*): kalman"<<std::endl;}
    // now call the Kalman update as usual
    KalmanCalculator kalman ;
    status |= kalman.init( p.r(), p.H(), fitpar, p.V() ) ;
    //status |= kalman.init( p.r(), p.H(), fitpar, (&p.V()) ? ( p.V().NonZeros()>0 ? &p.V() : 0 ) : 0  ) ;
    kalman.updatePar( fitpar ) ;
    kalman.updateCov( fitpar ) ;
    fitpar->addChiSquare( kalman.chisq(), m_dim, p.particle() ) ;
    if(vtxverbose>=3) { std::cout << "Constraint::filter(FitParams*,FitParams*): \""<<name()<<"\" Chisquare contribution: "  <<kalman.chisq()<<" with "<<m_dim<<" dimensions from particle "<<p.particle()->name()<<std::endl;}
    if(vtxverbose>=5) { std::cout << "Constraint::filter(FitParams*,FitParams*): done"<<std::endl;}
  }
  if( status.failure()){
    std::cout << "Constraint::filter(FitParams*,FitParams*): error filtering constraint: "
    << name() << " "; status.Print(std::cout); std::cout << std::endl ;}

  return status ;
}

void DecayTreeFitter::Constraint::print(std::ostream& os) const
{
  os << "node index = " << m_node->index()
  << " name = " << m_node->name().c_str()
  << " constraint type = " << m_type
  << " depth = " << m_depth << std::endl ;
}

std::string DecayTreeFitter::Constraint::name() const
{
  std::string rc = "unknown constraint!" ;
  switch(m_type)
  {
    case beamspot:     rc = "beamspot" ; break ;
    case beamenergy:   rc = "beamenergy" ; break ;
    case composite:    rc = "composite" ; break ;
    case resonance:    rc = "resonance" ; break ;
    case track:        rc = "track" ; break ;
    case photon:       rc = "photon" ; break ;
    case kinematic:    rc = "kinematic" ; break ;
    case geometric:    rc = "geometric" ; break ;
    case mass:         rc = "mass" ; break ;
    case massEnergy:   rc = "massEnergy" ; break ;
    case lifetime:     rc = "lifetime" ; break ;
    case merged:       rc = "merged" ; break ;
    case conversion:   rc = "conversion" ; break ;
    case ntypes:
    case unknown:
      break ;
  }
  return rc ;
}
