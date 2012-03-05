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

#include <assert.h>

#include "RhoBase/TRho.h"
#include "RhoBase/TFactory.h"
#include "RhoBase/TCandidate.h"
#include "RhoBase/TFitParams.h"
#include "RhoBase/TCandList.h"
#include "RhoBase/TCandListIterator.h"
#include "RhoBase/VAbsVertex.h"
#include "RhoBase/VAbsMicroCandidate.h"
#include "RhoBase/VAbsTruth.h"


ClassImp(TCandidate)

#include <iostream>
#include <iomanip>
using namespace std;

TCandidate::TCandidate() :
fFastMode(kFALSE),
fLocked(kFALSE),
fTheMother(0),
fDecayVtx(0),
fPdtEntry(0),
fPdgCode(0),
fIsAResonance(kFALSE),
fTruth(0), 
fMicroCand(0),
fTrackNumber(-1),
fUid(0),
fDaugList(0),
nDaug(0),
nCons(0)
{
  fMarker[0] = fMarker[1] = fMarker[2] = fMarker[3] = 0;
  SetUid();
  SetPidInfo(0);
  SetMcIdx(-1);
  SetMcMotherIdx(-1);
}

TCandidate::TCandidate( const TLorentzVector &v, Double_t charge, VAbsVertex* vp ) : 
fFastMode(kFALSE),
fLocked(kFALSE), 
fTheMother(0),
fDecayVtx(vp), 
fPdtEntry(0), 
fPdgCode(0),
fIsAResonance(kFALSE),
fTruth(0),
fMicroCand(0),
fTrackNumber(-1),
fUid(0),
fDaugList(0),
nDaug(0),
nCons(0)
{ 
  fMarker[0] = fMarker[1] = fMarker[2] = fMarker[3] = 0;
  SetCharge( charge );
  SetP7( ( vp!=0 ? vp->Point() : TVector3(0.,0.,0.) ), v );
  SetUid();
  SetPidInfo(0);
  SetMcIdx(-1);
  SetMcMotherIdx(-1);
}


TCandidate::TCandidate( const TVector3 &v, Double_t charge, VAbsVertex* vp ) : 
fFastMode(kFALSE),
fLocked(kFALSE), 
fTheMother(0),
fDecayVtx(vp), 
fPdtEntry(0), 
fPdgCode(0),
fIsAResonance(kFALSE),
fTruth(0),
fMicroCand(0),
fTrackNumber(-1),
fUid(0),
fDaugList(0),
nDaug(0),
nCons(0)
{ 
  fMarker[0] = fMarker[1] = fMarker[2] = fMarker[3] = 0;
  TDatabasePDG *pdg = TRho::Instance()->GetPDG();    // Access particle DB
  TParticlePDG* pdt = 0;
  if( fabs(charge)<1.e-06 ) 
    pdt = pdg->GetParticle( "gamma" );
  else
    pdt = (charge>0) ? pdg->GetParticle( "pi+" ) : pdg->GetParticle( "pi-" );
  SetPos( vp!=0 ? vp->Point() : TVector3(0.,0.,0.) );
  SetP3(v);
  SetType( pdt );
  SetUid();
  SetPidInfo(0);
  SetMcIdx(-1);
  SetMcMotherIdx(-1);
}

TCandidate::TCandidate( const TVector3 &v, const TParticlePDG* pdt, VAbsVertex* vp ) : 
fFastMode(kFALSE),
fLocked(kFALSE), 
fTheMother(0),
fDecayVtx(vp), 
fPdtEntry(0), 
fPdgCode(0),
fIsAResonance(kFALSE),
fTruth(0),
fMicroCand(0),
fTrackNumber(-1),
fUid(0),
fDaugList(0),
nDaug(0),
nCons(0)
{
  fMarker[0] = fMarker[1] = fMarker[2] = fMarker[3] = 0;
  SetPos( vp!=0 ? vp->Point() : TVector3(0.,0.,0.) );
  SetP3(v);
  SetType( pdt );
  SetUid();
  SetPidInfo(0);
  SetMcIdx(-1);
  SetMcMotherIdx(-1);
}


TCandidate::TCandidate( const TCandidate& o ) 
{
  fFastMode = o.fFastMode;
  fLocked = kFALSE; 
  fTheMother = 0;
  fDecayVtx = o.fDecayVtx;
  fPdtEntry = o.fPdtEntry; 
	fPdgCode  = o.fPdgCode;
  fIsAResonance = o.fIsAResonance;
  fTruth = o.fTruth; 
  fTrackNumber = o.fTrackNumber;
  fUid = o.fUid;
  fMicroCand = o.fMicroCand;
  
  fMarker[0] = o.fMarker[0];
  fMarker[1] = o.fMarker[1];
  fMarker[2] = o.fMarker[2];
  fMarker[3] = o.fMarker[3];
  
  fCharge    = o.fCharge;
  fXposition = o.fXposition;
  fYposition = o.fYposition;
  fZposition = o.fZposition;
  fXmomentum = o.fXmomentum;
  fYmomentum = o.fYmomentum;
  fZmomentum = o.fZmomentum;
  fEnergy    = o.fEnergy;
  
  if (!fFastMode) {
    int i;
    for (i=0; i<MATRIXSIZE;i++) fErrP7[i] = o.fErrP7[i];
    for (i=0; i<5;i++) fParams[i] = o.fParams[i];
    for (i=0; i<15;i++) fCov[i] = o.fCov[i];
  }
  
  fDaugList = 0;
  nDaug = o.nDaug;
  fDaughters.clear();
  
  if (o.nDaug > 0) {
    for (int i=0;i<nDaug;i++) {
	    //****** changed due to std::list
	    //fDaughters[i] = o.fDaughters[i];
	    fDaughters.push_back(o.fDaughters[i]);
	    
	    fDaughters[i]->fTheMother = this;
    }
  }
  
  nCons = 0;
  if (o.nCons > 0) {
    for (int i=0;i<o.nCons;i++) AddConstraint(*o.fConstraints[i]);
  }
  
  for (int i=0;i<30;i++)
    fPidLH[i]=o.fPidLH[i];
  
  fMcIdx=o.fMcIdx;
  fMcMotherIdx=o.fMcMotherIdx;
}

// This is the special constructor to bring a TCandidate into
// life from the MicroCandidate

TCandidate::TCandidate(VAbsMicroCandidate &a, Int_t n, VAbsVertex* vp, Bool_t fast ) : 
fFastMode(fast),
fLocked(kFALSE), 
fTheMother(0),
fDecayVtx(vp),
fPdtEntry(0), 
fPdgCode(0),
fIsAResonance(kFALSE),
fTruth(0),
fMicroCand(&a),
fTrackNumber(-1),
fDaugList(0),
nDaug(0),
nCons(0)
{
  fMarker[0] = fMarker[1] = fMarker[2] = fMarker[3] = 0;
  // Set kinematics from MicroCandidate
  
  SetP4(a.GetLorentzVector());
  SetPos(a.GetPosition());
  SetCharge(a.GetCharge());
  
  if (!fFastMode) {
    const Float_t *err = a.GetErrorP7(); // 4 momentum & point
    const Float_t *cov = a.GetCov(); // helix cov
    const Float_t *par = a.GetParams(); // helix params
    int i;
    if (err!=0) for (i=0; i<MATRIXSIZE;i++) fErrP7[i] = err[i];
    if (par!=0) for (i=0; i<5;i++) fParams[i] = par[i];
    if (cov!=0) for (i=0; i<15;i++) fCov[i] = cov[i];
  }
  
  SetUid(n);//what about UID??? 
	
  SetPidInfo(0);
	SetPidInfo(0,a.GetElectronPidLH());
	SetPidInfo(1,a.GetMuonPidLH());
	SetPidInfo(2,a.GetPionPidLH());
	SetPidInfo(3,a.GetKaonPidLH());
	SetPidInfo(4,a.GetProtonPidLH());
	
  SetMcIdx(a.GetMcIndex());
  SetMcMotherIdx(-10); // STE: It needs some update, maybe
}


TCandidate::TCandidate(VAbsTruth &a, Int_t n, VAbsVertex* vp, Bool_t fast ) : 
fFastMode(fast),
fLocked(kFALSE), 
fTheMother(0),
fDecayVtx(vp),
fPdtEntry(0),
fPdgCode(0),
fIsAResonance(kFALSE),
fTruth(&a),
fMicroCand(0),
fTrackNumber(-1),
fDaugList(0),
nDaug(0),
nCons(0)
{
  fMarker[0] = fMarker[1] = fMarker[2] = fMarker[3] = 0;
  // Set kinematics from Truth
  SetP4(a.GetMomentum());
  SetPos(a.GetStartVertex());
  fPdtEntry = a.GetPdtEntry();
	if (fPdtEntry) fPdgCode  = a.GetPdtEntry()->PdgCode();
  if (!fPdtEntry) cout << 
    "TCandidate(VAbsTruth&,Int_t,VAbsVertex*) : Error: Could not get PdtEntry. No charge set"
    << endl;
  SetCharge( fPdtEntry ? fPdtEntry->Charge() : 0 );
  
  // Set the markers according to the matched track
  // The match index is -1 in case of no matching track
  if (a.GetAodMatchIndex()>=0) SetMarker(a.GetAodMatchIndex());
  
  SetMcTruth(&a); // Note the underlying truth object, MK jan.2k
  SetPidInfo(0);
  SetMcIdx(-1);
  SetMcMotherIdx(-1);
}

//--------------
// Destructor --
//--------------

TCandidate::~TCandidate( )
{
  RemoveAssociations();
}

//--------------
// Operations --
//--------------

// assignment double
TCandidate& 
TCandidate::operator = (const TCandidate& o) 
{
  fFastMode = o.fFastMode;
  fLocked = kFALSE; 
  fTheMother = 0; 
  fDecayVtx = o.fDecayVtx;
  fPdtEntry = o.fPdtEntry; 
	fPdgCode  = o.fPdgCode;
  fIsAResonance = o.fIsAResonance;
  fTruth = o.fTruth; 
  fTrackNumber = o.fTrackNumber;
  fUid = o.fUid;
  fMicroCand = o.fMicroCand;
  
  fMarker[0] = o.fMarker[0];
  fMarker[1] = o.fMarker[1];
  fMarker[2] = o.fMarker[2];
  fMarker[3] = o.fMarker[3];
  
  fCharge    = o.fCharge;
  fXposition = o.fXposition;
  fYposition = o.fYposition;
  fZposition = o.fZposition;
  fXmomentum = o.fXmomentum;
  fYmomentum = o.fYmomentum;
  fZmomentum = o.fZmomentum;
  fEnergy    = o.fEnergy;
  
  if (!fFastMode) {
    int i;
    for (i=0; i<MATRIXSIZE;i++) fErrP7[i] = o.fErrP7[i];
    for (i=0; i<5;i++) fParams[i] = o.fParams[i];
    for (i=0; i<15;i++) fCov[i] = o.fCov[i];
  }
  
  fDaugList = 0;
  nDaug = o.nDaug;
  if (o.nDaug > 0) {
    for (int i=0;i<nDaug;i++) {
	    fDaughters[i] = o.fDaughters[i];
	    fDaughters[i]->fTheMother = this;
    }
  }
  
  nCons = 0;
  if (o.nCons > 0) {
    for (int i=0;i<o.nCons;i++) AddConstraint(*o.fConstraints[i]);
  }
  
  for (int i=0;i<30;i++)
    fPidLH[i]=o.fPidLH[i];
  
  fMcIdx=o.fMcIdx;  
  fMcMotherIdx=o.fMcMotherIdx;
  
  return *this;
}

void 
TCandidate::SetMotherLink( TCandidate* m )
{
  assert( m!=0 );  // what kind of mother is it ?!
  
  // already a mother ?  not allowed !
  assert( fTheMother==0 );
  
  // should not be called for a local candidate...
  //assert( !isLocal() );
  
  // commented by KG, 2/2012
/*  if (m->nDaug>=5) {
    cerr << "TCandidate::SetMotherLink: Can not add more than 5 daughters." << endl;
    return;
  }*/
  
  // set the mother link
  fTheMother = m;
  
  // ... and the mother's daughter link
  //fTheMother->fDaughters.push_back(this);
  fTheMother->AddDaughterLinkSimple(this);
  // No markers set. MC candidates will not be "combined".
  
  // special for MC trees
  if( fTheMother==0 ) fTheMother=m;
  
  // if it's a resonance and the mother has already a decay vertex,
  // set the decay vertex of the mother to the daughter
  if( IsAResonance() && fTheMother->DecayVtx()!=0 && fTheMother->DecayVtx()!=DecayVtx())
  {
    SetDecayVtx( fTheMother->DecayVtx() );
    // if it is a resonance and the mother has no other vertex, the vertex of the reonance is given to her.  
  }
  
}

void 
TCandidate::DropMotherLink()
{
  // no mother
  if( fTheMother==0 ) return;
  
  // case of a non-local candidate
  
  //if( !isLocal() )
  { 
    // the mother looses this as a daughter
    for (int i=0;i<fTheMother->nDaug;i++)
	    if (fTheMother->fDaughters[i] == this) {
        fTheMother->nDaug--;
        fTheMother->fDaughters[i] = fTheMother->fDaughters[fTheMother->nDaug];
	    }
  }
  
  // OEdipus rex : kill the mother link
  fTheMother = (TCandidate*)0;
}

TConsistency*
TCandidate::Consistency( const TParticlePDG* pdt ) const
{
  return 0;
}

void 
TCandidate::AddToVertexingList( TCandList& outGoing )
{
  if( IsAResonance() )
  {
    for (int i=0;i<nDaug;i++) fDaughters[i]->AddToVertexingList( outGoing );
  }
  else
    outGoing.Put( *this );
}

const TCandidate*
TCandidate::CloneInTree( const TCandidate& c ) const 
{
  if( IsCloneOf( c ) ) return this;
  for (int i=0;i<nDaug;i++) 
  {
    const TCandidate* b = fDaughters[i]->CloneInTree( c );
    if( b!=0 ) return b;
  }
  return 0;
}


void
TCandidate::InvalidateFit() 
{
  // new function (GHM 05/99)
  // this function invalidates the fit to let fitters know
  // that if given this candidate, they have to refit it.
  
  VAbsVertex* theDecayVtx = DecayVtx();
  if( theDecayVtx!=0 )
  {
    if( theDecayVtx->Status() != VAbsVertex::UnFitted )
    {
	    // clone the vertex...
	    VAbsVertex* newDecayVtx = (VAbsVertex*) theDecayVtx->Clone();
	    
	    // ...and invalid the clone
	    newDecayVtx->SetStatus( VAbsVertex::UnFitted );
	    
	    // now set the new decay vertex
	    SetDecayVtx( newDecayVtx );
    }
  } 
  
  // and do that recursively
  for (int i=0;i<nDaug;i++) fDaughters[i]->InvalidateFit();
}

std::ostream&  
operator << (std::ostream& o, const TCandidate& a) 
{
  a.PrintOn(o); return o;
}

const VAbsVertex* TCandidate::ProductionVtx() const
{
  if( fTheMother!=0 ) return fTheMother->DecayVtx();
  else return 0;
}

Bool_t TCandidate::operator== (const TCandidate& c) const
{ return Equals(c) ; }

Bool_t TCandidate::operator!= (const TCandidate& c) const
{ return (*this!=c) ; }

// This comes from the content

void 
TCandidate::SetMass( Double_t m ) 
{ 
  // set the mass of a non-composite doesn't make sense
  // assert( IsComposite() );  
  // for the moment, just print an error message and go away
  //if( !IsComposite() && fPdtEntry!=0 )
  //  {
  //    return;
  //  }
  // sets the mass
  TFitParams::SetMass( m );
  
}

void 
TCandidate::SetEnergy(Double_t mEnergy)
{ 
  if( IsComposite() )
  {
    // this implementation leaves P unchanged; subclasses may differ
    TFitParams::SetEnergy( mEnergy );
  }
  else
  {
    SetMassAndEnergy( Mass(), mEnergy );
  }
}

void 
TCandidate::SetMomentum(Double_t newp) 
{ 
  // this implementation leaves mass unchanged; subclasses may differ
  Double_t pscale = ( P() == 0 ? 0 : newp / P() );
  SetP3( pscale*P3() );
}

void 
TCandidate::SetType( int pdgcode ) 
{
	fPdgCode=pdgcode;
  TDatabasePDG *pdg = TRho::Instance()->GetPDG();	// Access particle DB
  TParticlePDG* pdt;
  if( pdt=pdg->GetParticle( pdgcode ) ) SetType( pdt );
}

void 
TCandidate::SetType( const TParticlePDG *pdt ) 
{ 
  if( pdt==fPdtEntry || pdt==0 ) return;
  fPdtEntry = pdt;
	fPdgCode = pdt->PdgCode();
  
  //
  // by default :
  //   if the proper lifetime multiplied by light velocity is less
  //    than a nanometer, the candidate is considered a resonance
  //      (a state that does not fly)
  //
  fIsAResonance=kFALSE;
  if( fPdtEntry->Width()>1E-15 /*Lifetime()<1e-08*/ ) fIsAResonance=kTRUE;
  
  if( !IsComposite() )
  {
    
    // the mass has changed since the type has changed
    SetMass( Mass() );
    
    // set the charge
    SetCharge( pdt->Charge() );
  }
  else {
    if ( Charge()!=pdt->Charge()) {
	    cerr
      << "ERROR: attempt to call TCandidate::SetType(\""
      << pdt->ParticleClass() << "\") for a composite" << endl 
      << "       TCandidate whose daughters have total charge "
      << Charge() << endl;
	    //assert( Charge()==pdt->Charge() );
    }
  }
}



void 
TCandidate::SetMcTruth( VAbsTruth * mctruth )
{ 
  fTruth = mctruth;
}

void 
TCandidate::SetDecayVtx(VAbsVertex*  theVtx ) 
{ 
  
  // protection agains null pointers
  if( theVtx==0 ) return;
  
  //
  // Warning : this is a recursive algorithm
  //
  
  // this vertex is not already set
  if( fDecayVtx!=theVtx )
  { 
    // set the new decay vertex reference
    fDecayVtx = theVtx;
    
  }
  else
  {
    cerr << "trying to reset the same vertex ! " << endl;
    // this is used to refresh the vertex links
    //theVtx->_inComingCand=0;
    //theVtx->_outGoingCands.clear();
    //theVtx->_resonances.clear();
  }
  
  // update the vertex back links
  if( IsAResonance() ) 
    theVtx->SetResonances(this);
  else
  {
    //assert( theVtx->GetInComingCand()==0 );
    theVtx->SetInComingCand(this);
  }
  // now loop on daughters
  for (int i=0;i<nDaug;i++) 
  {
    if( fDaughters[i]->IsAResonance() ) 
      fDaughters[i]->SetDecayVtx( theVtx );
    else
      theVtx->SetOutGoingCands( fDaughters[i] );
  }
}

// --------------------------------------------------
// Geneology functions, no longer in a separate class
// --------------------------------------------------

Int_t 
TCandidate::NDaughters() const 
{
  return nDaug;
}

void 
TCandidate::AddDaughterLink( const TCandidate* cand ) 
{
  //assert( cand!=0 );
  
  // first copy the candidate
  TCandidate* d = const_cast<TCandidate*>(cand);
  
  // as soon as there are daughters, the charge is
  // given by the sum of the daughter charges
  if( nDaug==0 ) SetCharge(0);
  SetCharge( Charge()+cand->Charge() );
  
  // set the daughter's mother link 
  // ******** modified K Goetzen
  d->SetMotherLink(this);
  
  fMarker[0] |= d->GetMarker(0);
  fMarker[1] |= d->GetMarker(1);
  fMarker[2] |= d->GetMarker(2);
  fMarker[3] |= d->GetMarker(3);
}

void 
TCandidate::AddDaughterLinkSimple( const TCandidate* cand ) 
{
  //assert( cand!=0 );
  
  // first copy the candidate
  TCandidate* d = const_cast<TCandidate*>(cand);
  
  // as soon as there are daughters, the charge is
  // given by the sum of the daughter charges
  if( nDaug==0 ) SetCharge(0);
  SetCharge( Charge()+cand->Charge() );
	
  /*    if (nDaug>=5) {
   cerr << "TCandidate::SetMotherLinkSimple: Can not add more than 5 daughters." << endl;
   return;
   }*/
	
	// *** Changed experimentally to std::list
  //fDaughters[nDaug++] = d;
  
  fDaughters.push_back(d);
  nDaug=fDaughters.size();
  
  // set the daughter's mother link 
  // ******** modified K Goetzen
  //d->SetMotherLink(this);
  
  //fMarker[0] |= d->GetMarker(0);
  //fMarker[1] |= d->GetMarker(1);
  //fMarker[2] |= d->GetMarker(2);
  //fMarker[3] |= d->GetMarker(3);
}

void 
TCandidate::RemoveDaughter( TCandidate* d )
{
  // check for consistency
  assert( d!=0 && d->TheMother()==this );
  
  // the charge
  SetCharge( Charge() - d->Charge() );
  
  // destroy the daughter
  //delete d;
  
}


//
// Access functions
//

Double_t 
TCandidate::Mass() const    
{ 
  if( !IsComposite() && fPdtEntry!=0 ) 
    return fPdtEntry->Mass();
  else 
    return M(); 
}

TCandidate* 
TCandidate::Daughter(Int_t n)
{
  if (n >=0 && n < nDaug ) 
    return fDaughters[n];
  else
    return 0;
}

TCandListIterator
TCandidate::DaughterIterator() const
{
  static TCandList emptyList;
  static TCandListIterator empty(emptyList);
  if (NDaughters()==0) return empty;
  
  // return an iterator to the daughter list
  TCandList *l = const_cast<TCandList *>(fDaugList);
  if (l==0) l = new TCandList("DaugList",5);
  l->Cleanup();
  for (int i=0;i<nDaug;i++) l->Put(*fDaughters[i]);
  return TCandListIterator(*l);
}

TConstraint& 
TCandidate::AddConstraint( TConstraint& oneMore )
{
  TConstraint *newConstraint = new TConstraint(oneMore);
  fConstraints[nCons++] = newConstraint;
  return *newConstraint;
}

TConstraint& 
TCandidate::AddConstraint( TConstraint::Type  type )
{
  TConstraint oneMore( type );
  return AddConstraint( oneMore );
}

void 
TCandidate::RemoveConstraint( const TConstraint& oneLess )
{ 
}

void 
TCandidate::RemoveConstraint( TConstraint::Type type )
{ 
  TConstraint oneLess(type);
  RemoveConstraint( oneLess );
}


Int_t 
TCandidate::NConstraints() const
{
  return nCons;
}

const TConstraint* 
TCandidate::Constraint( Int_t i )
{
  if(i>=nCons) return 0;
  return fConstraints[i];
}

const TConstraint* 
TCandidate::Constraint( TConstraint::Type type ) const
{
  Short_t i;
  for( i=0; i<nCons; i++ )
  {
    if( type==fConstraints[i]->GetType() ) return fConstraints[i];
  }
  return 0;
}

// former inline implementations

VAbsVertex*    
TCandidate::ProductionVtx()
{
  if( fTheMother!=0 ) return fTheMother->DecayVtx();
  else return 0;
}

const VAbsVertex*    
TCandidate::DecayVtx() const 
{ return fDecayVtx; }

VAbsVertex*    
TCandidate::DecayVtx() 
{ return fDecayVtx; }

const TParticlePDG*        
TCandidate::PdtEntry() const 
{ 
	return fPdtEntry; 
}

const VAbsTruth*       
TCandidate::McTruth() const 
{ return fTruth; }

Bool_t
TCandidate::IsComposite() const 
{
  return nDaug>0;
}

Bool_t
TCandidate::IsAResonance() const
{
  return fIsAResonance;
}

void 
TCandidate::SetFly() 
{
  if( !fIsAResonance ) return;
  else
  {
    // can only change to new value is a vertex not already present
    //assert( fDecayVtx==0 );
    fIsAResonance = kFALSE;
  }
}

void 
TCandidate::SetNoFly()
{
  if( fIsAResonance ) return;
  else
  {
    // can only change to new value is a vertex not already present
    //assert( fDecayVtx==0 ); 
    fIsAResonance = kTRUE;
  }
}

TVector3 
TCandidate::Origin() const             
{ 
  return Pos();
}

void 
TCandidate::SetType(const char* name) 
{ 
  TDatabasePDG *pdg = TRho::Instance()->GetPDG();	// Access particle DB
  TParticlePDG* pdt;
  if( pdt=pdg->GetParticle( name ) ) SetType( pdt );
}

void TCandidate::PrintOn(std::ostream& o) const 
{
  o <<Uid()<< " (" << P4().X() << "," << P4().Y() << "," << P4().Z() << ";" << P4().T() <<  ") " << Charge() << " " << Mass() << " GeV/c2 ";
  o.setf(ios::hex | ios::showbase);
  o << " ("
	<< GetMarker(3)
	<< ","
	<< GetMarker(2)
	<< ","
	<< GetMarker(1)
	<< ","
	<< GetMarker(0)
	<< ") ";
  o.unsetf(ios::hex | ios::showbase);

  // print daughter links
  o << "d: "<< nDaug;
  for (int i=0;i<nDaug;i++) o << " " << fDaughters[i]->Uid();
  if (DecayVtx()) o << " dcy: "<<*DecayVtx();
  o << " PID:";
  for(int k=0;k<5;k++) o << fPidLH[k] <<","; // take the first 5 pid entries to check charged p,pi,e,mu,K
}


void TCandidate::SetMarker(UInt_t n)
{
  
  fMarker[0] = fMarker[1] = fMarker[2] = fMarker[3] = 0;
  
  if (n<32) 
    fMarker[0] = 1<<n;
  else if (n<64)
    fMarker[1] = 1<<(n%32);
  else if (n<96)
    fMarker[2] = 1<<(n%64);
  else if (n<128)
    fMarker[3] = 1<<(n%96);
  else
    cout << "TCandidate warning: More than 128 tracks; overlap may occur" << endl;
}

// Constructor from CandBase

TCandidate::TCandidate( TLorentzVector p4, 
                       TError &p4Err,
                       TCandListIterator& iterDau,
                       VAbsVertex& theVertex,
                       const TParticlePDG* hypo )
:
fFastMode(kFALSE),
fLocked(kFALSE), 
fTheMother(0),
fDecayVtx(0), 
fPdtEntry(0), 
fPdgCode(0),
fIsAResonance(kFALSE),
fTruth(0),
fMicroCand(0),
fTrackNumber(-1),
fDaugList(0),
nDaug(0),
nCons(0)
{
  fMarker[0] = fMarker[1] = fMarker[2] = fMarker[3] = 0;
  
  // create the local candidate
  //createLocalCand();
  
  // first set the mother/daughter links
  TCandidate* dau=0;
  iterDau.Rewind();
  Int_t nDau=0;
  while( dau=iterDau.Next() ) 
  {
    nDau++;
    AddDaughterLink( dau );
  }
  
  // a composite cand is not supposed not to have a vertex...
  if( nDau==0 )
    cerr << "A composite cand is supposed to have daughters ! " << endl;
  
  // set the trajector
  SetTrajectory(p4,p4Err,(Int_t)Charge(),hypo,&theVertex);
}


void TCandidate::SetTrajectory(const TLorentzVector& p4, const TError& p4Err,
                               Int_t charge,const TParticlePDG* hypo,
                               VAbsVertex* dVtx )
{
  if(dVtx){
    SetP7(dVtx->Point(),p4);
    SetCov7(dVtx->XXCov(),p4Err,dVtx->XPCov());
    //BbrPointErr pos(dVtx->point(),dVtx->xxCov());
    //_deferCompTrk = true;
    
    //     HepSymMatrix ppc(3);
    //     for(Int_t k=0;k<3;k++)
    //       for (Int_t j=k;j<3;j++)
    // 	ppc[j][k]=p4Err[j][k];
    //     BbrVectorErr mom(p4.vect(),ppc);
    
    //     TrkCompTrk * compTrk=new TrkCompTrk( pos,mom,dVtx->xpCov(),charge,
    // 					 dVtx->chiSquared(),dVtx->nDof());
    //     setTrkCompTrk(compTrk);
    
    SetDecayVtx(dVtx);
  }else{
    SetP4(p4);
    SetCovP4(p4Err);
  }
  
  if (hypo) SetType(hypo);
  if (Uid()==0) SetUid();
}

void TCandidate::SetUid(UInt_t uid)
{
  static UInt_t u = 10000;
  if (uid!=0)
    u = uid;
  else
    u++;
  
  fUid = u;
  if ((fMarker[0]|fMarker[1]|fMarker[2]|fMarker[3])==0) SetMarker(u%128);
}

Bool_t
TCandidate::IsCloneOf(const TCandidate& o, Bool_t checkType) const    
{
  // Original behaviour of TCandidate::IsCloneOf()
  if ( Uid() == o.Uid() && !checkType) return kTRUE;
  
  if ( (IsComposite() && !o.IsComposite()) || 
      (!IsComposite() && o.IsComposite()) ) return kFALSE;
  
  // for single tracks and clusters, it is enough to compare 
  // UIDs and PDT types
  if ( !IsComposite() && !o.IsComposite() ) {
    return ( Uid() == o.Uid() && 
            (!checkType || PdtEntry() == o.PdtEntry() ));
  }
  
  // if we got here, must be true
  //return kTRUE;
  return kFALSE;
}

void TCandidate::RemoveAssociations()
{
  // Clean up asociations to allow new w/ placement
  //if (fTheMother!=0) DropMotherLink(); fTheMother = 0;
  
  // COMMENTED BY K GOETZEN *********************************
  //if (fDaugList!=0) { delete fDaugList; fDaugList = 0; nDaug=0;}
  
  // ADDED BY K GOETZEN *********************************
  nDaug=0;
  //now clear std::vector
  fDaughters.clear();
  
  // ************************
  
  //if (fDecayVtx!=0) { delete fDecayVtx; fDecayVtx = 0; }
  for (int i=0;i<nCons;i++) { delete fConstraints[i]; fConstraints[i]=0; nCons=0;}
}

TCandidate* TCandidate::Combine(const TCandidate& c) 
{
  TCandidate tmp(P4()+c.P4(),Charge()+c.Charge());
  TCandidate *cand = TFactory::Instance()->NewCandidate(tmp);
  cand->SetMarker(fMarker[0]|c.fMarker[0],0);
  cand->SetMarker(fMarker[1]|c.fMarker[1],1);
  cand->SetMarker(fMarker[2]|c.fMarker[2],2);
  cand->SetMarker(fMarker[3]|c.fMarker[3],3);
  
  cand->SetCovP4(P4Cov()+c.P4Cov());
  
  // *************** modified by K Goetzen
  cand->AddDaughterLinkSimple(this);
  cand->AddDaughterLinkSimple(&c);
  // ****************
  return cand;
}

//************** added Combine for more candidates K.Goetzen, 05/2008
TCandidate* TCandidate::Combine(const TCandidate& c1,const TCandidate& c2) 
{
  TCandidate tmp(P4()+c1.P4()+c2.P4(),Charge()+c1.Charge()+c2.Charge());
  TCandidate *cand = TFactory::Instance()->NewCandidate(tmp);
  cand->SetMarker(fMarker[0]|c1.fMarker[0]|c2.fMarker[0],0);
  cand->SetMarker(fMarker[1]|c1.fMarker[1]|c2.fMarker[1],1);
  cand->SetMarker(fMarker[2]|c1.fMarker[2]|c2.fMarker[2],2);
  cand->SetMarker(fMarker[3]|c1.fMarker[3]|c2.fMarker[3],3);
  
  cand->SetCovP4(P4Cov()+c1.P4Cov()+c2.P4Cov());
  
  // *************** modified by K Goetzen
  cand->AddDaughterLinkSimple(this);
  cand->AddDaughterLinkSimple(&c1);
  cand->AddDaughterLinkSimple(&c2);
  // ****************
  return cand;
}

TCandidate* TCandidate::Combine(const TCandidate& c1,const TCandidate& c2,const TCandidate& c3) 
{
  TCandidate tmp(P4()+c1.P4()+c2.P4()+c3.P4(),Charge()+c1.Charge()+c2.Charge()+c3.Charge());
  TCandidate *cand = TFactory::Instance()->NewCandidate(tmp);
  cand->SetMarker(fMarker[0]|c1.fMarker[0]|c2.fMarker[0]|c3.fMarker[0],0);
  cand->SetMarker(fMarker[1]|c1.fMarker[1]|c2.fMarker[1]|c3.fMarker[1],1);
  cand->SetMarker(fMarker[2]|c1.fMarker[2]|c2.fMarker[2]|c3.fMarker[2],2);
  cand->SetMarker(fMarker[3]|c1.fMarker[3]|c2.fMarker[3]|c3.fMarker[3],3);
  
  cand->SetCovP4(P4Cov()+c1.P4Cov()+c2.P4Cov()+c3.P4Cov());
  
  // *************** modified by K Goetzen
  cand->AddDaughterLinkSimple(this);
  cand->AddDaughterLinkSimple(&c1);
  cand->AddDaughterLinkSimple(&c2);
  cand->AddDaughterLinkSimple(&c3);
  // ****************
  return cand;
}


void TCandidate::SetMarker(UInt_t l,UInt_t m) {
  if (m<4)
    fMarker[m]=l;
  else
    cout << "TCandidate: Trying to set non-existent marker " << m << endl;
}

//
// *************************** added for testing/use in pandaROOT, K.G. 11/2007
//

void TCandidate::SetPidInfo(double *pidinfo)
{
  for (int i=0;i<30;i++)
    if (pidinfo) fPidLH[i]=pidinfo[i];
    else fPidLH[i]=0;
}

void TCandidate::SetPidInfo(int hypo, double value)
{
  if (hypo>=0 && hypo<30)
    fPidLH[hypo] = value;
}

double TCandidate::GetPidInfo(int hypo)
{
  if (hypo>=0 && hypo<30)
    return fPidLH[hypo];
  else
    return -999.0;
}

const double* TCandidate::GetPidInfo() const
{
  return fPidLH;
}
