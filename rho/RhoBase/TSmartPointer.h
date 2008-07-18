#ifndef TSMARTPOINTER_H
#define TSMARTPOINTER_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TSmartPointer							//
//                                                                      //
// Definition of smart pointers						//
// read information from branches on demand				//
//                                                                      //
// Author: Marcel Kunze, RUB, March 1999				//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//									//
//////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "TBranch.h"

class TSmartPointer : public TObject {
    
public :
    TSmartPointer(TBranch *c);   
    virtual ~TSmartPointer() {}
    static void SetEventNumber(Int_t n);
    void SetBranch(TBranch *b) { fBranch = b; }
    Int_t CheckBranch();
    Bool_t *GetBool();
    Char_t *GetChar();
    Short_t *GetShort();
    Int_t *GetInt();
    Float_t *GetFloat();
    
private :
    TBranch *fBranch;
    static Int_t fEventNumber; //!All branches work on the same event
    
public:    
    ClassDef(TSmartPointer,1) //T smart pointer
};

#endif
