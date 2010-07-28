#ifndef TPARM_H
#define TPARM_H
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

//----------------------
// Base Class Headers --
//----------------------
#include <iostream>

#include "TObject.h"
#include "TNamed.h"
#include "THashList.h"

class TParmBool;
class TParmInt;
class TParmDouble;
class TParmString;

//		---------------------
// 		-- Class Interface --
//		---------------------

class TParm : public TNamed {
    
public:
    TParm() {}
    TParm( const char* const theCommand, TObject* theTarget);
    virtual ~TParm( );
    void AddDescription(const char * userString );
    static void Set(const char * p,Bool_t b,const char *t,TObject *x=0);
    static void Set(const char * p,Int_t i,const char *t,TObject *x=0);
    static void Set(const char * p,Double_t d,const char *t,TObject *x=0);
    static void Set(const char * p,const char *s,const char *t,TObject *x=0);
    // Set the value from a character string
    // return the number of args used, -1 if failed
    virtual int SetValue(int argc, char **arg) { return 0; }
    const char *KeywordName() const { return GetName(); }
    const char *ValueName() const { return "Not defined"; }
    const char *Description() const  { return GetTitle(); }
    static Bool_t GetBool(const char * p,TObject *x=0);
    static Int_t GetInt(const char * p,TObject *x=0);
    static Double_t GetDouble(const char * p,TObject *x=0);
    static TString GetString(const char * p,TObject *x=0);
    static Double_t GetParm(const char * p,TObject *x=0);
    static std::ostream& PrintAllParm(std::ostream& o=std::cout,TObject *x=0);

    TObject* Module() { return fTarget; }
protected:
    THashList& GetBoolList();
    THashList& GetIntList();
    THashList& GetDoubleList();
    THashList& GetStringList();
    TObject *fTarget; //! Do not stream
    static THashList fBoolList;
    static THashList fIntList;
    static THashList fDoubleList;
    static THashList fStringList;
private:
public:
    ClassDef(TParm,1) // Parameter definition
};

R__EXTERN THashList gGeneralList;

class TParmBool : public TParm {
    
public:
    TParmBool() {}
    TParmBool( const char* const theCommand, TObject* theTarget, Bool_t def = kFALSE );
    virtual ~TParmBool( ) {}
    virtual Bool_t Value() const     { return fParm; }
    const char*   Description() const { return "bool parameter: set or list"; }
    const char*   Type() const {return "Boolean";}
    void Set(const Bool_t v) { fParm = v; }
    virtual int SetValue(int argc, char **arg);
private:
    Bool_t fParm;
public:
    ClassDef(TParmBool,1) // Bool parameter definition
};


class TParmInt : public TParm {
    
public:
    TParmInt() {}
    TParmInt( const char* const theCommand, TObject* theTarget, Int_t def = 0 );
    virtual ~TParmInt( ) {}
    virtual Int_t Value() const     { return fParm; }
    const char*   Description() const { return "int parameter: set or list"; }
    const char*   Type() const {return "int";}
    void Set(const Int_t v) { fParm = v; }
    virtual int SetValue(int argc, char **arg);
private:
    Int_t fParm;
public:
    ClassDef(TParmInt,1) // Int parameter definition
};


class TParmDouble : public TParm {
    
public:
    TParmDouble() {}
    TParmDouble( const char* const theCommand, TObject* theTarget, Double_t def );
    virtual ~TParmDouble( ) {}
    virtual Double_t Value() const     { return fParm; }
    const char*   Description() const { return "double parameter: set or list"; }
    const char*   Type() const {return "Double";}
    void Set(const Double_t v) { fParm = v; }
    virtual int SetValue(int argc, char **arg);
private:
    Double_t fParm;
public:
    ClassDef(TParmDouble,1) // Double parameter definition
};


class TParmString : public TParm {
    
public:
    TParmString() {}
    TParmString(const char* theCommand, TObject* theTarget, const TString& def );
    virtual ~TParmString( ) {}
    virtual TString Value() const     { return fParm; }
    const char*   Description() const { return "string parameter: set or list"; }
    const char*   Type() const {return "String";}
    void Set(const TString v) { fParm = v; }
    virtual int SetValue(int argc, char **arg);
private:
    TString fParm;
public:
    ClassDef(TParmString,1) // String parameter definition
};


template <class DefType> class TParmGeneral : public TParm {
public:
    TParmGeneral() {}
    TParmGeneral( const char* const theCommand, TObject* theTarget, DefType def = 0 )
        : TParm( theCommand, theTarget ), fParm(def)
{
        SetName(theCommand);
        SetTitle("New parameter");
        gGeneralList.Add(this);
}
    virtual ~TParmGeneral( ) {}
    virtual DefType Value() const { return (DefType) fParm; }
    const char*   Description () const { return "general parameter: set or list"; }
    const char*   Type () const { return "General";}
    inline virtual int SetValue(int argc, char **arg);
protected:
private:
    DefType fParm;
};
/*
class TParmGeneral<Double_t> : public TParmDouble {
public:
    TParmGeneral<Double_t>() :  TParmDouble() {}
    TParmGeneral<Double_t>( const char* const theCommand, TObject* theTarget, Double_t def = 0 )
        : TParmDouble(theCommand,theTarget,def) {}
};

class TParmGeneral<Float_t> : public TParmDouble {
public:
    TParmGeneral<Float_t>() :  TParmDouble() {}
    TParmGeneral<Float_t>( const char* const theCommand, TObject* theTarget, Float_t def = 0 )
        : TParmDouble(theCommand,theTarget,def) {}
};

class TParmGeneral<Int_t> : public TParmInt {
public:
    TParmGeneral<Int_t>() :  TParmInt() {}
    TParmGeneral<Int_t>( const char* const theCommand, TObject* theTarget, Int_t def = 0 )
        : TParmInt(theCommand,theTarget,def) {}
};

class TParmGeneral<Long_t> : public TParmInt {
public:
    TParmGeneral<Long_t>() :  TParmInt() {}
    TParmGeneral<Long_t>( const char* const theCommand, TObject* theTarget, Int_t def = 0 )
        : TParmInt(theCommand,theTarget,def) {}
};

class TParmGeneral<Short_t> : public TParmInt {
public:
    TParmGeneral<Short_t>() :  TParmInt() {}
    TParmGeneral<Short_t>( const char* const theCommand, TObject* theTarget, Int_t def = 0 )
        : TParmInt(theCommand,theTarget,def) {}
};

class TParmGeneral<Bool_t> : public TParmBool {
public:
    TParmGeneral<Bool_t>() :  TParmBool() {}
    TParmGeneral<Bool_t>( const char* const theCommand, TObject* theTarget, Bool_t def = kFALSE )
        : TParmBool(theCommand,theTarget,def) {}
};

class TParmGeneral<char> : public TParmBool {
public:
    TParmGeneral<char>() :  TParmBool() {}
    TParmGeneral<char>( const char* const theCommand, TObject* theTarget, char def = kFALSE )
        : TParmBool(theCommand,theTarget,def) {}
};

class TParmGeneral<TString> : public TParmString {
public:
    TParmGeneral<TString>() :  TParmString() {}
    TParmGeneral<TString>( const char* const theCommand, TObject* theTarget, TString def = TString("") )
        : TParmString(theCommand,theTarget,def) {}
};
*/
#endif


