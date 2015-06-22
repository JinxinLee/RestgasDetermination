/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
// -------------------------------------------------------------------------
//  PndMapSorter.cxx
//   Created on: Jul 30, 2010
//       Author: stockman
// -------------------------------------------------------------------------

#include "PndMapSorter.h"

#include "FairTimeStamp.h"              // for FairTimeStamp

FairTimeStamp* PndMapSorter::CreateElement(FairTimeStamp* data)
{
	return (FairTimeStamp*)data->Clone();
}

void PndMapSorter::AddElement(FairTimeStamp* digi, double timestamp)
{
	fMapBuffer.insert(std::pair<double,FairTimeStamp*>(timestamp, CreateElement(digi)));
}

void PndMapSorter::WriteOutData(double time)
{
	for (std::multimap<double, FairTimeStamp*>::iterator itr = fMapBuffer.begin(); itr != fMapBuffer.lower_bound(time - fTimeOffset); itr++)
		fOutputData.push_back(itr->second);
	fMapBuffer.erase(fMapBuffer.begin(), fMapBuffer.lower_bound(time - fTimeOffset));
}

void PndMapSorter::WriteOutAll(){
	for (std::multimap<double, FairTimeStamp*>::iterator itr = fMapBuffer.begin(); itr != fMapBuffer.end(); itr++)
		fOutputData.push_back(itr->second);
}

ClassImp(PndMapSorter);
