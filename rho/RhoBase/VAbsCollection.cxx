//////////////////////////////////////////////////////////////////////////
//                                                                      //
// VAbsCollection	                                                //
//                                                                      //
// Definition of an abstract interface for event and run collections    //
//                                                                      //
// Author: TB (TUD)							//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "RhoBase/VAbsCollection.h"

ClassImp(VAbsCollection)

#include <iostream>
using namespace std;

Bool_t
VAbsCollection::InitRead()
{
    return kTRUE; 
}
