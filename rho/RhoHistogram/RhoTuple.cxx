//////////////////////////////////////////////////////////////////////////
//                                                                      //
// RhoTuple                                                             //
//                                                                      //
// Nested class hierarchy to hold information about RhoTuple columns.   //
//                                                                      //
// Author List:                                                         //
// Marcel Kunze, RUB,Mar. 99                                            //
// Apr.2001 (MK),Faster implementation based on THashList               //
// Copyright (C) 1999-2001,Ruhr-University Bochum.                      //
// Ralf Kliemt, HIM/GSI Feb.2013 (Cleanup & Restructuring)              //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TTree.h"
#include "TBranch.h"
#include "TString.h"
#include "TFile.h"

#include "RhoHistogram/RhoTuple.h"

ClassImp ( RhoTuple )

#include <iostream>
using namespace std;

RhoTuple::RhoTuple()
{
  fMap   = new THashList();
}

// Constructor to create a tuple with name and title:
RhoTuple::RhoTuple ( const char* name,const char* title ) :
  TNamed ( name,title )
{
  fMap  = new THashList();
  fTree = new TTree ( name,title );
  fTree->SetDirectory ( 0 );
}

// Destructor:
RhoTuple::~RhoTuple()
{
  delete fTree;
  delete fMap;
}

// Column booking/filling. All these have the same name - Column(...)

// Specify the data for a Column. The string is to the key to
// the Column,so it must be unique.


// ===== = Bool type ======
// Make/fill Column with a single value
void RhoTuple::Column ( const char* label,
                        Bool_t value,
                        Bool_t defval,
                        const char*  ) //  block//[R.K.03/2017] unused variable(s)
{
  RhoColumn* colp = ( RhoColumn* ) fMap->FindObject ( label );
  if ( colp ) {
    // Column exists,fill corresponding memory location with value:
    colp->SetValue ( &value );
    colp->SetUseDefValue ( 0 );
  } else {
    // Create a new Column:
    colp = new RhoBoolColumn ( label,value,defval,fTree );
    fMap->Add ( colp );
  }

}

// Make/fill Column-array. Length is fixed at creation time.
void RhoTuple::Column ( const char* label,
                        const RhoHTAbsValVector<Bool_t> &vector,
                        Bool_t defval,
                        const char*  ) // block //[R.K.03/2017] unused variable(s)
{
  RhoColumn* colp = ( RhoColumn* ) fMap->FindObject ( label );
  if ( colp ) {
    // Column exists,fill corresponding memory location with value:
    colp->SetValue ( &vector );
    colp->SetUseDefValue ( kFALSE );
  } else {
    // Create a new Column:
    colp = new RhoBoolArrColumn ( label,vector,defval,fTree );
    fMap->Add ( colp );
  }

}

// Make/fill Column-array. Length is variable and is taken from
// another Column.
void RhoTuple::Column ( const char* label,
                        const RhoHTAbsValVector<Bool_t> &vector,
                        const char* ilab,
                        Bool_t defval,
                        const char*  ) // block //[R.K.03/2017] unused variable(s)
{
  RhoColumn* colp = ( RhoColumn* ) fMap->FindObject ( label );
  if ( colp ) {
    // Column exists,fill corresponding memory location with value:
    colp->SetValue ( &vector, ( RhoColumn* ) fMap->FindObject ( ilab ) );
    colp->SetUseDefValue ( kFALSE );
  } else {
    // Create a new branch:
    RhoColumn* indexPtr = ( RhoColumn* ) fMap->FindObject ( ilab );
    colp = new RhoBoolDynArrColumn ( label,vector,defval,indexPtr,fTree );
    fMap->Add ( colp );
  }

}


// ===== = Int type ======
// Make/fill Column with a single value
void RhoTuple::Column ( const char* label,
                        Int_t value,
                        Int_t defval,
                        const char* , // block //[R.K.03/2017] unused variable(s)
                        const RhoHTRange<Int_t> & ) // range //[R.K.03/2017] unused variable(s)
{
  RhoColumn* colp = ( RhoColumn* ) fMap->FindObject ( label );
  if ( colp ) {
    // Column exists,fill corresponding memory location with value:
    colp->SetValue ( &value );
    colp->SetUseDefValue ( 0 );
  } else {
    // Create a new Column:
    colp = new RhoIntColumn ( label,value,defval,fTree );
    fMap->Add ( colp );
  }

}

// Make/fill Column-array. Length is fixed at creation time.
void RhoTuple::Column ( const char* label,
                        const RhoHTAbsValVector<Int_t> &vector,
                        Int_t defval,
                        const char* , // block //[R.K.03/2017] unused variable(s)
                        const RhoHTRange<Int_t> & ) // range //[R.K.03/2017] unused variable(s)
{
  RhoColumn* colp = ( RhoColumn* ) fMap->FindObject ( label );
  if ( colp ) {
    // Column exists,fill corresponding memory location with value:
    colp->SetValue ( &vector );
    colp->SetUseDefValue ( kFALSE );
  } else {
    // Create a new Column:
    colp = new RhoIntArrColumn ( label,vector,defval,fTree );
    fMap->Add ( colp );
  }

}

// Make/fill Column-array. Length is variable and is taken from
// another Column.
void RhoTuple::Column ( const char* label,
                        const RhoHTAbsValVector<Int_t> &vector,
                        const char* ilab,
                        Int_t defval,
                        const char* , // block //[R.K.03/2017] unused variable(s)
                        const RhoHTRange<Int_t> & ) // range //[R.K.03/2017] unused variable(s)
{
  RhoColumn* colp = ( RhoColumn* ) fMap->FindObject ( label );
  if ( colp ) {
    // Column exists,fill corresponding memory location with value:
    colp->SetValue ( &vector, ( RhoColumn* ) fMap->FindObject ( ilab ) );
    colp->SetUseDefValue ( kFALSE );
  } else {
    // Create a new branch:
    RhoColumn* indexPtr = ( RhoColumn* ) fMap->FindObject ( ilab );
    colp = new RhoIntDynArrColumn ( label,vector,defval,indexPtr,fTree );
    fMap->Add ( colp );
  }

}


// ===== = Float type ======
// Make/fill Column with a single value
void RhoTuple::Column ( const char* label,
                        Float_t value,
                        Float_t defval,
                        const char* , // block //[R.K.03/2017] unused variable(s)
                        const RhoHTRange<Float_t> & ) //  range//[R.K.03/2017] unused variable(s)
{
  RhoColumn* colp = ( RhoColumn* ) fMap->FindObject ( label );
  if ( colp ) {
    // Column exists,fill corresponding memory location with value:
    colp->SetValue ( &value );
    colp->SetUseDefValue ( 0 );
  } else {
    // Create a new Column:
    colp = new RhoFloatColumn ( label,value,defval,fTree );
    fMap->Add ( colp );
  }


}

// Make/fill Column-array. Length is fixed at creation time.
void RhoTuple::Column ( const char* label,
                        const TVector& vector,
                        Float_t defval,
                        const char* , // block //[R.K.03/2017] unused variable(s)
                        const RhoHTRange<Float_t> & ) // range //[R.K.03/2017] unused variable(s)
{
  RhoColumn* colp = ( RhoColumn* ) fMap->FindObject ( label );
  if ( colp ) {
    // Column exists,fill corresponding memory location with value:
    colp->SetValue ( &vector );
    colp->SetUseDefValue ( 0 );
  } else {
    // Create a new Column:
    colp = new RhoFloatArrColumn ( label,vector,defval,fTree );
    fMap->Add ( colp );
  }


}

// Make/fill Column-array. Length is variable and is taken from
// another Column.
void RhoTuple::Column ( const char* label,
                        const TVector& vector,
                        const char* ilab,
                        Float_t defval,
                        const char* , // block //[R.K.03/2017] unused variable(s)
                        const RhoHTRange<Float_t> & ) // range //[R.K.03/2017] unused variable(s)
{
  RhoColumn* colp = ( RhoColumn* ) fMap->FindObject ( label );
  if ( colp ) {
    // Column exists,fill corresponding memory location with value:
    colp->SetValue ( &vector, ( RhoColumn* ) fMap->FindObject ( ilab ) );
    colp->SetUseDefValue ( 0 );
  } else {
    // Create a new branch:
    RhoColumn* indexPtr = ( RhoColumn* ) fMap->FindObject ( ilab );
    colp = new RhoFloatDynArrColumn ( label,vector,defval,indexPtr,fTree );
    fMap->Add ( colp );
  }

}

// Make/fill Column-array. Length is fixed at creation time.
void RhoTuple::Column ( const char* label,
                        const RhoHTAbsValVector<Float_t> &vector,
                        Float_t defval,
                        const char* , // block //[R.K.03/2017] unused variable(s)
                        const RhoHTRange<Float_t> & ) // range //[R.K.03/2017] unused variable(s)
{
  RhoColumn* colp = ( RhoColumn* ) fMap->FindObject ( label );
  if ( colp ) {
    // Column exists,fill corresponding memory location with value:
    colp->SetValue ( &vector );
    colp->SetUseDefValue ( kFALSE );
  } else {
    // Create a new Column:
    colp = new RhoFloatArrColumn ( label,vector,defval,fTree );
    fMap->Add ( colp );
  }


}

// Make/fill Column-array. Length is variable and is taken from
// another Column.
void RhoTuple::Column ( const char* label,
                        const RhoHTAbsValVector<Float_t> &vector,
                        const char* ilab,
                        Float_t defval,
                        const char* , // block //[R.K.03/2017] unused variable(s)
                        const RhoHTRange<Float_t> & ) // range //[R.K.03/2017] unused variable(s)
{
  RhoColumn* colp = ( RhoColumn* ) fMap->FindObject ( label );
  if ( colp ) {
    // Column exists,fill corresponding memory location with value:
    colp->SetValue ( &vector, ( RhoColumn* ) fMap->FindObject ( ilab ) );
    colp->SetUseDefValue ( kFALSE );
  } else {
    // Create a new branch:
    RhoColumn* indexPtr = ( RhoColumn* ) fMap->FindObject ( ilab );
    colp = new RhoFloatDynArrColumn ( label,vector,defval,indexPtr,fTree );
    fMap->Add ( colp );
  }

}

// ===== = Double type ======
// Make/fill Column with a single value
void RhoTuple::Column ( const char* label,
                        Double_t value,
                        Double_t defval,
                        const char* , // block //[R.K.03/2017] unused variable(s)
                        const RhoHTRange<Double_t> & ) // range //[R.K.03/2017] unused variable(s)
{
  RhoColumn* colp = ( RhoColumn* ) fMap->FindObject ( label );
  if ( colp ) {
    // Column exists,fill corresponding memory location with value:
    colp->SetValue ( &value );
    colp->SetUseDefValue ( 0 );
  } else {
    // Create a new Column:
    colp = new RhoDoubleColumn ( label,value,defval,fTree );
    fMap->Add ( colp );
  }


}

// Make/fill Column-array. Length is fixed at creation time.
void RhoTuple::Column ( const char* label,
                        const RhoHTAbsValVector<Double_t> &vector,
                        Double_t defval,
                        const char* , // block //[R.K.03/2017] unused variable(s)
                        const RhoHTRange<Double_t> & ) // range //[R.K.03/2017] unused variable(s)
{
  RhoColumn* colp = ( RhoColumn* ) fMap->FindObject ( label );
  if ( colp ) {
    // Column exists,fill corresponding memory location with value:
    colp->SetValue ( &vector );
    colp->SetUseDefValue ( kFALSE );
  } else {
    // Create a new Column:
    colp = new RhoDoubleArrColumn ( label,vector,defval,fTree );
    fMap->Add ( colp );
  }


}

// Make/fill Column-array. Length is variable and is taken from
// another Column.
void RhoTuple::Column ( const char* label,
                        const RhoHTAbsValVector<Double_t> &vector,
                        const char* ilab,
                        Double_t defval,
                        const char* , // block //[R.K.03/2017] unused variable(s)
                        const RhoHTRange<Double_t> & ) // range //[R.K.03/2017] unused variable(s)
{
  RhoColumn* colp = ( RhoColumn* ) fMap->FindObject ( label );
  if ( colp ) {
    // Column exists,fill corresponding memory location with value:
    colp->SetValue ( &vector, ( RhoColumn* ) fMap->FindObject ( ilab ) );
    colp->SetUseDefValue ( kFALSE );
  } else {
    // Create a new branch:
    RhoColumn* indexPtr = ( RhoColumn* ) fMap->FindObject ( ilab );
    colp = new RhoDoubleDynArrColumn ( label,vector,defval,indexPtr,fTree );
    fMap->Add ( colp );
  }

}

// ===== = string Columns ======
// Can actually be variable length in ROOT,N is ignored:
void RhoTuple::Column ( const char* label,
                        const char* value,
                        Int_t , // N //[R.K.03/2017] unused variable(s)
                        const char* defval,
                        const char*  ) // block //[R.K.03/2017] unused variable(s)
{
  RhoColumn* colp = ( RhoColumn* ) fMap->FindObject ( label );
  if ( colp ) {
    // Branch exists,fill corresponding memory location with value:
    Column ( label,value );
  } else {
    // Create a new branch:
    colp = new RhoStringColumn ( label,value,defval,fTree );
    fMap->Add ( colp );
  }

}

void RhoTuple::Column ( const char* label,const char* value )
{
  RhoColumn* colp = ( RhoColumn* ) fMap->FindObject ( label );
  if ( colp ) {
    colp->SetValue ( value );
    colp->SetUseDefValue ( 0 );
  } else {
    cerr << "TTuple::Column: Column "<< label << " does not exist" << endl;
  }

}


// Dump all the data into the ntuple and then clear
void RhoTuple::DumpData()
{
  for ( Int_t i=0; i<fMap->GetSize(); i++ ) {
    RhoColumn* col = ( RhoColumn* ) fMap->At ( i );
    if ( col->GetUseDefValue() ) { col->SetDefValue(); }
    col->SetUseDefValue ( 1 );
  }
  fTree->Fill();

}

// Set all the data to their default values:
void RhoTuple::ClearData()
{
  for ( Int_t i=0; i<fMap->GetSize(); i++ ) {
    RhoColumn* col = ( RhoColumn* ) fMap->At ( i );
    col->SetDefValue();
    col->SetUseDefValue ( 1 );
  }
  return;

}

// Return title of ntuple:
const char* RhoTuple::Title() const
{
  return fTree->GetTitle();
}

// number of Columns
Int_t RhoTuple::NColumns() const
{
  return fMap->GetSize();
}

// Return label for a particular Column with index i:
const char* RhoTuple::Label ( Int_t i ) const
{
  TString str;
  if ( i >= 0 && i < fMap->GetSize() ) { str = ( ( RhoColumn* ) fMap->At ( i ) )->GetLabel(); }
  else { str = "unknown Column index"; }
  return str.Data();

}

// Print info about ntuple:
void RhoTuple::PrintOn ( std::ostream& )  const
{
  cout << "TTuple: ntuple " << Title() << " has " << NColumns()
       << " Columns." << endl;
  cout << "Complete printout follows: " << endl;
  fTree->Print();
}

void RhoTuple::WriteToFile ( TString fname, TString opt )
{
  TFile* f=new TFile ( fname,opt );
  fTree->Write();
  f->Close();
  delete f;
}

void RhoTuple::AddToFile ( TString fname )
{
  TFile* f=new TFile ( fname,"UPDATE" );
  fTree->Write();
  f->Close();
  delete f;
}
