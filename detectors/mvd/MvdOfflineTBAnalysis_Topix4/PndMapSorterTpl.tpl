/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
// -------------------------------------------------------------------------
//  PndMapSorterTpl.cxx
//   Created on: Jul 30, 2010
//       Author: stockman
// -------------------------------------------------------------------------

#include "PndMapSorterTpl.h"

#include "FairTimeStamp.h"              // for FairTimeStamp


template <typename TData>
void PndMapSorterTpl<TData>::AddElement(TData digi, double timestamp)
{
	if (fVerbose > 0)
		std::cout << "PndMapSorterTpl::AddElement " << timestamp << std::endl;
	fMapBuffer.insert(std::pair<double,TData>(timestamp, digi));
}

template <typename TData>
void PndMapSorterTpl<TData>::WriteOutData(double time)
{
	if (time > fTimeOffset){
		if (fVerbose > 0)
			std::cout << "PndMapSorterTpl::WriteOutData for time " << time << std::endl;
	//	std::cout << "PrintMap before!" << std::endl;
	//	PrintMap();
		for (typename std::multimap<double, TData>::iterator itr = fMapBuffer.begin(); itr != fMapBuffer.lower_bound(time - fTimeOffset); itr++){
			fOutputData.push_back(itr->second);
			if (fVerbose > 1){
				std::cout << "TS: " << itr->first << " " << itr->second.GetTimeStamp() << " diff " <<  itr->first - fOldTS << std::endl;
			}
			if (itr->first - fOldTS < 0)
				std::cout << "*** Error *** PndMapSorterTpl::WriteOutData negative difference " << itr->first << " - " << fOldTS << std::endl;
			fOldTS = itr->first;
		}
		fMapBuffer.erase(fMapBuffer.begin(), fMapBuffer.lower_bound(time - fTimeOffset));
//		std::cout << "PrintMap after!" << std::endl;
//		PrintMap();
	}
}

template <typename TData>
void PndMapSorterTpl<TData>::WriteOutAll(){
	if (fVerbose > 0)
		std::cout << "PndMapSorterTpl::WriteOutAll called!" << std::endl;
	for (auto itr : fMapBuffer)
		fOutputData.push_back(itr.second);
	fMapBuffer.clear();
}
