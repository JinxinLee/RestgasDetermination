#ifndef RHOCANDIDATE_H
#define RHOCANDIDATE_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// RhoCandidate                                                         //
//                                                                      //
// Contains (polymorphic) contents for RhoCandidate objects             //
// Candidate "Tracks" or "Particles" for analysis use                   //
//                                                                      //
// Author List:                                                         //
// Sascha Berger, RUB, Feb.99                                           //
// Marcel Kunze,  RUB, Aug.99                                           //
// Copyright (C) 1999-2001, Ruhr-University Bochum.                     //
// Ralf Kliemt, HIM/GSI Feb.2013 (Cleanup & Restructuring)              //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <vector>

#include "TNtuple.h"
#include "TVector3.h"
#include "RhoMath/RhoVector3Err.h"
#include "TLorentzVector.h"
#include "TObject.h"
#include "RhoBase/RhoCandList.h"
#include "FairRecoCandidate.h"
#include "RhoMath/RhoLorentzVectorErr.h"
#include "FairMultiLinkedData_Interface.h"

//class VAbsVertex;
//class VAbsTruth;
class TParticlePDG;
class RhoError;
class RhoCandListIterator;

#define MATRIXSIZE 28
#define MAXDAUG 10


//      ---------------------
//      -- Class Interface --
//      ---------------------
class RhoCandidate : public FairMultiLinkedData_Interface
{

   protected:

    //--------------------
    // Instance Members --
    //--------------------

     // Fast mode : Do not use error matrix
    Bool_t fFastMode;  //! Do not stream

    // Lock : if true, modifications are a fatal error
    Bool_t fLocked;  //! Do not stream

    // The mother
    RhoCandidate* fTheMother;  //! Do not stream
   // Counted reference to the vertex
    RhoVector3Err fDecayVtx;       //! Vertex

    // Identity
    const TParticlePDG* fPdtEntry;  //! Pointer to particle database
    int   fPdgCode;

    // is this a resonance ?
    Bool_t fIsAResonance;       //! Rsonance flag

    // Monte-Carlo truth
//    VAbsTruth* fTruth;            //!  Pointer to MCTruth info

    // Interface to objects storable in micro database
    FairRecoCandidate* fMicroCand; // !Pointer to micro data

    UInt_t fTrackNumber;  //! Micro association
    UInt_t fUid;  //! unique number

    // Daughters
//    std::vector<RhoCandidate*> fDaughters; //! List of Daughters
    RhoCandidate* fDaughters[MAXDAUG]; //! List of Daughters
	Int_t fNDaug;
    // Constraints
//    TConstraint* fConstraints[5];   //! Array of constraints
    Short_t fNCons;          //! Number of constraints

    UInt_t fMarker[4];    //! Overlap

    // added by K Goetzen
    double fPidLH[30];

  private:
    Double_t fChi2;

    //[ralfk:may2013] changed mc truth access to direct pointers
    //int    fMcIdx;
    RhoCandidate* fMcTruth;//! pointer, objects created by RhoFactory

    RhoCandidate* fFit; //! pointer, objects created by RhoFactory
    // the params
    Char_t   fCharge;             // The electrical charge
    Float_t  fXposition;          // The origin in x
    Float_t  fYposition;          // The origin in y
    Float_t  fZposition;          // The origin in z
    Double_t fXmomentum;          // The momentum in x
    Double_t fYmomentum;          // The momentum in y
    Double_t fZmomentum;          // The momentum in z
    Double_t fEnergy;             // The total energy
    Float_t  fErrP7[MATRIXSIZE];  // The symmetric 7*7 error matrix

  public:

    //--------------------
    // Public interface --
    //--------------------

    //
    // Constructors
    //

    /** Default ctor. Does not set particle type, etc. */
    RhoCandidate();

    /** Ctor from a momentum vector, charge, and origin vertex.
     Does not set the particle type, but uses the mass from the
     argument 4vector.
     @param v      A THepLorentzVector representing the 4momentum
     @param charge The candidates charge, a Double_t so you can represent quarks
     @param vp     Pointer to the vertex where the candidate was created. The default zero value means to assume the origin
     */
    RhoCandidate ( const TLorentzVector& v,
                   Double_t charge = 0,
                   RhoVector3Err* vp = 0 );

    /** Ctor from a momentum vector, particle type, and origin vertex.
     The particle type to pion for charged, photon for neutrals
     @param v      A TVector3 representing the momentum
     @param pdt    A pointer to a PdtEntry for the charge, type, etc.
     @param vp     Pointer to the vertex where the candidate was created. The default zero value means to assume the origin
     */
    RhoCandidate ( const TVector3& v,
                   const TParticlePDG* pdt,
                   RhoVector3Err* vp=0 );


    /** Copy ctor */
    RhoCandidate ( const RhoCandidate& );
    //RhoCandidate ( const RhoCandidate* );

    // Special constructor from MicroCandidate
    RhoCandidate ( FairRecoCandidate& a, Int_t n);
    RhoCandidate ( FairRecoCandidate& a, Int_t n, RhoVector3Err& vp, Bool_t fast= kFALSE );

//     RhoCandidate ( TLorentzVector p4,
//                    RhoError& p4Err,
//                    RhoCandListIterator& iterDau,
//                    RhoVector3Err& theVertex,
//                    const TParticlePDG* hypo = 0 );

    //
    // Destructor
    //
    virtual ~RhoCandidate();

    //
    // Accessors to contained information
    //
    Double_t             Mass() const;
    Double_t             GetMass() const { return Mass(); }

    const TParticlePDG*  PdtEntry() const;
    int                 PdgCode() {return fPdgCode;}

    //
    // By convention, the 4-momentum is given :
    //  - at the point of closest approach from the origin for non-composites
    //  - at the decay vertex for composite candidates
    //

    // The accessors and modifiers have been noved from TFitParams
    // accessors
    Double_t    GetCharge()   const { return fCharge; }
    Double_t    Charge()   const { return fCharge; }
    TVector3    GetPosition()   const { return TVector3 ( fXposition,fYposition,fZposition ); }
    TVector3    Pos()   const { return GetPosition(); } // Position where the fourmomentum is defined
    TVector3    GetDecayPos() const { return TVector3(fDecayVtx); } // position of decay (set by vertexing, if applicable)
    TVector3    GetMomentum()   const { return TVector3 ( fXmomentum,fYmomentum,fZmomentum ); }
    TVector3    GetVect()   const { return TVector3 ( fXmomentum,fYmomentum,fZmomentum ); }
    Double_t    Px() const { return fXmomentum; }
    Double_t    Py() const { return fYmomentum; }
    Double_t    Pz() const { return fZmomentum; }
    RhoVector3Err   PosWCov()   const;
    RhoError   PosCov()   const;
    TLorentzVector   P4()   const { return TLorentzVector ( fXmomentum,fYmomentum,fZmomentum,fEnergy ); }
    RhoError   P4Cov()   const;
    RhoError   P4Err()   const { return P4Cov(); }
    RhoLorentzVectorErr P4WErr()  const;
    TVector3    P3()   const { return GetMomentum(); }
    RhoError   P3Cov()   const;
    RhoVector3Err  P3WErr()  const;
    Double_t    E()   const { return fEnergy; }
    Double_t    Energy() const  { return fEnergy; }
    Double_t    GetEnergy() const  { return fEnergy; }
    Float_t*    GetErrP7() { return fErrP7; }
    Double_t    EVar()   const;
    Double_t    M()   const;
    Double_t    P()   const;
    TMatrixD   Cov7()   const;
    TMatrixD   XPCov()   const;
    Double_t    Pt()  const { return TMath::Sqrt ( fXmomentum*fXmomentum+fYmomentum*fYmomentum ); }
    TMatrixD    GetDecayPosCov() const {return TMatrixD(fDecayVtx.CovMatrix());}


    //  Modifiers
    //
    // Set type of candidate from a particle data table entry.
    // The setType function sets the type and therefore the mass
    // for any RhoCandidate which has no daughter.
    // Otherwise, i.e. for composite RhoCandidates,
    // the setType function only sets the type, _the mass is not set_.
    void SetType ( const TParticlePDG* pdt );
    void SetType ( const char* name );
    void SetType ( int pdgcode );

    // modifiers


    void SetCharge ( Double_t charge ) { fCharge = ( Char_t ) charge; }
    void SetMass ( Double_t mass );
    void SetEnergy ( Double_t newE );// Set the energy (at constant mass, by default)
    void SetE ( Double_t energy ) { fEnergy = energy; }
    void SetMassAndEnergy ( Double_t mass, Double_t energy );
    void SetPosition ( const TVector3& pos );
    void SetPos ( const TVector3& pos ) { SetPosition ( pos ); }
    void SetMomentum ( Double_t newP );// Set the momentum (at constant mass, by default)
    void SetP3 ( const TVector3& p3 );
    void SetVect ( const TVector3& p3 ) { SetP3 ( p3 ); }
    void SetP4 ( Double_t mass, const TVector3& p3 );
    void SetP4 ( const TLorentzVector& p4 );
    void SetP7 ( const TVector3& pos, const TLorentzVector& p4 );
    void SetCovPos ( const TMatrixD& covPos);
    void SetCovP4 ( const TMatrixD& covP4 );
    void SetCov7 ( const TMatrixD& cov7 );
    void SetCov7 ( const TMatrixD& covPos, const TMatrixD& covP4 );
    void SetCov7 ( const TMatrixD& covPos, const TMatrixD& covP4,const TMatrixD& covPosP4 );
    void Set ( const TVector3& pos, const TLorentzVector& p4, const TMatrixD& cov7 );
    void Set ( Double_t mass,const RhoVector3Err& posErr, const RhoVector3Err& p3Err, const TMatrixD& xpErr );
    void SetErr ( Float_t* err ) {if ( err!=0 ) for ( int i=0; i<MATRIXSIZE; i++ ) { fErrP7[i] = err[i]; }}

    void SetRecoCandidate ( FairRecoCandidate& micro ) { fMicroCand = &micro; }
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

    void Boost ( Double_t bx, Double_t by, Double_t bz );
    void Boost ( const TVector3& p )    { Boost ( p.X(),p.Y(),p.Z() ); }

    //
    //  Genealogy
    //

    // access to the mother
    const RhoCandidate* TheMother() const { return fTheMother; }
    RhoCandidate* TheMother() { return fTheMother; }
    // access to daughters
    Int_t NDaughters() const;
    //RhoCandListIterator DaughterIterator() const;
    //const RhoCandidate* Daughter ( Int_t n ) const;
    RhoCandidate* Daughter ( Int_t n );
    void RemoveAssociations();

    Bool_t IsComposite()  const; // true when nDaugthers > 0
    Bool_t IsAResonance() const; // true when c*tau is very small ( < 1nm )

    //
    //  Access to vertex information
    //

    RhoVector3Err DecayVtx() {return fDecayVtx;};

    Double_t Chi2() const {return fChi2;}
    void SetChi2 ( Double_t chi2 ) {fChi2=chi2;}
    void SetFit ( RhoCandidate* b ) {fFit=b;}
    RhoCandidate* GetFit() const { return fFit;}
    //  recursive function that invalidates the vertex fit.
    //  it sets all the vertices to the UnFitted status.
    //  warning : the function may trig lots of copies and clones.
    void InvalidateFit(); //defunct a.t.m.

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
    Bool_t IsCloneOf ( const RhoCandidate&, Bool_t checkType = kFALSE ) const;

    // two Candidates are equal if they share the same Base
    Bool_t operator== ( const RhoCandidate* ) const;

    // and different if they don not
    Bool_t operator!= (  RhoCandidate* ) const;

    // this function returns the pointer of the first clone
    // of a given Candidate found in the decay tree of the
    // present Candidate, if present, else the null pointer.
    const RhoCandidate* CloneInTree ( const RhoCandidate& ) const;

    //
    // Locks
    //
    // set the flag that prevents a cand from being changed

    void Lock() { fLocked = kTRUE; }
    void UnLock() { fLocked = kFALSE; }
    Bool_t IsLocked() {return fLocked;}
    //
    // Constraints
    //

    // modifiers
//    TConstraint& AddConstraint( TConstraint& );
//    TConstraint& AddConstraint( TConstraint::Type );
//    void RemoveConstraint( const TConstraint& );
//    void RemoveConstraint( TConstraint::Type );

    // access
//    Int_t NConstraints() const;
//    Int_t NumberOfConstraints() const { return NConstraints(); }
//    const TConstraint* Constraint( Int_t i ) ;
//    const TConstraint* Constraint( TConstraint::Type ) const;

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

    RhoCandidate& operator= ( const RhoCandidate& );

    //
    // Prints
    //
    void PrintOn ( std::ostream& o=std::cout ) const;

    void SetFast ( Bool_t yesno ) { fFastMode = yesno; }
    Bool_t IsFast() const { return fFastMode; }

    FairRecoCandidate* GetRecoCandidate() const { return fMicroCand; }

    RhoCandidate* Combine ( RhoCandidate* c );
    //************** added Combine for more candidates K.Goetzen, 05/2008
    RhoCandidate* Combine ( RhoCandidate* c1, RhoCandidate* c2 );
    RhoCandidate* Combine ( RhoCandidate* c1, RhoCandidate* c2, RhoCandidate* c3 );
    RhoCandidate* Combine ( RhoCandidate* c1, RhoCandidate* c2, RhoCandidate* c3, RhoCandidate* c4 );
    RhoCandidate* Combine ( RhoCandidate* c1, RhoCandidate* c2, RhoCandidate* c3, RhoCandidate* c4, RhoCandidate* c5 );
    RhoCandidate* Combine ( RhoCandidate* c1, RhoCandidate* c2, RhoCandidate* c3, RhoCandidate* c4, RhoCandidate* c5, RhoCandidate* c6 );
    RhoCandidate* Combine ( RhoCandidate* c1, RhoCandidate* c2, RhoCandidate* c3, RhoCandidate* c4, RhoCandidate* c5, RhoCandidate* c6, RhoCandidate* c7 );
    RhoCandidate* Combine ( RhoCandidate* c1, RhoCandidate* c2, RhoCandidate* c3, RhoCandidate* c4, RhoCandidate* c5, RhoCandidate* c6, RhoCandidate* c7, RhoCandidate* c8 );
    RhoCandidate* Combine ( RhoCandidate* c1, RhoCandidate* c2, RhoCandidate* c3, RhoCandidate* c4, RhoCandidate* c5, RhoCandidate* c6, RhoCandidate* c7, RhoCandidate* c8, RhoCandidate* c9 );
    RhoCandidate* Combine ( RhoCandidate* c1, RhoCandidate* c2, RhoCandidate* c3, RhoCandidate* c4, RhoCandidate* c5, RhoCandidate* c6, RhoCandidate* c7, RhoCandidate* c8, RhoCandidate* c9, RhoCandidate* c10 );

    // two Candidates overlap if they are identical
    // (same pointers), equal (same Base),
    // clones (same Uid), representing a same
    // reconstructed object, or having daughters
    // that are overlapping

    Bool_t Overlaps ( const RhoCandidate* c ) const {
      return ( ( fMarker[0] & c->fMarker[0] ) !=0 || ( fMarker[1] & c->fMarker[1] ) !=0 ||
               ( fMarker[2] & c->fMarker[2] ) !=0 || ( fMarker[3] & c->fMarker[3] ) !=0 );
    }

    Bool_t Equals ( const RhoCandidate* c ) const {
      return ( ( fMarker[0] == c->fMarker[0] ) && ( fMarker[1] == c->fMarker[1] ) &&
               ( fMarker[2] == c->fMarker[2] ) && ( fMarker[3] == c->fMarker[3] ) );
    }

    UInt_t GetMarker ( UInt_t m=0 ) const {
      if ( m<4 ) {
        return fMarker[m];
      } else {
        return 0;
      }
    }

    void SetMarker ( UInt_t l,UInt_t m );
    void SetMarker ( UInt_t n );

    Int_t GetTrackNumber()  const { return fTrackNumber; }
    void  SetTrackNumber ( Int_t trnum=-1 ) {fTrackNumber=trnum; };
    Int_t Uid() const   { return fUid; }
    void  SetUid ( UInt_t uid=0 );


//     void SetTrajectory ( const TLorentzVector& p4, const RhoError& p4Err,
//                          Int_t charge,const TParticlePDG* hypo,
//                          RhoVector3Err dVtx );


    void SetPidInfo ( double* pidinfo=0 );
    void SetPidInfo ( int hypo, double value );
    double GetPidInfo ( int hypo );
    const double* GetPidInfo() const;

    //[ralfk:may2013] changed mc truth access to direct pointers
    //void SetMcIdx ( int idx ) {fMcIdx=idx;}
    //int GetMcIdx() {return fMcIdx;}
    void SetMcTruth ( RhoCandidate* mct ) {fMcTruth=mct;}
    RhoCandidate* GetMcTruth() const {return fMcTruth;}

    Bool_t IsLocal() const { return kTRUE; }

    // Set the decay vertex - operators can do that
    void SetDecayVtx ( RhoVector3Err theVtx ) ;

    // Sets the mother link and adds a daughter link in the mother
    void SetMotherLink ( RhoCandidate* m , bool verbose=true );

    // Drop the mother link
    void DropMotherLink();

    // Add a daughter link and set the daughters mother link
    //void AddDaughterLink ( const RhoCandidate* );

    // Add a daughter link without touching the daughters
    void AddDaughterLinkSimple ( const RhoCandidate*, bool verbose=true );

    // Remove a daughter
    void RemoveDaughter ( RhoCandidate* );

    Double_t Correlation ( Int_t x1,Int_t x2,const TMatrixD& m,const TMatrixD& cov ) const;

    ClassDef ( RhoCandidate,3 ) // Candidate base class

    //friend class TBooster;
    //friend class TOperatorBase;
};

// standalone print
std::ostream&  operator << ( std::ostream& o, const RhoCandidate& );

#endif
