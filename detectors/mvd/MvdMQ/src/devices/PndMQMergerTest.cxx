/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
/**
 * PndMQMergerTest.cxx
 *
 * @since 2012-12-06
 * @author D. Klein, A. Rybalchenko
 */

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include "baseMQtools.h"

#include "PndMQMerger.h"
#include "PndMQMergerTest.h"

using namespace std;

PndMQMergerTest::PndMQMergerTest() : PndMQMerger()
{
}


void PndMQMergerTest::ProcessData(std::map<std::string, BurstData>& dataToProcess)
{
	for (auto itr : dataToProcess){
		LOG(INFO) << "Data in Burst: " << itr.first;
//		for (auto eventItr : itr.second.fData)
//			for (auto dataItr : eventItr)
//				LOG(INFO) << dataItr->GetTimeStamp();
	}
	LOG(INFO) << "Finished!";
}
