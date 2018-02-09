// ******************************************************
// DecayTreeFitter Package
// We thank the original author Wouter Hulsbergen 
// (BaBar, LHCb) for providing the sources. 
// http://arxiv.org/abs/physics/0503191v1 (2005)
// Adaptation & Development for PANDA: Ralf Kliemt (2015)
// ******************************************************
#ifndef MERGEDCONSTRAINT_H
#define MERGEDCONSTRAINT_H 1

#include <vector>
#include "Constraint.h"
#include "Rtypes.h"

namespace DecayTreeFitter
{
  class MergedConstraint : public Constraint
  {
  public:
    typedef std::vector<Constraint*> constraintlist ;

    MergedConstraint() : Constraint(Constraint::merged) {}
    virtual ~MergedConstraint() {}

    MergedConstraint( const constraintlist& list ) :
    Constraint(Constraint::merged),m_list(list) {
      int d(0) ;
      for(constraintlist::iterator it = m_list.begin() ;
          it != m_list.end(); ++it) d += (*it)->dim() ;
      setDim(d) ;
    }

    virtual ErrCode project(const FitParams* fitpar, Projection& p) const ;

    void push_back(Constraint* c) {
      m_list.push_back(c) ;
      setDim(dim()+c->dim()) ;
      setNIter(std::max(nIter(),c->nIter())) ;
    }

    virtual void print(std::ostream& os=std::cout) const ;

  private:
    constraintlist m_list ;
    ClassDef ( MergedConstraint,1 )
  } ;

}

#endif
