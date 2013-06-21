/*
 * ROOTDataHelper.h
 *
 *  Created on: Jun 15, 2013
 *      Author: steve
 */

#ifndef ROOTDATAHELPER_H_
#define ROOTDATAHELPER_H_

#include <tr1/memory>

class TH1D;
class TGraphErrors;

using std::tr1::shared_ptr;

class Data;

class ROOTDataHelper {
public:
	ROOTDataHelper();
	virtual ~ROOTDataHelper();

	shared_ptr<Data> createBinnedData(TH1D* hist_1d);
	shared_ptr<Data> createBinnedData(TGraphErrors* graph_1d);
};

#endif /* ROOTDATAHELPER_H_ */
