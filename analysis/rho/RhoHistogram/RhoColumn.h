#ifndef RHOCOLUMN_H
#define RHOCOLUMN_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// RhoColumn                                                            //
//                                                                      //
// Nested class hierarchy to hold information about TTuple columns.     //
//                                                                      //
// Author List:                                                         //
// Marcel Kunze,  RUB, Mar. 99                                          //
// Apr.2001 (MK), Inherit from TNamed to support THashList              //
// Copyright (C) 1999-2001, Ruhr-University Bochum.                     //
// Ralf Kliemt, HIM/GSI Feb.2013 (Cleanup & Restructuring)              //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TNamed.h"
#include "TString.h"
#include "TVector.h"

class TTree;
class TBranch;

#ifndef RhoHTRange_HH
#define RhoHTRange_HH

template<class T>
class RhoHTRange
{

    //--------------------
    // Declarations     --
    //--------------------

    // Typedefs, consts, and enums

    //--------------------
    // Instance Members --
    //--------------------

  public:

    // Constructors
    RhoHTRange() :
      fDefined ( kFALSE ), fLower ( 0 ), fUpper ( 0 ) {
    }

    RhoHTRange ( T lowerv, T upperv ) :
      fDefined ( kTRUE ), fLower ( lowerv ), fUpper ( upperv ) {
    }

    // Copy Constructor
    RhoHTRange ( const RhoHTRange<T>& o ) :
      fDefined ( o.fDefined ), fLower ( o.fLower ), fUpper ( o.fUpper ) {
    }

    // Destructor
    virtual ~RhoHTRange() {}

    // Operators

    RhoHTRange<T>& operator= ( const RhoHTRange<T> &o ) {
      if ( &o == this ) { return *this; }
      fDefined = o.fDefined ;
      fLower = o.fLower ;
      fUpper = o.fUpper ;
      return *this ;
    }

    // Selectors (const)
    Bool_t operator() () const {
      return fDefined ;
    }

    // get upper/lower limits
    T lower() const {
      return fLower ;
    }
    T upper() const {
      return fUpper ;
    }

    // Modifiers

  protected:

    // Helper functions

  private:

    // Friends

    // Data members
    Bool_t fDefined ;
    T fLower, fUpper ;

};
#endif


#ifndef RhoHTAbsValVector_HH
#define RhoHTAbsValVector_HH

template<class T>
class RhoHTAbsValVector
{

  public:

    // This must return the number of the stored elements
    virtual size_t length() const = 0 ;
    virtual ~RhoHTAbsValVector(){};
    // This provides access to the indifidual elements.
    // Index runs from 0 to length()-1. Here () means that we do not requre
    // index checking from it, though the real classe may do what they like.
    virtual const T& operator() ( size_t i ) const = 0 ;

};
#endif

// Parent class (abstract):
class RhoColumn : public TNamed
{
  public:
    RhoColumn ( const char* l ) :
      TNamed ( l,l ), fLabel ( l ), fUseDefValue ( 0 ), fPointer ( 0 ), fBranch ( 0 ) {}
    virtual ~RhoColumn() {}
    const TString& GetLabel() const {
      return fLabel;
    }
    TBranch* GetBrPointer() {
      return fBranch;
    }
    void* GetPointer() {
      return fPointer;
    }
    void SetPointer ( void* p ) {
      fPointer = p;
    }
    void SetUseDefValue ( Int_t b ) {
      fUseDefValue = b;
    }
    const Int_t& GetUseDefValue() const {
      return fUseDefValue;
    }
    virtual void SetDefValue() = 0;
    virtual void SetValue ( const void*, RhoColumn* cp=0 ) = 0;
  protected:
    TString fLabel;
    Int_t fUseDefValue;
    void* fPointer;
    TBranch* fBranch;
};

// Classes for Bool_t:
class RhoBoolColumn : public RhoColumn
{
  public:
    RhoBoolColumn ( const char*, const Bool_t&, const Bool_t&, TTree* );
    virtual ~RhoBoolColumn() {
      delete ( Bool_t* ) fPointer;
    }
    virtual void SetDefValue() {
      * ( Char_t* ) fPointer = fDefValue;
    }
    virtual void SetValue ( const void* p, RhoColumn*  ) {
      * ( Char_t* ) fPointer = * ( const Bool_t* ) p;
    }
  private:
    Bool_t fDefValue;
};

class RhoBoolArrColumn : public RhoColumn
{
  public:
    RhoBoolArrColumn ( const char*, const RhoHTAbsValVector<Bool_t> &, const Bool_t&,
                       TTree* );
    virtual ~RhoBoolArrColumn() {
      delete[] ( Bool_t* ) fPointer;
    }
    virtual void SetDefValue() {
      for ( Int_t i = 0; i < fMax; ++i ) { ( ( Char_t* ) fPointer ) [i] = fDefValue; }
    }
    virtual void SetValue ( const void*, RhoColumn* cp=0 );
  private:
    Bool_t fDefValue;
    Int_t fMax;
};

class RhoBoolDynArrColumn : public RhoColumn
{
  public:
    RhoBoolDynArrColumn ( const char*, const RhoHTAbsValVector<Bool_t> &,
                          const Bool_t&, RhoColumn*, TTree* );
    virtual ~RhoBoolDynArrColumn() {
      delete[] ( Bool_t* ) fPointer;
    }
    virtual void SetDefValue();
    virtual void SetValue ( const void*, RhoColumn* cp=0 );
  private:
    Bool_t fDefValue;
    RhoColumn* fIndexPtr;
};

// Classes for Int_t:
class RhoIntColumn : public RhoColumn
{
  public:
    RhoIntColumn ( const char*, const Int_t&, const Int_t&, TTree* );
    virtual ~RhoIntColumn() {
      delete ( Int_t* ) fPointer;
    }
    virtual void SetDefValue() {
      * ( Int_t* ) fPointer = fDefValue;
    }
    virtual void SetValue ( const void* p, RhoColumn*  ) {
      * ( Int_t* ) fPointer = * ( const Int_t* ) p;
    }
  private:
    Int_t fDefValue;
};

class RhoIntArrColumn : public RhoColumn
{
  public:
    RhoIntArrColumn ( const char*, const RhoHTAbsValVector<Int_t> &, const Int_t&,
                      TTree* );
    virtual ~RhoIntArrColumn() {
      delete[] ( Int_t* ) fPointer;
    }
    virtual void SetDefValue() {
      for ( Int_t i = 0; i < fMax; ++i ) { ( ( Int_t* ) fPointer ) [i] = fDefValue; }
    }
    virtual void SetValue ( const void*, RhoColumn* cp=0 );
  private:
    Int_t fDefValue;
    Int_t fMax;
};

class RhoIntDynArrColumn : public RhoColumn
{
  public:
    RhoIntDynArrColumn ( const char*, const RhoHTAbsValVector<Int_t> &,
                         const Int_t&, RhoColumn*, TTree* );
    virtual ~RhoIntDynArrColumn() {
      delete[] ( Int_t* ) fPointer;
    }
    virtual void SetDefValue();
    virtual void SetValue ( const void*, RhoColumn* cp=0 );
  private:
    Int_t fDefValue;
    RhoColumn* fIndexPtr;
};

// Classes for Float_t:
class RhoFloatColumn : public RhoColumn
{
  public:
    RhoFloatColumn ( const char*, const Float_t&, const Float_t&, TTree* );
    virtual ~RhoFloatColumn() {
      delete ( Float_t* ) fPointer;
    }
    virtual void SetDefValue() {
      * ( Float_t* ) fPointer = fDefValue;
    }
    virtual void SetValue ( const void* p, RhoColumn*  ) {
      * ( Float_t* ) fPointer = * ( const Float_t* ) p;
    }
  private:
    Float_t fDefValue;
};

class RhoFloatArrColumn : public RhoColumn
{
  public:
    RhoFloatArrColumn ( const char*, const RhoHTAbsValVector<Float_t> &, const Float_t&,
                        TTree* );
    RhoFloatArrColumn ( const char*, const TVector&, const Float_t&,
                        TTree* );
    virtual ~RhoFloatArrColumn() {
      delete[] ( Float_t* ) fPointer;
    }
    virtual void SetDefValue() {
      for ( Int_t i = 0; i < fMax; ++i ) { ( ( Float_t* ) fPointer ) [i] = fDefValue; }
    }
    virtual void SetValue ( const void*, RhoColumn* cp=0 );
  private:
    Float_t fDefValue;
    Int_t fMax;
};

class RhoFloatDynArrColumn : public RhoColumn
{
  public:
    RhoFloatDynArrColumn ( const char*, const RhoHTAbsValVector<Float_t> &,
                           const Float_t&, RhoColumn*, TTree* );
    RhoFloatDynArrColumn ( const char*, const TVector&,
                           const Float_t&, RhoColumn*, TTree* );
    virtual ~RhoFloatDynArrColumn() {
      delete[] ( Float_t* ) fPointer;
    }
    virtual void SetDefValue();
    virtual void SetValue ( const void*, RhoColumn* cp=0 );
  private:
    Float_t fDefValue;
    RhoColumn* fIndexPtr;
};

// Classes for Double_t:
class RhoDoubleColumn : public RhoColumn
{
  public:
    RhoDoubleColumn ( const char*, const Double_t&, const Double_t&, TTree* );
    virtual ~RhoDoubleColumn() {
      delete ( Double_t* ) fPointer;
    }
    virtual void SetDefValue() {
      * ( Double_t* ) fPointer = fDefValue;
    }
    virtual void SetValue ( const void* p, RhoColumn*  ) {
      * ( Double_t* ) fPointer = * ( const Double_t* ) p;
    }
  private:
    Double_t fDefValue;
};

class RhoDoubleArrColumn : public RhoColumn
{
  public:
    RhoDoubleArrColumn ( const char*, const RhoHTAbsValVector<Double_t> &,
                         const Double_t&, TTree* );
    virtual ~RhoDoubleArrColumn() {
      delete[] ( Double_t* ) fPointer;
    }
    virtual void SetDefValue() {
      for ( Int_t i = 0; i < fMax; ++i ) { ( ( Double_t* ) fPointer ) [i] = fDefValue; }
    }
    virtual void SetValue ( const void*, RhoColumn* cp=0 );
  private:
    Double_t fDefValue;
    Int_t fMax;
};

class RhoDoubleDynArrColumn : public RhoColumn
{
  public:
    RhoDoubleDynArrColumn ( const char*, const RhoHTAbsValVector<Double_t> &,
                            const Double_t&, RhoColumn*, TTree* );
    virtual ~RhoDoubleDynArrColumn() {
      delete[] ( Double_t* ) fPointer;
    }
    virtual void SetDefValue();
    virtual void SetValue ( const void*, RhoColumn* cp=0 );
  private:
    Double_t fDefValue;
    RhoColumn* fIndexPtr;
};

// String column:
class RhoStringColumn : public RhoColumn
{
  public:
    RhoStringColumn ( const TString&, const TString&,
                      const TString&, TTree* );
    virtual ~RhoStringColumn() {
      delete[] ( TString* ) fPointer;
    }
    virtual void SetDefValue();
    virtual void SetValue ( const void*, RhoColumn* cp=0 );
  private:
    TString fDefValue;
};

#endif
