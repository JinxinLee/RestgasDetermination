//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TParm								//
//                                                                      //
// Handling of parameters						//
//                                                                      //
// Author: Marcel Kunze, RUB, 1999-2000					//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "RhoBase/TParm.h"
#include "TString.h"

ClassImp(TParm)
ClassImp(TParmBool)
ClassImp(TParmInt)
ClassImp(TParmDouble)
ClassImp(TParmString)

#include <iostream>
#include <iomanip>
using namespace std;

TParm::TParm(const char* const theCommand, TObject* theTarget):
TNamed(theCommand,"None"),
fTarget(theTarget)
{
}

TParm::~TParm( )
{
}


void
TParm::AddDescription( const char * userString )
{
  SetTitle(userString);
}


void
TParm::Set(const char *p,Bool_t b,const char *t,TObject *x)
{
  TString key(p);
  
  if (x == 0) {
    static TNamed *y = new TNamed("Framework","Framework");
    x = y;
  }
  
  if (key.Index("::")<0) key = TString(x->GetName()) + "::" + key;
  
  TParmBool* boolParm = (TParmBool*) fBoolList.FindObject(key);
  
  if (boolParm == 0) {
    boolParm = new TParmBool(p,x,b);
  }
  else {
    boolParm->Set(b);
  }
  
  if (t!=0) boolParm->AddDescription(t);
}


void
TParm::Set(const char *p,Int_t i,const char *t,TObject *x)
{
  TString key(p);
  
  if (x == 0) {
    static TNamed *y = new TNamed("Framework","Framework");
    x = y;
  }
  
  if (key.Index("::")<0) key = TString(x->GetName()) + "::" + key;
  
  TParmInt* intParm = (TParmInt*) fIntList.FindObject(key);
  
  if (intParm == 0) {
    intParm = new TParmInt(p,x,i);
  }
  else {
    intParm->Set(i);
  }
  
  if (t!=0) intParm->AddDescription(t);
}


void
TParm::Set(const char *p,Double_t d,const char *t,TObject *x)
{
  TString key(p);
  
  if (x == 0) {
    static TNamed *y = new TNamed("Framework","Framework");
    x = y;
  }
  
  if (key.Index("::")<0) key = TString(x->GetName()) + "::" + key;
  
  TParmDouble* doubleParm = (TParmDouble*) fDoubleList.FindObject(key);
  
  if (doubleParm == 0) {
    doubleParm = new TParmDouble(p,x,d);
  }
  else {
    doubleParm->Set(d);
  }
  
  if (t!=0) doubleParm->AddDescription(t);
}


void
TParm::Set(const char *p,const char *s,const char *t,TObject *x)
{
  TString key(p);
  
  if (x == 0) {
    static TNamed *y = new TNamed("Framework","Framework");
    x = y;
  }
  
  if (key.Index("::")<0) key = TString(x->GetName()) + "::" + key;
  
  TParmString* stringParm = (TParmString*) fStringList.FindObject(key);
  
  if (stringParm == 0) {
    stringParm = new TParmString(p,x,s);
  }
  else {
    stringParm->Set(s);
  }
  
  if (t!=0) stringParm->AddDescription(t);
}


Bool_t
TParm::GetBool(const char *p,TObject *x)
{
  TString key(p);
  
  if (x == 0) {
    static TNamed *y = new TNamed("Framework","Framework");
    x = y;
  }
  
  if (key.Index("::")<0) key = TString(x->GetName()) + "::" + key;
  
  TParmBool* boolParm = (TParmBool*) fBoolList.FindObject(key);
  
  if (boolParm == 0) {
    return kFALSE;
  }
  else {
    return boolParm->Value();
  }
}


Int_t
TParm::GetInt(const char *p,TObject *x)
{
  TString key(p);
  
  if (x == 0) {
    static TNamed *y = new TNamed("Framework","Framework");
    x = y;
  }
  
  if (key.Index("::")<0) key = TString(x->GetName()) + "::" + key;
  
  TParmInt* intParm = (TParmInt*) fIntList.FindObject(key);
  
  if (intParm == 0) {
    return 0;
  }
  else {
    return intParm->Value();
  }
}


Double_t
TParm::GetDouble(const char *p,TObject *x)
{
  TString key(p);
  
  if (x == 0) {
    static TNamed *y = new TNamed("Framework","Framework");
    x = y;
  }
  
  if (key.Index("::")<0) key = TString(x->GetName()) + "::" + key;
  
  TParmDouble* doubleParm = (TParmDouble*) fDoubleList.FindObject(key);
  
  if (doubleParm == 0) {
    return 0.0;
  }
  else {
    return doubleParm->Value();
  }
}


Double_t
TParm::GetParm(const char *p,TObject *x)
{
  TString key(p);
  
  if (x == 0) {
    static TNamed *y = new TNamed("Framework","Framework");
    x = y;
  }
  
  if (key.Index("::")<0) key = TString(x->GetName()) + "::" + key;
  
  TParmDouble* doubleParm = (TParmDouble*) fDoubleList.FindObject(key);
  
  if (doubleParm != 0) {
    return doubleParm->Value();
  }
  
  TParmInt* intParm = (TParmInt*) fIntList.FindObject(key);
  
  if (intParm != 0) {
    return intParm->Value();
  }
  
  TParmBool* boolParm = (TParmBool*) fBoolList.FindObject(key);
  
  if (boolParm != 0) {
    return boolParm->Value();
  }
  
  return 0.0;
}


TString
TParm::GetString(const char *p,TObject *x)
{
  static TString nullString("none");
  
  TString key(p);
  
  if (x == 0) {
    static TNamed *y = new TNamed("Framework","Framework");
    x = y;
  }
  
  if (key.Index("::")<0) key = TString(x->GetName()) + "::" + key;
  
  TParmString* stringParm = (TParmString*) fStringList.FindObject(key);
  
  if (stringParm == 0) {
    return nullString;
  }
  else {
    return stringParm->Value();
  }
  
}


THashList TParm::fBoolList;
THashList TParm::fIntList;
THashList TParm::fDoubleList;
THashList TParm::fStringList;
//DllExport THashList gGeneralList;

//		-------------------------------------------
// 		-- Friend Function Definitions --
//		-------------------------------------------
std::ostream& TParm::PrintAllParm(std::ostream& o, TObject* theTarget)
{
  o << endl;
  if (theTarget == 0)
    o << "Parameters which can be modified (on the command line):" << endl;
  else
    o << "Parameters for module " << theTarget->GetName() << endl;
  o.setf(ios::right);
  
  Int_t n;
  TObject *theModule=0;
  
  o << endl;
  o << "Boolean" << endl;
  o << "-------" << endl;
  TParmBool* boolParm;
  for(n=0;n<fBoolList.GetSize();n++) {
    boolParm = (TParmBool*) fBoolList.At(n);
    TString val = "false";
    if (boolParm->Value()) val = "true";
    
    theModule = boolParm->Module();
    TString name = boolParm->GetName();
    o << setw(30) << name.Data() << " " << setw(25) << val.Data() << "\t(" << boolParm->GetTitle() << ")" << endl;
  }
  
  o << endl;
  o << "Integer" << endl;
  o << "-------" << endl;
  TParmInt* intParm;
  for(n=0;n<fIntList.GetSize();n++) {
    intParm = (TParmInt*) fIntList.At(n);
    
    theModule = intParm->Module();
    TString name = intParm->GetName();
    o << setw(30) << name.Data() << " " << setw(25) << intParm->Value() << "\t(" << intParm->GetTitle() << ")" << endl;
    
  }
  
  o << endl;
  o << "Double" << endl;
  o << "------" << endl;
  TParmDouble* doubleParm;
  for(n=0;n<fDoubleList.GetSize();n++) {
    doubleParm = (TParmDouble*) fDoubleList.At(n);
    
    theModule = doubleParm->Module();
    TString name = doubleParm->GetName();
    o << setw(30) << name.Data() << " " << setw(25) << doubleParm->Value() << "\t(" << doubleParm->GetTitle() << ")" << endl;
    
  }
  
  o << endl;
  o << "String" << endl;
  o << "------" << endl;
  TParmString* stringParm;
  for(n=0;n<fStringList.GetSize();n++) {
    stringParm = (TParmString*) fStringList.At(n);
    
    theModule = stringParm->Module();
    TString name = stringParm->GetName();
    o << setw(30) << name.Data() << " " << setw(25) << (stringParm->Value()).Data() << "\t(" << stringParm->GetTitle() << ")" << endl;
    
  }
  
  //   o << endl;
  //   o << "General" << endl;
  //   o << "-------" << endl;
  //   for(n=0;n<gGeneralList.GetSize();n++) {
	//TObject *generalParm = gGeneralList.At(n);
	//TString name = generalParm->GetName();
	//TString desc = generalParm->GetTitle();
	//o << setw(30) << name.Data() << " " << setw(25) << "\t(" << desc.Data() << ")" << endl;
  
  //   }
  
  o << endl << endl;
  
  return o;
}

// Specializations

TParmBool::TParmBool( const char* const theCommand, TObject* theTarget, Bool_t def )
: TParm( TString(theTarget->GetName())+"::"+theCommand, theTarget ), fParm(def) 
{ 
  fBoolList.Add(this);
}

int TParmBool::SetValue(int argc, char **arg) {
  if (argc < 0) return -1;
  TString val(arg[argc]);
  if (val=="no" || val=="false" || val=="0")
    fParm = kFALSE;
  else
    fParm = kTRUE;
  return 1;
}

TParmInt::TParmInt( const char* const theCommand, TObject* theTarget, Int_t def )
: TParm( TString(theTarget->GetName())+"::"+theCommand, theTarget ), fParm(def)
{
  fIntList.Add(this);
}

int TParmInt::SetValue(int argc, char **arg) {
  if (argc < 0) return -1;
  if (sscanf(arg[argc],"%d", &fParm) != 1) return -1;
  return 1;
}

TParmDouble::TParmDouble( const char* const theCommand, TObject* theTarget, Double_t def )
: TParm( TString(theTarget->GetName())+"::"+theCommand, theTarget ), fParm(def)
{ 
  fDoubleList.Add(this);
}

int TParmDouble::SetValue(int argc, char **arg) {
  if (argc < 0) return -1;
  if (sscanf(arg[argc],"%lf",&fParm) != 1) return -1;
  return 1;
}

TParmString::TParmString( const char* theCommand, TObject* theTarget,const TString& def )
: TParm( TString(theTarget->GetName())+"::"+theCommand, theTarget ), fParm(def)
{ 
  fStringList.Add(this);
}

int TParmString::SetValue(int argc, char **arg) {
  if (argc < 0) return -1;
  fParm = TString(arg[argc]);
  return 1;
}

// Accessors

THashList& TParm::GetBoolList() { return fBoolList; }
THashList& TParm::GetIntList() { return fIntList; }
THashList& TParm::GetDoubleList() { return fDoubleList; }
THashList& TParm::GetStringList() { return fStringList; }
