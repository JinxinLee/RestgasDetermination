/*
 * ROOTDataHelper.h
 *
 *  Created on: Jun 15, 2013
 *      Author: steve
 */

#ifndef ROOTDATAHELPER_H_
#define ROOTDATAHELPER_H_


#ifdef HAS_SHAREDPOINTER
#include <memory>
using std::shared_ptr;
#else
#include <tr1/memory>
using std::tr1::shared_ptr;
#endif

class TH1D;
class TGraphErrors;
class Data;

class ROOTDataHelper {
public:
	ROOTDataHelper();
	virtual ~ROOTDataHelper();

	shared_ptr<Data> createBinnedData(TH1D* hist_1d);
	shared_ptr<Data> createBinnedData(TGraphErrors* graph_1d);
};

#endif /* ROOTDATAHELPER_H_ */
