// ******************************************************
// DecayTreeFitter Package
// We thank the original author Wouter Hulsbergen 
// (BaBar, LHCb) for providing the sources. 
// http://arxiv.org/abs/physics/0503191v1 (2005)
// Adaptation & Development for PANDA: Ralf Kliemt (2015)
// ******************************************************
#ifndef INTERNALRESONANCE_H
#define INTERNALRESONANCE_H 1

#include "InternalParticle.h"
#include "Rtypes.h"

namespace DecayTreeFitter
{
  class FitParams ;

  class Resonance : public InternalParticle
  {
  public:
    Resonance(RhoCandidate* bc, const ParticleBase* mother,
              const Configuration& config) ;
    virtual ~Resonance() ;

    virtual int dim() const { return 4 ; }
    virtual int type() const { return kResonance ; }
    inline virtual std::string parname(int index) const ;

    inline virtual ErrCode initPar1(FitParams*) ;
    inline virtual ErrCode initPar2(FitParams*) ;

    virtual int posIndex() const { return mother()->posIndex()   ; }
    virtual int momIndex() const { return index() ; }
    virtual int lenIndex() const { return -1 ; }
    virtual bool hasPosition() const { return false ; }

  private:
     ClassDef ( Resonance,1 )
 } ;

}


#endif
