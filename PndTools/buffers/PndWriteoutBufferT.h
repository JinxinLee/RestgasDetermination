/*
 * PndWriteoutBufferT.h
 *
 *  Created on: Jul 30, 2010
 *      Author: stockman
 */

#ifndef PNDWRITEOUTBUFFERT_H_
#define PNDWRITEOUTBUFFERT_H_



#include "TObject.h"
#include "PndSdsDigiPixel.h"

#include <map>


template <class T> class PndWriteoutBufferT : public TObject{
public:
	PndWriteoutBufferT(){};
	virtual ~PndWriteoutBufferT(){};

	std::vector<T> GetRemoveOldData(double time);
	void FillNewData(T& data, double activeTime);

	T Modify(T& oldData, T& newData){ return oldData;};
	double CalcNewActiveTime(double oldActiveTime, T& newData){ return oldActiveTime;};

protected:
	std::multimap<double, T> fDeadTime_map;
	std::map<T, double> fData_map;

	ClassDef(PndWriteoutBufferT, 1);
};

#endif /* PNDWRITEOUTBUFFERT_H_ */
