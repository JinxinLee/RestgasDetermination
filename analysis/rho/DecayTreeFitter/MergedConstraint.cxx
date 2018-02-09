// ******************************************************
// DecayTreeFitter Package
// We thank the original author Wouter Hulsbergen 
// (BaBar, LHCb) for providing the sources. 
// http://arxiv.org/abs/physics/0503191v1 (2005)
// Adaptation & Development for PANDA: Ralf Kliemt (2015)
// ******************************************************
#include "FitParams.h"
#include "ParticleBase.h"
#include "MergedConstraint.h"

using namespace DecayTreeFitter;
ClassImp(MergedConstraint);

ErrCode
DecayTreeFitter::MergedConstraint::project(const FitParams* fitpar,
                          Projection& p) const
{
  ErrCode status ;
  for(constraintlist::const_iterator it = m_list.begin() ;
      it != m_list.end() ; ++it) {
    status |= (*it)->project(fitpar,p) ;
    p.incrementOffset((*it)->dim()) ;
  }

  return status ;
}

void DecayTreeFitter::MergedConstraint::print(std::ostream& os) const
{
  os << "Merged constraint: " << std::endl ;
  for(constraintlist::const_iterator it = m_list.begin() ;
      it != m_list.end() ; ++it) {
    os << "          " ;
    (*it)->print(os) ;
  }
}
