//////////////////////////////////////////////////////////////////////////
//                                                                      //
// RhoCandList                                                          //
//                                                                      //
// Container class for RhoCandidates                                    //
//                                                                      //
// Author List:                                                         //
// Marcel Kunze,  RUB, Feb. 99                                          //
// Copyright (C) 1999-2001, Ruhr-University Bochum.                     //
// Ralf Kliemt, HIM/GSI Feb.2013 (Cleanup & Restructuring)              //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <stdlib.h>

#include "RhoBase/RhoCandList.h"
#include "RhoBase/RhoFactory.h"
#include "RhoBase/RhoCandidate.h"
#include "RhoBase/RhoParticleSelectorBase.h"
#include "RhoBase/RhoVertexSelectorBase.h"
#include "RhoTools/RhoCalculationTools.h"

ClassImp ( RhoCandList )

#include <iostream>
using namespace std;

RhoCandList::RhoCandList ( const char* name, UInt_t capacity ) :
  TNamed ( name,name ), fFast ( kFALSE )
{
  fOwnList = new TObjArray ( capacity );
}


// Perform a deep copy
RhoCandList::RhoCandList ( RhoCandList& l ) : TNamed(l)
{
  fFast = l.fFast;
  fOwnList = new TObjArray( l.GetLength() );

  Cleanup();
  const Int_t n = l.GetLength();
  for ( int i=0; i<n; i++ ) {
    Put ( l.Get ( i ) );
  }
}

//--------------
// Destructor --
//--------------
// Deletes the list and the owned objects
RhoCandList::~RhoCandList( )
{
  Cleanup();
  delete fOwnList;
}

//----------------------
//-- public Functions --
//----------------------

void RhoCandList::Cleanup( )
{
    fOwnList->Clear(); //Delete() would destruct the candidates which will be cleaned by the TFactory!
}

void RhoCandList::SetNumberOfTracks ( Int_t n )
{
  cerr << "RhoCandList::SetNumberOfTracks is deprecated. n="<<n << endl;
}

Int_t RhoCandList::GetNumberOfTracks() const
{
  return fOwnList->GetLast() +1;
}

void RhoCandList::Put ( const RhoCandidate* c, Int_t i )
{
  RhoCandidate* newCand = RhoFactory::Instance()->NewCandidate ( c );
  if ( i<0 ) {
    fOwnList->Add ( newCand );
  } else {
    ( *fOwnList ) [i] = newCand;
  }
}

void RhoCandList::InsertAt ( Int_t i, const RhoCandidate* c )
{
  fOwnList->AddAtAndExpand ( ( TObject* ) c,i );
  Put ( c,i );
}


RhoCandidate* RhoCandList::Get ( Int_t i )
{
  if ( i >= GetNumberOfTracks() ) { return 0; }
  return ( RhoCandidate* ) ( fOwnList->UncheckedAt ( i ) );
}

RhoCandidate* RhoCandList::GetConst ( Int_t i ) const
{
  if ( i >= GetNumberOfTracks() ) { return 0; }
  return ( RhoCandidate* ) ( fOwnList->UncheckedAt ( i ) );
}

RhoCandList* RhoCandList::GetFittedList()
{
  // Create and return a new list of the fitted candidates
 
  // new list with extended name from the unfitted and intelligent starting size
  RhoCandList* fittedlist = new RhoCandList( Form("%s Fitted",fName.Data()) , GetNumberOfTracks() );
  FillFittedList(*fittedlist);
  return fittedlist;
}

void RhoCandList::FillFittedList(RhoCandList &fittedlist)
{
  RhoCandidate* aFit=0;
  fittedlist.Cleanup();
  for(int j=0;j<GetNumberOfTracks();j++) {
    aFit = GetConst(j)->GetFit();
    if(!aFit) Error("RhoCandList::FillFittedList","Fit pointer does not exist!");
    fittedlist.Put(aFit);
  }
  return;
}


// Compare the marker and remove corresponding entry (MK,12/99)
// This allows to remove objects in several lists
Int_t RhoCandList::Remove ( RhoCandidate* c )
{
  Int_t nRemoved = 0;
  Int_t n = GetNumberOfTracks();
  for ( Int_t i=0; i<n; i++ ) {
    RhoCandidate* b = Get ( i );
    if ( b->Equals ( c ) ) {
      fOwnList->RemoveAt ( i );
      nRemoved++;
    }
  }
  fOwnList->Compress();
  return nRemoved;
}

Int_t RhoCandList::RemoveFamily ( RhoCandidate* c )
{
  Int_t nRemoved = 0;
  Int_t n = GetNumberOfTracks();
  for ( Int_t i=0; i<n; i++ ) {
    RhoCandidate* b = Get ( i );
    if ( b->Overlaps ( c ) ) {
      fOwnList->RemoveAt ( i );
      nRemoved++;
    }
  }
  fOwnList->Compress();
  return nRemoved;
}

Int_t RhoCandList::RemoveClones()
{
  Int_t nRemoved = 0;
  Int_t n = GetNumberOfTracks();
  for ( Int_t i=0; i<n-1; i++ ) {
    RhoCandidate* b = Get ( i );
    if ( b==0 ) { continue; }
    for ( Int_t j=i+1; j<n; ++j ) {
      RhoCandidate* c = Get ( j );
      if ( c==0 ) { continue; }
      if ( b->Equals ( c ) ) {
        fOwnList->RemoveAt ( j );
        nRemoved++;
      }
    }
  }
  fOwnList->Compress();
  return nRemoved;
}

Int_t RhoCandList::OccurrencesOf ( RhoCandidate* c )
{
  Int_t nCand = 0;
  const Int_t n = GetNumberOfTracks();
  for ( Int_t i=0; i<n; i++ ) {
    RhoCandidate* b = Get ( i );
    if ( b->Equals ( c ) ) { nCand++; }
  }
  return nCand;
}

Double_t RhoCandList::GetTotalEnergy ( Double_t emin )
{
  Double_t e = 0.0;
  const Int_t n = GetNumberOfTracks();
  for ( Int_t i=0; i<n; i++ ) {
    RhoCandidate* c = Get ( i );
    Double_t energy = c->Energy();
    if ( energy > emin ) { e += energy; }
  }
  return e;
}

TVector3 RhoCandList::GetTotalMomentum ( Double_t pmin )
{
  TVector3 p ( 0.0,0.0,0.0 );
  const Int_t n = GetNumberOfTracks();
  for ( Int_t i=0; i<n; i++ ) {
    RhoCandidate* c = Get ( i );
    TVector3 p3 = c->P3();
    if ( p3.Mag() > pmin ) { p = p + p3; }
  }
  return p;
}

void RhoCandList::Boost ( const TVector3& p )
{
  const Int_t n = GetNumberOfTracks();
  for ( Int_t i=0; i<n; i++ ) {
    Get ( i )->Boost ( p );
  }
}

void RhoCandList::PrintOn ( std::ostream& o ) const
{
  const Int_t n = GetNumberOfTracks();
  o << "content: " << n << " entries." << endl;
  if ( n!=0 ) {
    for ( Int_t i=0; i<n; i++ ) {
      const RhoCandidate* c = GetConst ( i );
      o << "[" << i << "] " << *c;
      o << endl;
    }
  }
}

void RhoCandList::Remainder ( RhoCandList& l )
{
  const Int_t n = l.GetNumberOfTracks();
  for ( int i=0; i<n; i++ ) {
    const RhoCandidate* c = l.Get ( i );
    if ( c->GetMarker() !=0 ) { fOwnList->RemoveAt ( i ); }
  }
  fOwnList->Compress();
}

// Makes now a deep copy rather than just cpoying pointers (MK,12/99)
void RhoCandList::operator = ( const RhoCandList& l )
{
  fFast = l.fFast;

  Cleanup();
  const Int_t n = l.GetNumberOfTracks();
  for ( int i=0; i<n; i++ ) {
    Put ( l.GetConst( i ) );
  }
}

RhoCandidate* RhoCandList::operator[] ( Int_t i )
{
  return Get(i);
}

void RhoCandList::SetType ( const TParticlePDG* pdt, int start )
{
  if (start<0) start=0;
  for ( Int_t i=start; i<GetNumberOfTracks(); i++ ) {
    Get(i)->SetType ( pdt );
  }
}

void RhoCandList::SetType ( const char* name, int start )
{
  if (start<0) start=0;
  for ( Int_t i=start; i<GetNumberOfTracks(); i++ ) {
    Get(i)->SetType ( name );
  }
}

void RhoCandList::SetType ( Int_t pdgcode, int start )
{
  if (start<0) start=0;
  for ( Int_t i=start; i<GetNumberOfTracks(); i++ ) {
    Get(i)->SetType ( pdgcode );
  }
}


void RhoCandList::Combine ( RhoCandList& l1, RhoCandList& l2 )
{
  Cleanup();
  CombineAndAppend ( l1, l2 );
}

void RhoCandList::Combine ( RhoCandList& l1, RhoCandList& l2, RhoCandList& l3 )
{
  Cleanup();
  CombineAndAppend ( l1, l2, l3 );
}

void RhoCandList::Combine ( RhoCandList& l1, RhoCandList& l2, RhoCandList& l3, RhoCandList& l4 )
{
  Cleanup();
  CombineAndAppend ( l1, l2, l3, l4 );
}

void RhoCandList::Combine ( RhoCandList& l1, RhoCandList& l2, RhoCandList& l3, RhoCandList& l4, RhoCandList& l5 )
{
  Cleanup();
  CombineAndAppend ( l1, l2, l3, l4, l5 );
}

void RhoCandList::Combine ( RhoCandList& l1, RhoCandList& l2, RhoCandList& l3, RhoCandList& l4, RhoCandList& l5, RhoCandList& l6 )
{
  Cleanup();
  CombineAndAppend ( l1, l2, l3, l4, l5, l6 );
}

void RhoCandList::Combine ( RhoCandList& l1, RhoCandList& l2, RhoCandList& l3, RhoCandList& l4, RhoCandList& l5, RhoCandList& l6, RhoCandList& l7 )
{
  Cleanup();
  CombineAndAppend ( l1, l2, l3, l4, l5, l6, l7 );
}

void RhoCandList::Combine ( RhoCandList& l1, RhoCandList& l2, RhoCandList& l3, RhoCandList& l4, RhoCandList& l5, RhoCandList& l6, RhoCandList& l7, RhoCandList& l8 )
{
  Cleanup();
  CombineAndAppend ( l1, l2, l3, l4, l5, l6, l7, l8 );
}

void RhoCandList::Combine ( RhoCandList& l1, RhoCandList& l2, RhoCandList& l3, RhoCandList& l4, RhoCandList& l5, RhoCandList& l6, RhoCandList& l7, RhoCandList& l8, RhoCandList& l9 )
{
  Cleanup();
  CombineAndAppend ( l1, l2, l3, l4, l5, l6, l7, l8, l9 );
}

void RhoCandList::Combine ( RhoCandList& l1, RhoCandList& l2, RhoCandList& l3, RhoCandList& l4, RhoCandList& l5, RhoCandList& l6, RhoCandList& l7, RhoCandList& l8, RhoCandList& l9, RhoCandList& l10 )
{
  Cleanup();
  CombineAndAppend ( l1, l2, l3, l4, l5, l6, l7, l8, l9, l10 );
}


void RhoCandList::CombineAndAppend ( RhoCandList& l1, RhoCandList& l2 )
{
  //printf("RhoCandList::CombineAndAppend()\n");
  TLorentzVector vl;
  Double_t charge;

  const int len1=l1.GetLength();
  const int len2=l2.GetLength();

  int i1,i2;
  int st2;

  for ( i1=0; i1<len1; i1++ ) {
    st2=0;
    if ( &l1==&l2 ) { st2=i1+1; }

    for ( i2=st2; i2<len2; i2++ ) {
      if ( l1[i1]->Overlaps ( l2[i2] ) ) { continue; }

      vl=l1[i1]->P4() +l2[i2]->P4();
      charge=l1[i1]->Charge() +l2[i2]->Charge();

      RhoCandidate c ( vl,charge );
      c.SetCovP4 ( l1[i1]->P4Cov() +l2[i2]->P4Cov() );
//std::cout<<"  -- Combining 2 Cov Matrices from candidates  c1:"<<&(l1[i1]->P4Cov())<<"   c2:"<<&(l2[i2]->P4Cov())<<std::endl;
//RhoCalculationTools::PrintMatrix(l1[i1]->P4Cov());
//RhoCalculationTools::PrintMatrix(l2[i2]->P4Cov());
//std::cout<<"  -- result"<<std::endl;
////RhoCalculationTools::PrintMatrix(c.P4Cov());
//RhoCalculationTools::PrintMatrix( l1[i1]->P4Cov() + l2[i2]->P4Cov() );
//RhoCalculationTools::PrintMatrix( ((RhoError)l1[i1]->P4Cov()) +((RhoError)l2[i2]->P4Cov()));
//RhoCalculationTools::PrintMatrix( ((TMatrixD)l1[i1]->P4Cov()) +((TMatrixD)l2[i2]->P4Cov()));

      c.SetMarker ( l1[i1]->GetMarker ( 0 ) |l2[i2]->GetMarker ( 0 ),0 );
      c.SetMarker ( l1[i1]->GetMarker ( 1 ) |l2[i2]->GetMarker ( 1 ),1 );
      c.SetMarker ( l1[i1]->GetMarker ( 2 ) |l2[i2]->GetMarker ( 2 ),2 );
      c.SetMarker ( l1[i1]->GetMarker ( 3 ) |l2[i2]->GetMarker ( 3 ),3 );

      c.SetInsertHistory(kFALSE);
      if(l1[i1]->GetPointerToLinks() != 0) c.AddLinks(*l1[i1]->GetPointerToLinks());
      if(l2[i2]->GetPointerToLinks() != 0) c.AddLinks(*l2[i2]->GetPointerToLinks());

      Put ( &c );
      // after putting (does a copy and drops daughter links)
      RhoCandidate* cInList = Get(GetLength()-1);
      cInList->AddDaughterLinkSimple(l1[i1]) ;
      cInList->AddDaughterLinkSimple(l2[i2]) ;
    }
  }

}

void RhoCandList::CombineAndAppend ( RhoCandList& l1, RhoCandList& l2, RhoCandList& l3 )
{
  TLorentzVector vl;
  Double_t charge;

  const int len1=l1.GetLength();
  const int len2=l2.GetLength();
  const int len3=l3.GetLength();

  int i1,i2,i3;
  int st2,st3;

  for ( i1=0; i1<len1; i1++ ) {
    st2=0;
    if ( &l2==&l1 ) { st2=i1+1; }

    for ( i2=st2; i2<len2; i2++ ) {
      if ( l1[i1]->Overlaps ( l2[i2] ) ) { continue; }

      st3=0;
      if ( &l3==&l2 ) { st3=i2+1; }
      else if ( &l3==&l1 ) { st3=i1+1; }

      for ( i3=st3; i3<len3; i3++ ) {
        if ( l3[i3]->Overlaps ( l2[i2] ) 
          || l3[i3]->Overlaps ( l1[i1] ) ) { continue; }

        vl=l1[i1]->P4() +l2[i2]->P4() +l3[i3]->P4();
        charge=l1[i1]->Charge() +l2[i2]->Charge() +l3[i3]->Charge();

        RhoCandidate c ( vl,charge );
        c.SetCovP4 ( l1[i1]->P4Cov() +l2[i2]->P4Cov() +l3[i3]->P4Cov() );

        c.SetMarker ( l1[i1]->GetMarker ( 0 ) |l2[i2]->GetMarker ( 0 ) |l3[i3]->GetMarker ( 0 ),0 );
        c.SetMarker ( l1[i1]->GetMarker ( 1 ) |l2[i2]->GetMarker ( 1 ) |l3[i3]->GetMarker ( 1 ),1 );
        c.SetMarker ( l1[i1]->GetMarker ( 2 ) |l2[i2]->GetMarker ( 2 ) |l3[i3]->GetMarker ( 2 ),2 );
        c.SetMarker ( l1[i1]->GetMarker ( 3 ) |l2[i2]->GetMarker ( 3 ) |l3[i3]->GetMarker ( 3 ),3 );

        c.SetInsertHistory(kFALSE);
        if(l1[i1]->GetPointerToLinks() != 0) c.AddLinks(*l1[i1]->GetPointerToLinks());
        if(l2[i2]->GetPointerToLinks() != 0) c.AddLinks(*l2[i2]->GetPointerToLinks());
        if(l3[i3]->GetPointerToLinks() != 0) c.AddLinks(*l3[i3]->GetPointerToLinks());

        Put ( &c );
        // after putting (does a copy and drops daughter links)
        RhoCandidate* cInList = Get(GetLength()-1);
        cInList->AddDaughterLinkSimple(l1[i1]) ;
        cInList->AddDaughterLinkSimple(l2[i2]) ;
        cInList->AddDaughterLinkSimple(l3[i3]) ;
      }
    }
  }
}

void RhoCandList::CombineAndAppend ( RhoCandList& l1, RhoCandList& l2, RhoCandList& l3, RhoCandList& l4 )
{
  TLorentzVector vl;
  Double_t charge;

  const int len1=l1.GetLength();
  const int len2=l2.GetLength();
  const int len3=l3.GetLength();
  const int len4=l4.GetLength();

  int i1,i2,i3,i4;
  int st2,st3,st4;

  for ( i1=0; i1<len1; i1++ ) {
    st2=0;
    if ( &l2==&l1 ) { st2=i1+1; }

    for ( i2=st2; i2<len2; i2++ ) {
      if ( l1[i1]->Overlaps ( l2[i2] ) ) { continue; }

      st3=0;
      if ( &l3==&l2 ) { st3=i2+1; }
      else if ( &l3==&l1 ) { st3=i1+1; }

      for ( i3=st3; i3<len3; i3++ ) {
        if ( l3[i3]->Overlaps ( l2[i2] ) 
          || l3[i3]->Overlaps ( l1[i1] ) ) { continue; }

        st4=0;
        if ( &l4==&l3 ) { st4=i3+1; }
        else if ( &l4==&l2 ) { st4=i2+1; }
        else if ( &l4==&l1 ) { st4=i1+1; }

        for ( i4=st4; i4<len4; i4++ ) {
          if ( l4[i4]->Overlaps ( l3[i3] ) 
            || l4[i4]->Overlaps ( l2[i2] ) 
            || l4[i4]->Overlaps ( l1[i1] ) ) { continue; }

          vl=l1[i1]->P4() +l2[i2]->P4() +l3[i3]->P4() +l4[i4]->P4();
          charge=l1[i1]->Charge() +l2[i2]->Charge() +l3[i3]->Charge() +l4[i4]->Charge();

          RhoCandidate c ( vl,charge );
          c.SetCovP4 ( l1[i1]->P4Cov() +l2[i2]->P4Cov() +l3[i3]->P4Cov() +l4[i4]->P4Cov() );

          c.SetMarker ( l1[i1]->GetMarker ( 0 ) |l2[i2]->GetMarker ( 0 ) |l3[i3]->GetMarker ( 0 ) |l4[i4]->GetMarker ( 0 ),0 );
          c.SetMarker ( l1[i1]->GetMarker ( 1 ) |l2[i2]->GetMarker ( 1 ) |l3[i3]->GetMarker ( 1 ) |l4[i4]->GetMarker ( 1 ),1 );
          c.SetMarker ( l1[i1]->GetMarker ( 2 ) |l2[i2]->GetMarker ( 2 ) |l3[i3]->GetMarker ( 2 ) |l4[i4]->GetMarker ( 2 ),2 );
          c.SetMarker ( l1[i1]->GetMarker ( 3 ) |l2[i2]->GetMarker ( 3 ) |l3[i3]->GetMarker ( 3 ) |l4[i4]->GetMarker ( 3 ),3 );

          c.SetInsertHistory(kFALSE);
          if(l1[i1]->GetPointerToLinks() != 0) c.AddLinks(*l1[i1]->GetPointerToLinks());
          if(l2[i2]->GetPointerToLinks() != 0) c.AddLinks(*l2[i2]->GetPointerToLinks());
          if(l3[i3]->GetPointerToLinks() != 0) c.AddLinks(*l3[i3]->GetPointerToLinks());
          if(l4[i4]->GetPointerToLinks() != 0) c.AddLinks(*l4[i4]->GetPointerToLinks());

          Put ( &c );
          // after putting (does a copy and drops daughter links)
          RhoCandidate* cInList = Get(GetLength()-1);
          cInList->AddDaughterLinkSimple(l1[i1]) ;
          cInList->AddDaughterLinkSimple(l2[i2]) ;
          cInList->AddDaughterLinkSimple(l3[i3]) ;
          cInList->AddDaughterLinkSimple(l4[i4]) ;
        }
      }
    }
  }
}

void RhoCandList::CombineAndAppend ( RhoCandList& l1, RhoCandList& l2, RhoCandList& l3, RhoCandList& l4, RhoCandList& l5)
{
  TLorentzVector vl;
  Double_t charge;

  const int len1=l1.GetLength();
  const int len2=l2.GetLength();
  const int len3=l3.GetLength();
  const int len4=l4.GetLength();
  const int len5=l5.GetLength();

  int i1,i2,i3,i4,i5;
  int st2,st3,st4,st5;

  for ( i1=0; i1<len1; i1++ ) {
    st2=0;
    if ( &l2==&l1 ) { st2=i1+1; }

    for ( i2=st2; i2<len2; i2++ ) {
      if ( l1[i1]->Overlaps ( l2[i2] ) ) { continue; }

      st3=0;
      if ( &l3==&l2 ) { st3=i2+1; }
      else if ( &l3==&l1 ) { st3=i1+1; }

      for ( i3=st3; i3<len3; i3++ ) {
        if ( l3[i3]->Overlaps ( l2[i2] ) 
          || l3[i3]->Overlaps ( l1[i1] ) ) { continue; }

        st4=0;
        if ( &l4==&l3 ) { st4=i3+1; }
        else if ( &l4==&l2 ) { st4=i2+1; }
        else if ( &l4==&l1 ) { st4=i1+1; }

        for ( i4=st4; i4<len4; i4++ ) {
          if ( l4[i4]->Overlaps ( l3[i3] ) 
            || l4[i4]->Overlaps ( l2[i2] ) 
            || l4[i4]->Overlaps ( l1[i1] ) ) { continue; }

          st5=0;
          if ( &l5==&l4 ) { st5=i4+1; }
          else if ( &l5==&l3 ) { st5=i3+1; }
          else if ( &l5==&l2 ) { st5=i2+1; }
          else if ( &l5==&l1 ) { st5=i1+1; }

          for ( i5=st5; i5<len5; i5++ ) {
            if ( l5[i5]->Overlaps ( l4[i4] ) 
              || l5[i5]->Overlaps ( l3[i3] ) 
              || l5[i5]->Overlaps ( l2[i2] ) 
              || l5[i5]->Overlaps ( l1[i1] ) ) { continue; }
                
            vl=l1[i1]->P4() +l2[i2]->P4() +l3[i3]->P4() +l4[i4]->P4() +l5[i5]->P4();
            charge=l1[i1]->Charge() +l2[i2]->Charge() +l3[i3]->Charge() +l4[i4]->Charge() +l5[i5]->Charge() ;
          
            RhoCandidate c ( vl,charge );
            c.SetCovP4 ( l1[i1]->P4Cov() +l2[i2]->P4Cov() +l3[i3]->P4Cov() +l4[i4]->P4Cov() +l5[i5]->P4Cov() );
          
            c.SetMarker ( l1[i1]->GetMarker ( 0 ) |l2[i2]->GetMarker ( 0 ) |l3[i3]->GetMarker ( 0 ) |l4[i4]->GetMarker ( 0 ) |l5[i5]->GetMarker ( 0 ) ,0 );
            c.SetMarker ( l1[i1]->GetMarker ( 1 ) |l2[i2]->GetMarker ( 1 ) |l3[i3]->GetMarker ( 1 ) |l4[i4]->GetMarker ( 1 ) |l5[i5]->GetMarker ( 1 ) ,1 );
            c.SetMarker ( l1[i1]->GetMarker ( 2 ) |l2[i2]->GetMarker ( 2 ) |l3[i3]->GetMarker ( 2 ) |l4[i4]->GetMarker ( 2 ) |l5[i5]->GetMarker ( 2 ) ,2 );
            c.SetMarker ( l1[i1]->GetMarker ( 3 ) |l2[i2]->GetMarker ( 3 ) |l3[i3]->GetMarker ( 3 ) |l4[i4]->GetMarker ( 3 ) |l5[i5]->GetMarker ( 3 ) ,3 );
          
            c.SetInsertHistory(kFALSE);
            if(l1[i1]->GetPointerToLinks() != 0) c.AddLinks(*l1[i1]->GetPointerToLinks());
            if(l2[i2]->GetPointerToLinks() != 0) c.AddLinks(*l2[i2]->GetPointerToLinks());
            if(l3[i3]->GetPointerToLinks() != 0) c.AddLinks(*l3[i3]->GetPointerToLinks());
            if(l4[i4]->GetPointerToLinks() != 0) c.AddLinks(*l4[i4]->GetPointerToLinks());
            if(l5[i5]->GetPointerToLinks() != 0) c.AddLinks(*l5[i5]->GetPointerToLinks());
          
          
            Put ( &c );
            // after putting (does a copy and drops daughter links)
            RhoCandidate* cInList = Get(GetLength()-1);
            cInList->AddDaughterLinkSimple(l1[i1]) ;
            cInList->AddDaughterLinkSimple(l2[i2]) ;
            cInList->AddDaughterLinkSimple(l3[i3]) ;
            cInList->AddDaughterLinkSimple(l4[i4]) ;
            cInList->AddDaughterLinkSimple(l5[i5]) ;
          } //loop5
        } //loop4
      } //loop3
    } //loop2
  } //loop1
}


void RhoCandList::CombineAndAppend ( RhoCandList& l1, RhoCandList& l2, RhoCandList& l3, RhoCandList& l4, RhoCandList& l5, RhoCandList& l6)
{
  TLorentzVector vl;
  Double_t charge;

  const int len1=l1.GetLength();
  const int len2=l2.GetLength();
  const int len3=l3.GetLength();
  const int len4=l4.GetLength();
  const int len5=l5.GetLength();
  const int len6=l6.GetLength();

  int i1,i2,i3,i4,i5,i6;
  int st2,st3,st4,st5,st6;

  for ( i1=0; i1<len1; i1++ ) {
    st2=0;
    if ( &l2==&l1 ) { st2=i1+1; }

    for ( i2=st2; i2<len2; i2++ ) {
      if ( l1[i1]->Overlaps ( l2[i2] ) ) { continue; }

      st3=0;
      if ( &l3==&l2 ) { st3=i2+1; }
      else if ( &l3==&l1 ) { st3=i1+1; }

      for ( i3=st3; i3<len3; i3++ ) {
        if ( l3[i3]->Overlaps ( l2[i2] ) 
          || l3[i3]->Overlaps ( l1[i1] ) ) { continue; }

        st4=0;
        if ( &l4==&l3 ) { st4=i3+1; }
        else if ( &l4==&l2 ) { st4=i2+1; }
        else if ( &l4==&l1 ) { st4=i1+1; }

        for ( i4=st4; i4<len4; i4++ ) {
          if ( l4[i4]->Overlaps ( l3[i3] ) 
            || l4[i4]->Overlaps ( l2[i2] ) 
            || l4[i4]->Overlaps ( l1[i1] ) ) { continue; }

          st5=0;
          if ( &l5==&l4 ) { st5=i4+1; }
          else if ( &l5==&l3 ) { st5=i3+1; }
          else if ( &l5==&l2 ) { st5=i2+1; }
          else if ( &l5==&l1 ) { st5=i1+1; }

          for ( i5=st5; i5<len5; i5++ ) {
            if ( l5[i5]->Overlaps ( l4[i4] ) 
              || l5[i5]->Overlaps ( l3[i3] ) 
              || l5[i5]->Overlaps ( l2[i2] ) 
              || l5[i5]->Overlaps ( l1[i1] ) ) { continue; }
            
            st6=0;
            if ( &l6==&l5 ) { st6=i5+1; }
            else if ( &l6==&l4 ) { st6=i4+1; }
            else if ( &l6==&l3 ) { st6=i3+1; }
            else if ( &l6==&l2 ) { st6=i2+1; }
            else if ( &l6==&l1 ) { st6=i1+1; }
            for ( i6=st6; i6<len6; i6++ ) { //loop 6
              if (l6[i6]->Overlaps ( l5[i5] ) 
               || l6[i6]->Overlaps ( l4[i4] ) 
               || l6[i6]->Overlaps ( l3[i3] ) 
               || l6[i6]->Overlaps ( l2[i2] ) 
               || l6[i6]->Overlaps ( l1[i1] ) ) { continue; }//overlap 6
                
              vl=l1[i1]->P4() +l2[i2]->P4() +l3[i3]->P4() +l4[i4]->P4() +l5[i5]->P4() +l6[i6]->P4();
              charge=l1[i1]->Charge() +l2[i2]->Charge() +l3[i3]->Charge() +l4[i4]->Charge() +l5[i5]->Charge() +l6[i6]->Charge();
            
              RhoCandidate c ( vl,charge );
              c.SetCovP4 ( l1[i1]->P4Cov() +l2[i2]->P4Cov() +l3[i3]->P4Cov() +l4[i4]->P4Cov() +l5[i5]->P4Cov()  +l6[i6]->P4Cov() );
            
              c.SetMarker ( l1[i1]->GetMarker ( 0 ) |l2[i2]->GetMarker ( 0 ) |l3[i3]->GetMarker ( 0 ) |l4[i4]->GetMarker ( 0 ) |l5[i5]->GetMarker ( 0 ) |l6[i6]->GetMarker ( 0 ) ,0 );
              c.SetMarker ( l1[i1]->GetMarker ( 1 ) |l2[i2]->GetMarker ( 1 ) |l3[i3]->GetMarker ( 1 ) |l4[i4]->GetMarker ( 1 ) |l5[i5]->GetMarker ( 1 ) |l6[i6]->GetMarker ( 1 ) ,1 );
              c.SetMarker ( l1[i1]->GetMarker ( 2 ) |l2[i2]->GetMarker ( 2 ) |l3[i3]->GetMarker ( 2 ) |l4[i4]->GetMarker ( 2 ) |l5[i5]->GetMarker ( 2 ) |l6[i6]->GetMarker ( 2 ) ,2 );
              c.SetMarker ( l1[i1]->GetMarker ( 3 ) |l2[i2]->GetMarker ( 3 ) |l3[i3]->GetMarker ( 3 ) |l4[i4]->GetMarker ( 3 ) |l5[i5]->GetMarker ( 3 ) |l6[i6]->GetMarker ( 3 ) ,3 );
            
              c.SetInsertHistory(kFALSE);
              if(l1[i1]->GetPointerToLinks() != 0) c.AddLinks(*l1[i1]->GetPointerToLinks());
              if(l2[i2]->GetPointerToLinks() != 0) c.AddLinks(*l2[i2]->GetPointerToLinks());
              if(l3[i3]->GetPointerToLinks() != 0) c.AddLinks(*l3[i3]->GetPointerToLinks());
              if(l4[i4]->GetPointerToLinks() != 0) c.AddLinks(*l4[i4]->GetPointerToLinks());
              if(l5[i5]->GetPointerToLinks() != 0) c.AddLinks(*l5[i5]->GetPointerToLinks());
              if(l6[i6]->GetPointerToLinks() != 0) c.AddLinks(*l6[i6]->GetPointerToLinks());
            
            
              Put ( &c );
              // after putting (does a copy and drops daughter links)
              RhoCandidate* cInList = Get(GetLength()-1);
              cInList->AddDaughterLinkSimple(l1[i1]) ;
              cInList->AddDaughterLinkSimple(l2[i2]) ;
              cInList->AddDaughterLinkSimple(l3[i3]) ;
              cInList->AddDaughterLinkSimple(l4[i4]) ;
              cInList->AddDaughterLinkSimple(l5[i5]) ;
              cInList->AddDaughterLinkSimple(l6[i6]) ;
            } //loop6
          } //loop5
        } //loop4
      } //loop3
    } //loop2
  } //loop1
}


void RhoCandList::CombineAndAppend ( RhoCandList& l1, RhoCandList& l2, RhoCandList& l3, RhoCandList& l4, RhoCandList& l5, RhoCandList& l6, RhoCandList& l7)
{
  TLorentzVector vl;
  Double_t charge;

  const int len1=l1.GetLength();
  const int len2=l2.GetLength();
  const int len3=l3.GetLength();
  const int len4=l4.GetLength();
  const int len5=l5.GetLength();
  const int len6=l6.GetLength();
  const int len7=l7.GetLength();

  int i1,i2,i3,i4,i5,i6,i7;
  int st2,st3,st4,st5,st6,st7;

  for ( i1=0; i1<len1; i1++ ) {
    st2=0;
    if ( &l2==&l1 ) { st2=i1+1; }

    for ( i2=st2; i2<len2; i2++ ) {
      if ( l1[i1]->Overlaps ( l2[i2] ) ) { continue; }

      st3=0;
      if ( &l3==&l2 ) { st3=i2+1; }
      else if ( &l3==&l1 ) { st3=i1+1; }

      for ( i3=st3; i3<len3; i3++ ) {
        if ( l3[i3]->Overlaps ( l2[i2] ) 
          || l3[i3]->Overlaps ( l1[i1] ) ) { continue; }

        st4=0;
        if ( &l4==&l3 ) { st4=i3+1; }
        else if ( &l4==&l2 ) { st4=i2+1; }
        else if ( &l4==&l1 ) { st4=i1+1; }

        for ( i4=st4; i4<len4; i4++ ) {
          if ( l4[i4]->Overlaps ( l3[i3] ) 
            || l4[i4]->Overlaps ( l2[i2] ) 
            || l4[i4]->Overlaps ( l1[i1] ) ) { continue; }

          st5=0;
          if ( &l5==&l4 ) { st5=i4+1; }
          else if ( &l5==&l3 ) { st5=i3+1; }
          else if ( &l5==&l2 ) { st5=i2+1; }
          else if ( &l5==&l1 ) { st5=i1+1; }

          for ( i5=st5; i5<len5; i5++ ) {
            if ( l5[i5]->Overlaps ( l4[i4] ) 
              || l5[i5]->Overlaps ( l3[i3] ) 
              || l5[i5]->Overlaps ( l2[i2] ) 
              || l5[i5]->Overlaps ( l1[i1] ) ) { continue; }
            
            st6=0;
            if ( &l6==&l5 ) { st6=i5+1; }
            else if ( &l6==&l4 ) { st6=i4+1; }
            else if ( &l6==&l3 ) { st6=i3+1; }
            else if ( &l6==&l2 ) { st6=i2+1; }
            else if ( &l6==&l1 ) { st6=i1+1; }
            for ( i6=st6; i6<len6; i6++ ) { //loop 6
              if (l6[i6]->Overlaps ( l5[i5] ) 
               || l6[i6]->Overlaps ( l4[i4] ) 
               || l6[i6]->Overlaps ( l3[i3] ) 
               || l6[i6]->Overlaps ( l2[i2] ) 
               || l6[i6]->Overlaps ( l1[i1] ) ) { continue; }//overlap 6
              
              st7=0;
              if ( &l7==&l6 ) { st7=i6+1; }
              else if ( &l7==&l5 ) { st7=i5+1; }
              else if ( &l7==&l4 ) { st7=i4+1; }
              else if ( &l7==&l3 ) { st7=i3+1; }
              else if ( &l7==&l2 ) { st7=i2+1; }
              else if ( &l7==&l1 ) { st7=i1+1; }
              for ( i7=st7; i7<len7; i7++ ) { //loop 7
                if (l7[i7]->Overlaps ( l6[i6] ) 
                 || l7[i7]->Overlaps ( l5[i5] ) 
                 || l7[i7]->Overlaps ( l4[i4] ) 
                 || l7[i7]->Overlaps ( l3[i3] ) 
                 || l7[i7]->Overlaps ( l2[i2] ) 
                 || l7[i7]->Overlaps ( l1[i1] ) ) { continue; }//overlap 7
                
                vl=l1[i1]->P4() +l2[i2]->P4() +l3[i3]->P4() +l4[i4]->P4() +l5[i5]->P4() +l6[i6]->P4() +l7[i7]->P4();
                charge=l1[i1]->Charge() +l2[i2]->Charge() +l3[i3]->Charge() +l4[i4]->Charge() +l5[i5]->Charge() +l6[i6]->Charge() +l7[i7]->Charge();
              
                RhoCandidate c ( vl,charge );
                c.SetCovP4 ( l1[i1]->P4Cov() +l2[i2]->P4Cov() +l3[i3]->P4Cov() +l4[i4]->P4Cov() +l5[i5]->P4Cov()  +l6[i6]->P4Cov()  +l7[i7]->P4Cov());
              
                c.SetMarker ( l1[i1]->GetMarker ( 0 ) |l2[i2]->GetMarker ( 0 ) |l3[i3]->GetMarker ( 0 ) |l4[i4]->GetMarker ( 0 ) |l5[i5]->GetMarker ( 0 ) |l6[i6]->GetMarker ( 0 ) |l7[i7]->GetMarker ( 0 ),0 );
                c.SetMarker ( l1[i1]->GetMarker ( 1 ) |l2[i2]->GetMarker ( 1 ) |l3[i3]->GetMarker ( 1 ) |l4[i4]->GetMarker ( 1 ) |l5[i5]->GetMarker ( 1 ) |l6[i6]->GetMarker ( 1 ) |l7[i7]->GetMarker ( 1 ),1 );
                c.SetMarker ( l1[i1]->GetMarker ( 2 ) |l2[i2]->GetMarker ( 2 ) |l3[i3]->GetMarker ( 2 ) |l4[i4]->GetMarker ( 2 ) |l5[i5]->GetMarker ( 2 ) |l6[i6]->GetMarker ( 2 ) |l7[i7]->GetMarker ( 2 ),2 );
                c.SetMarker ( l1[i1]->GetMarker ( 3 ) |l2[i2]->GetMarker ( 3 ) |l3[i3]->GetMarker ( 3 ) |l4[i4]->GetMarker ( 3 ) |l5[i5]->GetMarker ( 3 ) |l6[i6]->GetMarker ( 3 ) |l7[i7]->GetMarker ( 3 ),3 );
              
                c.SetInsertHistory(kFALSE);
                if(l1[i1]->GetPointerToLinks() != 0) c.AddLinks(*l1[i1]->GetPointerToLinks());
                if(l2[i2]->GetPointerToLinks() != 0) c.AddLinks(*l2[i2]->GetPointerToLinks());
                if(l3[i3]->GetPointerToLinks() != 0) c.AddLinks(*l3[i3]->GetPointerToLinks());
                if(l4[i4]->GetPointerToLinks() != 0) c.AddLinks(*l4[i4]->GetPointerToLinks());
                if(l5[i5]->GetPointerToLinks() != 0) c.AddLinks(*l5[i5]->GetPointerToLinks());
                if(l6[i6]->GetPointerToLinks() != 0) c.AddLinks(*l6[i6]->GetPointerToLinks());
                if(l7[i7]->GetPointerToLinks() != 0) c.AddLinks(*l7[i7]->GetPointerToLinks());
              
              
                Put ( &c );
                // after putting (does a copy and drops daughter links)
                RhoCandidate* cInList = Get(GetLength()-1);
                cInList->AddDaughterLinkSimple(l1[i1]) ;
                cInList->AddDaughterLinkSimple(l2[i2]) ;
                cInList->AddDaughterLinkSimple(l3[i3]) ;
                cInList->AddDaughterLinkSimple(l4[i4]) ;
                cInList->AddDaughterLinkSimple(l5[i5]) ;
                cInList->AddDaughterLinkSimple(l6[i6]) ;
                cInList->AddDaughterLinkSimple(l7[i7]) ;
              } //loop7
            } //loop6
          } //loop5
        } //loop4
      } //loop3
    } //loop2
  } //loop1
}


void RhoCandList::CombineAndAppend ( RhoCandList& l1, RhoCandList& l2, RhoCandList& l3, RhoCandList& l4, RhoCandList& l5, RhoCandList& l6, RhoCandList& l7, RhoCandList& l8)
{
  TLorentzVector vl;
  Double_t charge;

  const int len1=l1.GetLength();
  const int len2=l2.GetLength();
  const int len3=l3.GetLength();
  const int len4=l4.GetLength();
  const int len5=l5.GetLength();
  const int len6=l6.GetLength();
  const int len7=l7.GetLength();
  const int len8=l8.GetLength();

  int i1,i2,i3,i4,i5,i6,i7,i8;
  int st2,st3,st4,st5,st6,st7,st8;

  for ( i1=0; i1<len1; i1++ ) {
    st2=0;
    if ( &l2==&l1 ) { st2=i1+1; }

    for ( i2=st2; i2<len2; i2++ ) {
      if ( l1[i1]->Overlaps ( l2[i2] ) ) { continue; }

      st3=0;
      if ( &l3==&l2 ) { st3=i2+1; }
      else if ( &l3==&l1 ) { st3=i1+1; }

      for ( i3=st3; i3<len3; i3++ ) {
        if ( l3[i3]->Overlaps ( l2[i2] ) 
          || l3[i3]->Overlaps ( l1[i1] ) ) { continue; }

        st4=0;
        if ( &l4==&l3 ) { st4=i3+1; }
        else if ( &l4==&l2 ) { st4=i2+1; }
        else if ( &l4==&l1 ) { st4=i1+1; }

        for ( i4=st4; i4<len4; i4++ ) {
          if ( l4[i4]->Overlaps ( l3[i3] ) 
            || l4[i4]->Overlaps ( l2[i2] ) 
            || l4[i4]->Overlaps ( l1[i1] ) ) { continue; }

          st5=0;
          if ( &l5==&l4 ) { st5=i4+1; }
          else if ( &l5==&l3 ) { st5=i3+1; }
          else if ( &l5==&l2 ) { st5=i2+1; }
          else if ( &l5==&l1 ) { st5=i1+1; }

          for ( i5=st5; i5<len5; i5++ ) {
            if ( l5[i5]->Overlaps ( l4[i4] ) 
              || l5[i5]->Overlaps ( l3[i3] ) 
              || l5[i5]->Overlaps ( l2[i2] ) 
              || l5[i5]->Overlaps ( l1[i1] ) ) { continue; }
            
            st6=0;
            if ( &l6==&l5 ) { st6=i5+1; }
            else if ( &l6==&l4 ) { st6=i4+1; }
            else if ( &l6==&l3 ) { st6=i3+1; }
            else if ( &l6==&l2 ) { st6=i2+1; }
            else if ( &l6==&l1 ) { st6=i1+1; }
            for ( i6=st6; i6<len6; i6++ ) { //loop 6
              if (l6[i6]->Overlaps ( l5[i5] ) 
               || l6[i6]->Overlaps ( l4[i4] ) 
               || l6[i6]->Overlaps ( l3[i3] ) 
               || l6[i6]->Overlaps ( l2[i2] ) 
               || l6[i6]->Overlaps ( l1[i1] ) ) { continue; }//overlap 6
              
              st7=0;
              if ( &l7==&l6 ) { st7=i6+1; }
              else if ( &l7==&l5 ) { st7=i5+1; }
              else if ( &l7==&l4 ) { st7=i4+1; }
              else if ( &l7==&l3 ) { st7=i3+1; }
              else if ( &l7==&l2 ) { st7=i2+1; }
              else if ( &l7==&l1 ) { st7=i1+1; }
              for ( i7=st7; i7<len7; i7++ ) { //loop 7
                if (l7[i7]->Overlaps ( l6[i6] ) 
                 || l7[i7]->Overlaps ( l5[i5] ) 
                 || l7[i7]->Overlaps ( l4[i4] ) 
                 || l7[i7]->Overlaps ( l3[i3] ) 
                 || l7[i7]->Overlaps ( l2[i2] ) 
                 || l7[i7]->Overlaps ( l1[i1] ) ) { continue; }//overlap 7
                
                st8=0;
                if ( &l8==&l7 ) { st8=i7+1; }
                else if ( &l8==&l6 ) { st8=i6+1; }
                else if ( &l8==&l5 ) { st8=i5+1; }
                else if ( &l8==&l4 ) { st8=i4+1; }
                else if ( &l8==&l3 ) { st8=i3+1; }
                else if ( &l8==&l2 ) { st8=i2+1; }
                else if ( &l8==&l1 ) { st8=i1+1; }
                for ( i8=st8; i8<len8; i8++ ) { //loop 8
                  if (l8[i8]->Overlaps ( l7[i7] ) 
                   || l8[i8]->Overlaps ( l6[i6] ) 
                   || l8[i8]->Overlaps ( l5[i5] ) 
                   || l8[i8]->Overlaps ( l4[i4] ) 
                   || l8[i8]->Overlaps ( l3[i3] ) 
                   || l8[i8]->Overlaps ( l2[i2] ) 
                   || l8[i8]->Overlaps ( l1[i1] ) ) { continue; }//overlap 8

                  vl=l1[i1]->P4() +l2[i2]->P4() +l3[i3]->P4() +l4[i4]->P4() +l5[i5]->P4() +l6[i6]->P4() +l7[i7]->P4() +l8[i8]->P4();
                  charge=l1[i1]->Charge() +l2[i2]->Charge() +l3[i3]->Charge() +l4[i4]->Charge() +l5[i5]->Charge() +l6[i6]->Charge() +l7[i7]->Charge() +l8[i8]->Charge();
                
                  RhoCandidate c ( vl,charge );
                  c.SetCovP4 ( l1[i1]->P4Cov() +l2[i2]->P4Cov() +l3[i3]->P4Cov() +l4[i4]->P4Cov() +l5[i5]->P4Cov()  +l6[i6]->P4Cov()  +l7[i7]->P4Cov()  +l8[i8]->P4Cov() );
                
                  c.SetMarker ( l1[i1]->GetMarker ( 0 ) |l2[i2]->GetMarker ( 0 ) |l3[i3]->GetMarker ( 0 ) |l4[i4]->GetMarker ( 0 ) |l5[i5]->GetMarker ( 0 ) |l6[i6]->GetMarker ( 0 ) |l7[i7]->GetMarker ( 0 ) |l8[i8]->GetMarker ( 0 ),0 );
                  c.SetMarker ( l1[i1]->GetMarker ( 1 ) |l2[i2]->GetMarker ( 1 ) |l3[i3]->GetMarker ( 1 ) |l4[i4]->GetMarker ( 1 ) |l5[i5]->GetMarker ( 1 ) |l6[i6]->GetMarker ( 1 ) |l7[i7]->GetMarker ( 1 ) |l8[i8]->GetMarker ( 1 ),1 );
                  c.SetMarker ( l1[i1]->GetMarker ( 2 ) |l2[i2]->GetMarker ( 2 ) |l3[i3]->GetMarker ( 2 ) |l4[i4]->GetMarker ( 2 ) |l5[i5]->GetMarker ( 2 ) |l6[i6]->GetMarker ( 2 ) |l7[i7]->GetMarker ( 2 ) |l8[i8]->GetMarker ( 2 ),2 );
                  c.SetMarker ( l1[i1]->GetMarker ( 3 ) |l2[i2]->GetMarker ( 3 ) |l3[i3]->GetMarker ( 3 ) |l4[i4]->GetMarker ( 3 ) |l5[i5]->GetMarker ( 3 ) |l6[i6]->GetMarker ( 3 ) |l7[i7]->GetMarker ( 3 ) |l8[i8]->GetMarker ( 3 ),3 );
                
                  c.SetInsertHistory(kFALSE);
                  if(l1[i1]->GetPointerToLinks() != 0) c.AddLinks(*l1[i1]->GetPointerToLinks());
                  if(l2[i2]->GetPointerToLinks() != 0) c.AddLinks(*l2[i2]->GetPointerToLinks());
                  if(l3[i3]->GetPointerToLinks() != 0) c.AddLinks(*l3[i3]->GetPointerToLinks());
                  if(l4[i4]->GetPointerToLinks() != 0) c.AddLinks(*l4[i4]->GetPointerToLinks());
                  if(l5[i5]->GetPointerToLinks() != 0) c.AddLinks(*l5[i5]->GetPointerToLinks());
                  if(l6[i6]->GetPointerToLinks() != 0) c.AddLinks(*l6[i6]->GetPointerToLinks());
                  if(l7[i7]->GetPointerToLinks() != 0) c.AddLinks(*l7[i7]->GetPointerToLinks());
                  if(l8[i8]->GetPointerToLinks() != 0) c.AddLinks(*l8[i8]->GetPointerToLinks());
                
                
                  Put ( &c );
                  // after putting (does a copy and drops daughter links)
                  RhoCandidate* cInList = Get(GetLength()-1);
                  cInList->AddDaughterLinkSimple(l1[i1]) ;
                  cInList->AddDaughterLinkSimple(l2[i2]) ;
                  cInList->AddDaughterLinkSimple(l3[i3]) ;
                  cInList->AddDaughterLinkSimple(l4[i4]) ;
                  cInList->AddDaughterLinkSimple(l5[i5]) ;
                  cInList->AddDaughterLinkSimple(l6[i6]) ;
                  cInList->AddDaughterLinkSimple(l7[i7]) ;
                  cInList->AddDaughterLinkSimple(l8[i8]) ;
                } //loop8
              } //loop7
            } //loop6
          } //loop5
        } //loop4
      } //loop3
    } //loop2
  } //loop1
}


void RhoCandList::CombineAndAppend ( RhoCandList& l1, RhoCandList& l2, RhoCandList& l3, RhoCandList& l4, RhoCandList& l5, RhoCandList& l6, RhoCandList& l7, RhoCandList& l8, RhoCandList& l9)
{
  TLorentzVector vl;
  Double_t charge;

  const int len1=l1.GetLength();
  const int len2=l2.GetLength();
  const int len3=l3.GetLength();
  const int len4=l4.GetLength();
  const int len5=l5.GetLength();
  const int len6=l6.GetLength();
  const int len7=l7.GetLength();
  const int len8=l8.GetLength();
  const int len9=l9.GetLength();

  int i1,i2,i3,i4,i5,i6,i7,i8,i9;
  int st2,st3,st4,st5,st6,st7,st8,st9;

  for ( i1=0; i1<len1; i1++ ) {
    st2=0;
    if ( &l2==&l1 ) { st2=i1+1; }

    for ( i2=st2; i2<len2; i2++ ) {
      if ( l1[i1]->Overlaps ( l2[i2] ) ) { continue; }

      st3=0;
      if ( &l3==&l2 ) { st3=i2+1; }
      else if ( &l3==&l1 ) { st3=i1+1; }

      for ( i3=st3; i3<len3; i3++ ) {
        if ( l3[i3]->Overlaps ( l2[i2] ) 
          || l3[i3]->Overlaps ( l1[i1] ) ) { continue; }

        st4=0;
        if ( &l4==&l3 ) { st4=i3+1; }
        else if ( &l4==&l2 ) { st4=i2+1; }
        else if ( &l4==&l1 ) { st4=i1+1; }

        for ( i4=st4; i4<len4; i4++ ) {
          if ( l4[i4]->Overlaps ( l3[i3] ) 
            || l4[i4]->Overlaps ( l2[i2] ) 
            || l4[i4]->Overlaps ( l1[i1] ) ) { continue; }

          st5=0;
          if ( &l5==&l4 ) { st5=i4+1; }
          else if ( &l5==&l3 ) { st5=i3+1; }
          else if ( &l5==&l2 ) { st5=i2+1; }
          else if ( &l5==&l1 ) { st5=i1+1; }

          for ( i5=st5; i5<len5; i5++ ) {
            if ( l5[i5]->Overlaps ( l4[i4] ) 
              || l5[i5]->Overlaps ( l3[i3] ) 
              || l5[i5]->Overlaps ( l2[i2] ) 
              || l5[i5]->Overlaps ( l1[i1] ) ) { continue; }
            
            st6=0;
            if ( &l6==&l5 ) { st6=i5+1; }
            else if ( &l6==&l4 ) { st6=i4+1; }
            else if ( &l6==&l3 ) { st6=i3+1; }
            else if ( &l6==&l2 ) { st6=i2+1; }
            else if ( &l6==&l1 ) { st6=i1+1; }
            for ( i6=st6; i6<len6; i6++ ) { //loop 6
              if (l6[i6]->Overlaps ( l5[i5] ) 
               || l6[i6]->Overlaps ( l4[i4] ) 
               || l6[i6]->Overlaps ( l3[i3] ) 
               || l6[i6]->Overlaps ( l2[i2] ) 
               || l6[i6]->Overlaps ( l1[i1] ) ) { continue; }//overlap 6
              
              st7=0;
              if ( &l7==&l6 ) { st7=i6+1; }
              else if ( &l7==&l5 ) { st7=i5+1; }
              else if ( &l7==&l4 ) { st7=i4+1; }
              else if ( &l7==&l3 ) { st7=i3+1; }
              else if ( &l7==&l2 ) { st7=i2+1; }
              else if ( &l7==&l1 ) { st7=i1+1; }
              for ( i7=st7; i7<len7; i7++ ) { //loop 7
                if (l7[i7]->Overlaps ( l6[i6] ) 
                 || l7[i7]->Overlaps ( l5[i5] ) 
                 || l7[i7]->Overlaps ( l4[i4] ) 
                 || l7[i7]->Overlaps ( l3[i3] ) 
                 || l7[i7]->Overlaps ( l2[i2] ) 
                 || l7[i7]->Overlaps ( l1[i1] ) ) { continue; }//overlap 7
                
                st8=0;
                if ( &l8==&l7 ) { st8=i7+1; }
                else if ( &l8==&l6 ) { st8=i6+1; }
                else if ( &l8==&l5 ) { st8=i5+1; }
                else if ( &l8==&l4 ) { st8=i4+1; }
                else if ( &l8==&l3 ) { st8=i3+1; }
                else if ( &l8==&l2 ) { st8=i2+1; }
                else if ( &l8==&l1 ) { st8=i1+1; }
                for ( i8=st8; i8<len8; i8++ ) { //loop 8
                  if (l8[i8]->Overlaps ( l7[i7] ) 
                   || l8[i8]->Overlaps ( l6[i6] ) 
                   || l8[i8]->Overlaps ( l5[i5] ) 
                   || l8[i8]->Overlaps ( l4[i4] ) 
                   || l8[i8]->Overlaps ( l3[i3] ) 
                   || l8[i8]->Overlaps ( l2[i2] ) 
                   || l8[i8]->Overlaps ( l1[i1] ) ) { continue; }//overlap 8
                  
                  st9=0;
                  if ( &l9==&l8 ) { st9=i8+1; }
                  else if ( &l9==&l7 ) { st9=i7+1; }
                  else if ( &l9==&l6 ) { st9=i6+1; }
                  else if ( &l9==&l5 ) { st9=i5+1; }
                  else if ( &l9==&l4 ) { st9=i4+1; }
                  else if ( &l9==&l3 ) { st9=i3+1; }
                  else if ( &l9==&l2 ) { st9=i2+1; }
                  else if ( &l9==&l1 ) { st9=i1+1; }
                  for ( i9=st9; i9<len9; i9++ ) { //loop 9
                    if (l9[i9]->Overlaps ( l8[i8] ) 
                     || l9[i9]->Overlaps ( l7[i7] ) 
                     || l9[i9]->Overlaps ( l6[i6] ) 
                     || l9[i9]->Overlaps ( l5[i5] ) 
                     || l9[i9]->Overlaps ( l4[i4] ) 
                     || l9[i9]->Overlaps ( l3[i3] ) 
                     || l9[i9]->Overlaps ( l2[i2] ) 
                     || l9[i9]->Overlaps ( l1[i1] ) ) { continue; }//overlap 9

                    vl=l1[i1]->P4() +l2[i2]->P4() +l3[i3]->P4() +l4[i4]->P4() +l5[i5]->P4() +l6[i6]->P4() +l7[i7]->P4() +l8[i8]->P4() +l9[i9]->P4();
                    charge=l1[i1]->Charge() +l2[i2]->Charge() +l3[i3]->Charge() +l4[i4]->Charge() +l5[i5]->Charge() +l6[i6]->Charge() +l7[i7]->Charge() +l8[i8]->Charge() +l9[i9]->Charge();
                  
                    RhoCandidate c ( vl,charge );
                    c.SetCovP4 ( l1[i1]->P4Cov() +l2[i2]->P4Cov() +l3[i3]->P4Cov() +l4[i4]->P4Cov() +l5[i5]->P4Cov()  +l6[i6]->P4Cov()  +l7[i7]->P4Cov()  +l8[i8]->P4Cov()  +l9[i9]->P4Cov() );
                  
                    c.SetMarker ( l1[i1]->GetMarker ( 0 ) |l2[i2]->GetMarker ( 0 ) |l3[i3]->GetMarker ( 0 ) |l4[i4]->GetMarker ( 0 ) |l5[i5]->GetMarker ( 0 ) |l6[i6]->GetMarker ( 0 ) |l7[i7]->GetMarker ( 0 ) |l8[i8]->GetMarker ( 0 ) |l9[i9]->GetMarker ( 0 ),0 );
                    c.SetMarker ( l1[i1]->GetMarker ( 1 ) |l2[i2]->GetMarker ( 1 ) |l3[i3]->GetMarker ( 1 ) |l4[i4]->GetMarker ( 1 ) |l5[i5]->GetMarker ( 1 ) |l6[i6]->GetMarker ( 1 ) |l7[i7]->GetMarker ( 1 ) |l8[i8]->GetMarker ( 1 ) |l9[i9]->GetMarker ( 1 ),1 );
                    c.SetMarker ( l1[i1]->GetMarker ( 2 ) |l2[i2]->GetMarker ( 2 ) |l3[i3]->GetMarker ( 2 ) |l4[i4]->GetMarker ( 2 ) |l5[i5]->GetMarker ( 2 ) |l6[i6]->GetMarker ( 2 ) |l7[i7]->GetMarker ( 2 ) |l8[i8]->GetMarker ( 2 ) |l9[i9]->GetMarker ( 2 ),2 );
                    c.SetMarker ( l1[i1]->GetMarker ( 3 ) |l2[i2]->GetMarker ( 3 ) |l3[i3]->GetMarker ( 3 ) |l4[i4]->GetMarker ( 3 ) |l5[i5]->GetMarker ( 3 ) |l6[i6]->GetMarker ( 3 ) |l7[i7]->GetMarker ( 3 ) |l8[i8]->GetMarker ( 3 ) |l9[i9]->GetMarker ( 3 ),3 );
                  
                    c.SetInsertHistory(kFALSE);
                    if(l1[i1]->GetPointerToLinks() != 0) c.AddLinks(*l1[i1]->GetPointerToLinks());
                    if(l2[i2]->GetPointerToLinks() != 0) c.AddLinks(*l2[i2]->GetPointerToLinks());
                    if(l3[i3]->GetPointerToLinks() != 0) c.AddLinks(*l3[i3]->GetPointerToLinks());
                    if(l4[i4]->GetPointerToLinks() != 0) c.AddLinks(*l4[i4]->GetPointerToLinks());
                    if(l5[i5]->GetPointerToLinks() != 0) c.AddLinks(*l5[i5]->GetPointerToLinks());
                    if(l6[i6]->GetPointerToLinks() != 0) c.AddLinks(*l6[i6]->GetPointerToLinks());
                    if(l7[i7]->GetPointerToLinks() != 0) c.AddLinks(*l7[i7]->GetPointerToLinks());
                    if(l8[i8]->GetPointerToLinks() != 0) c.AddLinks(*l8[i8]->GetPointerToLinks());
                    if(l9[i9]->GetPointerToLinks() != 0) c.AddLinks(*l9[i9]->GetPointerToLinks());
                  
                  
                    Put ( &c );
                    // after putting (does a copy and drops daughter links)
                    RhoCandidate* cInList = Get(GetLength()-1);
                    cInList->AddDaughterLinkSimple(l1[i1]) ;
                    cInList->AddDaughterLinkSimple(l2[i2]) ;
                    cInList->AddDaughterLinkSimple(l3[i3]) ;
                    cInList->AddDaughterLinkSimple(l4[i4]) ;
                    cInList->AddDaughterLinkSimple(l5[i5]) ;
                    cInList->AddDaughterLinkSimple(l6[i6]) ;
                    cInList->AddDaughterLinkSimple(l7[i7]) ;
                    cInList->AddDaughterLinkSimple(l8[i8]) ;
                    cInList->AddDaughterLinkSimple(l9[i9]) ;
                  } //loop9
                } //loop8
              } //loop7
            } //loop6
          } //loop5
        } //loop4
      } //loop3
    } //loop2
  } //loop1
}

void RhoCandList::CombineAndAppend ( RhoCandList& l1, RhoCandList& l2, RhoCandList& l3, RhoCandList& l4, RhoCandList& l5, RhoCandList& l6, RhoCandList& l7, RhoCandList& l8, RhoCandList& l9, RhoCandList& l10)
{
  TLorentzVector vl;
  Double_t charge;

  const int len1=l1.GetLength();
  const int len2=l2.GetLength();
  const int len3=l3.GetLength();
  const int len4=l4.GetLength();
  const int len5=l5.GetLength();
  const int len6=l6.GetLength();
  const int len7=l7.GetLength();
  const int len8=l8.GetLength();
  const int len9=l9.GetLength();
  const int len10=l10.GetLength();

  int i1,i2,i3,i4,i5,i6,i7,i8,i9,i10;
  int st2,st3,st4,st5,st6,st7,st8,st9,st10;

  for ( i1=0; i1<len1; i1++ ) {
    st2=0;
    if ( &l2==&l1 ) { st2=i1+1; }

    for ( i2=st2; i2<len2; i2++ ) {
      if ( l1[i1]->Overlaps ( l2[i2] ) ) { continue; }

      st3=0;
      if ( &l3==&l2 ) { st3=i2+1; }
      else if ( &l3==&l1 ) { st3=i1+1; }

      for ( i3=st3; i3<len3; i3++ ) {
        if ( l3[i3]->Overlaps ( l2[i2] ) 
          || l3[i3]->Overlaps ( l1[i1] ) ) { continue; }

        st4=0;
        if ( &l4==&l3 ) { st4=i3+1; }
        else if ( &l4==&l2 ) { st4=i2+1; }
        else if ( &l4==&l1 ) { st4=i1+1; }

        for ( i4=st4; i4<len4; i4++ ) {
          if ( l4[i4]->Overlaps ( l3[i3] ) 
            || l4[i4]->Overlaps ( l2[i2] ) 
            || l4[i4]->Overlaps ( l1[i1] ) ) { continue; }

          st5=0;
          if ( &l5==&l4 ) { st5=i4+1; }
          else if ( &l5==&l3 ) { st5=i3+1; }
          else if ( &l5==&l2 ) { st5=i2+1; }
          else if ( &l5==&l1 ) { st5=i1+1; }

          for ( i5=st5; i5<len5; i5++ ) {
            if ( l5[i5]->Overlaps ( l4[i4] ) 
              || l5[i5]->Overlaps ( l3[i3] ) 
              || l5[i5]->Overlaps ( l2[i2] ) 
              || l5[i5]->Overlaps ( l1[i1] ) ) { continue; }
            
            st6=0;
            if ( &l6==&l5 ) { st6=i5+1; }
            else if ( &l6==&l4 ) { st6=i4+1; }
            else if ( &l6==&l3 ) { st6=i3+1; }
            else if ( &l6==&l2 ) { st6=i2+1; }
            else if ( &l6==&l1 ) { st6=i1+1; }
            for ( i6=st6; i6<len6; i6++ ) { //loop 6
              if (l6[i6]->Overlaps ( l5[i5] ) 
               || l6[i6]->Overlaps ( l4[i4] ) 
               || l6[i6]->Overlaps ( l3[i3] ) 
               || l6[i6]->Overlaps ( l2[i2] ) 
               || l6[i6]->Overlaps ( l1[i1] ) ) { continue; }//overlap 6
              
              st7=0;
              if ( &l7==&l6 ) { st7=i6+1; }
              else if ( &l7==&l5 ) { st7=i5+1; }
              else if ( &l7==&l4 ) { st7=i4+1; }
              else if ( &l7==&l3 ) { st7=i3+1; }
              else if ( &l7==&l2 ) { st7=i2+1; }
              else if ( &l7==&l1 ) { st7=i1+1; }
              for ( i7=st7; i7<len7; i7++ ) { //loop 7
                if (l7[i7]->Overlaps ( l6[i6] ) 
                 || l7[i7]->Overlaps ( l5[i5] ) 
                 || l7[i7]->Overlaps ( l4[i4] ) 
                 || l7[i7]->Overlaps ( l3[i3] ) 
                 || l7[i7]->Overlaps ( l2[i2] ) 
                 || l7[i7]->Overlaps ( l1[i1] ) ) { continue; }//overlap 7
                
                st8=0;
                if ( &l8==&l7 ) { st8=i7+1; }
                else if ( &l8==&l6 ) { st8=i6+1; }
                else if ( &l8==&l5 ) { st8=i5+1; }
                else if ( &l8==&l4 ) { st8=i4+1; }
                else if ( &l8==&l3 ) { st8=i3+1; }
                else if ( &l8==&l2 ) { st8=i2+1; }
                else if ( &l8==&l1 ) { st8=i1+1; }
                for ( i8=st8; i8<len8; i8++ ) { //loop 8
                  if (l8[i8]->Overlaps ( l7[i7] ) 
                   || l8[i8]->Overlaps ( l6[i6] ) 
                   || l8[i8]->Overlaps ( l5[i5] ) 
                   || l8[i8]->Overlaps ( l4[i4] ) 
                   || l8[i8]->Overlaps ( l3[i3] ) 
                   || l8[i8]->Overlaps ( l2[i2] ) 
                   || l8[i8]->Overlaps ( l1[i1] ) ) { continue; }//overlap 8
                  
                  st9=0;
                  if ( &l9==&l8 ) { st9=i8+1; }
                  else if ( &l9==&l7 ) { st9=i7+1; }
                  else if ( &l9==&l6 ) { st9=i6+1; }
                  else if ( &l9==&l5 ) { st9=i5+1; }
                  else if ( &l9==&l4 ) { st9=i4+1; }
                  else if ( &l9==&l3 ) { st9=i3+1; }
                  else if ( &l9==&l2 ) { st9=i2+1; }
                  else if ( &l9==&l1 ) { st9=i1+1; }
                  for ( i9=st9; i9<len9; i9++ ) { //loop 9
                    if (l9[i9]->Overlaps ( l8[i8] ) 
                     || l9[i9]->Overlaps ( l7[i7] ) 
                     || l9[i9]->Overlaps ( l6[i6] ) 
                     || l9[i9]->Overlaps ( l5[i5] ) 
                     || l9[i9]->Overlaps ( l4[i4] ) 
                     || l9[i9]->Overlaps ( l3[i3] ) 
                     || l9[i9]->Overlaps ( l2[i2] ) 
                     || l9[i9]->Overlaps ( l1[i1] ) ) { continue; }//overlap 9
                    
                    st10=0;
                    if ( &l10==&l9 ) { st10=i9+1; }
                    else if ( &l10==&l8 ) { st10=i8+1; }
                    else if ( &l10==&l7 ) { st10=i7+1; }
                    else if ( &l10==&l6 ) { st10=i6+1; }
                    else if ( &l10==&l5 ) { st10=i5+1; }
                    else if ( &l10==&l4 ) { st10=i4+1; }
                    else if ( &l10==&l3 ) { st10=i3+1; }
                    else if ( &l10==&l2 ) { st10=i2+1; }
                    else if ( &l10==&l1 ) { st10=i1+1; }
                    for ( i10=st10; i10<len10; i10++ ) { //loop 10
                      if ( l10[i10]->Overlaps ( l9[i9] ) 
                        || l10[i10]->Overlaps ( l8[i8] ) 
                        || l10[i10]->Overlaps ( l7[i7] ) 
                        || l10[i10]->Overlaps ( l6[i6] ) 
                        || l10[i10]->Overlaps ( l5[i5] ) 
                        || l10[i10]->Overlaps ( l4[i4] ) 
                        || l10[i10]->Overlaps ( l3[i3] ) 
                        || l10[i10]->Overlaps ( l2[i2] ) 
                        || l10[i10]->Overlaps ( l1[i1] ) ) { continue; }//overlap 10



                       vl=l1[i1]->P4() +l2[i2]->P4() +l3[i3]->P4() +l4[i4]->P4() +l5[i5]->P4() +l6[i6]->P4() +l7[i7]->P4() +l8[i8]->P4() +l9[i9]->P4() +l10[i10]->P4();
                       charge=l1[i1]->Charge() +l2[i2]->Charge() +l3[i3]->Charge() +l4[i4]->Charge() +l5[i5]->Charge() +l6[i6]->Charge() +l7[i7]->Charge() +l8[i8]->Charge() +l9[i9]->Charge() +l10[i10]->Charge();
                    
                       RhoCandidate c ( vl,charge );
                       c.SetCovP4 ( l1[i1]->P4Cov() +l2[i2]->P4Cov() +l3[i3]->P4Cov() +l4[i4]->P4Cov() +l5[i5]->P4Cov()  +l6[i6]->P4Cov()  +l7[i7]->P4Cov()  +l8[i8]->P4Cov()  +l9[i9]->P4Cov()  +l10[i10]->P4Cov() );
                    
                       c.SetMarker ( l1[i1]->GetMarker ( 0 ) |l2[i2]->GetMarker ( 0 ) |l3[i3]->GetMarker ( 0 ) |l4[i4]->GetMarker ( 0 ) |l5[i5]->GetMarker ( 0 ) |l6[i6]->GetMarker ( 0 ) |l7[i7]->GetMarker ( 0 ) |l8[i8]->GetMarker ( 0 ) |l9[i9]->GetMarker ( 0 ) |l10[i10]->GetMarker ( 0 ),0 );
                       c.SetMarker ( l1[i1]->GetMarker ( 1 ) |l2[i2]->GetMarker ( 1 ) |l3[i3]->GetMarker ( 1 ) |l4[i4]->GetMarker ( 1 ) |l5[i5]->GetMarker ( 1 ) |l6[i6]->GetMarker ( 1 ) |l7[i7]->GetMarker ( 1 ) |l8[i8]->GetMarker ( 1 ) |l9[i9]->GetMarker ( 1 ) |l10[i10]->GetMarker ( 1 ),1 );
                       c.SetMarker ( l1[i1]->GetMarker ( 2 ) |l2[i2]->GetMarker ( 2 ) |l3[i3]->GetMarker ( 2 ) |l4[i4]->GetMarker ( 2 ) |l5[i5]->GetMarker ( 2 ) |l6[i6]->GetMarker ( 2 ) |l7[i7]->GetMarker ( 2 ) |l8[i8]->GetMarker ( 2 ) |l9[i9]->GetMarker ( 2 ) |l10[i10]->GetMarker ( 2 ),2 );
                       c.SetMarker ( l1[i1]->GetMarker ( 3 ) |l2[i2]->GetMarker ( 3 ) |l3[i3]->GetMarker ( 3 ) |l4[i4]->GetMarker ( 3 ) |l5[i5]->GetMarker ( 3 ) |l6[i6]->GetMarker ( 3 ) |l7[i7]->GetMarker ( 3 ) |l8[i8]->GetMarker ( 3 ) |l9[i9]->GetMarker ( 3 ) |l10[i10]->GetMarker ( 3 ),3 );
                    
                       c.SetInsertHistory(kFALSE);
                       if(l1[i1]->GetPointerToLinks() != 0)	c.AddLinks(*l1[i1]->GetPointerToLinks());
                       if(l2[i2]->GetPointerToLinks() != 0) c.AddLinks(*l2[i2]->GetPointerToLinks());
                       if(l3[i3]->GetPointerToLinks() != 0)	c.AddLinks(*l3[i3]->GetPointerToLinks());
                       if(l4[i4]->GetPointerToLinks() != 0)	c.AddLinks(*l4[i4]->GetPointerToLinks());
                       if(l5[i5]->GetPointerToLinks() != 0) c.AddLinks(*l5[i5]->GetPointerToLinks());
                       if(l6[i6]->GetPointerToLinks() != 0) c.AddLinks(*l6[i6]->GetPointerToLinks());
                       if(l7[i7]->GetPointerToLinks() != 0) c.AddLinks(*l7[i7]->GetPointerToLinks());
                       if(l8[i8]->GetPointerToLinks() != 0) c.AddLinks(*l8[i8]->GetPointerToLinks());
                       if(l9[i9]->GetPointerToLinks() != 0) c.AddLinks(*l9[i9]->GetPointerToLinks());
                       if(l10[i10]->GetPointerToLinks() != 0) c.AddLinks(*l10[i10]->GetPointerToLinks());
                    
                    
                       Put ( &c );
                       // after putting (does a copy and drops daughter links)
                       RhoCandidate* cInList = Get(GetLength()-1);
                       cInList->AddDaughterLinkSimple(l1[i1]) ;
                       cInList->AddDaughterLinkSimple(l2[i2]) ;
                       cInList->AddDaughterLinkSimple(l3[i3]) ;
                       cInList->AddDaughterLinkSimple(l4[i4]) ;
                       cInList->AddDaughterLinkSimple(l5[i5]) ;
                       cInList->AddDaughterLinkSimple(l6[i6]) ;
                       cInList->AddDaughterLinkSimple(l7[i7]) ;
                       cInList->AddDaughterLinkSimple(l8[i8]) ;
                       cInList->AddDaughterLinkSimple(l9[i9]) ;
                       cInList->AddDaughterLinkSimple(l10[i10]) ;
                    } //loop10
                  } //loop9
                } //loop8
              } //loop7
            } //loop6
          } //loop5
        } //loop4
      } //loop3
    } //loop2
  } //loop1
}

std::ostream&  operator << ( std::ostream& o, const RhoCandList& a )
{
  a.PrintOn ( o );
  return o;
}

//extern "C" void qsort(void *, size_t, size_t, int (const void *,const void *));

typedef int compare_function ( const void*, const void* );

void RhoCandList::Sort ( int ( *compfunc ) ( const RhoCandidate**, const RhoCandidate** ) )
{
  qsort ( fOwnList, GetNumberOfTracks(), sizeof ( void* ), ( compare_function* ) compfunc );
}


void RhoCandList::Select ( RhoCandList& l, Bool_t ( *selfunc ) ( RhoCandidate* ) )
{
  Cleanup();
  const Int_t n = l.GetNumberOfTracks();
  for ( Int_t i=0; i<n; i++ ) {
    RhoCandidate* c = Get ( i );
    if ( selfunc ( c ) ) {
      Put ( c );
    }
  }
}

// Destructive selection

void RhoCandList::Select ( RhoParticleSelectorBase* pidmgr )
{
  const Int_t n = GetNumberOfTracks();
  for ( Int_t i=0; i<n; i++ ) {
    RhoCandidate* c = Get ( i );
    if ( !pidmgr->Accept ( c ) ) { fOwnList->RemoveAt ( i ); }
  }
  fOwnList->Compress();
}

// Non-destructive selection

void RhoCandList::Select ( RhoCandList& l, RhoParticleSelectorBase* pidmgr )
{
  Cleanup();
  const Int_t n = l.GetNumberOfTracks();
  for ( Int_t i=0; i<n; i++ ) {
    RhoCandidate* c = l.Get ( i );
    if ( pidmgr->Accept ( c ) ) {
      Put ( c );
    }
  }
}

void RhoCandList::Append ( RhoCandList& l, RhoParticleSelectorBase* pidmgr )
{
  const Int_t n = l.GetNumberOfTracks();
  for ( Int_t i=0; i<n; i++ ) {
    RhoCandidate* c = l.Get ( i );
    if ( 0==pidmgr || pidmgr->Accept ( c ) ) {
      Put ( c );
    }
  }
}
