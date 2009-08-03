
#include <iostream>
using std::cout;
using std::cerr;
using std::cin;
using std::endl;

#include <valarray>
using std::valarray;

#include <vector>
using std::vector;

#include <string>
using std::string;

#include <list>
using std::list;

#include <limits>

#include <fstream>
using std::fstream;

#include <utility>
using std::pair;

#include <map>
using std::map;

#include <cmath>

#include "TROOT.h"
#include "TRint.h"
#include "TVector3.h"
#include "TRandom.h"
#include "TRandom3.h"
#include "TRotation.h"
#include "TPaveText.h"
#include "TString.h"
#include "TMatrixD.h"
#include "TMath.h"

#include "Math/Vector3D.h"
using ROOT::Math::XYZVector;

#include "Math/Point3D.h"
using ROOT::Math::XYZPoint;

#include "Math/Transform3D.h"
using ROOT::Math::Transform3D;

#include "Math/RotationX.h"
using ROOT::Math::RotationX;
#include "Math/RotationY.h"
using ROOT::Math::RotationY;
#include "Math/RotationZ.h"
using ROOT::Math::RotationZ;
#include "Math/Rotation3D.h"
using ROOT::Math::Rotation3D;


#include "PndDrcPhoton.h"
#include "PndDrcSurfPolyFlat.h"
#include "PndDrcSurfQuadFlatDiff.h"
#include "PndDrcSurfPolyAsphere.h"
#include "PndDrcSurfPolySphere.h"
#include "PndDrcSurfPolyPara.h"
#include "PndDrcSurfPolyAsphere.h"
#include "PndDrcOptReflSilver.h"
#include "PndDrcOptReflNone.h"
#include "PndDrcOptReflPerfect.h"
#include "PndDrcOptMatLithotecQ0.h"
#include "PndDrcOptMatBK7.h"
#include "PndDrcOptMatVacuum.h"
#include "PndDrcOptDevSys.h"
#include "PndDrcOptVol.h"
#include "PndDrcOptDevManager.h"
#include "PndDrcUtil.h"


int main(int argc, char *argv[])
{
  cout << "Parameter usage:" << endl;
  cout << "  1. Par.: incidence angle [deg] (default: 57 deg)" << endl;
  cout << "  2. Par.: hit pos. X on bar [mm] (default: 1/2 slab width 8.5 mm )" << endl;
  cout << "  3. Par.: hit pos. Y on bar [mm] (default: 0 mm)" << endl;
  cout << "  4. Par.: hit pos. Z on bar [mm] (default: -500 mm); NEGATIVE NUMBER !!!" << endl;
  cout << "  if one parameter was set filename is \"Screen_beamspot_Xdeg_X_Y_Z.C\" (deault Screen.C)" << endl << endl;

	double conical_const =  0; //sphere
// 	double conical_const = 0.5; //parabola

// 	int ioption = 1; // parallel beam (0 deg)
// 	int ioption = 2; // 0,  +- 20,  +-40 deg
	int ioption = 3; // C-cone
// 	int ioption = 4; // random in front of lens.
// 	int ioption = 5; // single photon for debugging.
// 	int ioption = 6; // grid 5 deg

// 	double pi=3.1415926535;
	double pi = TMath::Pi();

	PndDrcOptReflPerfect refl_perfect;
	PndDrcOptReflNone    refl_none; // absorbed

	double slab_width  = 17;
	double slab_height = 35;

	PndDrcOptMatLithotecQ0  quartz;
	PndDrcOptMatVacuum      vacuum;
	PndDrcOptMatBK7         bk7;

	double lambda = 435;


// 	double wavelength[4];
// 	wavelength[0]=300;
// 	wavelength[1]=435;
// 	wavelength[2]=532;
// 	wavelength[3]=589;
// 	for( int i=0; i < 4; i++ )
// 	{
// 		cout << "n_quartz    (" << wavelength[i] << " nm) = " << quartz.RefIndex(wavelength[i])     << endl;
// 		cout << "n_vacuum    (" << wavelength[i] << " nm) = " << vacuum.RefIndex(wavelength[i])     << endl;
// 		cout << "n_nlak33a   (" << wavelength[i] << " nm) = " << nlak33a.RefIndex(wavelength[i])    << endl;
// 		cout << "n_llf1      (" << wavelength[i] << " nm) = " << llf1.RefIndex(wavelength[i])       << endl;
// 		cout << "n_bk7       (" << wavelength[i] << " nm) = " << bk7.RefIndex(wavelength[i])        << endl;
// 		cout << "n_lensIndex (" << wavelength[i] << " nm) = " << lensIndex.RefIndex(wavelength[i])  << endl;
// 		cout << "n_oilIndex  (" << wavelength[i] << " nm) = " << oilIndex.RefIndex(wavelength[i])   << endl << endl;
// 	}


  // Note: wrong x-orientation for the points to declare a surface but only surfaces were coupled later
  //       therefore it should be no problem



// lens
//==============================================================================
	XYZPoint q0(-slab_width/2, +slab_height/2, 0);
	XYZPoint q1(-slab_width/2, -slab_height/2, 0);
	XYZPoint q2(+slab_width/2, -slab_height/2, 0);
	XYZPoint q3(+slab_width/2, +slab_height/2, 0);

	PndDrcSurfPolyAsphere lens_sphere;
	lens_sphere.SetVerbosity(0);
	lens_sphere.AddPoint(q0);
	lens_sphere.AddPoint(q1);
	lens_sphere.AddPoint(q2);
	lens_sphere.AddPoint(q3);
	lens_sphere.SetRadius(77.52); // f = R/(n-1) prev R: 75mm (BK7 Newport f=150 R=77.52 at 589nm)
	lens_sphere.SetPrintColor(2);
	lens_sphere.SetConicalConstant(conical_const);
	lens_sphere.SetName("lens_sphere");
	double dist = lens_sphere.CenterPoint().Z(); // in general the lens radius 77.52
// 	cout << "dist = " << dist << endl;
	lens_sphere.AddTransform(Transform3D(XYZVector(0,0,-(dist)))); // ( dist |  =>  |(


// 	cout << "Transform3D:" << endl;
// 	TMatrixD m(3,4);
// 	Transform3D transi = Transform3D(XYZVector(0,0,-(dist)));
// 	Transform3D transid; // identy rotation + zero translation
// //   Transform3D addtrans = transi * transid;
// //   Transform3D transInv = addtrans.Inverse();
// 	transi.GetComponents(m.GetMatrixArray());
// 	for(int i=0;i<3;i++)
// 	{
// 		for(int j=0;j<4;j++)
// 		{
// 			cout << m[i][j] << " ";
// 		}
// 		cout << endl;
// 	}
// 	cout << endl;


    double lensBase = 7.5; // roughly measured lens thickness: 7.5 ; old: 5
	cout << "lens thickness should be greater than: |" << lens_sphere.LimitingPoint(0).Z() << "|" << " and is currently " << lensBase << " mm" << endl;

	q0+=XYZVector(0,0,-lensBase);
	q1+=XYZVector(0,0,-lensBase);
	q2+=XYZVector(0,0,-lensBase);
	q3+=XYZVector(0,0,-lensBase);

	PndDrcSurfPolyFlat lens_base;
	lens_base.SetVerbosity(0);
	lens_base.AddPoint(q0);
	lens_base.AddPoint(q1);
	lens_base.AddPoint(q2);
	lens_base.AddPoint(q3);
	lens_base.SetPrintColor(2);
	lens_base.SetName("lens_base");

	PndDrcSurfQuadFlatDiff lens_side1;
	lens_side1.AddSurface(lens_base, q0,q1);
	lens_side1.AddSurface(lens_sphere, lens_sphere.LimitingPoint(0),lens_sphere.LimitingPoint(1));
	lens_side1.SetPrintColor(2);
	lens_side1.SetName("lens_side1");

	PndDrcSurfQuadFlatDiff lens_side2;
	lens_side2.SetVerbosity(0);
	lens_side2.AddSurface(lens_base, q1,q2);
	lens_side2.AddSurface(lens_sphere, lens_sphere.LimitingPoint(1),lens_sphere.LimitingPoint(2));
	lens_side2.SetPrintColor(2);
	lens_side2.SetName("lens_side2");

	PndDrcSurfQuadFlatDiff lens_side3;
	lens_side3.AddSurface(lens_base, q2,q3);
	lens_side3.AddSurface(lens_sphere, lens_sphere.LimitingPoint(2),lens_sphere.LimitingPoint(3));
	lens_side3.SetPrintColor(2);
	lens_side3.SetName("lens_side3");

	PndDrcSurfQuadFlatDiff lens_side4;
	lens_side4.AddSurface(lens_base, q3,q0);
	lens_side4.AddSurface(lens_sphere, lens_sphere.LimitingPoint(3),lens_sphere.LimitingPoint(0));
	lens_side4.SetPrintColor(2);
	lens_side4.SetName("lens_side4");


// 	cout << "LimitingPoint:" << endl;
// 	cout << lens_sphere.LimitingPoint(0) << endl;
// 	cout << lens_sphere.LimitingPoint(1) << endl;
// 	cout << lens_sphere.LimitingPoint(2) << endl;
// 	cout << lens_sphere.LimitingPoint(3) << endl << endl;


// 	lens_side1.SetReflectivity(refl_none);
// 	lens_side2.SetReflectivity(refl_none);
// 	lens_side3.SetReflectivity(refl_none);
// 	lens_side4.SetReflectivity(refl_none);
// 	lens_sphere.SetReflectivity(refl_none);
// 	lens_sphere.SetPixel();
// 	lens_side1.SetPixel();
// 	lens_side2.SetPixel();
// 	lens_side3.SetPixel();
// 	lens_side4.SetPixel();
// 	lens_side1.SetVerbosity(4);
// 	lens_side2.SetVerbosity(4);
// 	lens_side3.SetVerbosity(4);
// 	lens_side4.SetVerbosity(4);


	PndDrcOptVol lens;
	lens.SetVerbosity(0);
	lens.AddSurface(lens_base);
	lens.AddSurface(lens_sphere);
	lens.AddSurface(lens_side1);
	lens.AddSurface(lens_side2);
	lens.AddSurface(lens_side3);
	lens.AddSurface(lens_side4);
    lens.SetOptMaterial(bk7);//bk7
	cout << "lens: bk7" << endl;
	lens.SetName("lens");

	double lens_shift = lensBase; // lensBase
// 	if( -lens_sphere.LimitingPoint(0).Z() > lens_shift )
// 		cout << " *** need a bigger lens shift" << endl;
	lens.AddTransform(Transform3D(XYZVector(0,0,lens_shift)));



// box (fishtank) with oil
//==============================================================================
	double vacBoxwidth = 10; // old: 4.5 ; new: 10
	if( lensBase > vacBoxwidth )
		cout << " *** no air gap possible" << endl;

  // old: X & Y [-300,300] new: X [-150,150] & Y [-100,100]
  //                                                           screen
	XYZPoint p1(-150.0, +100.0, vacBoxwidth);  //            p5----------p8
	XYZPoint p2(-150.0, -100.0, vacBoxwidth);  //           /|          /|
	XYZPoint p3(+150.0, -100.0, vacBoxwidth);  //          / |         / |
	XYZPoint p4(+150.0, +100.0, vacBoxwidth);  //         /  p6-------/--p7
	//                                                   /  /        /  /
	//                                                  /  /        /  /
	//                                                 /  /        /  /
	//                                                /  /        /  /
	XYZPoint p5(-150.0, +100.0, +230); //           p1----------p4  /
	XYZPoint p6(-150.0, -100.0, +230); //           |  /        |  /
	XYZPoint p7(+150.0, -100.0, +230); //           | /         | /
	XYZPoint p8(+150.0, +100.0, +230); //           p2----------p3


	PndDrcSurfPolyFlat box_side1;
	box_side1.AddPoint(p1);
	box_side1.AddPoint(p2);
	box_side1.AddPoint(p3);
	box_side1.AddPoint(p4);
	box_side1.SetName("box_side1");

	PndDrcSurfPolyFlat box_side2;
	box_side2.AddPoint(p2);
	box_side2.AddPoint(p3);
	box_side2.AddPoint(p7);
	box_side2.AddPoint(p6);
	box_side2.SetName("box_side2");

	PndDrcSurfPolyFlat box_side3;
	box_side3.AddPoint(p3);
	box_side3.AddPoint(p4);
	box_side3.AddPoint(p8);
	box_side3.AddPoint(p7);
	box_side3.SetName("box_side3");

	PndDrcSurfPolyFlat box_side4;
	box_side4.AddPoint(p1);
	box_side4.AddPoint(p4);
	box_side4.AddPoint(p8);
	box_side4.AddPoint(p5);
	box_side4.SetName("box_side4");

	PndDrcSurfPolyFlat box_side5;
	box_side5.AddPoint(p5);
	box_side5.AddPoint(p6);
	box_side5.AddPoint(p7);
	box_side5.AddPoint(p8);
	box_side5.SetName("box_side5");

	PndDrcSurfPolyFlat box_side6;
	box_side6.AddPoint(p5);
	box_side6.AddPoint(p6);
	box_side6.AddPoint(p2);
	box_side6.AddPoint(p1);
	box_side6.SetName("box_side6");


// 	box_side2.SetReflectivity(refl_none);
// 	box_side3.SetReflectivity(refl_none);
// 	box_side4.SetReflectivity(refl_none);
// 	box_side6.SetReflectivity(refl_none);
	box_side5.SetPixel();


	PndDrcOptVol box;
	box.SetVerbosity(0);
	box.AddSurface(box_side1);
	box.AddSurface(box_side2);
	box.AddSurface(box_side3);
	box.AddSurface(box_side4);
	box.AddSurface(box_side5);
	box.AddSurface(box_side6);
	box.SetOptMaterial(quartz);
	cout << "box (oil): quartz" << endl;
	box.SetName("box");



// quartz bar (slab)
//==============================================================================
	XYZPoint s5(-slab_width/2, +slab_height/2, 0);
	XYZPoint s6(+slab_width/2, +slab_height/2, 0);
	XYZPoint s7(-slab_width/2, -slab_height/2, 0);
	XYZPoint s8(+slab_width/2, -slab_height/2, 0);
	XYZPoint t5(-slab_width/2, +slab_height/2, -800);
	XYZPoint t6(+slab_width/2, +slab_height/2, -800);
	XYZPoint t7(-slab_width/2, -slab_height/2, -800);
	XYZPoint t8(+slab_width/2, -slab_height/2, -800);
    // comment ???
  //s 5 6 at dist-1
  //  7 8
	//
  //t 5 6 at dist-1-1000
  //  7 8
	//
	PndDrcSurfPolyFlat slab_side1;
	slab_side1.SetVerbosity(0);
	slab_side1.AddPoint(s5); // face
	slab_side1.AddPoint(s6);
	slab_side1.AddPoint(s8);
	slab_side1.AddPoint(s7);
	slab_side1.SetName("slab_side1");

	PndDrcSurfPolyFlat slab_side2;
	slab_side2.AddPoint(s5);//left
	slab_side2.AddPoint(s7);
	slab_side2.AddPoint(t7);
	slab_side2.AddPoint(t5);
	slab_side2.SetName("slab_side2");

	PndDrcSurfPolyFlat slab_side3;
	slab_side3.AddPoint(s7);//down
	slab_side3.AddPoint(s8);
	slab_side3.AddPoint(t8);
	slab_side3.AddPoint(t7);
	slab_side3.SetName("slab_side3");

	PndDrcSurfPolyFlat slab_side4;
	slab_side4.AddPoint(s6);//right
	slab_side4.AddPoint(s8);
	slab_side4.AddPoint(t8);
	slab_side4.AddPoint(t6);
	slab_side4.SetName("slab_side4");

	PndDrcSurfPolyFlat slab_side5;
	slab_side5.AddPoint(s5);//top
	slab_side5.AddPoint(s6);
	slab_side5.AddPoint(t6);
	slab_side5.AddPoint(t5);
	slab_side5.SetName("slab_side5");

	PndDrcSurfPolyFlat slab_side6;
	slab_side6.AddPoint(t5); // face
	slab_side6.AddPoint(t6);
	slab_side6.AddPoint(t8);
	slab_side6.AddPoint(t7);
	slab_side6.SetName("slab_side6");

//   slab_side1.SetReflectivity(refl_none);
//   slab_side2.SetReflectivity(refl_perfect);
//   slab_side3.SetReflectivity(refl_perfect);
//   slab_side4.SetReflectivity(refl_perfect);
//   slab_side5.SetReflectivity(refl_perfect);
//   slab_side6.SetReflectivity(refl_perfect);
//   slab_side1.SetPixel();

	PndDrcOptVol slab;
  //slab.SetVerbosity(5);
	slab.AddSurface(slab_side1);
	slab.AddSurface(slab_side2);
	slab.AddSurface(slab_side3);
	slab.AddSurface(slab_side4);
	slab.AddSurface(slab_side5);
	slab.AddSurface(slab_side6);
	slab.SetOptMaterial(quartz);
	cout << "slab: quartz" << endl;
	slab.SetName("slab");



// small air box
//==============================================================================
//     XYZPoint b1(-300.0, +300.0, 0); //                    b5----------b8
//     XYZPoint b2(-300.0, -300.0, 0); //                   /|          /|
//     XYZPoint b3(+300.0, -300.0, 0); //                  / |         / |
//     XYZPoint b4(+300.0, +300.0, 0); //                 /  b6-------/--b7
	//     //                                                /  /        /  /
	//     //                                               /  /        /  /
	//     //                                              /  /        /  /
	//     //                                             /  /        /  /
	//     XYZPoint b5(-300.0, +300.0, vacBoxwidth); //  b1----------b4 /
	//     XYZPoint b6(-300.0, -300.0, vacBoxwidth); //  | /         | /
	//     XYZPoint b7(+300.0, -300.0, vacBoxwidth); //  |/          |/
//     XYZPoint b8(+300.0, +300.0, vacBoxwidth); //  b2---------b3
	//
//     PndDrcSurfPolyFlat smallBox_side1;
//     smallBox_side1.AddPoint(b1);
//     smallBox_side1.AddPoint(b2);
//     smallBox_side1.AddPoint(b3);
//     smallBox_side1.AddPoint(b4);
//     smallBox_side1.SetName("smallBox_side1");
	//
//     PndDrcSurfPolyFlat smallBox_side2;
//     smallBox_side2.AddPoint(b2);
//     smallBox_side2.AddPoint(b3);
//     smallBox_side2.AddPoint(b7);
//     smallBox_side2.AddPoint(b6);
//     smallBox_side2.SetReflectivity(refl_none);
//     smallBox_side2.SetName("smallBox_side2");
	//
//     PndDrcSurfPolyFlat smallBox_side3;
//     smallBox_side3.AddPoint(b3);
//     smallBox_side3.AddPoint(b4);
//     smallBox_side3.AddPoint(b8);
//     smallBox_side3.AddPoint(b7);
//     smallBox_side3.SetReflectivity(refl_none);
//     smallBox_side3.SetName("smallBox_side3");
	//
//     PndDrcSurfPolyFlat smallBox_side4;
//     smallBox_side4.AddPoint(b1);
//     smallBox_side4.AddPoint(b4);
//     smallBox_side4.AddPoint(b8);
//     smallBox_side4.AddPoint(b5);
//     smallBox_side4.SetReflectivity(refl_none);
//     smallBox_side4.SetName("smallBox_side4");
	//
//     PndDrcSurfPolyFlat smallBox_side5;
//     smallBox_side5.AddPoint(b5);
//     smallBox_side5.AddPoint(b6);
//     smallBox_side5.AddPoint(b7);
//     smallBox_side5.AddPoint(b8);
//     smallBox_side5.SetName("smallBox_side5");
	//
//     PndDrcSurfPolyFlat smallBox_side6;
//     smallBox_side6.AddPoint(b5);
//     smallBox_side6.AddPoint(b6);
//     smallBox_side6.AddPoint(b2);
//     smallBox_side6.AddPoint(b1);
//     smallBox_side6.SetReflectivity(refl_none);
//     smallBox_side6.SetName("smallBox_side6");
	//
	//
//     PndDrcOptVol smallBox;
//     smallBox.SetVerbosity(0);
//     smallBox.AddSurface(smallBox_side1);
//     smallBox.AddSurface(smallBox_side2);
//     smallBox.AddSurface(smallBox_side3);
//     smallBox.AddSurface(smallBox_side4);
//     smallBox.AddSurface(smallBox_side5);
//     smallBox.AddSurface(smallBox_side6);
//     smallBox.SetOptMaterial(vacuum);
//     cout << "small box: vacuum" << endl;
//     smallBox.SetName("smallBox");
	//


// small box with hole for air_lens
//==============================================================================
	XYZPoint b1(-300.0, +300.0, 0);             //             b5--------- b8
	XYZPoint b2(-300.0, -300.0, 0);             //            /|          / |
	XYZPoint b3(+300.0, -300.0, 0);             //           / |         /  |
	XYZPoint b4(+300.0, +300.0, 0);             //          /  |        /   |
	XYZPoint b5(-300.0, +300.0, vacBoxwidth);   //         /   |       /    |
	XYZPoint b6(-300.0, -300.0, vacBoxwidth);   //        /    |      /     |
	XYZPoint b7(+300.0, -300.0, vacBoxwidth);   //       /     b6----/------b7
	XYZPoint b8(+300.0, +300.0, vacBoxwidth);   //      /    /      /       /
	//                                                b1--s1--s4--b4       /
	XYZPoint s1(-slab_width/2, +300.0, 0);      //    |   |   |   |       /
	XYZPoint s2(-slab_width/2, -300.0, 0);      //    |   |   |   |      /
	XYZPoint s3(+slab_width/2, -300.0, 0);      //    |---s5--s6--|     /
	XYZPoint s4(+slab_width/2, +300.0, 0);      //    |   | H |   |    /       H: lens-hole
	//                                                |---s7--s8--|   /
	//                                                |   |   |   |  /
	//                                                |   |   |   | /
  //                                                b2--s2--s3--b3


	PndDrcSurfPolyFlat smallBox_side1a;
	smallBox_side1a.AddPoint(b1);
	smallBox_side1a.AddPoint(b2);
	smallBox_side1a.AddPoint(s2);
	smallBox_side1a.AddPoint(s1);
	smallBox_side1a.SetReflectivity(refl_none);
	smallBox_side1a.SetName("smallBox_side1a");

	PndDrcSurfPolyFlat smallBox_side1b;
	smallBox_side1b.AddPoint(s4);
	smallBox_side1b.AddPoint(s3);
	smallBox_side1b.AddPoint(b3);
	smallBox_side1b.AddPoint(b4);
	smallBox_side1b.SetReflectivity(refl_none);
	smallBox_side1b.SetName("smallBox_side1b");

	PndDrcSurfPolyFlat smallBox_side1c;
	smallBox_side1c.AddPoint(s1);
	smallBox_side1c.AddPoint(s5);
	smallBox_side1c.AddPoint(s6);
	smallBox_side1c.AddPoint(s4);
	smallBox_side1c.SetReflectivity(refl_none);
	smallBox_side1c.SetName("smallBox_side1c");

	PndDrcSurfPolyFlat smallBox_side1d;
	smallBox_side1d.AddPoint(s7);
	smallBox_side1d.AddPoint(s2);
	smallBox_side1d.AddPoint(s3);
	smallBox_side1d.AddPoint(s8);
	smallBox_side1d.SetReflectivity(refl_none);
	smallBox_side1d.SetName("smallBox_side1d");


	lens_sphere.AddTransform(Transform3D(XYZVector(0,0,lens_shift)));
	lens_side1.AddTransform(Transform3D(XYZVector(0,0,lens_shift)));
	lens_side2.AddTransform(Transform3D(XYZVector(0,0,lens_shift)));
	lens_side3.AddTransform(Transform3D(XYZVector(0,0,lens_shift)));
	lens_side4.AddTransform(Transform3D(XYZVector(0,0,lens_shift)));

	PndDrcSurfPolyAsphere smallBox_lens_sphere = lens_sphere;
	PndDrcSurfQuadFlatDiff smallBox_lens_side1 = lens_side1;
	PndDrcSurfQuadFlatDiff smallBox_lens_side2 = lens_side2;
	PndDrcSurfQuadFlatDiff smallBox_lens_side3 = lens_side3;
	PndDrcSurfQuadFlatDiff smallBox_lens_side4 = lens_side4;

	smallBox_lens_sphere.SetName("smallBox_lens_sphere");
	smallBox_lens_side1.SetName("smallBox_lens_side1");
	smallBox_lens_side2.SetName("smallBox_lens_side2");
	smallBox_lens_side3.SetName("smallBox_lens_side3");
	smallBox_lens_side4.SetName("smallBox_lens_side4");

	PndDrcSurfPolyFlat smallBox_side2;
	smallBox_side2.AddPoint(b2);
	smallBox_side2.AddPoint(b3);
	smallBox_side2.AddPoint(b7);
	smallBox_side2.AddPoint(b6);
	smallBox_side2.SetReflectivity(refl_none);
	smallBox_side2.SetName("smallBox_side2");

	PndDrcSurfPolyFlat smallBox_side3;
	smallBox_side3.AddPoint(b3);
	smallBox_side3.AddPoint(b4);
	smallBox_side3.AddPoint(b8);
	smallBox_side3.AddPoint(b7);
	smallBox_side3.SetReflectivity(refl_none);
	smallBox_side3.SetName("smallBox_side3");

	PndDrcSurfPolyFlat smallBox_side4;
	smallBox_side4.AddPoint(b1);
	smallBox_side4.AddPoint(b4);
	smallBox_side4.AddPoint(b8);
	smallBox_side4.AddPoint(b5);
	smallBox_side4.SetReflectivity(refl_none);
	smallBox_side4.SetName("smallBox_side4");

	PndDrcSurfPolyFlat smallBox_side5;
	smallBox_side5.AddPoint(b5);
	smallBox_side5.AddPoint(b6);
	smallBox_side5.AddPoint(b7);
	smallBox_side5.AddPoint(b8);
	smallBox_side5.SetName("smallBox_side5");

	PndDrcSurfPolyFlat smallBox_side6;
	smallBox_side6.AddPoint(b5);
	smallBox_side6.AddPoint(b6);
	smallBox_side6.AddPoint(b2);
	smallBox_side6.AddPoint(b1);
	smallBox_side6.SetReflectivity(refl_none);
	smallBox_side6.SetName("smallBox_side6");

//   smallBox_lens_sphere.SetReflectivity(refl_none);

	PndDrcOptVol smallBox;
	smallBox.SetVerbosity(0);
	smallBox.AddSurface(smallBox_side1a);
	smallBox.AddSurface(smallBox_side1b);
	smallBox.AddSurface(smallBox_side1c);
	smallBox.AddSurface(smallBox_side1d);

	smallBox.AddSurface(smallBox_lens_sphere);
	smallBox.AddSurface(smallBox_lens_side1);
	smallBox.AddSurface(smallBox_lens_side2);
	smallBox.AddSurface(smallBox_lens_side3);
	smallBox.AddSurface(smallBox_lens_side4);

	smallBox.AddSurface(smallBox_side2);
	smallBox.AddSurface(smallBox_side3);
	smallBox.AddSurface(smallBox_side4);
	smallBox.AddSurface(smallBox_side5);
	smallBox.AddSurface(smallBox_side6);
	smallBox.SetOptMaterial(vacuum);
	cout << "small box: vacuum" << endl;
	smallBox.SetName("smallBox");



// connect all objects
//==============================================================================
	PndDrcOptDevSys opt_system;
	opt_system.SetNameCopyNumber("opt_system");
	opt_system.SetVerbosity(0);
	opt_system.AddDevice(slab);
	opt_system.AddDevice(lens);
	opt_system.AddDevice(smallBox);
	opt_system.AddDevice(box);


    // couple surface 1 of device 1 with surface 2 of device 2
    //                       dev1   dev2    surf1     surf2

// 	opt_system.CoupleDevice("slab","smallBox","slab_side1","smallBox_side1");
// 	opt_system.CoupleDevice("slab","box","slab_side1","box_side1");

	opt_system.CoupleDevice("slab","lens","slab_side1", "lens_base");
	opt_system.CoupleDevice("lens","smallBox","lens_side1","smallBox_lens_side1");
	opt_system.CoupleDevice("lens","smallBox","lens_side2","smallBox_lens_side2");
	opt_system.CoupleDevice("lens","smallBox","lens_side3","smallBox_lens_side3");
	opt_system.CoupleDevice("lens","smallBox","lens_side4","smallBox_lens_side4");
	opt_system.CoupleDevice("lens","smallBox","lens_sphere","smallBox_lens_sphere");
	opt_system.CoupleDevice("smallBox","box","smallBox_side5","box_side1");


	PndDrcOptDevManager* manager = new PndDrcOptDevManager();
	manager->AddDeviceSystem(opt_system);



// create visual photon propagation in the system
//==============================================================================
	fstream geo;
	geo.open("Geo.C",std::ios::out);
	geo<<"{"<<endl;
	geo<<"    TCanvas *c1 = new TCanvas(\"c1\"); "<<endl;
	cout << endl << " root version: "<< int((TROOT*)gROOT -> GetVersionInt()) << endl << endl;
	if ( ((TROOT*)gROOT)->GetVersionInt() < 51600)
	{
		geo<<"    TView *view = new TView(1);"<<endl;
	}
	else
	{
		geo<<"    TView *view = TView::CreateView(1);"<<endl;
	}
	geo<<"    view->SetRange(-450,-450,-900,450,450,900);"<<endl;
	geo<<"    Int_t i;"<<endl;
	geo<<"    view->SetView(180,45,90,i);"<<endl;
	geo<<"    view->SetView(90,0,90,i);"<<endl;
	geo<<"    view->Zoom();"<<endl;
	geo<<"    view->Zoom();"<<endl;
	geo<<"    view->Zoom();"<<endl;
	geo<<"    view->Zoom();"<<endl;
	geo<<"    view->Zoom();"<<endl;
	geo<<"    view->Zoom();"<<endl;
	geo<<"    view->Zoom();"<<endl;

  // the following command sets a flag within the manager and all photons from
  // now on will be traced and can be plotted by calling within root
  // .x Geo.C
  // .x Screen.C

// 	manager->Print(geo); // switcher: comment out means no visual simualtion => faster



// photon propagation
//==============================================================================
	list<PndDrcPhoton> list_photon; // get list

	PndDrcPhoton ph;
	ph.SetReflectionLimit(200);

    // for option 3
    double inci = -666; // incidence angle
    double startX_center = -666; // hit pos. X on bar
    double startY_center = -666; // hit pos. Y on bar
    double startZ_center = -666; // hit pos. Z on bar


	double beta = 0;
	bool photons_exist = false;

	if (ioption==1)
	{
		for( int i=0; i<10000; i++)
		{
			ph.SetPosition(XYZPoint(0,0,-100));
			ph.SetDirection(XYZVector(0,0,1));
			ph.SetWavelength(lambda);
			list_photon.push_back(ph);
		}

		photons_exist = true;
		manager->SetPhotonList(list_photon,"slab","opt_system",0,0);


    // draw chosen photons (for Geo)
    //==========================================================================================
// 		list_photon = manager->PhotonList();
// 		list<PndDrcPhoton>::iterator iph;
//
// 		int icnt=0;
//
// 		for(iph=list_photon.begin(); iph != list_photon.end(); ++iph)
// 		{
// 			icnt++;
//
// 			if( icnt == 9680 )
// 				(*iph).SetPrintFlag(true);
// 			else
// 				(*iph).SetPrintFlag(false);
// 		}
//
// 		manager->SetPhotonList(list_photon,"slab","opt_system",0,0);

	}

	if (ioption==2)
	{
		for (double angle1=-30; angle1<30.5; angle1+=5.0)
		{
			for (double xx=-0.2*slab_width; xx<=0.21*slab_width; xx+=.40*slab_width)
			{
				double yy=0;
	      //for (double lambda1=330; lambda1<631; lambda1+=100)
				{
					double angle;
					double lambda1 = lambda;
					ph.SetPosition(XYZPoint(xx,yy,-500.5));

					angle = pi/180.0 * angle1;
					ph.SetDirection(XYZVector(sin(+angle),0,cos(+angle)));
					ph.SetWavelength(lambda1);
					list_photon.push_back(ph);


				}
			}
		}
		photons_exist = true;
		manager->SetPhotonList(list_photon,"slab","opt_system",0,0);
	}
	if (ioption==3)
	{
		double kinE = 2.3; // kinetic energy in GeV
		double mass_p = 0.938; // proton mass in GeV
		beta = sqrt( 1 - pow( mass_p / (kinE + mass_p) ,2 ) ); // E = T + E0 = gamma * E0

		cout << "beta = " << beta << endl;


		inci = 57; // in degree

        if( argc == 2)
          inci = atof( argv[1]);

		cout << "incidence angle of particles (theta): " << inci << " deg" << endl;


		double dirX = -1;
		double dirY = 0;
		double dirZ = tan(inci/180.*pi);

		XYZVector dir( dirX, dirY, dirZ ); // particle flight direction
		cout << "particle flight direction: (" << dirX << ", " << dirY << ", " << dirZ << ")" << endl;


        // hit pos. on bar is now independent of the incidence angle
        double startX = slab_width / 2; // slab_width / 2
        double startY = 0; // 0
        double startZ = -500; // -500

		XYZPoint  pos;
		if( argc == 5 )
		{
			startX = atof(argv[1]);
			startY = atof(argv[2]);
			startZ = atof(argv[3]);
			pos = XYZVector(startX,startY,startZ);
		}
		else
			pos = XYZVector(startX,startY,startZ);

        // center position of beamspot for the output filename
        startX_center = startX;
        startY_center = startY;
        startZ_center = startZ;

		cout << "hit pos: (" << startX << ", " << startY << ", " << startZ << ")" << endl;


		int photon_number = 100; // 100
// 		float     range = 1000;
// 		photons_exist = manager->Cerenkov(pos,dir,beta,photon_number,range);



    // beamSpot simulation
    //==========================================================================================
        double limit = 50; // beam spot radius limit in mm
        double x_shift = cos((90-inci)/180.*pi) * limit;
        double z_shift = tan(inci/180.*pi) * x_shift;
        startX += x_shift;
        startZ -= z_shift;
        double z_shift2 = sin((90-inci)/180.*pi) * limit;
        TVector3 shift( startX + x_shift, startY, startZ + z_shift2 );
        TVector3 endBar( slab_width / 2, startY, 0 );
        double maxrange = (shift - endBar).Mag();
        cout << "maximum range (to generate photons only in bar): " << maxrange << endl;

        TVector3 z = TVector3(0,0,1);
		TVector3 ortho;
		ortho = z.Orthogonal();
		TRandom3 rand;
		TVector3 helper;
		TVector3 helper2;
		double gausSmear; // gaus smearing
		double transX = startX; // for the translation-(shift) (see above)
		double transY = startY;
		double transZ = startZ; // -200 ???
		double spotX;
		double spotY;
		double spotZ;
		XYZPoint spotPos;

        double radius = 20; // radius in mm is 1 sigma of the gaus smearing
        cout << "beam spot radius (1-sigma): " << radius << endl << endl;



//         fstream beamspot;
//         beamspot.open("beamspot_end.C",std::ios::out);
//
//         beamspot << "{" << endl;
//         beamspot << "gStyle->SetCanvasColor(0);"       << endl;
//         beamspot << "gStyle->SetCanvasBorderMode(0);"  << endl;
//         beamspot << "gStyle->SetFrameBorderMode(0);"   << endl;
//         beamspot << "gStyle->SetTitleFillColor(0);"    << endl;
//         beamspot << "gStyle->SetTitleFontSize(0.05);"  << endl;
//         beamspot << "TCanvas *c1 = new TCanvas( \"c1\", \"\" ,200, 10, 700, 500 );" << endl;
//         beamspot << "TString title=\"y-z plane [mm] \";"<< endl;
//         beamspot << "TH1F *hgr = new TH1F(\"hgr1\",title,100,-600,-400);"<<endl;
//         beamspot << "hgr->SetStats( 0 );" << endl;
//         beamspot << "hgr->SetMarkerStyle(7);"<<endl;
//         beamspot << "hgr->SetMarkerSize(1);"<<endl;
//         beamspot << "hgr->SetMinimum(-100);"<<endl;
//         beamspot << "hgr->SetMaximum(+100);"<<endl;
//         beamspot << "hgr->Draw(\"POL\");"<<endl << endl;
//
//         beamspot << "TLine *l1 = new TLine(-600,-17.5,-400,-17.5);"    << endl;
//         beamspot << "TLine *l2 = new TLine(-600,17.5,-400,17.5);"     << endl;
//         beamspot << "l1->SetLineWidth(3);" << endl;
//         beamspot << "l2->SetLineWidth(3);" << endl;
//         beamspot << "l1->Draw(\"same\");" << endl;
//         beamspot << "l2->Draw(\"same\");" << endl;


		for(int i=0; i<300; i++) // number of particles (300)
		{
			helper=ortho; // (-1,0,0) is the orthogonal of (0,0,1)
			helper.RotateZ(rand.Rndm()*2*pi);
			gausSmear=rand.Gaus(0,radius);
            if( fabs(gausSmear) > limit ) // beam spot limit
              continue;
			helper2 = TVector3(helper.X()*gausSmear, helper.Y()*gausSmear, helper.Z()*gausSmear);
			helper2.RotateY( (-90+inci)/180.*pi ); // rotation in the right direction
			spotX = helper2.X()+transX; // translation-(shift)
			spotY = helper2.Y()+transY;
			spotZ = helper2.Z()+transZ;
			spotPos = XYZVector(spotX,spotY,spotZ);
            manager->Cerenkov(spotPos,dir,beta,photon_number,maxrange); // maxrange instead of range


            if( spotX < slab_width / 2)
            {
              cout << "particle origin production is in the bar !!!" << endl;
              cout << spotX << " " << spotY << " " << spotZ << endl;
              cout << x_shift << " " << z_shift << endl << endl;
            }

            if( spotZ / cos((90-inci)/180.*pi) > maxrange )
            {
              cout << "particle trajectory can cross the lens and/or the fishtank" << endl;
              cout << spotX << " " << spotY << " " << spotZ << endl;
              cout << x_shift << " " << z_shift << endl << endl;
            }


//             cout << spotX << " " << spotY << " " << spotZ << endl;

//             beamspot << "TMarker* t = new TMarker("<<  spotZ + stepsToBar * dirZ <<","<< spotY + stepsToBar * dirY <<",20);"<<endl;
//             if( spotY + stepsToBar * dirY >= 17.5 || spotY + stepsToBar * dirY <= -17.5)
//               beamspot << "t->SetMarkerColor(4);" << endl;
//             else
//               beamspot << "t->SetMarkerColor(2);" << endl;
//             beamspot << "t->SetMarkerStyle(20);"<<endl;
//             beamspot << "t->SetMarkerSize(1);"<<endl;
//             beamspot << "t->Draw();"<<endl;

		}
		photons_exist = true; // maybe it is needed (if last particle in the loop don't hit the bar, this value will be set as false)
//         beamspot << "}" << endl;

    // draw chosen photons (for Geo)
    //==========================================================================================
// 		list_photon = manager->PhotonList();
// 		list<PndDrcPhoton>::iterator iph;
//
// 		int icnt = 0;
//
// 		for(iph=list_photon.begin(); iph != list_photon.end(); ++iph)
// 		{
// 			icnt++;
//
// 			if( icnt == 70 )
// 				(*iph).SetPrintFlag(true);
// 			else
// 				(*iph).SetPrintFlag(false);
// 		}
//
// 		manager->SetPhotonList(list_photon,"slab","opt_system",0,0);

	}
	if (ioption==4)
	{
		TRandom ran;
		for (int ii=0; ii<30; ii++)
		{
			ph.SetPosition(XYZPoint(0,0,dist-20));
			double phi =   ran.Uniform(0,2*pi);
			double costh = ran.Uniform(-1,1);
			double sinth = sqrt(1-costh*costh);
			ph.SetDirection(XYZVector(sinth*sin(phi),sinth*cos(phi),costh));
			ph.SetWavelength(lambda);
			list_photon.push_back(ph);
		}

		photons_exist = true;
		manager->SetPhotonList(list_photon,"slab","opt_system",0,0);
	}
	if (ioption==5)
	{
		double angle = pi/180.0 * 0;
		ph.SetDirection(XYZVector(0,sin(+angle),cos(+angle)));

		ph.SetPosition(XYZPoint(0,5,-5));
		ph.SetWavelength(400);
		list_photon.push_back(ph);
		ph.SetPosition(XYZPoint(0,-5,-5));
		ph.SetWavelength(400);
		list_photon.push_back(ph);


		ph.SetPosition(XYZPoint(0,5,-5));
		ph.SetWavelength(600);
		list_photon.push_back(ph);
		ph.SetPosition(XYZPoint(0,-5,-5));
		ph.SetWavelength(600);
		list_photon.push_back(ph);



		photons_exist = true;
		manager->SetPhotonList(list_photon,"slab","opt_system",0,0);
	}
	if (ioption==6)
	{
      // straight lines.
		TRandom ran;
		for (double angle=5; angle<=40.5; angle+=5)
		{

			cout<<" angle = "<<angle<<endl;
			for (double lambda1=630; lambda1>329; lambda1-=100)
			{
	      //double lambda1=lambda;
	      // go in x dir
				double y1 = tan(angle*pi/180);
				double x1 = y1;
				double scale = 3/angle;
				for (double y=-y1; y<= y1; y+= 2*y1/3*scale)
				{
					double xx=ran.Uniform(-0.5*slab_width,0.5*slab_width);
					double yy=ran.Uniform(-0.5*slab_height,0.5*slab_height);
					ph.SetPosition(XYZPoint(xx,yy,-0.5));
					double x = x1;
					ph.SetDirection(XYZVector(x,y,1).Unit());
					ph.SetWavelength(lambda1);
					list_photon.push_back(ph);
					ph.SetDirection(XYZVector(-x,y,1).Unit());
					ph.SetWavelength(lambda1);
					list_photon.push_back(ph);
				}

				for (double x=-x1; x<= x1; x+= 2*y1/3*scale)
				{
					double xx=ran.Uniform(-0.5*slab_width,0.5*slab_width);
					double yy=ran.Uniform(-0.5*slab_height,0.5*slab_height);
					ph.SetPosition(XYZPoint(xx,yy,-0.5));
					double y = y1;
					ph.SetDirection(XYZVector(x,y,1).Unit());
					ph.SetWavelength(lambda1);
					list_photon.push_back(ph);
					ph.SetDirection(XYZVector(x,-y,1).Unit());
					ph.SetWavelength(lambda1);
					list_photon.push_back(ph);
				}

			}
		}
		photons_exist = true;
		manager->SetPhotonList(list_photon,"slab","opt_system");
	}

	if (photons_exist)
	{
		manager->Propagate();                           // propagate photons
	}

	list_photon = manager->PhotonList(); // get list




	geo<<"}"<<endl;
	//
  // the intention is to play around with routines.

  // create a list of photons in bar
  //exit(1);
	geo.close();

  // write to screen...


	TString  outFile_str;
	if( argc >= 2 )
	{
        TString inci_str;
        TString startX_str;
        TString startY_str;
        TString startZ_str;

        inci_str += inci;
        inci_str.Remove( TString::kLeading , ' ' );
        startX_str += startX_center;
        startX_str.Remove( TString::kLeading , ' ' );
        startY_str += startY_center;
        startY_str.Remove( TString::kLeading , ' ' );
        startZ_str += startZ_center;
        startZ_str.Remove( TString::kLeading , ' ' );

        outFile_str = "Screen_" + inci_str + "deg_" + startX_str + "_" + startY_str + "_" + startZ_str + ".C" ;
	}
	else
		outFile_str = "Screen.C";

    cout << "FILE: " << outFile_str << endl;


	fstream scr;
	scr.open(outFile_str,std::ios::out);

	scr << "{" << endl;
	scr << "gStyle->SetCanvasColor(0);"       << endl;
	scr << "gStyle->SetCanvasBorderMode(0);"  << endl;
	scr << "gStyle->SetFrameBorderMode(0);"   << endl;
	scr << "gStyle->SetTitleFillColor(0);"    << endl;
	scr << "gStyle->SetTitleFontSize(0.05);"  << endl;
	scr << "TCanvas *c1 = new TCanvas( \"c1\", \"\" ,200, 10, 700, 500 );" << endl;

	TString str_beta;
	beta = floor( beta * pow(10 ,4) + 0.5 ) * pow(10, -4); // round beta to 4 digits (after point)
	str_beta+=beta;
	str_beta.Remove(TString::kLeading,' ');

	scr << "TString title;" << endl;
//     scr << "title=\"spatial position [mm] (beta=" << str_beta << ", #lambda =435nm)\";"<< endl;
	scr << "title=\"spatial position [mm] (beta=" << str_beta << ")\";" << endl;
    scr << "TH1F *hgr = new TH1F(\"hgr1\",title,600,-150,150);" << endl;
// 	scr << "TH1F *hgr = new TH1F(\"hgr1\",title,600,-400,400);" << endl;
	scr << "hgr->SetStats( 0 );"     << endl;
	scr << "hgr->SetMarkerStyle(7);" << endl;
	scr << "hgr->SetMarkerSize(0.5);"<< endl;
// 	scr << "hgr->SetMinimum(-400);"  << endl;
// 	scr << "hgr->SetMaximum(+400);"  << endl;
    scr << "hgr->SetMinimum(-100);"  << endl;
    scr << "hgr->SetMaximum(+100);"  << endl;
	scr << "hgr->Draw(\"POL\");"     << endl << endl;

    int icnt_measured = 0;
	int icnt_flying   = 0;
	int icnt_lost     = 0;
	int icnt_absorbed = 0;


	list<PndDrcPhoton>::iterator iph;
	PndDrcPhoton ph_old;


	fstream measured_dat;
	fstream absorbed_dat;
    fstream lost_dat;
	fstream measured_wave_dat;
	fstream absorbed_wave_dat;
    fstream lost_wave_dat;

	measured_dat.open("measured.tmp",std::ios::out);
	absorbed_dat.open("absorbed.tmp",std::ios::out);
    lost_dat.open("lost.tmp",std::ios::out);
	measured_wave_dat.open("measured_wave.tmp",std::ios::out);
	absorbed_wave_dat.open("absorbed_wave.tmp",std::ios::out);
    lost_wave_dat.open("lost_wave.tmp",std::ios::out);

	int n_iph = 0;

	for(iph=list_photon.begin(); iph != list_photon.end(); ++iph)
	{
		n_iph++;
		double wavi = (*iph).Wavelength();

		if ((*iph).Fate()==Drc::kPhotMeasured)
		{
			icnt_measured++;

			measured_dat << n_iph << endl;
			measured_wave_dat << wavi << endl;


			XYZPoint pos = (*iph).Position();
			double x     = pos.X();
			double y     = pos.Y();

//       if( x <= 150 && x >= -150 && y <= 100 && y >= -100 ){
			scr<<"    TMarker* t = new TMarker("<<x<<","<<y<<",20);"<<endl;
			scr<<"    t->SetMarkerStyle(7);"<<endl;
			scr<<"    t->SetMarkerColor("<<(*iph).ColorNumber((*iph).Wavelength())<<");"<<endl;
			scr<<"    t->SetMarkerSize(0.7);"<<endl;
			scr<<"    t->Draw();"<<endl;
//       }
			ph_old = (*iph);

		}
		else if ((*iph).Fate()==Drc::kPhotFlying)   icnt_flying++; // should never happen.
		else if ((*iph).Fate()==Drc::kPhotAbsorbed)
		{
			icnt_absorbed++;

			absorbed_dat << n_iph << endl;
			absorbed_wave_dat << wavi << endl;
		}
		else
		{
			icnt_lost++;

            lost_dat << n_iph << endl;
            lost_wave_dat << wavi << endl;
		}
	}

	measured_dat.close();
	absorbed_dat.close();
    lost_dat.close();
	measured_wave_dat.close();
	absorbed_wave_dat.close();
    lost_wave_dat.close();


    // MCP position
	scr << endl;
	scr << "TLine *l1 = new TLine(-19.125,-13.625,-70.125,-13.625);"    << endl;
	scr << "TLine *l2 = new TLine(-70.125,-13.625,-70.125,37.375);"     << endl;
	scr << "TLine *l3 = new TLine(-70.125,37.375,-19.125,37.375);"      << endl;
	scr << "TLine *l4 = new TLine(-19.125,37.375,-19.125,-13.625);"     << endl;
	scr << "l1->SetLineWidth(3);" << endl;
	scr << "l2->SetLineWidth(3);" << endl;
	scr << "l3->SetLineWidth(3);" << endl;
	scr << "l4->SetLineWidth(3);" << endl;
	scr << "l1->Draw(\"same\");" << endl;
	scr << "l2->Draw(\"same\");" << endl;
	scr << "l3->Draw(\"same\");" << endl;
	scr << "l4->Draw(\"same\");" << endl;
	scr << endl;
	scr << "TLine *l5 = new TLine(19.125,-13.625,70.125,-13.625);"  << endl;
	scr << "TLine *l6 = new TLine(70.125,-13.625,70.125,37.375);"   << endl;
	scr << "TLine *l7 = new TLine(70.125,37.375,19.125,37.375);"    << endl;
	scr << "TLine *l8 = new TLine(19.125,37.375,19.125,-13.625);"   << endl;
	scr << "l5->SetLineWidth(3);" << endl;
	scr << "l6->SetLineWidth(3);" << endl;
	scr << "l7->SetLineWidth(3);" << endl;
	scr << "l8->SetLineWidth(3);" << endl;
	scr << "l5->Draw(\"same\");" << endl;
	scr << "l6->Draw(\"same\");" << endl;
	scr << "l7->Draw(\"same\");" << endl;
	scr << "l8->Draw(\"same\");" << endl;


	int icnt = icnt_measured+icnt_flying+icnt_lost+icnt_absorbed;
	cout << endl << endl;
	cout<<" generated photons: "<<icnt<<endl;
	cout<<" measured  photons: "<<icnt_measured<<endl;
	cout<<" absorbed  photons: "<<icnt_absorbed<<endl;
	cout<<" lost      photons: "<<icnt_lost<<endl<<endl;


    scr << endl;
	scr << "TPaveText *stat = new TPaveText(0.8,0.88,0.98,0.98,\"brNDC\");" << endl;

	TString str_icnt;
	str_icnt+=icnt;
	str_icnt.Remove(TString::kLeading,' ');
	TString str_icnt_det;
	int det = icnt - icnt_lost - icnt_absorbed;
	str_icnt_det+=det;
	str_icnt_det.Remove(TString::kLeading,' ');

    scr << "stat->AddText(\"gen.: " << str_icnt << "\");" << endl;
	scr << "stat->AddText(\"det.: " << str_icnt_det << "\");" << endl;
	scr << "stat->SetFillColor(0);" << endl;
	scr << "stat->Draw();" << endl;
	scr<<"}"<<endl;
	scr.close();


	delete manager;

	return EXIT_SUCCESS;
}
