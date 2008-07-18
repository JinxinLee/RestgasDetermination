//////////////////////////////////////////////////////////////////////////
//                                                                      //
// VAbsReader								//
//                                                                      //
// Base class of the event reader classes				//
//                                                                      //
// Author: Marcel Kunze, RUB, May 2000					//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//									//
//////////////////////////////////////////////////////////////////////////

#include "RhoBase/VAbsReader.h"

ClassImp(VAbsReader)

#include <iostream>
using namespace std;

VAbsReader::VAbsReader(const char *file, const char * description) :
TNamed(file,description)
{
}

VAbsReader::~VAbsReader( ) 
{
}

