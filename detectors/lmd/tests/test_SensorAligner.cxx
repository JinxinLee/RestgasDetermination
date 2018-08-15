// Define Boost test module
#define BOOST_TEST_MODULE LmdSensorAlignment

#include <vector>

#include <boost/test/unit_test.hpp>

#include "TGeoMatrix.h"
#include "TRandom3.h"

#include "PndLmdHitPair.h"
#include "PndLmdSensorAligner.h"

BOOST_AUTO_TEST_SUITE(LmdSensorAlignment)

TVector3 createRandomPoint() {
	TVector3 point;
	double scale(0.2);
	point.SetX(gRandom->Uniform(scale));
	point.SetY(gRandom->Uniform(scale));
	point.SetZ(0.0);
	return point;
}

BOOST_AUTO_TEST_CASE(SimpleICPTest_Double) {
	PndLmdSensorAligner aligner;

	gRandom = new TRandom3();

	std::vector<TVector3> original_points;
	unsigned int num_points(100);
	for (unsigned int i = 0; i < num_points; ++i) {
		original_points.push_back(createRandomPoint());
	}

	TGeoCombiTrans ideal_matrix;
	double angle(2.1);
	ideal_matrix.RotateZ(angle);
	double shiftx(0.05);
	double shifty(-0.03);
	ideal_matrix.SetTranslation(shiftx, shifty, 0.0);

	PndLmdHitPair pair;
	for (auto const& point : original_points) {
		pair.setHit1(point);
		double a[3] = { point.X(), point.Y(), point.Z() };
		double b[3];
		ideal_matrix.MasterToLocal(a, b);
		pair.setHit2(TVector3(b[0], b[1], b[2]));
		aligner.addSimplePair(pair);  //returns true if addPair succeeded
	}

	aligner.calculateMatrix();
	Matrix result = aligner.getResultMatrix();
	//std::cout<<result<<std::endl;
	double sinangle = std::sin(TMath::Pi() * angle / 180);
	double tolerance(1e-10); //relative
	BOOST_CHECK_CLOSE(result.val[0][1], -sinangle, tolerance);
	BOOST_CHECK_CLOSE(result.val[1][0], sinangle, tolerance);
	BOOST_CHECK_CLOSE(result.val[0][3], shiftx, tolerance);
	BOOST_CHECK_CLOSE(result.val[1][3], shifty, tolerance);
}

BOOST_AUTO_TEST_CASE(SimpleICPTest_Float) {
	PndLmdSensorAligner aligner;

	gRandom = new TRandom3();

	std::vector<TVector3> original_points;
	unsigned int num_points(50);
	for (unsigned int i = 0; i < num_points; ++i) {
		original_points.push_back(createRandomPoint());
	}

	TGeoRotation lmd_rot("lmd_rot");
	lmd_rot.RotateX(0.0);
	lmd_rot.RotateY(0.04 / 3.14 * 180.);
	lmd_rot.RotateZ(0.0);
	TGeoCombiTrans simple_trafo_to_lmd;
	simple_trafo_to_lmd.SetRotation(lmd_rot);
	simple_trafo_to_lmd.SetTranslation(7.0, 3.0, 1100.0);

	TGeoCombiTrans ideal_matrix;
	double angle(2.0);
	ideal_matrix.RotateZ(angle);
	double shiftx(0.01);
	double shifty(-0.03);
	ideal_matrix.SetTranslation(shiftx, shifty, 0.0);

	PndLmdHitPair pair;
	for (auto const& point : original_points) {
		double a[3] = { point.X(), point.Y(), point.Z() };
		double b[3];
		ideal_matrix.MasterToLocal(a, b);
		// now transform points to pnd global frame
		double newa[3];
		simple_trafo_to_lmd.LocalToMaster(a, newa);
		double newb[3];
		simple_trafo_to_lmd.LocalToMaster(b, newb);
		// reduce precision
		for(unsigned i = 0; i < 3; ++i) {
			newa[i] = (float)newa[i];
			newb[i] = (float)newb[i];
		}
		// and now transform back to lmd
		simple_trafo_to_lmd.MasterToLocal(newa, a);
		simple_trafo_to_lmd.MasterToLocal(newb, b);

		pair.setHit1(TVector3(a[0], a[1], a[2]));
		pair.setHit2(TVector3(b[0], b[1], b[2]));
		aligner.addSimplePair(pair);
	}

	aligner.calculateMatrix();
	Matrix result = aligner.getResultMatrix();
	//std::cout<<result<<std::endl;
	double sinangle = std::sin(TMath::Pi() * angle / 180);
	double tolerance(1e-5); //absolute
	BOOST_CHECK_SMALL(std::fabs(result.val[0][1] + sinangle), tolerance);
	BOOST_CHECK_SMALL(std::fabs(result.val[1][0]- sinangle), tolerance);
	BOOST_CHECK_SMALL(std::fabs(result.val[0][3] - shiftx), tolerance);
	BOOST_CHECK_SMALL(std::fabs(result.val[1][3]- shifty)	, tolerance);
}

BOOST_AUTO_TEST_SUITE_END()
