/*
 * PndLmdAlignStructs.h
 *
 *	Header-only file to collect all structs used during SensorAlignment
 *
 *  Created on: Jul 20, 2017
 *      Author: Roman Klasen, roklasen@uni-mainz.de or klasen@kph.uni-mainz.de
 */

#ifndef LMD_LMDSENSORALIGNMENT_PNDLMDALIGNSTRUCTS_H_
#define LMD_LMDSENSORALIGNMENT_PNDLMDALIGNSTRUCTS_H_

#include "PndLmdHitPair.h"

#include <TH1D.h>
#include <TCanvas.h>

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

using std::max;
using std::min;
using std::cout;
using std::vector;

//calculates dynamic cut parameters for each overlapping area
struct dynamicCutHandler {
	int _overlapID;
	bool _ready;

	std::vector<double> samples;

	double _minDist, _maxDist, _hardMax, _mean, _RMS;

	dynamicCutHandler() {
		_overlapID = 0;
		_minDist = _maxDist = 0.0;
		_hardMax = 32 * 80e-4; //sensors should not be farther than 32 pixels, 2,5mm!
		_ready = false;
		_mean = 0;
		_RMS = 0;
	}

	void addToSamples(PndLmdHitPair pair) {

		//set first sample data
		if (samples.size() == 0) {
			_overlapID = pair.getOverlapId();
			_minDist = pair.getDistance();
		} else {
			if (_overlapID != pair.getOverlapId()) {
				cout
						<< "something is wrong! stored OverlapID does not match added ID!\n";
				return;
			}
		}

		//100 should suffice, but more is always better
		if (samples.size() > 150) {
			_ready = true;
		}

		double thisDistance = pair.getDistance();

		_minDist = std::min(_minDist, thisDistance);
		_maxDist = std::max(_maxDist, std::min(thisDistance, _hardMax)); //never choose maximum higher than hardMax

		// store distance only if in valid range, some distances are too large
		if (thisDistance >= _minDist && thisDistance <= _maxDist) {
			samples.push_back(pair.getDistance());
		}

		return;
	}

	void calcMinAndMax() {

		// choose 80 percent confidence interval:
		std::sort(samples.begin(), samples.end());
		int quantileMargin = samples.size() / 10;// shave 10% from front and back

		vector<double>::const_iterator first = samples.begin() + quantileMargin;
		vector<double>::const_iterator last = samples.end() - quantileMargin;
		vector<double> confidenceInterval(first, last);

		_minDist = confidenceInterval[0];
		_maxDist = confidenceInterval[confidenceInterval.size() - 1];

		// leave a little safety margin:
		double spread = _maxDist - _minDist;
		_minDist = std::max(0.0, confidenceInterval[0] - spread);//should not underflow 0
		_maxDist = std::min(_hardMax,
				confidenceInterval[confidenceInterval.size() - 1] + spread);//should not overflow _hardMax

		return;

	}

	bool ready() {
		return _ready;
	}

	double getMinDist() {
		return _minDist;
	}
	double getMaxDist() {
		return _maxDist;
	}
};

//simple pixel hit, maybe not even necessary
struct pixelHit {
	int _sensorId;
	double _col;
	double _row;

	double x() const {
		return _col;
	}

	pixelHit(int idVal, double colVal, double rowVal) {
		_sensorId = idVal;
		_col = colVal;
		_row = rowVal;
	}

	pixelHit() {
		_col = -1;
		_row = -1;
		_sensorId = -1;
	}
};

/*
 * contains multiple pixelHits that form a cluster. most routines are for checking,
 * if two separate pixelHits belong to the same cluster
 */
struct pixelCluster {
	int _sensorId;
	double centerCol, centerRow; //,centerZ;
	double clusterSize;
	vector<pixelHit> pixelHits;
	bool clusterReady;

	pixelCluster() {
		_sensorId = -1;
		centerCol = -1;
		centerRow = -1; //centerZ=-1;
		clusterSize = -1;
		clusterReady = false;
	}

	pixelCluster(const pixelHit &hit) {
		_sensorId = hit._sensorId;
		pixelHits.push_back(hit);

		centerCol = -1;
		centerRow = -1; //centerZ=-1;
		clusterSize = -1;
		clusterReady = false;
	}

	pixelCluster(const pixelCluster &copy) {
		_sensorId = copy._sensorId;
		for (size_t i = 0; i < copy.pixelHits.size(); i++) {
			pixelHits.push_back(copy.pixelHits[i]);
		}

		centerCol = -1;
		centerRow = -1; //centerZ=-1;
		clusterSize = -1;
		clusterReady = false;
	}

	//checks, if two clusters lie DIRECTLY next to each other, that means any two pixels
	//must be directly next to each other
	//TODO: inefficient code, may be improved
	bool isNeighbour(pixelCluster &other) {
		//first, they must be on same sensor
		if (_sensorId != other._sensorId) {
			return false;
		}
		double _col1, _col2, _row1, _row2;
		for (size_t i = 0; i < this->pixelHits.size(); i++) {
			_col1 = this->pixelHits[i]._col;
			_row1 = this->pixelHits[i]._row;
			for (size_t j = 0; j < other.pixelHits.size(); j++) {
				_col2 = other.pixelHits[j]._col;
				_row2 = other.pixelHits[j]._row;
				//check if neighboring, that means distance of pixels is smaller than 1.5 pixels
				if ((_col2 - _col1) * (_col2 - _col1)
						+ (_row2 - _row1) * (_row2 - _row1) < 2.25) {
					return true;
				}
			}
		}
		return false;
	}

	//merges other to this one
	void merge(pixelCluster &other) {
		for (size_t i = 0; i < other.pixelHits.size(); i++) {
			pixelHits.push_back(other.pixelHits[i]);
		}
	}

	void calculateCenter() {
		centerCol = 0;
		centerRow = 0;
		for (size_t i = 0; i < pixelHits.size(); i++) {
			centerCol += pixelHits[i]._col;
			centerRow += pixelHits[i]._row;
		}
		centerCol /= pixelHits.size();
		centerRow /= pixelHits.size();
		double tempDistance;
		//calculate size, go from corner to corner for clusters larger than 2 pixels
		if (pixelHits.size() == 1) {
			clusterSize = 1;
		} else {
			for (size_t i = 0; i < pixelHits.size(); i++) {
				for (size_t j = i + 1; j < pixelHits.size(); j++) {
					double deltax = (pixelHits[i]._col - pixelHits[j]._col);
					if (deltax > 0) {
						deltax = deltax + 1;
					}
					if (deltax < 0) {
						deltax = deltax - 1;
					}
					double deltay = (pixelHits[i]._row - pixelHits[j]._row);
					if (deltay > 0) {
						deltay = deltay + 1;
					}
					if (deltay < 0) {
						deltay = deltay - 1;
					}
					tempDistance = sqrt(deltax * deltax + deltay * deltay);
					clusterSize = max(clusterSize, tempDistance);
				}
			}
		}
		clusterReady = true;
	}

	void printPixels() {
		for (size_t i = 0; i < pixelHits.size(); i++) {
			cout << "pixelHit x:" << pixelHits[i]._col << ", y:"
					<< pixelHits[i]._row << " on sensor "
					<< pixelHits[i]._sensorId << "\n";
		}
	}
	void printCenter() {
		cout << "clusterCenter x:" << centerCol << ", y:" << centerRow
				<< " on sensor " << _sensorId << ", contains "
				<< pixelHits.size() << " pixels and is " << clusterSize
				<< " pixels in diameter." << "\n";
	}
};

#endif /* LMD_LMDSENSORALIGNMENT_PNDLMDALIGNSTRUCTS_H_ */
