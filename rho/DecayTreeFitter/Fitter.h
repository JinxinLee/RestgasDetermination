// ******************************************************
// DecayTreeFitter Package
// We thank the original author Wouter Hulsbergen 
// (BaBar, LHCb) for providing the sources. 
// http://arxiv.org/abs/physics/0503191v1 (2005)
// Adaptation & Development for PANDA: Ralf Kliemt (2015)
// ******************************************************
// $Id: Fitter.h,v 1.7 2010-05-29 13:39:56 ibelyaev Exp $
// ============================================================================
#ifndef DECAYTREEFITTER_FITTER_H
#define DECAYTREEFITTER_FITTER_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL
// ============================================================================
#include <vector>
// ============================================================================
// GaudiKernel
// ============================================================================
//#include "GaudiKernel/SmartIF.h"
// ============================================================================
// Track interfaces
// ============================================================================
//#include "TrackInterfaces/ITrackStateProvider.h"
// ============================================================================
// LHCbMath
// ============================================================================
//#include "LHCbMath/ValueWithError.h"
//#include "LHCbMath/ParticleParams.h"
// ============================================================================
// DaVinciTypes
// ============================================================================
//#include "Kernel/DecayTree.h"
// ============================================================================
#include "RecoTrackStateProvider.h"
#include "Rtypes.h"

#include "RhoDoubleErr.h"
#include "RhoCandidate.h"
#include "RhoFitterBase.h"

namespace DecayTreeFitter
{
  class DecayChain ;
  class FitParams ;
  class ParticleBase ;
  class ChiSquare ;

  // ==========================================================================
  /** @class Fitter DecayTreeFitter/Fitter.h
   *  ``Decay-Tree-Fitter''`
   *  @author Wouter Hulsbergen  Wouter.Hulsbergen@nikhef.nl
   */
  class Fitter : public RhoFitterBase
  {
  public:
    // ========================================================================
    enum FitStatus {
      UnFitted     = -1 ,
      Success      =  0 ,
      Failed            ,
      BadInput          ,
      NonConverged
    } ;
    // ========================================================================
  public:
    // ========================================================================
    /// constructor from the particle (decay head)
    Fitter ( RhoCandidate*   bc                  ,
            RecoTrackStateProvider* extrapolator        ,
            int verbosity = 0) ;
    /// constructor from the particle (decay head) and primary vertex
    Fitter ( RhoCandidate*   bc                  ,
            const RhoVector3Err& pv                  ,
            RecoTrackStateProvider* extrapolator = 0        ,
            int verbosity = 0) ;
    /// constructor from the particle (decay head) and pbeam
    Fitter ( RhoCandidate*   bc                  ,
            const RhoLorentzVectorErr& lv                  ,
            RecoTrackStateProvider* extrapolator        ,
            int verbosity = 0) ;
    /// constructor from the particle (decay head), pbeam and primary vertex
    Fitter ( RhoCandidate*   bc                  ,
            const RhoLorentzVectorErr& lv                  ,
            const RhoVector3Err& pv                  ,
            RecoTrackStateProvider* extrapolator = 0        ,
            int verbosity = 0) ;
    /// destructor
    virtual ~Fitter() ;                                           // destructor
    // ========================================================================
  public:
    // ========================================================================
    /// Add or remove a mass constraint
    void setMassConstraint ( RhoCandidate*   cand , bool add = true ) ;
    /// Add a constraint to a mass different from the property table mass
    void setMassConstraint ( RhoCandidate*   cand , double mass ) ;
    /// Add or remove a mass constraintfor a certain ParticleID
    //TODO    void setMassConstraint ( RhoCandidateID& pid  , bool add = true ) ;
    /// Add a constraint to a mass different from the property table mass
    //TODO    void setMassConstraint ( RhoCandidateID& pid  , double mass ) ;
    /// Fit the decay tree
    Bool_t Fit ( int maxNumberOfIterations  = 10 , double deltaChisquareConverged = 0.01 ) 
    {
      fit(maxNumberOfIterations,deltaChisquareConverged); 
      bool check = updateTree(m_particle);
      check = check && (m_status==0);
      return check;
    };
    Bool_t FitAll ( int maxit  = 10 , double dchiq = 0.01 ) { return Fit(maxit,dchiq); };
    /// Print the result of the fit
    std::string   print() const ;
    /// Print the result of the fit
    std::ostream& fillStream ( std::ostream& s ) const ;
    /// The top level particle that is fitted
    RhoCandidate* particle() const { return m_particle ; }
    /** Currently the only accessor to the actual fitted data
     *  @param p (INPUT) the particle
     *  @retrn the fitted parameters ( 0 for invaild parameters/fits)
     */
    RhoCandidate* fitParams ( RhoCandidate* p = 0 ) const {return p->GetFit();};
    /// Total chisquare
    //double     chiSquare () const { return m_chiSquare ; }
    double     chiSquare () const { return fChiSquare ; }
    /// Total number of DOFs
    int        nDof      () const ;
    /// Status of fit
    FitStatus  status    () const { return m_status ; }
    /// Number of iterations used by vertex fit
    int        nIter     () const { return m_niter  ; }
    /// get the chisquare of everything 'downstream' of a particle
    ChiSquare chiSquare( RhoCandidate* p ) const ;
    /**  Compute the decay length sum of two particles in
     *   the decay tree (useful for e.g. B->DD)
     */
    RhoDoubleErr
    decayLengthSum ( RhoCandidate*, RhoCandidate* ) const ;
    /** return an updated decay tree.
     *  this is not a final solution. will
     *  try to move more info to Particle
     */
    //    LHCb::DecayTree getFittedTree() const ;
    /** methods to retrieve the result in terms of RhoCandidates
     * (note: mother vertex is not updated, and decay length cannot be
     * stored anywhere. Use fitParams instead
     */
    //    RhoCandidate getFitted() const ;
    /** methods to retrieve the result in terms of RhoCandidates
     * (note: mother vertex is not updated, and decay length cannot be
     * stored anywhere. Use fitParams instead
     */
    //    RhoCandidate getFitted(RhoCandidate* cand) const ;
    /// update a particlular candidate in the tree
    bool updateCand ( RhoCandidate* cand ) const ;
    /// update a particlular candidate in the tree
    bool updateTree ( RhoCandidate* cand ) const ;
    /// error code
    int  errCode  () { return m_errCode ; }
    /// set the verbosity level (for debugging only)
    void setVerbose(int i) ;
    void SetToleranceZ(double z) {if(m_extrapolator) m_extrapolator->SetToleranceZ(z);};


	//FIXME ######@^$E&$#%#^!#@&*(#######  REMOVE!
    DecayChain*       GetDecayChainDirty ()       { return m_decaychain  ; }


    // ========================================================================
  public:
    // ========================================================================
    /// get the extrapolator
    RecoTrackStateProvider* extrapolator() { return m_extrapolator ; }
    /// set the track extrapolator
    void setStateProvider ( RecoTrackStateProvider* extrapolator ) ;
    // ========================================================================
  protected:
    // ========================================================================
    // expert interface. not yet for real consumption
    //    Gaudi::Math::ParticleParams
    //    fitParams(const ParticleBase& pb) const ;
    /// Name of a particle in the decay tree
    std::string name ( RhoCandidate* cand) const ;
    // ========================================================================
    RhoDoubleErr
    decayLengthSum(const ParticleBase&,const ParticleBase&) const ;
    // ========================================================================
    DecayChain*       decaychain ()       { return m_decaychain  ; }
    FitParams*        fitparams  ()       { return m_fitparams   ; }
    const DecayChain* decaychain () const { return m_decaychain  ; }
    const FitParams*  fitparams  () const { return m_fitparams   ; }
    // ========================================================================
    double globalChiSquare() const ;
    // ========================================================================
    // must be moved to derived class or so ...
    double add     ( RhoCandidate* cand ) ;
    double remove  ( RhoCandidate* cand ) ;
    void   updateIndex() ;
    // ========================================================================
    /// Fit the decay tree
    void   fit ( int maxNumberOfIterations  = 10 , double deltaChisquareConverged = 0.01 ) ;
    /// Fit just one step
    void fitOneStep() ;
    // ========================================================================

    RhoCandidate* fittedCand ( RhoCandidate* cand       ,
                              RhoCandidate*       headoftree ) const ;
    void            updateCand ( const ParticleBase&   pb         ,
                                RhoCandidate*       cand       ) const ;
    // ========================================================================
  private:
    // ========================================================================
    /// default constructor is disabled
    Fitter () ;                             //  default constructor is disabled
                                            /// copy constructor is disabled
    Fitter ( const Fitter& ) ;              //     copy constructor is disabled
                                            /// assignement operator is disabled
    Fitter& operator=( const Fitter& ) ;    // assignement operator is disabled
                                            // ========================================================================
  private:
    // ========================================================================
    RhoCandidate* m_particle   ;
    DecayChain*           m_decaychain ;
    FitParams*            m_fitparams  ;
    FitStatus             m_status     ;
    //double                m_chiSquare  ;
    int                   m_niter      ;
    int                   m_errCode    ;
    // ========================================================================
    //FIXME not being used???
    //    typedef std::map<RhoCandidate*, Gaudi::Math::ParticleParams> Map ;
    //    mutable Map m_map ;
    // ========================================================================
  protected:
    /// track extrapolator (if needed)
    RecoTrackStateProvider* m_extrapolator ; // track extrapolator
                                             // ========================================================================
    ClassDef ( Fitter,1 )
  } ;
  // ==========================================================================
}
// ============================================================================
// The END
// ============================================================================
#endif
