#ifndef TCOLUMN_H
#define TCOLUMN_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TColumn                                                              //
//                                                                      //
// Nested class hierarchy to hold information about TTuple columns.     //
//                                                                      //
// Author List:                                                         //
// Marcel Kunze,  RUB, Mar. 99                                          //
// Apr.2001 (MK), Inherit from TNamed to support THashList              //
// Copyright (C) 1999-2001, Ruhr-University Bochum.                     //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TNamed.h"
#include "TString.h"
#include "TVector.h"

class TTree;
class TBranch;

#ifndef HTRange_HH
#define HTRange_HH

template<class T>
class HTRange {
   
    //--------------------
    // Declarations     --
    //--------------------
   
    // Typedefs, consts, and enums
   
    //--------------------
    // Instance Members --
    //--------------------
   
public:
   
    // Constructors
    HTRange() :
      fDefined(kFALSE), fLower(0), fUpper(0)
      {
      }
     
      HTRange(T lowerv, T upperv) :
      fDefined(kTRUE), fLower(lowerv), fUpper(upperv)
      {
      }
     
      // Copy Constructor
      HTRange(const HTRange<T>& o) :
      fDefined(o.fDefined), fLower(o.fLower), fUpper(o.fUpper)
      {
      }
     
      // Destructor
      virtual ~HTRange() {}
     
      // Operators
     
      HTRange<T>& operator= (const HTRange<T> &o)
      {
          if (&o == this) return *this;
          fDefined = o.fDefined ;
          fLower = o.fLower ;
          fUpper = o.fUpper ;
          return *this ;
      }
     
      // Selectors (const)
      Bool_t operator()() const { return fDefined ; }
     
      // get upper/lower limits
      T lower() const { return fLower ; }
      T upper() const { return fUpper ; }
     
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


#ifndef HTAbsValVector_HH
#define HTAbsValVector_HH

template<class T>
class HTAbsValVector {
   
public:
   
    // This must return the number of the stored elements
    virtual size_t length() const = 0 ;
   
    // This provides access to the indifidual elements.
    // Index runs from 0 to length()-1. Here () means that we do not requre
    // index checking from it, though the real classe may do what they like.
    virtual const T& operator()(size_t i) const = 0 ;
   
};
#endif

// Parent class (abstract):
class TColumn : public TNamed {
public:
    TColumn(const char* l) :
      TNamed(l,l), fLabel(l), fUseDefValue(0), fPointer(0), fBranch(0) {}
      virtual ~TColumn() {}
      const TString &GetLabel() const { return fLabel; }
      TBranch* GetBrPointer() { return fBranch; }
      void* GetPointer() { return fPointer; }
      void SetPointer(void* p) { fPointer = p; }
      void SetUseDefValue(Int_t b) { fUseDefValue = b; }
      const Int_t & GetUseDefValue() const { return fUseDefValue; }
      virtual void SetDefValue() = 0;
      virtual void SetValue(const void*, TColumn* cp=0) = 0;
protected:
    TString fLabel;
    Int_t fUseDefValue;
    void* fPointer;
    TBranch* fBranch;
};

// Classes for Bool_t:
class BoolColumn : public TColumn {
public:
    BoolColumn(const char*, const Bool_t &, const Bool_t &, TTree*);
    virtual ~BoolColumn() { delete (Bool_t*)fPointer; }
    virtual void SetDefValue() { *(Char_t*)fPointer = fDefValue; }
    virtual void SetValue(const void* p, TColumn* cp=0) {
        *(Char_t*)fPointer = *(const Bool_t*)p;
    }
private:
    Bool_t fDefValue;
};

class BoolArrColumn : public TColumn {
public:
    BoolArrColumn(const char*, const HTAbsValVector<Bool_t> &, const Bool_t &,
        TTree*);
    virtual ~BoolArrColumn() { delete[] (Bool_t*)fPointer; }
    virtual void SetDefValue() {
        for(Int_t i = 0; i < fMax; ++i) ((Char_t*)fPointer)[i] = fDefValue;
    }
    virtual void SetValue(const void*, TColumn* cp=0);
private:
    Bool_t fDefValue;
    Int_t fMax;
};

class BoolDynArrColumn : public TColumn {
public:
    BoolDynArrColumn(const char*, const HTAbsValVector<Bool_t> &,
        const Bool_t &, TColumn*, TTree*);
    virtual ~BoolDynArrColumn() { delete[] (Bool_t*)fPointer; }
    virtual void SetDefValue();
    virtual void SetValue(const void*, TColumn* cp=0);
private:
    Bool_t fDefValue;
    TColumn* fIndexPtr;
};

// Classes for Int_t:
class IntColumn : public TColumn {
public:
    IntColumn(const char*, const Int_t &, const Int_t &, TTree*);
    virtual ~IntColumn() { delete (Int_t*)fPointer; }
    virtual void SetDefValue() { *(Int_t*)fPointer = fDefValue; }
    virtual void SetValue(const void* p, TColumn* cp=0) {
        *(Int_t*)fPointer = *(const Int_t*)p;
    }
private:
    Int_t fDefValue;
};

class IntArrColumn : public TColumn {
public:
    IntArrColumn(const char*, const HTAbsValVector<Int_t> &, const Int_t &,
        TTree*);
    virtual ~IntArrColumn() { delete[] (Int_t*)fPointer; }
    virtual void SetDefValue() {
        for(Int_t i = 0; i < fMax; ++i) ((Int_t*)fPointer)[i] = fDefValue;
    }
    virtual void SetValue(const void*, TColumn* cp=0);
private:
    Int_t fDefValue;
    Int_t fMax;
};

class IntDynArrColumn : public TColumn {
public:
    IntDynArrColumn(const char*, const HTAbsValVector<Int_t> &,
        const Int_t &, TColumn*, TTree*);
    virtual ~IntDynArrColumn() { delete[] (Int_t*)fPointer; }
    virtual void SetDefValue();
    virtual void SetValue(const void*, TColumn* cp=0);
private:
    Int_t fDefValue;
    TColumn* fIndexPtr;
};

// Classes for Float_t:
class FloatColumn : public TColumn {
public:
    FloatColumn(const char*, const Float_t &, const Float_t &, TTree*);
    virtual ~FloatColumn() { delete (Float_t*)fPointer; }
    virtual void SetDefValue() { *(Float_t*)fPointer = fDefValue; }
    virtual void SetValue(const void* p, TColumn* cp=0) {
        *(Float_t*)fPointer = *(const Float_t*)p;
    }
private:
    Float_t fDefValue;
};

class FloatArrColumn : public TColumn {
public:
    FloatArrColumn(const char*, const HTAbsValVector<Float_t> &, const Float_t &,
        TTree*);
    FloatArrColumn(const char*, const TVector &, const Float_t &,
        TTree*);
    virtual ~FloatArrColumn() { delete[] (Float_t*)fPointer; }
    virtual void SetDefValue() {
        for(Int_t i = 0; i < fMax; ++i) ((Float_t*)fPointer)[i] = fDefValue;
    }
    virtual void SetValue(const void*, TColumn* cp=0);
private:
    Float_t fDefValue;
    Int_t fMax;
};

class FloatDynArrColumn : public TColumn {
public:
    FloatDynArrColumn(const char*, const HTAbsValVector<Float_t> &,
        const Float_t &, TColumn*, TTree*);
    FloatDynArrColumn(const char*, const TVector &,
        const Float_t &, TColumn*, TTree*);
    virtual ~FloatDynArrColumn() { delete[] (Float_t*)fPointer; }
    virtual void SetDefValue();
    virtual void SetValue(const void*, TColumn* cp=0);
private:
    Float_t fDefValue;
    TColumn* fIndexPtr;
};

// Classes for Double_t:
class DoubleColumn : public TColumn {
public:
    DoubleColumn(const char*, const Double_t &, const Double_t &, TTree*);
    virtual ~DoubleColumn() { delete (Double_t*)fPointer; }
    virtual void SetDefValue() { *(Double_t*)fPointer = fDefValue; }
    virtual void SetValue(const void* p, TColumn* cp=0) {
        *(Double_t*)fPointer = *(const Double_t*)p;
    }
private:
    Double_t fDefValue;
};

class DoubleArrColumn : public TColumn {
public:
    DoubleArrColumn(const char*, const HTAbsValVector<Double_t> &,
        const Double_t &, TTree*);
    virtual ~DoubleArrColumn() { delete[] (Double_t*)fPointer; }
    virtual void SetDefValue() {
        for(Int_t i = 0; i < fMax; ++i) ((Double_t*)fPointer)[i] = fDefValue;
    }
    virtual void SetValue(const void*, TColumn* cp=0);
private:
    Double_t fDefValue;
    Int_t fMax;
};

class DoubleDynArrColumn : public TColumn {
public:
    DoubleDynArrColumn(const char*, const HTAbsValVector<Double_t> &,
                      const Double_t &, TColumn*, TTree*);
    virtual ~DoubleDynArrColumn() { delete[] (Double_t*)fPointer; }
    virtual void SetDefValue();
    virtual void SetValue(const void*, TColumn* cp=0);
private:
    Double_t fDefValue;
    TColumn* fIndexPtr;
};

// String column:
class StringColumn : public TColumn {
public:
    StringColumn(const TString &, const TString &,
        const TString &, TTree*);
    virtual ~StringColumn() { delete[] (TString*)fPointer; }
    virtual void SetDefValue();
    virtual void SetValue(const void*, TColumn* cp=0);
private:
    TString fDefValue;
};

#endif

