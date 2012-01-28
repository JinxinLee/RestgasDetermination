#ifndef TCANDIDATE_H
#define TCANDIDATE_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TCandidate (derived from TFitParams)					//
//                                                                      //
// Contains (polymorphic) contents for TCandidate objects		//
// Candidate "Tracks" or "Particles" for analysis use			//
//									//
// Author List:								//
// Sascha Berger, RUB, Feb.99						//
// Marcel Kunze,  RUB, Aug.99						//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//									//
//////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <vector>

#include "TNtuple.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "RhoBase/TConstraint.h"
#include "RhoBase/TFitParams.h"
#include "RhoBase/TCandList.h"


class VAbsMicroCandidate;
class VAbsVertex;
class VAbsTruth;
class TParticlePDG;
class Consistency;
class TEventManager;
class TError;
class TConsistency;
class TCandListIterator;


class BtaRecoPointers;
class TrkAbsFit;
class TrkRecoTrk;
class TrkCompTrk;
class AbsRecoCalo;
class PidInfoSummary;
class BtaMicroAdapter;
class BtaAbsRecoObject;


//		---------------------
// 		-- Class Interface --
//		---------------------
class TCandidate : public TFitParams
  {
    
    //--------------------
    // Instance Members --
    //--------------------
    
    // Fast mode : Do not use error matrix
    Bool_t fFastMode;  //! Do not stream
    
    // Lock : if true, modifications are a fatal error
    Bool_t fLocked;  //! Do not stream
    
    // The mother
    TCandidate* fTheMother;  //! Do not stream
    
  protected:
    
    //--------------------
    // Instance Members --
    //--------------------
    
    // Counted reference to the vertex
    VAbsVertex* fDecayVtx;	    //! Vertex
    
    // Identity
    const TParticlePDG* fPdtEntry;  //! Pointer to particle database
    int   fPdgCode;
    
    // is this a resonance ?
    Bool_t fIsAResonance;	    //! Rsonance flag
    
    // Monte-Carlo truth 
    VAbsTruth* fTruth;		    //!  Pointer to MCTruth info          
    
    // Interface to objects storable in micro database
    VAbsMicroCandidate* fMicroCand; // !Pointer to micro data
    
    UInt_t fTrackNumber;  //! Micro association

    // Daughters
    TCandList *fDaugList;	    //! List of daughters
    //    TCandidate* fDaughters[15];	    //! Array of daughters
    std::vector<TCandidate*>  fDaughters; //! List of Daughters
    Short_t nDaug;		    //! Number of daughters
    
    // Constraints
    TConstraint* fConstraints[5];   //! Array of constraints
    Short_t nCons;		    //! Number of constraints
    
    UInt_t fMarker[4];    //! Overlap
    
    // added by K Goetzen
    double fPidLH[30];
    int    fMcIdx;
    int    fMcMotherIdx; 
    
  public:
    
    //--------------------
    // Public interface --
    //--------------------
    
    //
    // Constructors
    //
    
    /** Default ctor. Does not set particle type, etc. */
    TCandidate();
    
    /** Ctor from a momentum vector, charge, and origin vertex.
     Does not set the particle type, but uses the mass from the 
     argument 4vector.
     @param v      A THepLorentzVector representing the 4momentum
     @param charge The candidates charge, a Double_t so you can represent quarks
     @param vp     Pointer to the vertex where the candidate was created. The default zero value means to assume the origin
     */
    TCandidate( const TLorentzVector &v, 
               Double_t charge = 0, 
               VAbsVertex* vp = 0 );
    
    /** Ctor from a 3momentum vector, charge, and origin vertex.
     The particle type (hence mass) is set to pion for charged, photon for neutrals 
     @param v      A TVector3 representing the 3momentum
     @param charge The candidates charge, a Double_t so you can represent quarks
     @param vp     Pointer to the vertex where the candidate was created. The default zero value means to assume the origin
     */
    TCandidate( const TVector3 &v, 
               Double_t charge, 
               VAbsVertex* vp=0 );
    
    /** Ctor from a momentum vector, particle type, and origin vertex.
     The particle type to pion for charged, photon for neutrals 
     @param v      A TVector3 representing the momentum
     @param pdt    A pointer to a PdtEntry for the charge, type, etc.
     @param vp     Pointer to the vertex where the candidate was created. The default zero value means to assume the origin
     */
    TCandidate( const TVector3 &v, 
               const TParticlePDG* pdt, 
               VAbsVertex* vp=0 );
    
    
    /** Copy ctor */
    TCandidate( const TCandidate & );
    
    // Special constructor from MicroCandidate
    TCandidate(VAbsMicroCandidate &a, Int_t n, VAbsVertex* vp=0, Bool_t fast= kFALSE);
    // Special constructor from McTruth
    TCandidate(VAbsTruth &truth, Int_t n = 0, VAbsVertex* vp=0, Bool_t fast=kTRUE );
    
    TCandidate( TLorentzVector p4, 
               TError& p4Err,
               TCandListIterator& iterDau,
               VAbsVertex& theVertex,
               const TParticlePDG* hypo = 0);
    //
    // Destructor
    //
    virtual ~TCandidate();
    
    //
    // Accessors to contained information
    //
    Double_t             Mass() const;
    Double_t             GetMass() const { return Mass(); }
    
    const TParticlePDG*  PdtEntry() const;
    int 				PdgCode() {return fPdgCode;}
    
    //
    // By convention, the 4-momentum is given :
    //  - at the point of closest approach from the origin for non-composites
    //  - at the decay vertex for composite candidates
    //
    
    // The accessors and modifiers have been noved to TFitParams
    
    //
    // accessors to specific information (reconstruction, Monte-Carlo)
    //
    
    const BtaRecoPointers*  getRecoPointers() const { return 0; }
    const TrkAbsFit*        trkAbsFit()       const { return 0; }
    const TrkRecoTrk*       recoTrk()         const { return 0; }
    const TrkCompTrk*       compTrk()         const { return 0; }
    const AbsRecoCalo*      recoCalo()        const { return 0; }
    const PidInfoSummary*   pidInfoSummary()  const { return 0; }
    
    // information stored in microDB
    const BtaMicroAdapter*  getMicroAdapter() const { return 0; }
    
    //
    // adaptor to the general interface for vertexing
    //
    const BtaAbsRecoObject* recoObject() const { return 0; }
    
    // This method returns the pointer to the Consistency object along with 
    // ownership. The client is responsible for deleting the result.  
    //
    TConsistency* Consistency( const TParticlePDG* pdt ) const;
    
    //
    //  Modifiers
    //
    // Set type of candidate from a particle data table entry.
    // The setType function sets the type and therefore the mass
    // for any TCandidate which has no daughter.
    // Otherwise, i.e. for composite TCandidates,
    // the setType function only sets the type, _the mass is not set_.
    void SetType( const TParticlePDG* pdt );
    void SetType( const char* name );
    void SetType( int pdgcode );
    
    // Set the mass
    // Inactive for non composite candidates
    void SetMass( Double_t newMass );
    
    // Set the energy (at constant mass, by default)
    void SetEnergy( Double_t newE );
    
    // Set the momentum (at constant mass, by default)
    void SetMomentum( Double_t newP );
    
    void SetMicroCandidate(VAbsMicroCandidate &micro) { fMicroCand = &micro; }
    // Allow the candidate to fly or not to fly
    // This overrides the default which is to consider the
    // candidate a resonance (a non-flying state) if it
    // has a proper decay length c*tau of less than a nanometer
    // according to the pdt table.
    // This function MUST be called before any call to setVertex,
    // or inconsistencies might occur.  A inconsistent call to 
    // one of these functions when the decay vertex is already set
    // will abort.
    void SetFly();
    void SetNoFly();
    
    // Set and get TMcTruth
    void SetMcTruth( VAbsTruth* mctruth );
    const VAbsTruth* McTruth() const;
    
    //
    //  Genealogy
    //  
    
    // access to the mother
    const TCandidate* TheMother() const { return fTheMother; }
    TCandidate* TheMother() { return fTheMother; }
    // access to daughters
    Int_t NDaughters() const;
    TCandListIterator DaughterIterator() const;
    TCandidate* Daughter(Int_t n);
    void RemoveAssociations();
    
    Bool_t IsComposite()  const; // true when nDaugthers > 0
    Bool_t IsAResonance() const; // true when c*tau is very small ( < 1nm )
    
    //
    //  Access to vertex information
    //
    
    const VAbsVertex*    DecayVtx() const;
    VAbsVertex*          DecayVtx();
    const VAbsVertex*    ProductionVtx() const; 
    VAbsVertex*          ProductionVtx(); 
    
    //  recursive function that invalidates the vertex fit.
    //  it sets all the vertices to the UnFitted status.
    //  warning : the function may trig lots of copies and clones.
    void InvalidateFit();
    
    //
    // Check for overlap
    //
    
    // the function isCloneOf  returns true if the two 
    // Candidates have shared in the past a common Base.
    // For instance, an identified muon candidate is the
    // clone of the original Candidate it originates from
    // (note that "clone" must be taken in an enlarged sense
    // here. In that example for instance, the two clones have
    // different masses).
    Bool_t IsCloneOf( const TCandidate&, Bool_t checkType = kFALSE ) const;
    
    // two Candidates are equal if they share the same Base
    Bool_t operator==( const TCandidate& ) const;
    
    // and different if they don not
    Bool_t operator!=( const TCandidate& ) const;
    
    // this function returns the pointer of the first clone 
    // of a given Candidate found in the decay tree of the
    // present Candidate, if present, else the null pointer.
    const TCandidate* CloneInTree( const TCandidate& ) const;
    
    //
    // Locks
    //
    // set the flag that prevents a cand from being changed
    
    void Lock() { fLocked = kTRUE; }
    
    //
    // Constraints
    //
    
    // modifiers
    TConstraint& AddConstraint( TConstraint& );
    TConstraint& AddConstraint( TConstraint::Type );
    void RemoveConstraint( const TConstraint& );
    void RemoveConstraint( TConstraint::Type );
    
    // access
    Int_t NConstraints() const;
    Int_t NumberOfConstraints() const { return NConstraints(); }
    const TConstraint* Constraint( Int_t i ) ;
    const TConstraint* Constraint( TConstraint::Type ) const;
    
    //
    //  Origin Point
    //    -  The origin point is the position of  the production vertex
    //        when present, else the "origin" (0.,0.,0.)
    //    -  The origin point is not necessarily the point 
    //        where the 4-momentum P4() is defined.  For composite
    //        candidates for instance, P4() is given at the decay point,
    //        not the production point.
    //    -  To get the momentum and covariance at the origin point :
    //
    //          TLorentzVector p4origin = cand->p4( cand->origin() );
    //
    
    TVector3 Origin() const;
    
    //
    // Operations
    //
    
    TCandidate& operator=( const TCandidate & );
    
    //
    // Prints
    //
    void PrintOn( std::ostream& o=std::cout ) const;
    
    void SetFast(Bool_t yesno) { fFastMode = yesno; }
    Bool_t IsFast() const { return fFastMode; }
    
    VAbsMicroCandidate& GetMicroCandidate() const { return *fMicroCand; }
    
    TCandidate* Combine(const TCandidate& c);
    //************** added Combine for more candidates K.Goetzen, 05/2008
    TCandidate* Combine(const TCandidate& c1,const TCandidate& c2);
    TCandidate* Combine(const TCandidate& c1,const TCandidate& c2,const TCandidate& c3);
    
    // two Candidates overlap if they are identical
    // (same pointers), equal (same Base),
    // clones (same Uid), representing a same
    // reconstructed object, or having daughters
    // that are overlapping
    
    Bool_t Overlaps(const TCandidate& c) const {
      return ( (fMarker[0] & c.fMarker[0])!=0 || (fMarker[1] & c.fMarker[1])!=0 ||
              (fMarker[2] & c.fMarker[2])!=0 || (fMarker[3] & c.fMarker[3])!=0 );
    }    	
    
    Bool_t Equals(const TCandidate& c) const {
      return ( (fMarker[0] == c.fMarker[0]) && (fMarker[1] == c.fMarker[1]) &&
              (fMarker[2] == c.fMarker[2]) && (fMarker[3] == c.fMarker[3]) );
    }    	
    
    UInt_t GetMarker(UInt_t m=0) const {
      if (m<4)
        return fMarker[m];
      else
        return 0;
    }
    
    void SetMarker(UInt_t l,UInt_t m);    
    void SetMarker(UInt_t n);
    
    Int_t GetTrackNumber()  const { return fTrackNumber; }
    Int_t Uid() const   { return fTrackNumber; }
    void  SetUid(UInt_t uid=0);
    
    // Set Constraints
    
    // Constrain the mass of the candidate to the given mass
    void SetMassConstraint(Double_t mass ) 
    { ::SetMassConstraint( *this, mass ); }
    
    // Constrain the mass of the candidate to its type mass
    void SetMassConstraint()
    { ::SetMassConstraint( *this ); }
    
    // Constrain the lifetime of the candidate (c*tau, in centimeters)
    void SetLifetimeConstraint( Double_t ctau )
    { ::SetLifetimeConstraint( *this, ctau ); }
    
    // Constrain the lifetime of the candidate to its type lifetime
    void SetLifetimeConstraint()
    { ::SetLifetimeConstraint( *this ); }
    
    // Constrain the energy of the candidate
    void SetEnergyConstraint( TDoubleErr &energy,const TVector3& boost=TVector3(0,0,0) )
    { ::SetEnergyConstraint( *this, energy,boost ); }
    
    void SetEnergyConstraint( TVectorErr& eMinusMom, TVectorErr& ePlusMom, Double_t scale=0.5)
    { ::SetEnergyConstraint( *this, eMinusMom, ePlusMom, scale); }
    
    void SetEnergyConstraint( TCandidate& cand, const TEventInfo* ev, Double_t scale=0.5)
    { ::SetEnergyConstraint( *this, ev, scale); }
    
    // Constraint the momentum of the candidate
    void SetMomentumConstraint()
    { ::SetMomentumConstraint( *this ); }
    
    void SetMomentumConstraint( const TVectorErr& momentum, 
                               const TVector3& boost=TVector3(0,0,0) )
    { ::SetMomentumConstraint( *this,momentum,boost); }
    
    // Beam Constrain - 
    // Constrain the head of a decay tree to come from a given vertex
    void SetBeamConstraint( const TPointErr& beam )
    { ::SetBeamConstraint( *this, beam ); }
    
    void SetBeamConstraint( const TEventInfo* ev)
    { ::SetBeamConstraint( *this, ev); }
    
    void SetTrajectory(const TLorentzVector& p4, const TError& p4Err,
                       Int_t charge,const TParticlePDG* hypo,
                       VAbsVertex* dVtx );
    
    // Add a daughter link
    // **** put to public K Goetzen
    void AddDaughterLinkSimple( const TCandidate* );
    
    void SetPidInfo(double *pidinfo=0);
    void SetPidInfo(int hypo, double value);
    double GetPidInfo(int hypo);
    const double* GetPidInfo() const;
    
    void SetMcIdx(int idx) {fMcIdx=idx;}
    int GetMcIdx() {return fMcIdx;}
    void SetMcMotherIdx(int idx) {fMcMotherIdx=idx;}
    int GetMcMotherIdx() {return fMcMotherIdx;}

    // private:
    //
    // Private functions (access to friends only)
    //
    
    // Jan.2012: [R.K.] Need public access for PndAnalysis
    
  public:
    Bool_t IsLocal() const { return kTRUE; }
    
    // Set the decay vertex - operators can do that 
    void SetDecayVtx( VAbsVertex*  theVtx ) ;
    
    // Sets the mother link
    void SetMotherLink( TCandidate* m );
    
    // Drop the mother link
    void DropMotherLink();
    
    // Add a daughter link
    void AddDaughterLink( const TCandidate* );
    
    // Remove a daughter 
    void RemoveDaughter( TCandidate* );
    
    // fill the list with outgoing candidates (recursive function)
    void AddToVertexingList( TCandList& );
    
  public:
    ClassDef(TCandidate,1) // Candidate base class
    
    friend class PAFReader;
    friend class KangaReader;
    friend class TBooster;
    friend class TOperatorBase;
  };

// standalone print
std::ostream&  operator << (std::ostream& o, const TCandidate&);

#endif

