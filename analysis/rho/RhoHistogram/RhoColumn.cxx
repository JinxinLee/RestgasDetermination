//////////////////////////////////////////////////////////////////////////
//                                                                      //
// RhoColumn                                                            //
//                                                                      //
// Nested class hierarchy to hold information about RhoTuple columns.   //
//                                                                      //
// Author List:                                                         //
// Marcel Kunze, RUB,Mar. 99                                            //
// Apr.2001 (MK),Inherit from TNamed to support THashList               //
// Copyright (C) 1999-2001,Ruhr-University Bochum.                      //
// Ralf Kliemt, HIM/GSI Feb.2013 (Cleanup & Restructuring)              //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TTree.h"
#include "TBranch.h"

#include "RhoHistogram/RhoColumn.h"

#include <iostream>
using namespace std;

// Bool columns,stored as char (= 8 bit unsigned int):
RhoBoolColumn::RhoBoolColumn ( const char* l,const Bool_t& v,const Bool_t& d,TTree* tp ) :
  RhoColumn ( l ),fDefValue ( d )
{
  // Create a new branch:
  fPointer = new Char_t;
  * ( Char_t* ) fPointer = v;
  TString leafs ( l ) ;
  leafs += "/b";
  fBranch = tp->Branch ( fLabel,fPointer,&*leafs,8000 );
}

RhoBoolArrColumn::RhoBoolArrColumn ( const char* l,
                                     const RhoHTAbsValVector<Bool_t> & v,
                                     const Bool_t& d,TTree* tp ) :
  RhoColumn ( l ),fDefValue ( d )
{
  // Create a new branch:
  fMax = v.length();
  Char_t* bp = new Char_t[fMax];
  fPointer = bp;
  for ( Int_t i = 0; i < fMax; ++i ) { bp[i] = v ( i ); }
  TString leafs ( fLabel );
  leafs += "[";
  char buf[33];
  sprintf ( buf,"%i",fMax );
  leafs += buf;
  leafs += "]/b";
  fBranch = tp->Branch ( fLabel, ( void* ) &bp[0],&*leafs,8000 );
}

void RhoBoolArrColumn::SetValue ( const void* p,RhoColumn*  )
{
  const RhoHTAbsValVector<Bool_t>* vp = ( const RhoHTAbsValVector<Bool_t>* ) p;
  if ( (Int_t)vp->length() < fMax ) {
    cerr << "BoolArrColumn::SetValue: input vector too short,"
         << "use default values" << endl;
    SetDefValue();
  } else {
    for ( Int_t i = 0; i < fMax; ++i ) { ( ( Char_t* ) fPointer ) [i] = ( *vp ) ( i ); }
  }
}

RhoBoolDynArrColumn::RhoBoolDynArrColumn ( const char* l,
    const RhoHTAbsValVector<Bool_t> & v,
    const Bool_t& d,RhoColumn* inp,
    TTree* tp ) :
  RhoColumn ( l ),fDefValue ( d ),fIndexPtr ( inp )
{
  // Make a new branch:
  Int_t* np = ( Int_t* ) fIndexPtr->GetPointer();
  Char_t* bp = new Char_t[*np];
  fPointer = bp;
  for ( Int_t i = 0; i < *np; ++i ) { bp[i] = v ( i ); }
  TString leafs ( fLabel );
  leafs += "[";
  leafs += fIndexPtr->GetLabel();
  leafs += "]/b";
  fBranch = tp->Branch ( fLabel, ( void* ) &bp[0],&*leafs,8000 );
}

void RhoBoolDynArrColumn::SetDefValue()
{
  if ( fPointer ) { delete [] ( Bool_t* ) fPointer; }
  Int_t fMax = * ( ( Int_t* ) ( fIndexPtr->GetPointer() ) );
  Char_t* bp = new Char_t[fMax];
  fPointer = bp;
  for ( Int_t i = 0; i < fMax; ++i ) { bp[i] = fDefValue; }
  fBranch->SetAddress ( &bp[0] );
}

void RhoBoolDynArrColumn::SetValue ( const void* p,RhoColumn* cp )
{
  const RhoHTAbsValVector<Bool_t>* vp = ( const RhoHTAbsValVector<Bool_t>* ) p;
  Int_t* np = ( Int_t* ) cp->GetPointer();
  if ( *np > (int)vp->length() ) {
    cerr << "BoolDynArrColumn::SetValue: input vector too short,"
         << "use default values" << endl;
    SetDefValue();
  } else {
    Char_t* bp = new Char_t[*np];
    if ( fPointer ) { delete[] ( Bool_t* ) fPointer; }
    fPointer = bp;
    for ( Int_t i = 0; i < *np; ++i ) { bp[i] = ( *vp ) ( i ); }
    fBranch->SetAddress ( &bp[0] );
  }
}


// Int columns:
RhoIntColumn::RhoIntColumn ( const char* l,const Int_t& v,const Int_t& d,TTree* tp ) :
  RhoColumn ( l ),fDefValue ( d )
{
  // Create a new branch:
  fPointer = new Int_t;
  * ( Int_t* ) fPointer = v;
  TString leafs ( l ) ;
  leafs += "/I";
  fBranch = tp->Branch ( fLabel,fPointer,&*leafs,8000 );
}

RhoIntArrColumn::RhoIntArrColumn ( const char* l,
                                   const RhoHTAbsValVector<Int_t> & v,
                                   const Int_t& d,TTree* tp ) :
  RhoColumn ( l ),fDefValue ( d )
{
  // Create a new branch:
  fMax = v.length();
  Int_t* ip = new Int_t[fMax];
  fPointer = ip;
  for ( Int_t i = 0; i < fMax; ++i ) { ip[i] = v ( i ); }
  TString leafs ( fLabel );
  leafs += "[";
  char buf[33];
  sprintf ( buf,"%i",fMax );
  leafs += buf;
  leafs += "]/I";
  fBranch = tp->Branch ( fLabel,&ip[0],&*leafs,8000 );
}

void RhoIntArrColumn::SetValue ( const void* p,RhoColumn*  )
{
  const RhoHTAbsValVector<Int_t>* vp = ( const RhoHTAbsValVector<Int_t>* ) p;
  if ( (Int_t)vp->length() < fMax ) {
    cerr << "IntArrColumn::SetValue: input vector too short,"
         << "use default values" << endl;
    SetDefValue();
  } else {
    for ( Int_t i = 0; i < fMax; ++i ) { ( ( Int_t* ) fPointer ) [i] = ( *vp ) ( i ); }
  }
}

RhoIntDynArrColumn::RhoIntDynArrColumn ( const char* l,
    const RhoHTAbsValVector<Int_t> & v,
    const Int_t& d,RhoColumn* inp,
    TTree* tp ) :
  RhoColumn ( l ),fDefValue ( d ),fIndexPtr ( inp )
{
  // Make a new branch:
  Int_t* np = ( Int_t* ) fIndexPtr->GetPointer();
  Int_t* ip = new Int_t[*np];
  fPointer = ip;
  for ( Int_t i = 0; i < *np; ++i ) { ip[i] = v ( i ); }
  TString leafs ( fLabel );
  leafs += "[";
  leafs += fIndexPtr->GetLabel();
  leafs += "]/I";
  fBranch = tp->Branch ( fLabel,&ip[0],&*leafs,8000 );
}

void RhoIntDynArrColumn::SetDefValue()
{
  if ( fPointer ) { delete [] ( Int_t* ) fPointer; }
  Int_t fMax = * ( ( Int_t* ) ( fIndexPtr->GetPointer() ) );
  Int_t* ip = new Int_t[fMax];
  fPointer = ip;
  for ( Int_t i = 0; i < fMax; ++i ) { ip[i] = fDefValue; }
  fBranch->SetAddress ( &ip[0] );
}

void RhoIntDynArrColumn::SetValue ( const void* p,RhoColumn* cp )
{
  const RhoHTAbsValVector<Int_t>* vp = ( const RhoHTAbsValVector<Int_t>* ) p;
  Int_t* np = ( Int_t* ) cp->GetPointer();
  if ( *np > (int)vp->length() ) {
    cerr << "IntDynArrColumn::SetValue: input vector too short,"
         << "use default values" << endl;
    SetDefValue();
  } else {
    Int_t* ip = new Int_t[*np];
    if ( fPointer ) { delete[] ( Int_t* ) fPointer; }
    fPointer = ip;
    for ( Int_t i = 0; i < *np; ++i ) { ip[i] = ( *vp ) ( i ); }
    fBranch->SetAddress ( &ip[0] );
  }
}

// Float columns:
RhoFloatColumn::RhoFloatColumn ( const char* l,const Float_t& v,const Float_t& d,TTree* tp ) :
  RhoColumn ( l ),fDefValue ( d )
{
  // Create a new branch:
  fPointer = new Float_t;
  * ( Float_t* ) fPointer = v;
  TString leafs ( l ) ;
  leafs += "/F";
  fBranch = tp->Branch ( fLabel,fPointer,&*leafs,8000 );
}

RhoFloatArrColumn::RhoFloatArrColumn ( const char* l,
                                       const RhoHTAbsValVector<Float_t> & v,
                                       const Float_t& d,TTree* tp ) :
  RhoColumn ( l ),fDefValue ( d )
{
  // Create a new branch:
  fMax = v.length();
  Float_t* fp = new Float_t[fMax];
  fPointer = fp;
  for ( Int_t i = 0; i < fMax; ++i ) { fp[i] = v ( i ); }
  TString leafs ( fLabel );
  leafs += "[";
  char buf[33];
  sprintf ( buf,"%i",fMax );
  leafs += buf;
  leafs += "]/F";
  fBranch = tp->Branch ( fLabel,&fp[0],&*leafs,8000 );
}

/*void RhoFloatArrColumn::SetValue ( const void* p,RhoColumn* cp )
{
  const RhoHTAbsValVector<Float_t>* vp = ( const RhoHTAbsValVector<Float_t>* ) p;
  if ( vp->length() < fMax ) {
    cerr << "FloatArrColumn::SetValue: input vector too short,"
         << "use default values" << endl;
    SetDefValue();
  } else {
    for ( Int_t i = 0; i < fMax; ++i ) { ( ( Float_t* ) fPointer ) [i] = ( *vp ) ( i ); }
  }
}*/
void RhoFloatArrColumn::SetValue ( const void* p,RhoColumn*  )
{
  const TVector *vp = (const TVector*) p;
  if ( vp->GetNoElements() < fMax ) {
    cerr << "FloatArrColumn::SetValue: input vector too short,"
         << "use default values" << endl;
    SetDefValue();
  } else {
    for ( Int_t i = 0; i < fMax; ++i ) { ( ( Float_t* ) fPointer ) [i] = ( *vp ) ( i ); }
  }
}

RhoFloatDynArrColumn::RhoFloatDynArrColumn ( const char* l,
    const RhoHTAbsValVector<Float_t> & v,
    const Float_t& d,RhoColumn* ip,
    TTree* tp ) :
  RhoColumn ( l ),fDefValue ( d ),fIndexPtr ( ip )
{
  // Make a new branch:
  Int_t* np = ( Int_t* ) fIndexPtr->GetPointer();
  Float_t* fp = new Float_t[*np];
  fPointer = fp;
  for ( Int_t i = 0; i < *np; ++i ) { fp[i] = v ( i ); }
  TString leafs ( fLabel );
  leafs += "[";
  leafs += fIndexPtr->GetLabel();
  leafs += "]/F";
  fBranch = tp->Branch ( fLabel,&fp[0],&*leafs,8000 );
}

void RhoFloatDynArrColumn::SetDefValue()
{
  if ( fPointer ) { delete [] ( Float_t* ) fPointer; }
  Int_t fMax = * ( ( Int_t* ) ( fIndexPtr->GetPointer() ) );
  Float_t* fp = new Float_t[fMax];
  fPointer = fp;
  for ( Int_t i = 0; i < fMax; ++i ) { fp[i] = fDefValue; }
  fBranch->SetAddress ( &fp[0] );
}

/*void RhoFloatDynArrColumn::SetValue ( const void* p,RhoColumn* cp )
{
  const RhoHTAbsValVector<Float_t>* vp = ( const RhoHTAbsValVector<Float_t>* ) p;
  Int_t* np = ( Int_t* ) cp->GetPointer();
  if ( *np > vp->length() ) {
    cerr << "IntDynArrColumn::SetValue: input vector too short,"
         << "use default values" << endl;
    SetDefValue();
  } else {
    Float_t* fp = new Float_t[*np];
    if ( fPointer ) { delete[] ( Float_t* ) fPointer; }
    fPointer = fp;
    for ( Int_t i = 0; i < *np; ++i ) { fp[i] = ( *vp ) ( i ); }
    fBranch->SetAddress ( &fp[0] );
  }
}*/
void RhoFloatDynArrColumn::SetValue ( const void* p,RhoColumn* cp )
{
  const TVector *vp = (const TVector*) p;
  Int_t* np = ( Int_t* ) cp->GetPointer();
  if ( *np > vp->GetNoElements() ) {
    cerr << "IntDynArrColumn::SetValue: input vector too short,"
         << "use default values" << endl;
    SetDefValue();
  } else {
    Float_t* fp = new Float_t[*np];
    if ( fPointer ) { delete[] ( Float_t* ) fPointer; }
    fPointer = fp;
    for ( Int_t i = 0; i < *np; ++i ) { fp[i] = ( *vp ) ( i ); }
    fBranch->SetAddress ( &fp[0] );
  }
}

RhoFloatArrColumn::RhoFloatArrColumn ( const char* l,const TVector& v,const Float_t& d,TTree* tp ) :
  RhoColumn ( l ),fDefValue ( d )
{
  // Create a new branch:
  fMax = v.GetNoElements();
  Real_t* fp = new Real_t[fMax];
  fPointer = fp;
  for ( Int_t i = 0; i < fMax; ++i ) { fp[i] = v ( i ); }
  TString leafs ( fLabel );
  leafs += "[";
  char buf[33];
  sprintf ( buf,"%i",fMax );
  leafs += buf;
  leafs += "]/F";
  fBranch = tp->Branch ( fLabel,&fp[0],&*leafs,8000 );
}

RhoFloatDynArrColumn::RhoFloatDynArrColumn ( const char* l,const TVector& v,const Float_t& d,RhoColumn* ip,TTree* tp ) :
  RhoColumn ( l ),fDefValue ( d ),fIndexPtr ( ip )
{
  // Make a new branch:
  Int_t* np = ( Int_t* ) fIndexPtr->GetPointer();
  Real_t* fp = new Real_t[*np];
  fPointer = fp;
  for ( Int_t i = 0; i < *np; ++i ) { fp[i] = v ( i ); }
  TString leafs ( fLabel );
  leafs += "[";
  leafs += fIndexPtr->GetLabel();
  leafs += "]/F";
  fBranch = tp->Branch ( fLabel,&fp[0],&*leafs,8000 );
}


// Double columns:
RhoDoubleColumn::RhoDoubleColumn ( const char* l,const Double_t& v,const Double_t& d,TTree* tp ) :
  RhoColumn ( l ),fDefValue ( d )
{
  // Create a new branch:
  fPointer = new Double_t;
  * ( Double_t* ) fPointer = v;
  TString leafs ( l ) ;
  leafs += "/D";
  fBranch = tp->Branch ( fLabel,fPointer,&*leafs,8000 );
}

RhoDoubleArrColumn::RhoDoubleArrColumn ( const char* l,
    const RhoHTAbsValVector<Double_t> & v,
    const Double_t& d,TTree* tp ) :
  RhoColumn ( l ),fDefValue ( d )
{
  // Create a new branch:
  fMax = v.length();
  Double_t* dp = new Double_t[fMax];
  fPointer = dp;
  for ( Int_t i = 0; i < fMax; ++i ) { dp[i] = v ( i ); }
  TString leafs ( fLabel );
  leafs += "[";
  char buf[33];
  sprintf ( buf,"%i",fMax );
  leafs += buf;
  leafs += "]/D";
  fBranch = tp->Branch ( fLabel,&dp[0],&*leafs,8000 );
}

void RhoDoubleArrColumn::SetValue ( const void* p,RhoColumn*  )
{
  const RhoHTAbsValVector<Double_t>* vp = ( const RhoHTAbsValVector<Double_t>* ) p;
  if ( (Int_t)vp->length() < fMax ) {
    cerr << "DoubleArrColumn::SetValue: input vector too short,"
         << "use default values" << endl;
    SetDefValue();
  } else {
    for ( Int_t i = 0; i < fMax; ++i ) { ( ( Double_t* ) fPointer ) [i] = ( *vp ) ( i ); }
  }
}

RhoDoubleDynArrColumn::RhoDoubleDynArrColumn ( const char* l,
    const RhoHTAbsValVector<Double_t> & v,
    const Double_t& d,RhoColumn* ip,
    TTree* tp ) :
  RhoColumn ( l ),fDefValue ( d ),fIndexPtr ( ip )
{
  // Make a new branch:
  Int_t* np = ( Int_t* ) ( fIndexPtr->GetPointer() );
  Double_t* dp = new Double_t[*np];
  fPointer = dp;
  for ( Int_t i = 0; i < *np; ++i ) { dp[i] = v ( i ); }
  TString leafs ( fLabel );
  leafs += "[";
  leafs += fIndexPtr->GetLabel();
  leafs += "]/D";
  fBranch = tp->Branch ( fLabel,&dp[0],&*leafs,8000 );
}

void RhoDoubleDynArrColumn::SetDefValue()
{
  if ( fPointer ) { delete [] ( Double_t* ) fPointer; }
  Int_t fMax = * ( ( Int_t* ) ( fIndexPtr->GetPointer() ) );
  Double_t* dp = new Double_t[fMax];
  fPointer = dp;
  for ( Int_t i = 0; i < fMax; ++i ) { dp[i] = fDefValue; }
  fBranch->SetAddress ( &dp[0] );
}

void RhoDoubleDynArrColumn::SetValue ( const void* p,RhoColumn* cp )
{
  const RhoHTAbsValVector<Double_t>* vp = ( const RhoHTAbsValVector<Double_t>* ) p;
  Int_t* np = ( Int_t* ) cp->GetPointer();
  if ( *np > (int)vp->length() ) {
    cerr << "IntDynArrColumn::SetValue: input vector too short,"
         << "use default values" << endl;
    SetDefValue();
  } else {
    Double_t* dp = new Double_t[*np];
    if ( fPointer ) { delete[] ( Double_t* ) fPointer; }
    fPointer = dp;
    for ( Int_t i = 0; i < *np; ++i ) { dp[i] = ( *vp ) ( i ); }
    fBranch->SetAddress ( &dp[0] );
  }
}


// String column:
RhoStringColumn::RhoStringColumn ( const TString& lb,const TString& v,const TString& d,TTree* tp ) :
  RhoColumn ( lb ),fDefValue ( d )
{
  // Create a new branch:
  Int_t l = v.Length();
  char* cp = new char[l+1];
  fPointer = cp;
  strcpy ( cp,&*v );
  TString leafs ( lb );
  leafs += "/C";
  fBranch = tp->Branch ( lb, ( void* ) cp,&*leafs,8000 );
}

void RhoStringColumn::SetDefValue()
{
  if ( fPointer ) { delete[] ( TString* ) fPointer; }
  Int_t l = fDefValue.Length();
  char* cp = new char[l+1];
  fPointer = cp;
  strcpy ( cp,&*fDefValue );
  fBranch->SetAddress ( &cp[0] );
}

void RhoStringColumn::SetValue ( const void* p,RhoColumn*  )
{
  const char* cpin = ( const char* ) p;
  if ( fPointer ) { delete[] ( TString* ) fPointer; }
  Int_t l = strlen ( cpin );
  char* chp = new char[l+1];
  fPointer = chp;
  strcpy ( chp,cpin );
  fBranch->SetAddress ( &chp[0] );
}
