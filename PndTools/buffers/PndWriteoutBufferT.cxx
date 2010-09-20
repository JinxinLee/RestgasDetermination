/*
 * PndWriteoutBufferT.cpp
 *
 *  Created on: Jul 30, 2010
 *      Author: stockman
 */

#include "PndWriteoutBufferT.h"
#include "FairRootManager.h"
#include "PndSdsDigiPixel.h"
#include "TClonesArray.h"
#include <iostream>

template<class T> PndWriteoutBufferT<T>::PndWriteoutBufferT(TString branchName, TString className):
	fBranchName(branchName), fClassName(className), fActivateTimeOrder(kTRUE)
{
	if (fBranchName == "" || fClassName == "")
		fTreeSave = false;
	else
		fTreeSave = true;
}

template<class T> void PndWriteoutBufferT<T>::WriteOutData(double time)
{
	FairRootManager* ioman = FairRootManager::Instance();

	if (fActivateTimeOrder){
		std::cout << "WriteOutData for time: " << time << std::endl;
		std::vector<T> data = GetRemoveOldData(time);
		if (data.size() > 0){
			TClonesArray* myArray = ioman->GetEmptyTClonesArray(fBranchName);
			if (!myArray)
				std::cout << "-E- PndWriteoutBuffer::WriteOutData " << fBranchName << " array is not available!" << std::endl;
			for (int i = 0; i < data.size(); i++){
				new ((*myArray)[i]) T(data[i]);
			}
			ioman->GetEmptyTClonesArray(fBranchName);
		}
	}
	else{
		ioman->GetEmptyTClonesArray(fBranchName);
	}
}

template<class T> void PndWriteoutBufferT<T>::WriteOutAllData()
{
	WriteOutData(fDeadTime_map.rbegin()->first);
}

template<class T> std::vector<T> PndWriteoutBufferT<T>::GetRemoveOldData(double time)
{
	typedef typename std::multimap<double, T>::iterator DTMapIter;
	std::vector<T> result;
	for(DTMapIter it = fDeadTime_map.begin(); it != fDeadTime_map.lower_bound(time); it++){
		std::cout << "-I- GetRemoveOldData: DeadTime: " << it->first << " Pixel: " << it->second << std::endl;
		result.push_back(it->second);
		if (fData_map.find(it->second) != fData_map.end()){
			fData_map.erase(fData_map.find(it->second));
		}
		else ;
			//std::cout << "Digi not found" << std::endl;
	}
	 fDeadTime_map.erase(fDeadTime_map.begin(), fDeadTime_map.lower_bound(time));
	return result;
}

template<class T> std::vector<T> PndWriteoutBufferT<T>::GetAllData()
{
	return GetRemoveOldData(fDeadTime_map.rbegin()->first);
}

template<class T> void PndWriteoutBufferT<T>::FillNewData(T& data, double activeTime)
{
	if (fActivateTimeOrder){
		typedef typename std::multimap<double, T>::iterator DTMapIter;
		typedef typename std::map<T, double>::iterator DataMapIter;

		DataMapIter datait = fData_map.find(data);
		if(datait != fData_map.end()){					//if an older active data object is already present
			std::cout << " OldData found! " << std::endl;
			std::cout << "New Data: " << activeTime << " : " << data << std::endl;
			double currentdeadtime = datait->second;
			T oldData;
			for (DTMapIter it = fDeadTime_map.lower_bound(currentdeadtime); it != fDeadTime_map.upper_bound(currentdeadtime); it++){
				oldData = it->second;
				std::cout << "Check Data: " << it->first << " : " << oldData << std::endl;
				if (oldData == data){
					std::cout << " oldData == data " << std::endl;
					std::cout << it->first << " : " << it->second << std::endl;
					fDeadTime_map.erase(it);
					break;
				}
			}
			double newDeadTime = CalcNewActiveTime(currentdeadtime, data);
			fData_map[data] = newDeadTime;
			T modifiedData = Modify(oldData, data);
			fDeadTime_map.insert(std::pair<double, T>(newDeadTime, modifiedData));
			std::cout << "Modified Data: " << newDeadTime << " : " << modifiedData << std::endl;
			//T newData = Modify(myData, data);
		}
		else{
			//std::cout << " Hat nicht geklappt! " << std::endl;
			fData_map.insert(std::pair<T, double>(data, activeTime));
			fDeadTime_map.insert(std::pair<double, T>(activeTime, data));
		}
	}
	else{
		FairRootManager* ioman = FairRootManager::Instance();
		TClonesArray* myArray = ioman->GetTClonesArray(fBranchName);
		new ((*myArray)[myArray->GetEntries()]) T(data);
	}
}

template <> double PndWriteoutBufferT<PndSdsDigiPixel>::CalcNewActiveTime(double oldActiveTime, PndSdsDigiPixel& newData){
	return oldActiveTime + newData.GetCharge()*4;
};

template <> PndSdsDigiPixel PndWriteoutBufferT<PndSdsDigiPixel>::Modify(PndSdsDigiPixel& oldData, PndSdsDigiPixel& newData){
	oldData.AddCharge(newData.GetCharge());
	return oldData;
};


template class PndWriteoutBufferT<PndSdsDigiPixel>;

//#include "Rtypes.h"
//template<class T> class PndWriteoutBufferT;
//typedef PndWriteoutBufferT<PndSdsDigiPixel> PndSdsPixelDigiBuffer;
//
//template PndSdsPixelDigiBuffer GetRemoveOldData <PndSdsDigiPixel> (double time);
//template PndSdsPixelDigiBuffer FillNewData <PndSdsDigiPixel> (PndSdsDigiPixel& data, double activeTime);

//ClassImp(PndWriteoutBufferT<PndSdsDigiPixel>);
