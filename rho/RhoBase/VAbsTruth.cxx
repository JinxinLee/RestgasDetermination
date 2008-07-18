//////////////////////////////////////////////////////////////////////////
//                                                                      //
// VAbsTruth								//
//                                                                      //
// Monte Carlo truth class						//
//									//
// Author List:								//
// Marcel Kunze, Bochum University, Aug. 99				//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//									//
//////////////////////////////////////////////////////////////////////////

#include "TDatabasePDG.h"
#include "RhoBase/TRho.h"
#include "RhoBase/VAbsTruth.h"

ClassImp(VAbsTruth)

using namespace std;

VAbsTruth::VAbsTruth()
{
}

VAbsTruth::~VAbsTruth() 
{
}

std::ostream&  operator << (std::ostream& o, const VAbsTruth& a) { a.PrintOn(o); return o; }
