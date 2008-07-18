//////////////////////////////////////////////////////////////////////////
//                                                                      //
// VAbsWriter								//
//                                                                      //
// Abstract Writer interface						//
//                                                                      //
// Author: Marcel Kunze, RUB, May 2001					//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//									//
//////////////////////////////////////////////////////////////////////////

#include "RhoBase/VAbsWriter.h"

ClassImp(VAbsWriter)

#include <iostream>
using namespace std;

VAbsWriter::VAbsWriter() : 
TNamed("VAbsWriter","VAbsWriter"), fCollection(0)
{
}

VAbsWriter::VAbsWriter(const char *file)
: TNamed(file,file), fCollection(0)
{
} 

VAbsWriter::VAbsWriter(VAbsCollection *collection)
: TNamed("Collection","Event Collection"), fCollection(collection)
{
} 

VAbsWriter::~VAbsWriter() 
{  
}
