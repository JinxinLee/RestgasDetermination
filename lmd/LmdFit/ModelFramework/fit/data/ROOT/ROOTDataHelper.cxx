/*
 * ROOTDataHelper.cxx
 *
 *  Created on: Jun 15, 2013
 *      Author: steve
 */

#include "ROOTDataHelper.h"
#include "fit/data/Data.h"

#include "TH1D.h"
#include "TGraphErrors.h"

#include <iostream>

ROOTDataHelper::ROOTDataHelper() {
	// TODO Auto-generated constructor stub

}

ROOTDataHelper::~ROOTDataHelper() {
	// TODO Auto-generated destructor stub
}

void ROOTDataHelper::fillBinnedData(shared_ptr<Data> data, const TH1D* hist_1d) const {
	data->clearData();

	for (int i = 1; i <= hist_1d->GetNbinsX(); i++) {
		shared_ptr<DataStructs::binned_data_point> datapoint(
				new DataStructs::binned_data_point());

		datapoint->bin_center_value[0] = hist_1d->GetBinCenter(i);
		datapoint->bin_widths[0] = hist_1d->GetBinWidth(i);
		datapoint->z = hist_1d->GetBinContent(i);
		datapoint->z_error = hist_1d->GetBinError(i);

		if (datapoint->z == 0.0)
			continue;

		DataPointProxy dpp;
		dpp.setBinnedDataPoint(datapoint);
		data->insertData(dpp);
	}
}

void ROOTDataHelper::fillBinnedData(shared_ptr<Data> data,
		const TGraphErrors* graph_1d) const {
	data->clearData();

	for (int i = 1; i <= graph_1d->GetN(); i++) {
		shared_ptr<DataStructs::binned_data_point> datapoint(
				new DataStructs::binned_data_point());
		graph_1d->GetPoint(i, datapoint->bin_center_value[0], datapoint->z);
		datapoint->z_error = graph_1d->GetErrorY(i);
		datapoint->bin_widths[0] = 1.0;

		if (datapoint->z == 0.0)
			continue;

		DataPointProxy dpp;
		dpp.setBinnedDataPoint(datapoint);
		data->insertData(dpp);
	}
}
