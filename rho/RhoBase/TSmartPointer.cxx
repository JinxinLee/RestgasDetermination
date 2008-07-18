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

#include "TTree.h"
#include "RhoBase/TSmartPointer.h"

ClassImp(TSmartPointer)

#include <iostream>
using namespace std;

Int_t  TSmartPointer::fEventNumber=0;

TSmartPointer::TSmartPointer(TBranch *c) : fBranch(c) 
{}

void TSmartPointer::SetEventNumber(Int_t n) 
{
    fEventNumber = n;
}

Int_t TSmartPointer::CheckBranch()
{
    if (!fBranch) { cerr << " TSmartPointer::CheckBranch: non existing branch" << endl; return -1; }
    if (!fBranch->GetTree()) return -1;
    
    if (fBranch->TestBit(kDoNotProcess)) {
	fBranch->ResetBit(kDoNotProcess);
	return fBranch->GetTree()->GetEntry(fEventNumber);
    }
    
    return -1;
}

Bool_t *TSmartPointer::GetBool()
{
    if (CheckBranch()<=0) return 0;
    return (Bool_t*) fBranch->GetAddress();
}

Char_t *TSmartPointer::GetChar()
{
    if (CheckBranch()<=0) return 0;
    return (Char_t*) fBranch->GetAddress();
}

Int_t *TSmartPointer::GetInt()
{
    if (CheckBranch()<=0) return 0;
    return (Int_t*) fBranch->GetAddress();
}

Short_t *TSmartPointer::GetShort()
{
    if (CheckBranch()<=0) return 0;
    return (Short_t*) fBranch->GetAddress();
}

Float_t *TSmartPointer::GetFloat()
{
    if (CheckBranch()<=0) return 0;
    return (Float_t*) fBranch->GetAddress();
}
