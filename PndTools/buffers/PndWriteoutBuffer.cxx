/*
 * PndWriteoutBuffer.cpp
 *
 *  Created on: Jul 30, 2010
 *      Author: stockman
 */

#include "PndWriteoutBuffer.h"
#include "FairRootManager.h"
#include "PndSdsDigiPixel.h"
#include "PndSdsDigiStrip.h"
#include "TClonesArray.h"
#include <iostream>

 PndWriteoutBuffer::PndWriteoutBuffer(TString branchName, TString className):
	fBranchName(branchName), fClassName(className), fActivateBuffering(kTRUE), fVerbose(3)
{
	if (fBranchName == "" || fClassName == "")
		fTreeSave = false;
	else
		fTreeSave = true;
}

 void PndWriteoutBuffer::WriteOutData(double time)
{
	FairRootManager* ioman = FairRootManager::Instance();
	std::vector<FairTimeStamp*> data;
	if (fActivateBuffering){
		if (fVerbose > 0) std::cout << "-I- PndWriteoutBuffer::WriteOutData for time: " << time << std::endl;
		data = GetRemoveOldData(time);
		if (fTreeSave && data.size() > 0){
			TClonesArray* myArray = ioman->GetTClonesArray(fBranchName);
			if (!myArray)
				std::cout << "-E- PndWriteoutBuffer::WriteOutData " << fBranchName << " array is not available!" << std::endl;
			if (fVerbose > 0) std::cout << "-I- PndWriteoutBuffer::WriteOutData size: " << data.size() << std::endl;
			for (int i = 0; i < data.size(); i++){
				AddNewDataToTClonesArray(data[i]);
				if (fVerbose > 1)std::cout << i << " : " << data[i] << std::endl;
			}
			//ioman->GetTClonesArray(fBranchName);
		}
	}
	else{
		ioman->GetTClonesArray(fBranchName);
	}
//	return data;
}

 void PndWriteoutBuffer::WriteOutAllData()
{
	if (fDeadTime_map.size() > 0){
		WriteOutData(fDeadTime_map.rbegin()->first + 1);
	}
}

 std::vector<FairTimeStamp*> PndWriteoutBuffer::GetRemoveOldData(double time)
{
	typedef std::multimap<double, FairTimeStamp*>::iterator DTMapIter;
	std::vector<FairTimeStamp*> result;
	for(DTMapIter it = fDeadTime_map.begin(); it != fDeadTime_map.lower_bound(time); it++){
		if (fVerbose > 1) std::cout << "-I- GetRemoveOldData: DeadTime: " << it->first << " Data: " << it->second << std::endl;
		result.push_back(it->second);
		EraseDataFromDataMap(it->second);
	}
	 fDeadTime_map.erase(fDeadTime_map.begin(), fDeadTime_map.lower_bound(time));
	return result;
}

 std::vector<FairTimeStamp*> PndWriteoutBuffer::GetAllData()
{
	return GetRemoveOldData(fDeadTime_map.rbegin()->first + 1);
}

//  void PndWriteoutBuffer::FillNewData(FairTimeStamp* data, double activeTime)
// {
// 	if (fActivateBuffering){
// 		typedef  std::multimap<double, FairTimeStamp*>::iterator DTMapIter;
// 		typedef  std::map<FairTimeStamp*, double>::iterator DataMapIter;
// 
// 		DataMapIter datait = fData_map.find(data);
// 		if(datait != fData_map.end()){					//if an older active data object is already present
// 			if (fVerbose > 1) std::cout << " OldData found! " << std::endl;
// 			if (fVerbose > 1) std::cout << "New Data: " << activeTime << " : " << data << std::endl;
// 			double currentdeadtime = datait->second;
// 			FairTimeStamp* oldData;
// 			for (DTMapIter it = fDeadTime_map.lower_bound(currentdeadtime); it != fDeadTime_map.upper_bound(currentdeadtime); it++){
// 				oldData = it->second;
// 				if (fVerbose > 1) std::cout << "Check Data: " << it->first << " : " << oldData << std::endl;
// 				if (oldData == data){
// 					if (fVerbose > 1) std::cout << " oldData == data " << std::endl;
// 					if (fVerbose > 1) std::cout << it->first << " : " << it->second << std::endl;
// 					fDeadTime_map.erase(it);
// 					break;
// 				}
// 			}
// //			double newDeadTime = CalcNewActiveTime(currentdeadtime, data);
// //			fData_map[data] = newDeadTime;
// 			std::vector<std::pair<double, FairTimeStamp*> > modifiedData = Modify(std::pair<double, FairTimeStamp*>(currentdeadtime, oldData), std::pair<double, FairTimeStamp*>(-1, data));
// 			for (int i = 0; i < modifiedData.size(); i++){
// 				fDeadTime_map.insert(modifiedData[i]);
// 				if (fVerbose > 1) std::cout << i << " :Modified Data: " << modifiedData[i].first << " : " << modifiedData[i].second << std::endl;
// 			}
// 			//T newData = Modify(myData, data);
// 		}
// 		else{
// 			//std::cout << " Hat nicht geklappt! " << std::endl;
// 			if (fVerbose > 1) std::cout << " Data Inserted: " << activeTime << " : " << data << std::endl;
// 			fData_map.insert(std::pair<FairTimeStamp*, double>(data, activeTime));
// 			fDeadTime_map.insert(std::pair<double, FairTimeStamp*>(activeTime, data));
// 		}
// 	}
// 	else{
// 		FairRootManager* ioman = FairRootManager::Instance();
// 		TClonesArray* myArray = ioman->GetTClonesArray(fBranchName);
// 		if (fVerbose > 1) std::cout << "Data Inserted: " << activeTime << " : " << data << std::endl;
// 		AddNewDataToTClonesArray(data);
// 	}
// }


 void PndWriteoutBuffer::FillNewData(FairTimeStamp* data, double activeTime)
{
	if (fActivateBuffering){
		typedef  std::multimap<double, FairTimeStamp*>::iterator DTMapIter;
		typedef  std::map<FairTimeStamp, double>::iterator DataMapIter;

		//DataMapIter datait = fData_map.find(*data);
		double timeOfOldData = FindTimeForData(data);
		if(timeOfOldData > -1){					//if an older active data object is already present
			if (fVerbose > 1) std::cout << " OldData found! " << std::endl;
			if (fVerbose > 1) std::cout << "New Data: " << activeTime << " : " << data << std::endl;
			double currentdeadtime = timeOfOldData;
			FairTimeStamp* oldData;
			for (DTMapIter it = fDeadTime_map.lower_bound(currentdeadtime); it != fDeadTime_map.upper_bound(currentdeadtime); it++){
				oldData = it->second;
				if (fVerbose > 1) std::cout << "Check Data: " << it->first << " : " << oldData << std::endl;
				if (oldData->equal(data)){
					if (fVerbose > 1) std::cout << " oldData == data " << std::endl;
					if (fVerbose > 1) std::cout << it->first << " : " << it->second << std::endl;
					fDeadTime_map.erase(it);
					EraseDataFromDataMap(oldData);
					break;
				}
			}
//			double newDeadTime = CalcNewActiveTime(currentdeadtime, data);
//			fData_map[data] = newDeadTime;
			std::vector<std::pair<double, FairTimeStamp*> > modifiedData = Modify(std::pair<double, FairTimeStamp*>(currentdeadtime, oldData), std::pair<double, FairTimeStamp*>(-1, data));
			for (int i = 0; i < modifiedData.size(); i++){
				FillNewData(modifiedData[i].second, modifiedData[i].first);
				if (fVerbose > 1) std::cout << i << " :Modified Data: " << modifiedData[i].first << " : " << modifiedData[i].second << std::endl;
			}
			//T newData = Modify(myData, data);
		}
		else{
			//std::cout << " Hat nicht geklappt! " << std::endl;
			if (fVerbose > 1) std::cout << " Data Inserted: " << activeTime << " : " << data << std::endl;
			//fData_map.insert(std::pair<FairTimeStamp, double>(*data, activeTime));
			fDeadTime_map.insert(std::pair<double, FairTimeStamp*>(activeTime, data));
			FillDataMap(data, activeTime);
		}
	}
	else{
		/*FairRootManager* ioman = FairRootManager::Instance();
		TClonesArray* myArray = ioman->GetTClonesArray(fBranchName);
		if (fVerbose > 1) std::cout << "Data Inserted: " << activeTime << " : " << data << std::endl;
		*/
		AddNewDataToTClonesArray(data);
	}
}

//  void PndWriteoutBuffer::FillNewData(FairTimeStamp* data, double activeTime)
// {
// 	if (fActivateBuffering){
// 		typedef  std::multimap<double, FairTimeStamp*>::iterator DTMapIter;
// 		typedef  std::map<FairTimeStamp*, double>::iterator DataMapIter;
// 
// 		//DataMapIter datait = fData_map.find(data);
// 		FairTimeStamp* oldData;	
// 		DTMapIter it;
// 		double currentdeadtime = -1;
// 		for ( it = fDeadTime_map.begin(); it != fDeadTime_map.end(); it++){
// 		  oldData = it->second;
// 		  //if (fVerbose > 1) std::cout << "Check Data: " << it->first << " : " << oldData << std::endl;
// 		  if (fVerbose > 1) std::cout << "  Data: " << data  << " : " << oldData << std::endl;
// 		  if (oldData->equal(data)){
// 			if (fVerbose > 1) std::cout << " oldData == data " << std::endl;
// 			if (fVerbose > 1) std::cout << it->first << " : " << it->second << std::endl;
// 			currentdeadtime = it->first;
// 			fDeadTime_map.erase(it);
// 			break;
// 		  }
// 		}
// 		
// 		if(it != fDeadTime_map.end()){					//if an older active data object is already present
// 			
// 			std::vector<std::pair<double, FairTimeStamp*> > modifiedData = Modify(std::pair<double, FairTimeStamp*>(currentdeadtime, oldData), std::pair<double, FairTimeStamp*>(-1, data));
// 			for (int i = 0; i < modifiedData.size(); i++){
// 				fDeadTime_map.insert(modifiedData[i]);
// 				if (fVerbose > 1) std::cout << i << " :Modified Data: " << modifiedData[i].first << " : " << modifiedData[i].second << std::endl;
// 			}
// 			//T newData = Modify(myData, data);
// 		}
// 		else{
// 			//std::cout << " Hat nicht geklappt! " << std::endl;
// 			if (fVerbose > 1) std::cout << " Data Inserted: " << activeTime << " : " << data << std::endl;
// 			//fData_map.insert(std::pair<FairTimeStamp*, double>(data, activeTime));
// 			fDeadTime_map.insert(std::pair<double, FairTimeStamp*>(activeTime, data));
// 		}
// 	}
// 	else{
// 		FairRootManager* ioman = FairRootManager::Instance();
// 		TClonesArray* myArray = ioman->GetTClonesArray(fBranchName);
// 		if (fVerbose > 1) std::cout << "Data Inserted: " << activeTime << " : " << data << std::endl;
// 		AddNewDataToTClonesArray(data);
// 	}
// }


ClassImp(PndWriteoutBuffer);
