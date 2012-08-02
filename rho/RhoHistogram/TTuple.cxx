//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TTuple		                                                //
//                                                                      //
// Nested class hierarchy to hold information about TTuple columns.	//
//									//
// Author List:								//
// Marcel Kunze, RUB,Mar. 99						//
// Apr.2001 (MK),Faster implementation based on THashList		//
// Copyright (C) 1999-2001,Ruhr-University Bochum.			//
//									//
//////////////////////////////////////////////////////////////////////////

#include "TTree.h"
#include "TBranch.h"
#include "TString.h"
#include "TFile.h"

#include "RhoHistogram/TTuple.h"

ClassImp(TTuple)

#include <iostream>
using namespace std;

TTuple::TTuple() 
{ 
    fMap   = new THashList(); 
}

// Constructor to create a tuple with name and title:
TTuple::TTuple(const char* name,const char* title) : 
TNamed(name,title)
{
    fMap  = new THashList(); 
    fTree = new TTree(name,title);
    fTree->SetDirectory(0);
}

// Destructor:
TTuple::~TTuple() 
{
    delete fTree;
    delete fMap;
}

// Column booking/filling. All these have the same name - Column(...)

// Specify the data for a Column. The string is to the key to
// the Column,so it must be unique.


// ===== = Bool type ======
// Make/fill Column with a single value
void TTuple::Column(const char* label,
			Bool_t value,
			Bool_t defval,
			const char* block) 
{   
    TColumn* colp = (TColumn*) fMap->FindObject(label);
    if(colp) {
	// Column exists,fill corresponding memory location with value:
	colp->SetValue(&value);
	colp->SetUseDefValue(0);
    }
    else {
	// Create a new Column:
	colp = new BoolColumn(label,value,defval,fTree);
	fMap->Add(colp);
    }
    
}

// Make/fill Column-array. Length is fixed at creation time.
void TTuple::Column(const char* label,
			const HTAbsValVector<Bool_t> &vector,
			Bool_t defval,
			const char* block)
{   
    TColumn* colp = (TColumn*) fMap->FindObject(label);
    if(colp) {
	// Column exists,fill corresponding memory location with value:
	colp->SetValue(&vector);
	colp->SetUseDefValue(kFALSE);
    }
    else {
	// Create a new Column:
	colp = new BoolArrColumn(label,vector,defval,fTree);
	fMap->Add(colp);
    }
    
}

// Make/fill Column-array. Length is variable and is taken from 
// another Column.
void TTuple::Column(const char* label,
			const HTAbsValVector<Bool_t> &vector,
			const char* ilab,
			Bool_t defval,
			const char* block) 
{ 
    TColumn* colp = (TColumn*) fMap->FindObject(label);
    if(colp) {
	// Column exists,fill corresponding memory location with value:
	colp->SetValue(&vector,(TColumn*)fMap->FindObject(ilab));
	colp->SetUseDefValue(kFALSE);
    }
    else {
	// Create a new branch:
	TColumn* indexPtr = (TColumn*) fMap->FindObject(ilab);
	colp = new BoolDynArrColumn(label,vector,defval,indexPtr,fTree);
	fMap->Add(colp);
    }
    
}


// ===== = Int type ======
// Make/fill Column with a single value
void TTuple::Column(const char* label,
			Int_t value,
			Int_t defval,
			const char* block,
			const HTRange<Int_t> &range)
{
    TColumn* colp = (TColumn*) fMap->FindObject(label);
    if(colp) {
	// Column exists,fill corresponding memory location with value:
	colp->SetValue(&value);
	colp->SetUseDefValue(0);
    }
    else {
	// Create a new Column:
	colp = new IntColumn(label,value,defval,fTree);
	fMap->Add(colp);
    }
    
}

// Make/fill Column-array. Length is fixed at creation time.
void TTuple::Column(const char* label,
			const HTAbsValVector<Int_t> &vector,
			Int_t defval,
			const char* block,
			const HTRange<Int_t> &range)
{
    TColumn* colp = (TColumn*) fMap->FindObject(label);
    if(colp) {
	// Column exists,fill corresponding memory location with value:
	colp->SetValue(&vector);
	colp->SetUseDefValue(kFALSE);
    }
    else {
	// Create a new Column:
	colp = new IntArrColumn(label,vector,defval,fTree);
	fMap->Add(colp);
    }
    
}

// Make/fill Column-array. Length is variable and is taken from 
// another Column.
void TTuple::Column(const char* label,
			const HTAbsValVector<Int_t> &vector,
			const char* ilab,
			Int_t defval,
			const char* block,
			const HTRange<Int_t> &range)
{
    TColumn* colp = (TColumn*) fMap->FindObject(label);
    if(colp) {
	// Column exists,fill corresponding memory location with value:
	colp->SetValue(&vector,(TColumn*)fMap->FindObject(ilab));
	colp->SetUseDefValue(kFALSE);
    }
    else {
	// Create a new branch:
	TColumn* indexPtr = (TColumn*) fMap->FindObject(ilab);
	colp = new IntDynArrColumn(label,vector,defval,indexPtr,fTree);
	fMap->Add(colp);
    }
    
}


// ===== = Float type ======
// Make/fill Column with a single value
void TTuple::Column(const char* label,
			Float_t value,
			Float_t defval,
			const char* block,
			const HTRange<Float_t> &range) 
{
    TColumn* colp = (TColumn*) fMap->FindObject(label);
    if(colp) {
	// Column exists,fill corresponding memory location with value:
	colp->SetValue(&value);
	colp->SetUseDefValue(0);
    }
    else {
	// Create a new Column:
	colp = new FloatColumn(label,value,defval,fTree);
	fMap->Add(colp);
    }
    
    
}

// Make/fill Column-array. Length is fixed at creation time.
void TTuple::Column(const char* label,
			const TVector &vector,
			Float_t defval,
			const char* block,
			const HTRange<Float_t> &range)
{
    TColumn* colp = (TColumn*) fMap->FindObject(label);
    if(colp) {
	// Column exists,fill corresponding memory location with value:
	colp->SetValue(&vector);
	colp->SetUseDefValue(0);
    }
    else {
	// Create a new Column:
	colp = new FloatArrColumn(label,vector,defval,fTree);
	fMap->Add(colp);
    }
    
    
}

// Make/fill Column-array. Length is variable and is taken from 
// another Column.
void TTuple::Column(const char* label,
			const TVector &vector,
			const char *ilab,
			Float_t defval,
			const char* block,
			const HTRange<Float_t> &range)
{   
    TColumn* colp = (TColumn*) fMap->FindObject(label);
    if(colp) {
	// Column exists,fill corresponding memory location with value:
	colp->SetValue(&vector,(TColumn*)fMap->FindObject(ilab));
	colp->SetUseDefValue(0);
    }
    else {
	// Create a new branch:
	TColumn* indexPtr = (TColumn*) fMap->FindObject(ilab);
	colp = new FloatDynArrColumn(label,vector,defval,indexPtr,fTree);
	fMap->Add(colp);
    }
    
}

// Make/fill Column-array. Length is fixed at creation time.
void TTuple::Column(const char* label,
			const HTAbsValVector<Float_t> &vector,
			Float_t defval,
			const char* block,
			const HTRange<Float_t> &range) 
{
    TColumn* colp = (TColumn*) fMap->FindObject(label);
    if(colp) {
	// Column exists,fill corresponding memory location with value:
	colp->SetValue(&vector);
	colp->SetUseDefValue(kFALSE);
    }
    else {
	// Create a new Column:
	colp = new FloatArrColumn(label,vector,defval,fTree);
	fMap->Add(colp);
    }
    
    
}

// Make/fill Column-array. Length is variable and is taken from 
// another Column.
void TTuple::Column(const char* label,
			const HTAbsValVector<Float_t> &vector,
			const char *ilab,
			Float_t defval,
			const char* block,
			const HTRange<Float_t> &range) 
{
    TColumn* colp = (TColumn*) fMap->FindObject(label);
    if(colp) {
	// Column exists,fill corresponding memory location with value:
	colp->SetValue(&vector,(TColumn*)fMap->FindObject(ilab));
	colp->SetUseDefValue(kFALSE);
    }
    else {
	// Create a new branch:
	TColumn* indexPtr = (TColumn*) fMap->FindObject(ilab);
	colp = new FloatDynArrColumn(label,vector,defval,indexPtr,fTree);
	fMap->Add(colp);
    }
    
}

// ===== = Double type ======
// Make/fill Column with a single value
void TTuple::Column(const char* label,
			Double_t value,
			Double_t defval,
			const char* block,
			const HTRange<Double_t> &range) 
{
    TColumn* colp = (TColumn*) fMap->FindObject(label);
    if(colp) {
	// Column exists,fill corresponding memory location with value:
	colp->SetValue(&value);
	colp->SetUseDefValue(0);
    }
    else {
	// Create a new Column:
	colp = new DoubleColumn(label,value,defval,fTree);
	fMap->Add(colp);
    }
    
    
}

// Make/fill Column-array. Length is fixed at creation time.
void TTuple::Column(const char* label,
			const HTAbsValVector<Double_t> &vector,
			Double_t defval,
			const char* block,
			const HTRange<Double_t> &range)
{ 
    TColumn* colp = (TColumn*) fMap->FindObject(label);
    if(colp) {
	// Column exists,fill corresponding memory location with value:
	colp->SetValue(&vector);
	colp->SetUseDefValue(kFALSE);
    }
    else {
	// Create a new Column:
	colp = new DoubleArrColumn(label,vector,defval,fTree);
	fMap->Add(colp);
    }
    
    
}

// Make/fill Column-array. Length is variable and is taken from 
// another Column.
void TTuple::Column(const char* label,
			const HTAbsValVector<Double_t> &vector,
			const char *ilab,
			Double_t defval,
			const char* block,
			const HTRange<Double_t> &range)
{
    TColumn* colp = (TColumn*) fMap->FindObject(label);
    if(colp) {
	// Column exists,fill corresponding memory location with value:
	colp->SetValue(&vector,(TColumn*)fMap->FindObject(ilab));
	colp->SetUseDefValue(kFALSE);
    }
    else {
	// Create a new branch:
	TColumn* indexPtr = (TColumn*) fMap->FindObject(ilab);
	colp = new DoubleDynArrColumn(label,vector,defval,indexPtr,fTree);
	fMap->Add(colp);
    }
    
}

// ===== = string Columns ======
// Can actually be variable length in ROOT,N is ignored:
void TTuple::Column(const char* label,
			const char* value,
			Int_t N,
			const char* defval,
			const char* block)
{
    TColumn* colp = (TColumn*) fMap->FindObject(label);
    if(colp) {
	// Branch exists,fill corresponding memory location with value:
	Column(label,value);
    }
    else {
	// Create a new branch:
	colp = new StringColumn(label,value,defval,fTree);
	fMap->Add(colp);
    }
    
}

void TTuple::Column(const char* label,const char* value)
{    
    TColumn* colp = (TColumn*) fMap->FindObject(label);
    if(colp) {
	colp->SetValue(value);
	colp->SetUseDefValue(0);
    }
    else {
	cerr << "TTuple::Column: Column "<< label << " does not exist" << endl;
    }
    
}


// Dump all the data into the ntuple and then clear
void TTuple::DumpData()
{    
    for (Int_t i=0;i<fMap->GetSize();i++)
    {
	TColumn *col = (TColumn*) fMap->At(i);
	if(col->GetUseDefValue()) col->SetDefValue();
	col->SetUseDefValue(1);
    }
    fTree->Fill(); 
    
}

// Set all the data to their default values:
void TTuple::ClearData() 
{   
    for (Int_t i=0;i<fMap->GetSize();i++)
    {
	TColumn *col = (TColumn*) fMap->At(i);
	col->SetDefValue();
	col->SetUseDefValue(1);
    }
    return; 
    
}

// Return title of ntuple:
const char* TTuple::Title() const 
{
    return fTree->GetTitle();
}

// number of Columns
Int_t TTuple::NColumns() const 
{
    return fMap->GetSize();    
}

// Return label for a particular Column with index i:
const char* TTuple::Label(Int_t i) const 
{
    TString str;
    if(i >= 0 && i < fMap->GetSize()) str = ((TColumn*)fMap->At(i))->GetLabel();
    else str = "unknown Column index";
    return str.Data();
    
}

// Print info about ntuple:
void TTuple::PrintOn(std::ostream & o)  const 
{  
    cout << "TTuple: ntuple " << Title() << " has " << NColumns() 
	<< " Columns." << endl;
    cout << "Complete printout follows: " << endl;
    fTree->Print(); 
}

void TTuple::WriteToFile(TString fname, TString opt)
{
	TFile *f=new TFile(fname,opt);
	fTree->Write();
	f->Close();
	delete f;
}

void TTuple::AddToFile(TString fname)
{
	TFile *f=new TFile(fname,"UPDATE");
	fTree->Write();
	f->Close();
	delete f;
}
