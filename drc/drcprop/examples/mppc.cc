
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

//#include <cmath>

#include "TROOT.h"
#include "TRint.h"
#include "TVector3.h"
#include "TRandom.h"
#include "TRandom3.h"
#include "TRotation.h"
#include "TPaveText.h"
#include "TString.h"
#include "TMatrixD.h"

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


#include "PndDrcSurfAbs.h"
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
#include "PndDrcOptMatNLAK33A.h"
#include "PndDrcOptMatLLF1.h"
#include "PndDrcOptMatBK7.h"
#include "PndDrcOptMatVacuum.h"
#include "PndDrcOptMatAl.h"
#include "PndDrcOptMatConstIndex.h"
#include "PndDrcOptDevSys.h"
#include "PndDrcOptVol.h"
#include "PndDrcOptDevManager.h"
#include "PndDrcUtil.h"

#include <cstdlib>


int main(int argc, char *argv[])
{
//     double conical_const =  0; //sphere
//     double conical_const = 0.5; //parabola

    int ioption = 1; // parallel beam (0 deg)
  //int ioption = 2; // 0,  +- 20,  +-40 deg
//     int ioption = 3; // C-cone
  //int ioption = 4; // random in front of lens.
  //int ioption = 5; // single photon for debugging.
  //int ioption = 6; // grid 5 deg

    double pi=3.1415926535;

    PndDrcOptReflPerfect refl_perfect;
    PndDrcOptReflNone    refl_none;

    double slab_width  = 17;
    double slab_height = 35;

    double curLensIndex = 1.5267; // BK7 at 435nm
    double curOilIndex  = 1.8;
    double curResinIndex= 1.63; // Epoxy resin at 435nm


    PndDrcOptMatLithotecQ0  quartz;
    PndDrcOptMatVacuum      vacuum;
    PndDrcOptMatNLAK33A     nlak33a;
    PndDrcOptMatLLF1        llf1;
    PndDrcOptMatBK7         bk7;
    PndDrcOptMatAl          alu;
    PndDrcOptMatConstIndex  lensIndex(curLensIndex);
    PndDrcOptMatConstIndex  oilIndex(curOilIndex);
    PndDrcOptMatConstIndex  resinIndex(curResinIndex);

    double lambda = 435;
    double dist = -666; // test



//     double wavelength[4];
//   wavelength[0]=200;
//     wavelength[0]=300;
//     wavelength[1]=435;
//     wavelength[2]=535;
//     wavelength[3]=589;
//     for( int i=0; i < 4; i++ )
//     {
//         cout << "n_quartz    (" << wavelength[i] << " nm) = " << quartz.RefIndex(wavelength[i])     << endl;
//         cout << "n_vacuum    (" << wavelength[i] << " nm) = " << vacuum.RefIndex(wavelength[i])     << endl;
//         cout << "n_nlak33a   (" << wavelength[i] << " nm) = " << nlak33a.RefIndex(wavelength[i])    << endl;
//         cout << "n_llf1      (" << wavelength[i] << " nm) = " << llf1.RefIndex(wavelength[i])       << endl;
//         cout << "n_bk7       (" << wavelength[i] << " nm) = " << bk7.RefIndex(wavelength[i])        << endl;
//         cout << "n_aluminium    (" << wavelength[i] << " nm) = " << alu.Extinction(wavelength[i])   << endl;
//         cout << "n_lensIndex (" << wavelength[i] << " nm) = " << lensIndex.RefIndex(wavelength[i])  << endl;
//         cout << "n_oilIndex  (" << wavelength[i] << " nm) = " << oilIndex.RefIndex(wavelength[i])   << endl << endl;
//     }
    //


  // Note: wrong x-orientation for the points to declare a surface but only surfaces were coupled later
  //       therefore it should be no problem



// lens
//==============================================================================
//     XYZPoint q0(-slab_width/2, +slab_height/2, 0);
//     XYZPoint q1(-slab_width/2, -slab_height/2, 0);
//     XYZPoint q2(+slab_width/2, -slab_height/2, 0);
//     XYZPoint q3(+slab_width/2, +slab_height/2, 0);
    //
//     PndDrcSurfPolyAsphere lens_sphere;
//     lens_sphere.SetVerbosity(0);
//     lens_sphere.AddPoint(q0);
//     lens_sphere.AddPoint(q1);
//     lens_sphere.AddPoint(q2);
//     lens_sphere.AddPoint(q3);
//     lens_sphere.SetRadius(77.52); // f = R/(n-1) prev R: 75mm (BK7 Newport f=150 R=77.52 at 589nm)
//     lens_sphere.SetPrintColor(2);
//     lens_sphere.SetConicalConstant(conical_const);
//     lens_sphere.SetName("lens_sphere");
//     double     dist = lens_sphere.CenterPoint().Z();
// //   cout << "dist = " << dist << endl;
//     lens_sphere.AddTransform(Transform3D(XYZVector(0,0,-(dist)))); // => )|


//   cout << "Transform3D:" << endl;
//   TMatrixD m(3,4);
//   Transform3D transi = Transform3D(XYZVector(0,0,-(dist)));
//   Transform3D transid;
//   Transform3D addtrans = transi * transid;
//   Transform3D transInv = addtrans.Inverse();
//   transInv.GetComponents(m.GetMatrixArray());
//   for(int i=0;i<3;i++)
//   {
//       for(int j=0;j<4;j++)
//       {
//           cout << m[i][j] << " ";
//       }
//       cout << endl;
//   }
//   cout << endl;


//     double lensBase = 5; // roughly measured lens thickness: 5 ; old: 3
//     cout << "lens thickness should be greater than: |" << lens_sphere.LimitingPoint(0).Z() << "|" << endl;
    //
//     q0+=XYZVector(0,0,-lensBase);//old:1.5, new:-3
//     q1+=XYZVector(0,0,-lensBase);
//     q2+=XYZVector(0,0,-lensBase);
//     q3+=XYZVector(0,0,-lensBase);
    //
//     PndDrcSurfPolyFlat lens_base;
//     lens_base.SetVerbosity(0);
//     lens_base.AddPoint(q0);
//     lens_base.AddPoint(q1);
//     lens_base.AddPoint(q2);
//     lens_base.AddPoint(q3);
//     lens_base.SetPrintColor(2);
//     lens_base.SetName("lens_base");
    //
//     PndDrcSurfQuadFlatDiff lens_side1;
//     lens_side1.AddSurface(lens_base,  q0,q1);
//     lens_side1.AddSurface(lens_sphere,
//                           lens_sphere.LimitingPoint(0),lens_sphere.LimitingPoint(1));
//     lens_side1.SetPrintColor(2);
//     lens_side1.SetName("lens_side1");
//     PndDrcSurfQuadFlatDiff lens_side2;
//     lens_side2.SetVerbosity(0);
//     lens_side2.AddSurface(lens_base,  q1,q2);
//     lens_side2.AddSurface(lens_sphere,
//                           lens_sphere.LimitingPoint(1),lens_sphere.LimitingPoint(2));
//     lens_side2.SetPrintColor(2);
//     lens_side2.SetName("lens_side2");
//     PndDrcSurfQuadFlatDiff lens_side3;
//     lens_side3.AddSurface(lens_base,  q2,q3);
//     lens_side3.AddSurface(lens_sphere,
//                           lens_sphere.LimitingPoint(2),lens_sphere.LimitingPoint(3));
//     lens_side3.SetPrintColor(2);
//     lens_side3.SetName("lens_side3");
//     PndDrcSurfQuadFlatDiff lens_side4;
//     lens_side4.AddSurface(lens_base,  q3,q0);
//     lens_side4.AddSurface(lens_sphere,
//                           lens_sphere.LimitingPoint(3),lens_sphere.LimitingPoint(0));
//     lens_side4.SetPrintColor(2);
//     lens_side4.SetName("lens_side4");
    //
    //
// //   cout << "LimitingPoint:" << endl;
// //     cout << lens_sphere.LimitingPoint(0) << endl;
// //   cout << lens_sphere.LimitingPoint(1) << endl;
// //   cout << lens_sphere.LimitingPoint(2) << endl;
// //   cout << lens_sphere.LimitingPoint(3) << endl << endl;
    //
    //
// //   lens_side1.SetReflectivity(refl_none);
// //   lens_side2.SetReflectivity(refl_none);
// //   lens_side3.SetReflectivity(refl_none);
// //   lens_side4.SetReflectivity(refl_none);
    //
    //
//     PndDrcOptVol lens;
//     lens.SetVerbosity(0);
//     lens.AddSurface(lens_base);
//     lens.AddSurface(lens_sphere);
//     lens.AddSurface(lens_side1);
//     lens.AddSurface(lens_side2);
//     lens.AddSurface(lens_side3);
//     lens.AddSurface(lens_side4);
//     lens.SetOptMaterial(bk7);
//     cout << "lens: bk7" << endl;
// //     cout << "lens: " << curLensIndex << endl;
//     lens.SetName("lens");
    //
//     double lens_shift = lensBase; // lensBase
// //   if( -lens_sphere.LimitingPoint(0).Z() > lens_shift )
// //       cout << " *** need a bigger lens shift" << endl;
//     lens.AddTransform(Transform3D(XYZVector(0,0,lens_shift)));



// box (fishtank) with oil
//==============================================================================
//     double vacBoxwidth = 10;// old: 4.5 ; new: 10
//     if( lensBase > vacBoxwidth )
//         cout << " *** no air gap possible" << endl;
    //
    //
//     //                                                          screen
//     XYZPoint p1(-300.0, +300.0, vacBoxwidth);  //            p5----------p8
//     XYZPoint p2(-300.0, -300.0, vacBoxwidth);  //           /|          /|
//     XYZPoint p3(+300.0, -300.0, vacBoxwidth);  //          / |         / |
//     XYZPoint p4(+300.0, +300.0, vacBoxwidth);  //         /  p6-------/--p7
    //     //                                                   /  /        /  /
    //     //                                                  /  /        /  /
    //     //                                                 /  /        /  /
    //     //                                                /  /        /  /
    //     XYZPoint p5(-300.0, +300.0, +230); //           p1--s1--s2--p4  /
    //     XYZPoint p6(-300.0, -300.0, +230); //           |  /s5--s6  |  /
    //     XYZPoint p7(+300.0, -300.0, +230); //           | / s7--s8  | /
//     XYZPoint p8(+300.0, +300.0, +230); //           p2--s3--s4--p3
    //
//     PndDrcSurfPolyFlat box_side1;
//     box_side1.AddPoint(p1);
//     box_side1.AddPoint(p2);
//     box_side1.AddPoint(p3);
//     box_side1.AddPoint(p4);
// //   box_side1.SetReflectivity(refl_none);
//     box_side1.SetName("box_side1");
//     PndDrcSurfPolyFlat box_side2;
//     box_side2.AddPoint(p2);
//     box_side2.AddPoint(p3);
//     box_side2.AddPoint(p7);
//     box_side2.AddPoint(p6);
//     box_side2.SetReflectivity(refl_none);
//     box_side2.SetName("box_side2");
//     PndDrcSurfPolyFlat box_side3;
//     box_side3.AddPoint(p3);
//     box_side3.AddPoint(p4);
//     box_side3.AddPoint(p8);
//     box_side3.AddPoint(p7);
//     box_side3.SetReflectivity(refl_none);
//     box_side3.SetName("box_side3");
//     PndDrcSurfPolyFlat box_side4;
//     box_side4.AddPoint(p1);
//     box_side4.AddPoint(p4);
//     box_side4.AddPoint(p8);
//     box_side4.AddPoint(p5);
//     box_side4.SetReflectivity(refl_none);
//     box_side4.SetName("box_side4");
//     PndDrcSurfPolyFlat box_side5;
//     box_side5.AddPoint(p5);
//     box_side5.AddPoint(p6);
//     box_side5.AddPoint(p7);
//     box_side5.AddPoint(p8);
//     box_side5.SetPixel();
//     box_side5.SetName("box_side5");
//     PndDrcSurfPolyFlat box_side6;
//     box_side6.AddPoint(p5);
//     box_side6.AddPoint(p6);
//     box_side6.AddPoint(p2);
//     box_side6.AddPoint(p1);
//   //   box_side6.SetPixel(); // ???
//     box_side6.SetReflectivity(refl_none);
//     box_side6.SetName("box_side6");
    //
    //
//     PndDrcOptVol box;
//     box.SetVerbosity(0);
//     box.AddSurface(box_side1);
//     box.AddSurface(box_side2);
//     box.AddSurface(box_side3);
//     box.AddSurface(box_side4);
//     box.AddSurface(box_side5);
//     box.AddSurface(box_side6);
//     box.SetOptMaterial(quartz);
//     cout << "box (oil): quartz" << endl;
// //     cout << "box (oil): " << curOilIndex << endl;
//     box.SetName("box");
    //
    //
    //
// // quartz bar (slab)
// //==============================================================================
//     XYZPoint s5(-slab_width/2, +slab_height/2, 0);
//     XYZPoint s6(+slab_width/2, +slab_height/2, 0);
//     XYZPoint s7(-slab_width/2, -slab_height/2, 0);
//     XYZPoint s8(+slab_width/2, -slab_height/2, 0);
//     XYZPoint t5(-slab_width/2, +slab_height/2, -800);
//     XYZPoint t6(+slab_width/2, +slab_height/2, -800);
//     XYZPoint t7(-slab_width/2, -slab_height/2, -800);
//     XYZPoint t8(+slab_width/2, -slab_height/2, -800);
//   //s 5 6 at dist-1
//   //  7 8
    //     //
//   //t 5 6 at dist-1-1000
//   //  7 8
    //     //
//     PndDrcSurfPolyFlat slab_side1;
//     slab_side1.SetVerbosity(0);
//     slab_side1.AddPoint(s5); // face
//     slab_side1.AddPoint(s6);
//     slab_side1.AddPoint(s8);
//     slab_side1.AddPoint(s7);
//     slab_side1.SetName("slab_side1");
//     PndDrcSurfPolyFlat slab_side2;
//     slab_side2.AddPoint(s5);//left
//     slab_side2.AddPoint(s7);
//     slab_side2.AddPoint(t7);
//     slab_side2.AddPoint(t5);
//     slab_side2.SetName("slab_side2");
//     PndDrcSurfPolyFlat slab_side3;
//     slab_side3.AddPoint(s7);//down
//     slab_side3.AddPoint(s8);
//     slab_side3.AddPoint(t8);
//     slab_side3.AddPoint(t7);
//     slab_side3.SetName("slab_side3");
//     PndDrcSurfPolyFlat slab_side4;
//     slab_side4.AddPoint(s6);//right
//     slab_side4.AddPoint(s8);
//     slab_side4.AddPoint(t8);
//     slab_side4.AddPoint(t6);
//     slab_side4.SetName("slab_side4");
//     PndDrcSurfPolyFlat slab_side5;
//     slab_side5.AddPoint(s5);//top
//     slab_side5.AddPoint(s6);
//     slab_side5.AddPoint(t6);
//     slab_side5.AddPoint(t5);
//     slab_side5.SetName("slab_side5");
//     PndDrcSurfPolyFlat slab_side6;
//     slab_side6.AddPoint(t5); // face
//     slab_side6.AddPoint(t6);
//     slab_side6.AddPoint(t8);
//     slab_side6.AddPoint(t7);
//     slab_side6.SetName("slab_side6");
    //
//   //slab_side1.SetReflectivity(refl_perfect);
//   //slab_side2.SetReflectivity(refl_perfect);
//   //slab_side3.SetReflectivity(refl_perfect);
//   //slab_side4.SetReflectivity(refl_perfect);
//   //slab_side5.SetReflectivity(refl_perfect);
//   //slab_side6.SetReflectivity(refl_perfect);
    //
//     PndDrcOptVol slab;
//   //slab.SetVerbosity(5);
//     slab.AddSurface(slab_side1);
//     slab.AddSurface(slab_side2);
//     slab.AddSurface(slab_side3);
//     slab.AddSurface(slab_side4);
//     slab.AddSurface(slab_side5);
//     slab.AddSurface(slab_side6);
//     slab.SetOptMaterial(quartz);
//     cout << "slab: quartz" << endl;
//     slab.SetName("slab");


// light catcher for a Si-PMT
//==============================================================================

    XYZPoint c5(-3.25, -3.25, 0);          //  c5------------c8
    XYZPoint c6(-3.25, +3.25, 0);          //   \           /
    XYZPoint c7(+3.25, +3.25, 0);          //    \         /
    XYZPoint c8(+3.25, -3.25, 0);          //    c1------c4

    if(argc == 1)
    {
        cout << "*****  first parameter defines the height of the light catcher  *****" << endl;
        return 0;
    }

    double catcher_height = atof(argv[1]); // light catcher height

    if(catcher_height == 0) // catch characters as false enter
    {
        cout << "*****  parameter \"height\" have to be greater than zero  *****" << endl;
        return 0;
    }

    XYZPoint c1(-1.5, -1.5, -catcher_height);
    XYZPoint c2(-1.5, +1.5, -catcher_height);
    XYZPoint c3(+1.5, +1.5, -catcher_height);
    XYZPoint c4(+1.5, -1.5, -catcher_height);


    PndDrcSurfPolyFlat catcher_detector;
    catcher_detector.AddPoint(c1);
    catcher_detector.AddPoint(c2);
    catcher_detector.AddPoint(c3);
    catcher_detector.AddPoint(c4);
//     catcher_detector.SetPixel();
//     catcher_detector.SetReflectivity(refl_none);
    catcher_detector.SetName("catcher_detector");
    PndDrcSurfPolyFlat catcher_side1;
    catcher_side1.AddPoint(c4);
    catcher_side1.AddPoint(c8);
    catcher_side1.AddPoint(c5);
    catcher_side1.AddPoint(c1);
//     catcher_side1.SetReflectivity(refl_perfect);
    catcher_side1.SetPrintColor(2);
    catcher_side1.SetName("catcher_side1");
    PndDrcSurfPolyFlat catcher_side2;
    catcher_side2.AddPoint(c1);
    catcher_side2.AddPoint(c5);
    catcher_side2.AddPoint(c6);
    catcher_side2.AddPoint(c2);
//     catcher_side2.SetReflectivity(refl_perfect);
    catcher_side2.SetPrintColor(2);
    catcher_side2.SetName("catcher_side2");
    PndDrcSurfPolyFlat catcher_side3;
    catcher_side3.AddPoint(c2);
    catcher_side3.AddPoint(c6);
    catcher_side3.AddPoint(c7);
    catcher_side3.AddPoint(c3);
//     catcher_side3.SetReflectivity(refl_perfect);
    catcher_side3.SetPrintColor(2);
    catcher_side3.SetName("catcher_side3");
    PndDrcSurfPolyFlat catcher_side4;
    catcher_side4.AddPoint(c3);
    catcher_side4.AddPoint(c7);
    catcher_side4.AddPoint(c8);
    catcher_side4.AddPoint(c4);
//     catcher_side4.SetReflectivity(refl_perfect);
    catcher_side4.SetPrintColor(2);
    catcher_side4.SetName("catcher_side4");
    PndDrcSurfPolyFlat catcher_side5;
    catcher_side5.AddPoint(c5);
    catcher_side5.AddPoint(c6);
    catcher_side5.AddPoint(c7);
    catcher_side5.AddPoint(c8);
    catcher_side5.SetName("catcher_side5");
    catcher_side5.SetPrintColor(2);


    PndDrcOptVol catcher;
    catcher.SetVerbosity(0);
    catcher.AddSurface(catcher_detector);
    catcher.AddSurface(catcher_side1);
    catcher.AddSurface(catcher_side2);
    catcher.AddSurface(catcher_side3);
    catcher.AddSurface(catcher_side4);
    catcher.AddSurface(catcher_side5);
    catcher.SetOptMaterial(vacuum);
    cout << "catcher: vacuum" << endl;
    catcher.SetName("catcher");

// alu catcher
//==============================================================================
    XYZPoint h1(-3.25, -3.25, -catcher_height);
    XYZPoint h2(-3.25, +3.25, -catcher_height);
    XYZPoint h3(+3.25, +3.25, -catcher_height);
    XYZPoint h4(+3.25, -3.25, -catcher_height);

    //#1
    PndDrcSurfPolyFlat catcherAlu1_side1;
    catcherAlu1_side1.AddPoint(c4);
    catcherAlu1_side1.AddPoint(c8);
    catcherAlu1_side1.AddPoint(c5);
    catcherAlu1_side1.AddPoint(c1);
    catcherAlu1_side1.SetPrintColor(6);
    catcherAlu1_side1.SetName("catcherAlu1_side1");
    PndDrcSurfPolyFlat catcherAlu1_side2;
    catcherAlu1_side2.AddPoint(c1);
    catcherAlu1_side2.AddPoint(c5);
    catcherAlu1_side2.AddPoint(h1);
    catcherAlu1_side2.SetPrintColor(6);
    catcherAlu1_side2.SetName("catcherAlu1_side2");
    PndDrcSurfPolyFlat catcherAlu1_side3;
    catcherAlu1_side3.AddPoint(h1);
    catcherAlu1_side3.AddPoint(h4);
    catcherAlu1_side3.AddPoint(c4);
    catcherAlu1_side3.AddPoint(c1);
    catcherAlu1_side3.SetPrintColor(6);
    catcherAlu1_side3.SetName("catcherAlu1_side3");
    PndDrcSurfPolyFlat catcherAlu1_side4;
    catcherAlu1_side4.AddPoint(c4);
    catcherAlu1_side4.AddPoint(c8);
    catcherAlu1_side4.AddPoint(h4);
    catcherAlu1_side4.SetPrintColor(6);
    catcherAlu1_side4.SetName("catcherAlu1_side4");
    PndDrcSurfPolyFlat catcherAlu1_side5;
    catcherAlu1_side5.AddPoint(h1);
    catcherAlu1_side5.AddPoint(h4);
    catcherAlu1_side5.AddPoint(c8);
    catcherAlu1_side5.AddPoint(c5);
    catcherAlu1_side5.SetPrintColor(6);
    catcherAlu1_side5.SetName("catcherAlu1_side5");


    PndDrcOptVol catcherAlu1;
    catcherAlu1.SetVerbosity(0);
    catcherAlu1.AddSurface(catcherAlu1_side1);
    catcherAlu1.AddSurface(catcherAlu1_side2);
    catcherAlu1.AddSurface(catcherAlu1_side3);
    catcherAlu1.AddSurface(catcherAlu1_side4);
    catcherAlu1.AddSurface(catcherAlu1_side5);
    catcherAlu1.SetOptMaterial(alu);
    cout << "catcherAlu: aluminium" << endl;
    catcherAlu1.SetName("catcherAlu1");


    //#2
    PndDrcSurfPolyFlat catcherAlu2_side1;
    catcherAlu2_side1.AddPoint(c1);
    catcherAlu2_side1.AddPoint(c5);
    catcherAlu2_side1.AddPoint(c6);
    catcherAlu2_side1.AddPoint(c2);
    catcherAlu2_side1.SetPrintColor(6);
    catcherAlu2_side1.SetName("catcherAlu2_side1");
    PndDrcSurfPolyFlat catcherAlu2_side2;
    catcherAlu2_side2.AddPoint(c2);
    catcherAlu2_side2.AddPoint(c6);
    catcherAlu2_side2.AddPoint(h2);
    catcherAlu2_side2.SetPrintColor(6);
    catcherAlu2_side2.SetName("catcherAlu2_side2");
    PndDrcSurfPolyFlat catcherAlu2_side3;
    catcherAlu2_side3.AddPoint(h2);
    catcherAlu2_side3.AddPoint(h1);
    catcherAlu2_side3.AddPoint(c1);
    catcherAlu2_side3.AddPoint(c2);
    catcherAlu2_side3.SetPrintColor(6);
    catcherAlu2_side3.SetName("catcherAlu2_side3");
    PndDrcSurfPolyFlat catcherAlu2_side4;
    catcherAlu2_side4.AddPoint(c1);
    catcherAlu2_side4.AddPoint(c5);
    catcherAlu2_side4.AddPoint(h1);
    catcherAlu2_side4.SetPrintColor(6);
    catcherAlu2_side4.SetName("catcherAlu2_side4");
    PndDrcSurfPolyFlat catcherAlu2_side5;
    catcherAlu2_side5.AddPoint(h2);
    catcherAlu2_side5.AddPoint(c6);
    catcherAlu2_side5.AddPoint(c5);
    catcherAlu2_side5.AddPoint(h1);
    catcherAlu2_side5.SetPrintColor(6);
    catcherAlu2_side5.SetName("catcherAlu2_side5");


    PndDrcOptVol catcherAlu2;
    catcherAlu2.SetVerbosity(0);
    catcherAlu2.AddSurface(catcherAlu2_side1);
    catcherAlu2.AddSurface(catcherAlu2_side2);
    catcherAlu2.AddSurface(catcherAlu2_side3);
    catcherAlu2.AddSurface(catcherAlu2_side4);
    catcherAlu2.AddSurface(catcherAlu2_side5);
    catcherAlu2.SetOptMaterial(alu);
    catcherAlu2.SetName("catcherAlu2");


    //#3
    PndDrcSurfPolyFlat catcherAlu3_side1;
    catcherAlu3_side1.AddPoint(c2);
    catcherAlu3_side1.AddPoint(c6);
    catcherAlu3_side1.AddPoint(c7);
    catcherAlu3_side1.AddPoint(c3);
    catcherAlu3_side1.SetPrintColor(6);
    catcherAlu3_side1.SetName("catcherAlu3_side1");
    PndDrcSurfPolyFlat catcherAlu3_side2;
    catcherAlu3_side2.AddPoint(c3);
    catcherAlu3_side2.AddPoint(c7);
    catcherAlu3_side2.AddPoint(h3);
    catcherAlu3_side2.SetPrintColor(6);
    catcherAlu3_side2.SetName("catcherAlu3_side2");
    PndDrcSurfPolyFlat catcherAlu3_side3;
    catcherAlu3_side3.AddPoint(h3);
    catcherAlu3_side3.AddPoint(h2);
    catcherAlu3_side3.AddPoint(c2);
    catcherAlu3_side3.AddPoint(c3);
    catcherAlu3_side3.SetPrintColor(6);
    catcherAlu3_side3.SetName("catcherAlu3_side3");
    PndDrcSurfPolyFlat catcherAlu3_side4;
    catcherAlu3_side4.AddPoint(c2);
    catcherAlu3_side4.AddPoint(h2);
    catcherAlu3_side4.AddPoint(c6);
    catcherAlu3_side4.SetPrintColor(6);
    catcherAlu3_side4.SetName("catcherAlu3_side4");
    PndDrcSurfPolyFlat catcherAlu3_side5;
    catcherAlu3_side5.AddPoint(h2);
    catcherAlu3_side5.AddPoint(c6);
    catcherAlu3_side5.AddPoint(c7);
    catcherAlu3_side5.AddPoint(h3);
    catcherAlu3_side5.SetPrintColor(6);
    catcherAlu3_side5.SetName("catcherAlu3_side5");


    PndDrcOptVol catcherAlu3;
    catcherAlu3.SetVerbosity(0);
    catcherAlu3.AddSurface(catcherAlu3_side1);
    catcherAlu3.AddSurface(catcherAlu3_side2);
    catcherAlu3.AddSurface(catcherAlu3_side3);
    catcherAlu3.AddSurface(catcherAlu3_side4);
    catcherAlu3.AddSurface(catcherAlu3_side5);
    catcherAlu3.SetOptMaterial(alu);
    catcherAlu3.SetName("catcherAlu3");


    //#4
    PndDrcSurfPolyFlat catcherAlu4_side1;
    catcherAlu4_side1.AddPoint(c7);
    catcherAlu4_side1.AddPoint(c8);
    catcherAlu4_side1.AddPoint(c4);
    catcherAlu4_side1.AddPoint(c3);
    catcherAlu4_side1.SetPrintColor(6);
    catcherAlu4_side1.SetName("catcherAlu4_side1");
    PndDrcSurfPolyFlat catcherAlu4_side2;
    catcherAlu4_side2.AddPoint(c3);
    catcherAlu4_side2.AddPoint(h3);
    catcherAlu4_side2.AddPoint(c7);
    catcherAlu4_side2.SetPrintColor(6);
    catcherAlu4_side2.SetName("catcherAlu4_side2");
    PndDrcSurfPolyFlat catcherAlu4_side3;
    catcherAlu4_side3.AddPoint(h3);
    catcherAlu4_side3.AddPoint(c3);
    catcherAlu4_side3.AddPoint(c4);
    catcherAlu4_side3.AddPoint(h4);
    catcherAlu4_side3.SetPrintColor(6);
    catcherAlu4_side3.SetName("catcherAlu4_side3");
    PndDrcSurfPolyFlat catcherAlu4_side4;
    catcherAlu4_side4.AddPoint(c4);
    catcherAlu4_side4.AddPoint(c8);
    catcherAlu4_side4.AddPoint(h4);
    catcherAlu4_side4.SetPrintColor(6);
    catcherAlu4_side4.SetName("catcherAlu4_side4");
    PndDrcSurfPolyFlat catcherAlu4_side5;
    catcherAlu4_side5.AddPoint(h4);
    catcherAlu4_side5.AddPoint(h3);
    catcherAlu4_side5.AddPoint(c7);
    catcherAlu4_side5.AddPoint(c8);
    catcherAlu4_side5.SetPrintColor(6);
    catcherAlu4_side5.SetName("catcherAlu4_side5");


    PndDrcOptVol catcherAlu4;
    catcherAlu4.SetVerbosity(0);
    catcherAlu4.AddSurface(catcherAlu4_side1);
    catcherAlu4.AddSurface(catcherAlu4_side2);
    catcherAlu4.AddSurface(catcherAlu4_side3);
    catcherAlu4.AddSurface(catcherAlu4_side4);
    catcherAlu4.AddSurface(catcherAlu4_side5);
    catcherAlu4.SetOptMaterial(alu);
    catcherAlu4.SetName("catcherAlu4");


// Si-PMT
//==============================================================================
    double pmt_z = - catcher_height - 0.05;

    XYZPoint r1(-1.5, -1.5, pmt_z);         //  c1------c4
    XYZPoint r2(-1.5, +1.5, pmt_z);         //  |        |
    XYZPoint r3(+1.5, +1.5, pmt_z);         //  |        |
    XYZPoint r4(+1.5, -1.5, pmt_z);         //  r1------r4

    PndDrcSurfPolyFlat pmt_screen;
    pmt_screen.AddPoint(r1);
    pmt_screen.AddPoint(r2);
    pmt_screen.AddPoint(r3);
    pmt_screen.AddPoint(r4);
    pmt_screen.SetPixel();
    pmt_screen.SetName("pmt_screen");
    PndDrcSurfPolyFlat pmt_side1;
    pmt_side1.AddPoint(r4);
    pmt_side1.AddPoint(c4);
    pmt_side1.AddPoint(c1);
    pmt_side1.AddPoint(r1);
    pmt_side1.SetReflectivity(refl_none);
    pmt_side1.SetName("pmt_side1");
    PndDrcSurfPolyFlat pmt_side2;
    pmt_side2.AddPoint(r1);
    pmt_side2.AddPoint(c1);
    pmt_side2.AddPoint(c2);
    pmt_side2.AddPoint(r2);
    pmt_side2.SetReflectivity(refl_none);
    pmt_side2.SetName("pmt_side2");
    PndDrcSurfPolyFlat pmt_side3;
    pmt_side3.AddPoint(r2);
    pmt_side3.AddPoint(c2);
    pmt_side3.AddPoint(c3);
    pmt_side3.AddPoint(r3);
    pmt_side3.SetReflectivity(refl_none);
    pmt_side3.SetName("pmt_side3");
    PndDrcSurfPolyFlat pmt_side4;
    pmt_side4.AddPoint(r3);
    pmt_side4.AddPoint(r4);
    pmt_side4.AddPoint(c4);
    pmt_side4.AddPoint(c3);
    pmt_side4.SetReflectivity(refl_none);
    pmt_side4.SetName("pmt_side4");
    PndDrcSurfPolyFlat pmt_side5;
    pmt_side5.AddPoint(c1);
    pmt_side5.AddPoint(c2);
    pmt_side5.AddPoint(c3);
    pmt_side5.AddPoint(c4);
//     pmt_side5.SetReflectivity(refl_none);
    pmt_side5.SetName("pmt_side5");


    PndDrcOptVol pmt;
    pmt.SetVerbosity(0);
    pmt.AddSurface(pmt_screen);
    pmt.AddSurface(pmt_side1);
    pmt.AddSurface(pmt_side2);
    pmt.AddSurface(pmt_side3);
    pmt.AddSurface(pmt_side4);
    pmt.AddSurface(pmt_side5);
    pmt.SetOptMaterial(resinIndex);
    cout << "pmt: resin" << endl;
    pmt.SetName("pmt");


// air box
//==============================================================================
    XYZPoint a1(-20, -20, 0);  // old Y:3.25
    XYZPoint a2(-20, +20, 0);
    XYZPoint a3(+20, +20, 0);
    XYZPoint a4(+20, -20, 0);

    XYZPoint a5(-20, -20, 20);
    XYZPoint a6(-20, +20, 20);
    XYZPoint a7(+20, +20, 20);
    XYZPoint a8(+20, -20, 20);


//     PndDrcSurfPolyFlat air_side1;
//     air_side1.AddPoint(c5);
//     air_side1.AddPoint(c6);
//     air_side1.AddPoint(c7);
//     air_side1.AddPoint(c8);
//     air_side1.SetPixel();
//     air_side1.SetName("air_side1");
//     PndDrcSurfPolyFlat air_side2;
//     air_side2.AddPoint(a1);
//     air_side2.AddPoint(a2);
//     air_side2.AddPoint(c6);
//     air_side2.AddPoint(c5);
//     air_side2.SetReflectivity(refl_none);
//     air_side2.SetPrintColor(1);
//     air_side2.SetName("air_side2");
//     PndDrcSurfPolyFlat air_side3;
//     air_side3.AddPoint(c8);
//     air_side3.AddPoint(c7);
//     air_side3.AddPoint(a3);
//     air_side3.AddPoint(a4);
//     air_side3.SetReflectivity(refl_none);
//     air_side3.SetPrintColor(1);
//     air_side3.SetName("air_side3");
//     PndDrcSurfPolyFlat air_side4;
//     air_side4.AddPoint(a1);
//     air_side4.AddPoint(a2);
//     air_side4.AddPoint(a6);
//     air_side4.AddPoint(a5);
//     air_side4.SetReflectivity(refl_none);
//     air_side4.SetPrintColor(1);
//     air_side4.SetName("air_side4");
//     PndDrcSurfPolyFlat air_side5;
//     air_side5.AddPoint(a4);
//     air_side5.AddPoint(a3);
//     air_side5.AddPoint(a7);
//     air_side5.AddPoint(a8);
//     air_side5.SetReflectivity(refl_none);
//     air_side5.SetPrintColor(1);
//     air_side5.SetName("air_side5");
//     PndDrcSurfPolyFlat air_side6;
//     air_side6.AddPoint(a5);
//     air_side6.AddPoint(a6);
//     air_side6.AddPoint(a7);
//     air_side6.AddPoint(a8);
//     air_side6.SetReflectivity(refl_none);
//     air_side6.SetPrintColor(1);
//     air_side6.SetName("air_side6");
//     PndDrcSurfPolyFlat air_side7;
//     air_side7.AddPoint(a1);
//     air_side7.AddPoint(a5);
//     air_side7.AddPoint(a8);
//     air_side7.AddPoint(a4);
//     air_side7.SetReflectivity(refl_none);
//     air_side7.SetPrintColor(1);
//     air_side7.SetName("air_side7");
//     PndDrcSurfPolyFlat air_side8;
//     air_side8.AddPoint(a2);
//     air_side8.AddPoint(a6);
//     air_side8.AddPoint(a7);
//     air_side8.AddPoint(a3);
//     air_side8.SetReflectivity(refl_none);
//     air_side8.SetPrintColor(1);
//     air_side8.SetName("air_side8");

    PndDrcSurfPolyFlat air_side1;
    air_side1.AddPoint(a1);
    air_side1.AddPoint(a2);
    air_side1.AddPoint(a3);
    air_side1.AddPoint(a4);
//     air_side1.SetPixel();
    air_side1.SetName("air_side1");
    PndDrcSurfPolyFlat air_side2;
    air_side2.AddPoint(a4);
    air_side2.AddPoint(a3);
    air_side2.AddPoint(a7);
    air_side2.AddPoint(a8);
    air_side2.SetReflectivity(refl_none);
    air_side2.SetPrintColor(1);
    air_side2.SetName("air_side2");
    PndDrcSurfPolyFlat air_side3;
    air_side3.AddPoint(a8);
    air_side3.AddPoint(a7);
    air_side3.AddPoint(a6);
    air_side3.AddPoint(a5);
    air_side3.SetReflectivity(refl_none);
    air_side3.SetPrintColor(1);
    air_side3.SetName("air_side3");
    PndDrcSurfPolyFlat air_side4;
    air_side4.AddPoint(a6);
    air_side4.AddPoint(a5);
    air_side4.AddPoint(a1);
    air_side4.AddPoint(a2);
    air_side4.SetReflectivity(refl_none);
    air_side4.SetPrintColor(1);
    air_side4.SetName("air_side4");
    PndDrcSurfPolyFlat air_side5;
    air_side5.AddPoint(a2);
    air_side5.AddPoint(a6);
    air_side5.AddPoint(a7);
    air_side5.AddPoint(a3);
    air_side5.SetReflectivity(refl_none);
    air_side5.SetPrintColor(1);
    air_side5.SetName("air_side5");
    PndDrcSurfPolyFlat air_side6;
    air_side6.AddPoint(a1);
    air_side6.AddPoint(a5);
    air_side6.AddPoint(a8);
    air_side6.AddPoint(a4);
    air_side6.SetReflectivity(refl_none);
    air_side6.SetPrintColor(1);
    air_side6.SetName("air_side6");


    PndDrcOptVol air;
    air.SetVerbosity(0);
    air.AddSurface(air_side1);
    air.AddSurface(air_side2);
    air.AddSurface(air_side3);
    air.AddSurface(air_side4);
    air.AddSurface(air_side5);
    air.AddSurface(air_side6);
//     air.AddSurface(air_side7);
//     air.AddSurface(air_side8);
    air.SetOptMaterial(vacuum);
    cout << "air: vacuum" << endl;
    air.SetName("air");


// small box with hole for air_lens
//==============================================================================
//     XYZPoint b1(-300.0, +300.0, 0);             //             b5--------- b8
//     XYZPoint b2(-300.0, -300.0, 0);             //            /|          / |
//     XYZPoint b3(+300.0, -300.0, 0);             //           / |         /  |
//     XYZPoint b4(+300.0, +300.0, 0);             //          /  |        /   |
//     XYZPoint b5(-300.0, +300.0, vacBoxwidth);   //         /   |       /    |
//     XYZPoint b6(-300.0, -300.0, vacBoxwidth);   //        /    |      /     |
//     XYZPoint b7(+300.0, -300.0, vacBoxwidth);   //       /     b6----/------b7
    //     XYZPoint b8(+300.0, +300.0, vacBoxwidth);   //      /    /      /       /
    //     //    b1--s1--s4--b4       /
    //     XYZPoint s1(-slab_width/2, +300.0, 0);      //    |   |   |   |       /
    //     XYZPoint s2(-slab_width/2, -300.0, 0);      //    |   |   |   |      /
    //     XYZPoint s3(+slab_width/2, -300.0, 0);      //    |---s5--s6--|     /
//     XYZPoint s4(+slab_width/2, +300.0, 0);      //    |   | H |   |    /       H: lens-hole
    //     //    |---s7--s8--|   /
    //     //    |   |   |   |  /
    //     //    |   |   |   | /
//                                                 //    b2--s2--s3--b3
    //
    //
//     PndDrcSurfPolyFlat smallBox_side1a;
//     smallBox_side1a.AddPoint(b1);
//     smallBox_side1a.AddPoint(b2);
//     smallBox_side1a.AddPoint(s2);
//     smallBox_side1a.AddPoint(s1);
//     smallBox_side1a.SetReflectivity(refl_none);
//     smallBox_side1a.SetName("smallBox_side1a");
//     PndDrcSurfPolyFlat smallBox_side1b;
//     smallBox_side1b.AddPoint(s4);
//     smallBox_side1b.AddPoint(s3);
//     smallBox_side1b.AddPoint(b3);
//     smallBox_side1b.AddPoint(b4);
//     smallBox_side1b.SetReflectivity(refl_none);
//     smallBox_side1b.SetName("smallBox_side1b");
//     PndDrcSurfPolyFlat smallBox_side1c;
//     smallBox_side1c.AddPoint(s1);
//     smallBox_side1c.AddPoint(s5);
//     smallBox_side1c.AddPoint(s6);
//     smallBox_side1c.AddPoint(s4);
//     smallBox_side1c.SetReflectivity(refl_none);
//     smallBox_side1c.SetName("smallBox_side1c");
//     PndDrcSurfPolyFlat smallBox_side1d;
//     smallBox_side1d.AddPoint(s7);
//     smallBox_side1d.AddPoint(s2);
//     smallBox_side1d.AddPoint(s3);
//     smallBox_side1d.AddPoint(s8);
//     smallBox_side1d.SetReflectivity(refl_none);
//     smallBox_side1d.SetName("smallBox_side1d");
    //
//     lens_sphere.AddTransform(Transform3D(XYZVector(0,0,lens_shift)));
//     lens_side1.AddTransform(Transform3D(XYZVector(0,0,lens_shift)));
//     lens_side2.AddTransform(Transform3D(XYZVector(0,0,lens_shift)));
//     lens_side3.AddTransform(Transform3D(XYZVector(0,0,lens_shift)));
//     lens_side4.AddTransform(Transform3D(XYZVector(0,0,lens_shift)));
    //
//     PndDrcSurfPolyAsphere smallBox_lens_sphere = lens_sphere;
//     PndDrcSurfQuadFlatDiff smallBox_lens_side1 = lens_side1;
//     PndDrcSurfQuadFlatDiff smallBox_lens_side2 = lens_side2;
//     PndDrcSurfQuadFlatDiff smallBox_lens_side3 = lens_side3;
//     PndDrcSurfQuadFlatDiff smallBox_lens_side4 = lens_side4;
    //
//     smallBox_lens_sphere.SetName("smallBox_lens_sphere");
//     smallBox_lens_side1.SetName("smallBox_lens_side1");
//     smallBox_lens_side2.SetName("smallBox_lens_side2");
//     smallBox_lens_side3.SetName("smallBox_lens_side3");
//     smallBox_lens_side4.SetName("smallBox_lens_side4");
    //
//     PndDrcSurfPolyFlat smallBox_side2;
//     smallBox_side2.AddPoint(b2);
//     smallBox_side2.AddPoint(b3);
//     smallBox_side2.AddPoint(b7);
//     smallBox_side2.AddPoint(b6);
//     smallBox_side2.SetReflectivity(refl_none);
//     smallBox_side2.SetName("smallBox_side2");
//     PndDrcSurfPolyFlat smallBox_side3;
//     smallBox_side3.AddPoint(b3);
//     smallBox_side3.AddPoint(b4);
//     smallBox_side3.AddPoint(b8);
//     smallBox_side3.AddPoint(b7);
//     smallBox_side3.SetReflectivity(refl_none);
//     smallBox_side3.SetName("smallBox_side3");
//     PndDrcSurfPolyFlat smallBox_side4;
//     smallBox_side4.AddPoint(b1);
//     smallBox_side4.AddPoint(b4);
//     smallBox_side4.AddPoint(b8);
//     smallBox_side4.AddPoint(b5);
//     smallBox_side4.SetReflectivity(refl_none);
//     smallBox_side4.SetName("smallBox_side4");
//     PndDrcSurfPolyFlat smallBox_side5;
//     smallBox_side5.AddPoint(b5);
//     smallBox_side5.AddPoint(b6);
//     smallBox_side5.AddPoint(b7);
//     smallBox_side5.AddPoint(b8);
//     smallBox_side5.SetName("smallBox_side5");
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
//     smallBox.AddSurface(smallBox_side1a);
//     smallBox.AddSurface(smallBox_side1b);
//     smallBox.AddSurface(smallBox_side1c);
//     smallBox.AddSurface(smallBox_side1d);
    //
//     smallBox.AddSurface(smallBox_lens_sphere);
//     smallBox.AddSurface(smallBox_lens_side1);
//     smallBox.AddSurface(smallBox_lens_side2);
//     smallBox.AddSurface(smallBox_lens_side3);
//     smallBox.AddSurface(smallBox_lens_side4);
    //
//     smallBox.AddSurface(smallBox_side2);
//     smallBox.AddSurface(smallBox_side3);
//     smallBox.AddSurface(smallBox_side4);
//     smallBox.AddSurface(smallBox_side5);
//     smallBox.AddSurface(smallBox_side6);
//     smallBox.SetOptMaterial(vacuum);
//     cout << "small box: vacuum" << endl;
//     smallBox.SetName("smallBox");
    //


    PndDrcOptDevSys opt_system;
    opt_system.SetNameCopyNumber("opt_system");
    opt_system.SetVerbosity(0);
    opt_system.AddDevice(catcher);
    opt_system.AddDevice(catcherAlu1);
    opt_system.AddDevice(catcherAlu2);
    opt_system.AddDevice(catcherAlu3);
    opt_system.AddDevice(catcherAlu4);
    opt_system.AddDevice(air);
    opt_system.AddDevice(pmt);
//     opt_system.AddDevice(box);
//     opt_system.AddDevice(smallBox);
//     opt_system.AddDevice(slab);
//     opt_system.AddDevice(lens);
//     opt_system.EmbedDevice(lens,smallBox);
//     opt_system.EmbedDevice(lens,box);

  // couple surface 1 of device 1 with surface 2 of device 2
  //                       dev1   dev2    surf1     surf2
//     opt_system.CoupleDevice("slab","box","slab_side1", "box_side1");
//     opt_system.CoupleDevice("slab","smallBox","slab_side1", "smallBox_side1");


    opt_system.CoupleDevice("air","catcher","air_side1", "catcher_side5");
    opt_system.CoupleDevice("catcher","catcherAlu1","catcher_side1", "catcherAlu1_side1");
    opt_system.CoupleDevice("catcher","catcherAlu2","catcher_side2", "catcherAlu2_side1");
    opt_system.CoupleDevice("catcher","catcherAlu3","catcher_side3", "catcherAlu3_side1");
    opt_system.CoupleDevice("catcher","catcherAlu4","catcher_side4", "catcherAlu4_side1");
    opt_system.CoupleDevice("catcher","pmt","catcher_detector", "pmt_side5");
//     opt_system.CoupleDevice("slab","lens","slab_side1", "lens_base");
//     opt_system.CoupleDevice("lens","smallBox","lens_side1", "smallBox_lens_side1");
//     opt_system.CoupleDevice("lens","smallBox","lens_side2", "smallBox_lens_side2");
//     opt_system.CoupleDevice("lens","smallBox","lens_side3", "smallBox_lens_side3");
//     opt_system.CoupleDevice("lens","smallBox","lens_side4", "smallBox_lens_side4");
//     opt_system.CoupleDevice("lens","smallBox","lens_sphere", "smallBox_lens_sphere");
//     opt_system.CoupleDevice("smallBox","box","smallBox_side5", "box_side1");


    PndDrcOptDevManager* manager = new PndDrcOptDevManager();
    manager->AddDeviceSystem(opt_system);


    fstream geo;
    geo.open("Geo.C",std::ios::out);
    geo<<"{"<<endl;
    geo<<"    TCanvas *c1 = new TCanvas(\"c1\"); "<<endl;
    cout<<" root version: "<< (long int)((TROOT*)gROOT -> GetVersionInt())<<endl;
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
    //
//     manager->Print(geo);


  //geo<<"}"<<endl;
  //exit(0);

    list<PndDrcPhoton> list_photon; // get list

    PndDrcPhoton ph;
    ph.SetReflectionLimit(200);


    double beta=0;
    bool photons_exist = false;
    if (ioption==1)
    {
//         slab_width*=0.5;
//         slab_height*=0.5;
        //
//         for (double xx=-slab_width/2; xx<=slab_width/2; xx+=slab_width/4)
//         {
//             for (double yy=-slab_height/2; yy<=slab_height/2; yy+=slab_height/4)
//             {
//           //double xx=0;
//           //double yy=0;
//                 ph.SetPosition(XYZPoint(xx,yy,dist-500));
//                 ph.SetDirection(XYZVector(0,0,1));
//                 ph.SetWavelength(lambda);
//                 list_photon.push_back(ph);
//           //goto raus;
//             }
//         }
//       // raus:

        if(argc == 2)
        {
            cout << "*****  second parameter defines the incident angle of the incoming photons  *****" << endl;
            return 0;
        }

        double inci = atof(argv[2]); // incident angle

//         double inci = 30;
        double phi  = 45;
        cout << "inci [deg] = " << inci << "  phi [deg] = " << phi << endl;

        for( double spotX = -3; spotX < 3.001; spotX+=0.1 )
        {
            for( double spotY = -3; spotY < 3.001; spotY+=0.1 )
            {
                double XX = spotX - sin(inci/180.*pi) * cos(phi/180.*pi) * 15 / cos(inci/180.*pi);
                double YY = spotY - sin(inci/180.*pi) * sin(phi/180.*pi) * 15 / cos(inci/180.*pi);
                ph.SetPosition(XYZPoint(XX,YY,15));
                ph.SetDirection(XYZVector(
                        sin(inci/180.*pi) * cos(phi/180.*pi) / cos(inci/180.*pi), sin(inci/180.*pi) * sin(phi/180.*pi) / cos(inci/180.*pi), -1
                                         ));
                ph.SetWavelength(lambda);
                list_photon.push_back(ph);
            }
        }

//         ph.SetPosition(XYZPoint(-8,0,15));
//         ph.SetDirection(XYZVector(tan(30/180.*pi),0,-1));
//         ph.SetWavelength(lambda);
//         list_photon.push_back(ph);
//         ph.SetPosition(XYZPoint(-11,0,15));
//         ph.SetDirection(XYZVector(tan(30/180.*pi),0,-1));
//         ph.SetWavelength(lambda);
//         list_photon.push_back(ph);

        photons_exist = true;
        manager->SetPhotonList(list_photon,"air","opt_system",0,0);

// draw chosen photons (for Geo)
//==========================================================================================
//                 list_photon = manager->PhotonList();
//         list<PndDrcPhoton>::iterator iph;
//
//         int icnt=0;
//
//         for(iph=list_photon.begin(); iph != list_photon.end(); ++iph)
//         {
//             icnt++;
//             if( icnt == 9 || icnt == 10 )
//             {
//                 (*iph).SetPrintFlag(true);
//             }
//             else
//             {
//                 (*iph).SetPrintFlag(false);
//             }
//         }
//         manager->SetPhotonList(list_photon,"air","opt_system",0,0);
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
      // -50 close to lens (0)
      // -900 close to far end (1000)

        double inci = 57; // in degree
//         XYZPoint  pos(20,-7.5,-440);
        XYZVector dir(-1,0,tan(inci/180.*pi) );
        beta = 0.9571;

        XYZPoint  pos;
        if( argc == 5 )
        {
            double x = atof(argv[1]);
            double y = atof(argv[2]);
            double z = atof(argv[3]);
            pos = XYZVector(x,y,z);
            cout << "start point (particle): X: " << x << " Y: " << y << " Z: " << z << endl;
        }
        else
            pos = XYZVector(20,-7.5,-440);


//       XYZPoint  pos(0,0,-800.01);
//       XYZVector dir(0,0,1 );
//       beta=0.7;

        int photon_number = 200;
//       float     range = 799.99;
        float     range = 100;
//         photons_exist = manager->Cerenkov(pos,dir,beta,photon_number,range,435,435);
//       photons_exist = manager->Cerenkov(pos,dir,beta,photon_number,range);



        // beamSpot simulation
        //==========================================================================================

        TVector3 z = TVector3(0,0,1);
        TVector3 ortho;
        ortho = z.Orthogonal();
        TRandom3 rand;
        TVector3 helper;
        TVector3 helper2;
        double gausSmear; // gaus smearing
        double radius = 10; // radius in mm is 1 sigma of the gaus smearing
        double transX = 20; // for the translation-(shift) (see above)
        double transY = -7.5;
        double transZ = -440;
        double spotX;
        double spotY;
        double spotZ;
        XYZPoint spotPos;

        for(int i=0; i<301; i++)
        {
            helper=ortho; // (-1,0,0) is the orthogonal of (0,0,1)
            helper.RotateZ(rand.Rndm()*2*pi);
            gausSmear=rand.Gaus(0,radius);
            helper2 = TVector3(helper.X()*gausSmear, helper.Y()*gausSmear, helper.Z()*gausSmear);
            helper2.RotateY( (-90+inci)/180.*pi ); // rotation in the right direction
            spotX = helper2.X()+transX; // translation-(shift)
            spotY = helper2.Y()+transY;
            spotZ = helper2.Z()+transZ;
            spotPos = XYZVector(spotX,spotY,spotZ);
            manager->Cerenkov(spotPos,dir,beta,photon_number,range);
        }
        photons_exist = true; // maybe it is needed (if last particle in the loop don't hit the bar, this value will be set as false)



        // draw chosen photons (for Geo)
        //==========================================================================================
//         list_photon = manager->PhotonList();
//         list<PndDrcPhoton>::iterator iph;
        //
// //         int icandidate = 45;
//         int icnt=0;
        //
//         for(iph=list_photon.begin(); iph != list_photon.end(); ++iph)
//         {
//             icnt++;
// //             if (icnt==icandidate)
// //             if(icnt == 164 || icnt == 1041 || icnt == 1050 || icnt == 1279 || icnt == 4135)
// //             if( icnt == 1559 || icnt == 1574 || icnt == 2114 )
//             if( icnt == 6 || icnt == 2491 )
//             {
//                 (*iph).SetPrintFlag(true);
//             }
//             else
//             {
//                 (*iph).SetPrintFlag(false);
//             }
//         }
//         manager->SetPhotonList(list_photon,"slab","opt_system",0,0);


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
//         cout << "yippie " << endl;
        manager->SetVerbosity(5); // doesn't work
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


    string  screeni;
    if( argc == 5 )
    {
        screeni = argv[4];
//         screeni = screeni + ".C";
        cout << "FILE: " << screeni << endl;
    }
    else
        screeni = "Screen.C";

    fstream scr;
    scr.open(screeni.c_str(),std::ios::out);
    scr << "{" << endl;
    scr << "gStyle->SetCanvasColor(0);"       << endl;
    scr << "gStyle->SetCanvasBorderMode(0);"  << endl;
    scr << "gStyle->SetFrameBorderMode(0);"   << endl;
    scr << "gStyle->SetTitleFillColor(0);"    << endl;
    scr << "gStyle->SetTitleFontSize(0.05);"  << endl;
    scr << "TCanvas *c1 = new TCanvas( \"c1\", \"\" ,200, 10, 700, 500 );" << endl;
//   scr<<"    TCanvas *c1 = new TCanvas(\"c1\"); "<<endl;
    TString str_beta;
    str_beta+=beta;
    str_beta.Remove(TString::kLeading,' ');
    scr << "TString title;"<<endl;
//     scr << "title=\"spatial position [mm] (beta=" << str_beta << ", #lambda =435nm)\";"<< endl;
    scr << "title=\"spatial position [mm] (beta=" << str_beta << ")\";"<< endl;
//     scr<<"    TH1F *hgr = new TH1F(\"hgr1\",title,600,-150,150);"<<endl;
    scr << "    TH1F *hgr = new TH1F(\"hgr1\",title,100,-5,5);"<<endl;
    scr << "hgr->SetStats( 0 );" << endl;
    scr << "    hgr->SetMarkerStyle(7);"<<endl;
    scr << "    hgr->SetMarkerSize(0.5);"<<endl;
    scr<<"    hgr->SetMinimum(-5);"<<endl;
    scr<<"    hgr->SetMaximum(+5);"<<endl;
//     scr << "    hgr->SetMinimum(-100);"<<endl;
//     scr << "    hgr->SetMaximum(+100);"<<endl;
    scr << "    hgr->Draw(\"POL\");"<<endl << endl;
    int icnt_measured = 0;
    int icnt_flying   = 0;
    int icnt_lost     = 0;
    int icnt_absorbed = 0;

  //TRotation rotInv_screen = rot_screen.Inverse();
    list<PndDrcPhoton>::iterator iph;
    PndDrcPhoton ph_old;


    fstream diffi;
    diffi.open("diffi",std::ios::out);
    int n_iph = 0;


    for(iph=list_photon.begin(); iph != list_photon.end(); ++iph)
    {

        n_iph++;
        if ((*iph).Fate()==Drc::kPhotMeasured)
        {
            icnt_measured++;

            diffi << n_iph << endl;


//       cout << icnt_measured << " \\ " << list_photon.size() << '\r';


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
        else if ((*iph).Fate()==Drc::kPhotAbsorbed) icnt_absorbed++;
        else
        {
            icnt_lost++;
        }
    }
    diffi.close();
  //cout<<" ratio = "<<dista/distb<<endl;


//     // MCP position
//     TLine *l1 = new TLine(-19.125,-13.625,-70.125,-13.625);
//     TLine *l2 = new TLine(-70.125,-13.625,-70.125,37.375);
//     TLine *l3 = new TLine(-70.125,37.375,-19.125,37.375);
//     TLine *l4 = new TLine(-19.125,37.375,-19.125,-13.625);
//     l1->SetLineWidth(3);
//     l2->SetLineWidth(3);
//     l3->SetLineWidth(3);
//     l4->SetLineWidth(3);
//     l1->Draw("same");
//     l2->Draw("same");
//     l3->Draw("same");
//     l4->Draw("same");
    //
//     TLine *l5 = new TLine(19.125,-13.625,70.125,-13.625);
//     TLine *l6 = new TLine(70.125,-13.625,70.125,37.375);
//     TLine *l7 = new TLine(70.125,37.375,19.125,37.375);
//     TLine *l8 = new TLine(19.125,37.375,19.125,-13.625);
//     l5->SetLineWidth(3);
//     l6->SetLineWidth(3);
//     l7->SetLineWidth(3);
//     l8->SetLineWidth(3);
//     l5->Draw("same");
//     l6->Draw("same");
//     l7->Draw("same");
//     l8->Draw("same");
//     scr << endl;
//     scr << "TLine *l1 = new TLine(-19.125,-13.625,-70.125,-13.625);"    << endl;
//     scr << "TLine *l2 = new TLine(-70.125,-13.625,-70.125,37.375);"     << endl;
//     scr << "TLine *l3 = new TLine(-70.125,37.375,-19.125,37.375);"      << endl;
//     scr << "TLine *l4 = new TLine(-19.125,37.375,-19.125,-13.625);"     << endl;
//     scr << "l1->SetLineWidth(3);" << endl;
//     scr << "l2->SetLineWidth(3);" << endl;
//     scr << "l3->SetLineWidth(3);" << endl;
//     scr << "l4->SetLineWidth(3);" << endl;
//     scr << "l1->Draw(\"same\");" << endl;
//     scr << "l2->Draw(\"same\");" << endl;
//     scr << "l3->Draw(\"same\");" << endl;
//     scr << "l4->Draw(\"same\");" << endl;
//     scr << endl;
//     scr << "TLine *l5 = new TLine(19.125,-13.625,70.125,-13.625);"  << endl;
//     scr << "TLine *l6 = new TLine(70.125,-13.625,70.125,37.375);"   << endl;
//     scr << "TLine *l7 = new TLine(70.125,37.375,19.125,37.375);"    << endl;
//     scr << "TLine *l8 = new TLine(19.125,37.375,19.125,-13.625);"   << endl;
//     scr << "l5->SetLineWidth(3);" << endl;
//     scr << "l6->SetLineWidth(3);" << endl;
//     scr << "l7->SetLineWidth(3);" << endl;
//     scr << "l8->SetLineWidth(3);" << endl;
//     scr << "l5->Draw(\"same\");" << endl;
//     scr << "l6->Draw(\"same\");" << endl;
//     scr << "l7->Draw(\"same\");" << endl;
//     scr << "l8->Draw(\"same\");" << endl;
    //


//   scr<<"}"<<endl;
//   scr.close();

    int icnt = icnt_measured+icnt_flying+icnt_lost+icnt_absorbed;
    cout << endl << endl;
    cout<<" generated photons: "<<icnt<<endl;
    cout<<" measured  photons: "<<icnt_measured<<endl;
    cout<<" absorbed  photons: "<<icnt_absorbed<<endl;
    cout<<" lost      photons: "<<icnt_lost<<endl<<endl;

    double eff = (double)icnt_measured/ (double) icnt;
    cout << "efficiency: " << eff << endl;
    TString str_height = argv[1];
    TString str_eff;
    str_eff+=eff;
    TString com = "echo " + str_height + " " + str_eff + " >> height_effi";
    system(com);

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
