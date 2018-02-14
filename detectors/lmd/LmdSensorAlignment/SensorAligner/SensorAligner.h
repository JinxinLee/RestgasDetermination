/*
 * SensorAligner.h
 *
 *  Created on: Jul 29, 2013
 *      Author: roman
 *
 *      This object is responsible for the actual alignment determination.
 *      That means, this object needs to know which CVD it is supposed to handle, and
 *      then collects all pair files neccesary (it must get exactly 9 pair files).
 *      It is responsible for the actual alignment stage, so sorting, filtering
 *      etc of the root file must occur earlier. This object needs refined pair files!
 *
 *      After alignment is done, it writes the transformation matrices, makes error
 *      estimation based on bootstrapped pair bunches (see below) and writes a complete
 *      log of the alignment procedure for this specific CVD. That log will contain
 *      the number of iterations for each overlapping area, the bootstrap error for
 *      each overlapping area and maybe time needed.
 *
 *      Error determination by bootstapping: collect HitPairs at random from pool and
 *      into bunches, make ICP determination for every bunch (at leat 100 times, maybe more),
 *      and calculate dx, dy and dR with sigmas for every overlapping area. This provides
 *      error for overlapping areas only! Error for calculated matrices must come from
 *      gaussian error calculation. This will come later.
 *
 *      Reminder: the notation frontSensorID to BackSensorID is local only! Real
 *      position comes from information of half, plane, cvd!
 *
 */

#ifndef SensorAligner_H_
#define SensorAligner_H_

/*
 * The class that does all the actual work. It reads hit pairs from pair files and alignes every sensor module using these pair files.
 * Normal mode: create SensorAligner, collect files from directory, align and write Matrices to Disk.
 * Single mode: set to single mode and load one pair file, align only this one area.
 *
 * getPairs: after pair files are read from disk, this method gets a pointer to that boost vector of hit pairs.
 * beware: sensor aligner deletes this vector, if deleteOnDestruction is true (default).
 * setPairs: provide Sensor aligner with vector of pairs that you got elsewhere. useful for RMS rounds.
 * beware: you must provide all nine pair vectors and set dataReady to true manually, or sensor aligner will
 * not work at all.
 */

#include <TROOT.h>
#include <matrix.h>
#include <PndLmdDim.h>
#include <PndLmdHitPair.h>

#include <vector>

class SensorAligner{
private:
	PndLmdDim* dimension;

	//center of pixel correction matrices
		Matrix matrixpixelEdgeToCenter;
		Matrix matrixactiveEdgeToCenter;
		Matrix matrixscaleActiveToSensor;
		Matrix newPixelsToSensorInCm, newSensorInCmToPixels;

public:

	//ClassDef(SensorAligner, 1);
	SensorAligner();
	SensorAligner(PndLmdDim* dimension);
	virtual ~SensorAligner();

	//quick and dirty! supply nice hit pairs!
	Matrix getMatrixFromHitPairs(vector<PndLmdHitPair> &pairs);

	Matrix getIdealMatrixOfficialGeometry(Int_t fromSensor, Int_t toSensor, bool misaligned=false);

	void demoICP();

};

#endif /* SensorAligner_H_ */
