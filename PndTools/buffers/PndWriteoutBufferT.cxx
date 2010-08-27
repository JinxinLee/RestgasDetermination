/*
 * PndWriteoutBufferT.cpp
 *
 *  Created on: Jul 30, 2010
 *      Author: stockman
 */

#include "PndWriteoutBufferT.h"

template<class T> std::vector<T> PndWriteoutBufferT<T>::GetRemoveOldData(double time)
{
	typedef typename std::multimap<double, T>::iterator DTMapIter;
	std::vector<T> result;
	for(DTMapIter it = fDeadTime_map.begin(); it != fDeadTime_map.lower_bound(time); it++){
		std::cout << it->second << std::endl;
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

template<class T> void PndWriteoutBufferT<T>::FillNewData(T& data, double activeTime)
{
	typedef typename std::multimap<double, T>::iterator DTMapIter;
	typedef typename std::map<T, double>::iterator DataMapIter;

	DataMapIter datait = fData_map.find(data);
	if(datait != fData_map.end()){
		std::cout << " OldData found! " << std::endl;
		double currentdeadtime = datait->second;
		T oldData;
		for (DTMapIter it = fDeadTime_map.upper_bound(currentdeadtime); it != fDeadTime_map.lower_bound(currentdeadtime); it++){
			oldData = it->second;
			if (oldData == data){
				std::cout << " oldData == data " << std::endl;
				fDeadTime_map.erase(it);
				break;
			}
		}
		double newDeadTime = CalcNewActiveTime(currentdeadtime, data);
		fData_map[data] = newDeadTime;
		T modifiedData = Modify(oldData, data);
		fDeadTime_map.insert(std::pair<double, T>(newDeadTime, modifiedData));
		//T newData = Modify(myData, data);
	}
	else{
		std::cout << " Hat nicht geklappt! " << std::endl;
		fData_map.insert(std::pair<T, double>(data, activeTime));
		fDeadTime_map.insert(std::pair<double, T>(activeTime, data));
	}
}

template <> double PndWriteoutBufferT<PndSdsDigiPixel>::CalcNewActiveTime(double oldActiveTime, PndSdsDigiPixel& newData){
	return oldActiveTime + newData.GetCharge()*4;
};

template <> PndSdsDigiPixel PndWriteoutBufferT<PndSdsDigiPixel>::Modify(PndSdsDigiPixel& oldData, PndSdsDigiPixel& newData){
	oldData.AddCharge(newData.GetCharge());
	return oldData;
}

template class PndWriteoutBufferT<PndSdsDigiPixel>;

templateClassImp(PndWriteoutBufferT);
