/*
 * DataPointProxy.h
 *
 *  Created on: Jun 16, 2013
 *      Author: steve
 */

#ifndef DATAPOINTPROXY_H_
#define DATAPOINTPROXY_H_

#include "DataStructs.h"

#include "SharedPtr.h"

class DataPointProxy {
private:
	shared_ptr<DataStructs::unbinned_data_point> udp;
	shared_ptr<DataStructs::binned_data_point> bdp;

	int state;
	bool is_point_used;
public:
	DataPointProxy();
	virtual ~DataPointProxy();

	bool isBinnedDataPoint() const;
	bool isUnbinnedDataPoint() const;
	const shared_ptr<DataStructs::binned_data_point> getBinnedDataPoint() const;
	const shared_ptr<DataStructs::unbinned_data_point> getUnbinnedDataPoint() const;
	void setBinnedDataPoint(shared_ptr<DataStructs::binned_data_point> bdp_);
	void setUnbinnedDataPoint(shared_ptr<DataStructs::unbinned_data_point> udp_);
	bool isPointUsed() const;
	void setPointUsed(bool is_point_used_);
};

#endif /* DATAPOINTPROXY_H_ */
