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
#include "TRotation.h"

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
#include "PndDrcSurfPolySphere.h"
#include "PndDrcSurfPolyPara.h"
#include "PndDrcSurfPolyAsphere.h"
#include "PndDrcOptReflSilver.h"
#include "PndDrcOptReflNone.h"
#include "PndDrcOptReflPerfect.h"
#include "PndDrcOptMatLithotecQ0.h"
#include "PndDrcOptMatNLAK33A.h"
#include "PndDrcOptMatLLF1.h"
#include "PndDrcOptMatVacuum.h"
#include "PndDrcOptDevSys.h"
#include "PndDrcOptVol.h"
#include "PndDrcOptDevManager.h"
#include "PndDrcUtil.h"


int main(int argc, char *argv[])
{
  //double conical_const = 0;


  //cout<<" constant = ";
  //cin>>conical_const;

  //double conical_const = -1; // parabola
  //double conical_const = 1; // ellipse
  //double conical_const = -1; // hyperbola

  //int ioption = 1; // parallel beam (0 deg)
  //int ioption = 2; // 0,  +- 20,  +-40 deg
  //int ioption = 3; // C-cone
  int ioption = 4; // random in front of lens.
  //int ioption = 5; // single photon for debugging.
  //int ioption = 6; // grid 5 deg

  double pi=3.1415926535;

  PndDrcOptReflPerfect refl_perfect;
  PndDrcOptReflNone    refl_none;
  double lambda=500;

  double slab_width  = 17;
  double slab_height = 17;


  XYZPoint q0(-slab_width/2, +slab_height/2, 0.0);
  XYZPoint q1(-slab_width/2, -slab_height/2, 0.0);
  XYZPoint q2(+slab_width/2, -slab_height/2, 0.0);
  XYZPoint q3(+slab_width/2, +slab_height/2, 0.0);

  PndDrcOptMatLithotecQ0 quartz;
  PndDrcOptMatNLAK33A    nlak33a;
  PndDrcOptMatVacuum     vacuum;
  PndDrcOptMatLLF1       llf1;

  PndDrcSurfPolySphere lens_sphere;
  lens_sphere.SetVerbosity(0);
  //lens_sphere.SetConicalConstant(conical_const);
  lens_sphere.AddPoint(q0);
  lens_sphere.AddPoint(q1);
  lens_sphere.AddPoint(q2);
  lens_sphere.AddPoint(q3);
  // screen 101 mm from b0
  double f,f1,f2,radius1,radius2;
  double n2 = llf1.RefIndex(lambda);
  double n1 = nlak33a.RefIndex(lambda);

  cout<<" n1 = "<<n1<<endl;
  cout<<" n2 = "<<n2<<endl;

  f  = 200;
  //cout<<" Give focal length : ";
  //cin>>f;

  //double viol = 0.1;             // Petzval violation
  f1 = f*(1.0+n2/(-n1));
  f2 = f*(1.0+n1/(-n2));

  cout<<" petzval = "<<f1*n1+f2*n2<<endl;
  cout<<" f  = "<<1.0/(1.0/f1+1.0/f2)<<endl;

  cout<<" f1 = "<<f1<<endl;
  cout<<" f2 = "<<f2<<endl;

  radius1 = -f1*(n2-n1)/n2;
  radius2 = -f2*(n1-n2)/n1;

  radius1 = f1*(n1-1);

  radius2 = 1.0/(f2*(n2-1)) + 1.0/radius1;
  radius2 = 1.0/radius2; 
  
  //radius1 = -19.5;
  //radius2 = 50;

  //cout<<" Give R1 (<0) : "<<endl;
  //radius1 = atof(argv[1]);
  //cout<<" Give R2 (>0) : "<<endl;
  //radius2 = atof(argv[2]);

  radius1 =-16.1;
  radius2 = 34.88;

  //radius1 = -999999;
  //radius2 = 1.0/(288*(n2-1));
  //radius2 = 1.0/radius2; 

  cout<<" r1 = "<<radius1<<endl;
  cout<<" r2 = "<<radius2<<endl;


  lens_sphere.SetRadius(fabs(radius1)); // f = R/(n-1) = 100 mm
  //lens_sphere.SetVerbosity(4);
  lens_sphere.SetPrintColor(4);
  lens_sphere.SetName("lens_sphere");

  RotationY rot(pi);
  Rotation3D rot_lens(rot); 
  Transform3D trans_rot_lens(rot_lens);
  RotationY rot2(0);
  Rotation3D rot_lens2(rot2); 
  Transform3D trans_rot_lens2(rot_lens2);
  lens_sphere.AddTransform(trans_rot_lens);

  double dist = lens_sphere.CenterPoint().Z();

  XYZVector shift_lens(0,0,-dist+0.2);

  
  Transform3D trans_shift_lens = Transform3D(shift_lens);
  lens_sphere.AddTransform(trans_shift_lens);

  //lens_sphere.shift(XYZVector(0,0,-(dist-2)));






  // Declare flat surfaces with arbitrary number of points.
  q0.SetZ(0); // later be rotated by 180 deg.
  q1.SetZ(0);
  q2.SetZ(0);
  q3.SetZ(0);
  q0 = trans_rot_lens * q0;
  q1 = trans_rot_lens * q1;
  q2 = trans_rot_lens * q2;
  q3 = trans_rot_lens * q3;

  PndDrcSurfPolyFlat lens_base;
  lens_base.SetVerbosity(0);
  lens_base.AddPoint(q0);
  lens_base.AddPoint(q1);
  lens_base.AddPoint(q2);
  lens_base.AddPoint(q3);
  lens_base.SetPrintColor(4);
  lens_base.SetName("lens_base");

  

  PndDrcSurfQuadFlatDiff lens_side1;
  lens_side1.AddSurface(lens_base,  q0,q1);
  lens_side1.AddSurface(lens_sphere,
			lens_sphere.LimitingPoint(0),lens_sphere.LimitingPoint(1));
  lens_side1.SetPrintColor(4);
  lens_side1.SetName("lens_side1");
  PndDrcSurfQuadFlatDiff lens_side2;
  lens_side2.SetVerbosity(0);
  lens_side2.AddSurface(lens_base,  q1,q2);
  lens_side2.AddSurface(lens_sphere,
			lens_sphere.LimitingPoint(1),lens_sphere.LimitingPoint(2));
  lens_side2.SetPrintColor(4);
  lens_side2.SetName("lens_side2");
  PndDrcSurfQuadFlatDiff lens_side3;
  lens_side3.AddSurface(lens_base,  q2,q3);
  lens_side3.AddSurface(lens_sphere,
			lens_sphere.LimitingPoint(2),lens_sphere.LimitingPoint(3));
  lens_side3.SetPrintColor(4);
  lens_side3.SetName("lens_side3");
  PndDrcSurfQuadFlatDiff lens_side4;
  lens_side4.AddSurface(lens_base,  q3,q0);
  lens_side4.AddSurface(lens_sphere,
			lens_sphere.LimitingPoint(3),lens_sphere.LimitingPoint(0));
  lens_side4.SetPrintColor(4);
  lens_side4.SetName("lens_side4");


  lens_side1.SetReflectivity(refl_none);
  lens_side2.SetReflectivity(refl_none);
  lens_side3.SetReflectivity(refl_none);
  lens_side4.SetReflectivity(refl_none);
  

  PndDrcOptVol lens;
  lens.SetVerbosity(0);
  lens.AddSurface(lens_base);
  lens.AddSurface(lens_sphere);
  lens.AddSurface(lens_side1);
  lens.AddSurface(lens_side2);
  lens.AddSurface(lens_side3);
  lens.AddSurface(lens_side4);
  lens.SetOptMaterial(nlak33a);
  //lens.SetOptMaterial(quartz);
  lens.SetName("lens");

 

  // create a second lense
  PndDrcSurfPolySphere lens2_sphere1(lens_sphere);
  //lens2_sphere1.SetConicalConstant(conical_const);
  lens2_sphere1.SetVerbosity(0);
  lens2_sphere1.SetPrintColor(4);
  lens2_sphere1.SetName("lens2_sphere1");



  PndDrcSurfPolySphere lens2_sphere2;
  //lens2_sphere2.SetConicalConstant(conical_const);
  lens2_sphere2.SetVerbosity(0);
  lens2_sphere2.SetPrintColor(3);

  q0 = trans_rot_lens2 * q0;
  q1 = trans_rot_lens2 * q1;
  q2 = trans_rot_lens2 * q2;
  q3 = trans_rot_lens2 * q3;
  q0.SetZ(0); // 1 mm appart from sphere edge
  q1.SetZ(0);
  q2.SetZ(0);
  q3.SetZ(0);

  lens2_sphere2.AddPoint(q0);
  lens2_sphere2.AddPoint(q1);
  lens2_sphere2.AddPoint(q2);
  lens2_sphere2.AddPoint(q3);

  lens2_sphere2.SetRadius(fabs(radius2));// f = R/(n-1) = 100 mm

  lens2_sphere2.SetPrintColor(3);
  lens2_sphere2.SetName("lens2_sphere2");
  lens2_sphere2.AddTransform(trans_rot_lens2);


  dist = lens_sphere.LimitingPoint(0).Z();
  double dist2 = lens2_sphere2.LimitingPoint(0).Z();
  XYZVector shift_lens2(0,0,dist-dist2+0.5);
  lens2_sphere2.AddTransform(Transform3D(XYZVector(shift_lens2)));

  PndDrcSurfQuadFlatDiff lens2_side1;
  lens2_side1.AddSurface(lens2_sphere1,
			lens2_sphere1.LimitingPoint(0),lens2_sphere1.LimitingPoint(1));
  lens2_side1.AddSurface(lens2_sphere2,
			lens2_sphere2.LimitingPoint(0),lens2_sphere2.LimitingPoint(1));
  lens2_side1.SetPrintColor(3);
  lens2_side1.SetName("lens2_side1");
  PndDrcSurfQuadFlatDiff lens2_side2;
  lens2_side2.AddSurface(lens2_sphere2,
			lens2_sphere1.LimitingPoint(1),lens2_sphere1.LimitingPoint(2));
  lens2_side2.AddSurface(lens2_sphere2,
			lens2_sphere2.LimitingPoint(1),lens2_sphere2.LimitingPoint(2));
  lens2_side2.SetPrintColor(3);
  lens2_side2.SetName("lens2_side2");
  PndDrcSurfQuadFlatDiff lens2_side3;
  lens2_side3.AddSurface(lens2_sphere1,
			lens2_sphere1.LimitingPoint(2),lens2_sphere1.LimitingPoint(3));
  lens2_side3.AddSurface(lens2_sphere2,
			lens2_sphere2.LimitingPoint(2),lens2_sphere2.LimitingPoint(3));
  lens2_side3.SetPrintColor(3);
  lens2_side3.SetName("lens2_side3");
  PndDrcSurfQuadFlatDiff lens2_side4;
  lens2_side4.SetVerbosity(0);
  lens2_side4.AddSurface(lens2_sphere1,
			lens2_sphere1.LimitingPoint(3),lens2_sphere1.LimitingPoint(0));
  lens2_side4.AddSurface(lens2_sphere2,
			lens2_sphere2.LimitingPoint(3),lens2_sphere2.LimitingPoint(0));
  lens2_side4.SetPrintColor(3);
  lens2_side4.SetName("lens2_side4");


  lens2_side1.SetReflectivity(refl_none);
  lens2_side2.SetReflectivity(refl_none);
  lens2_side3.SetReflectivity(refl_none);
  lens2_side4.SetReflectivity(refl_none);

  PndDrcOptVol lens2;
  lens2.SetVerbosity(0);
  lens2.AddSurface(lens2_sphere1);
  lens2.AddSurface(lens2_sphere2);
  lens2.AddSurface(lens2_side1);
  lens2.AddSurface(lens2_side2);
  lens2.AddSurface(lens2_side3);
  lens2.AddSurface(lens2_side4);
  lens2.SetOptMaterial(llf1);
  //lens2.SetOptMaterial(quartz);
  //lens2.SetOptMaterial(vacuum);
  lens2.SetName("lens2");

 
  double thick1 = lens_sphere.CenterPoint().Z();
  cout<<" thickness lens1 = "<<thick1<<endl;
  double thick2 = lens2_sphere2.CenterPoint().Z()-lens2_sphere1.CenterPoint().Z();
  cout<<" thickness lens2 = "<<thick2<<endl;
  // create a box with a hole for the lense

  // recalculate focal length       >0
  //double ff2 = 1.0/((n2-1)*(1.0/(-radius1)-1.0/(-radius2)
  //	    +(n1-1)*thick1/(n1*(-radius1)*(-radius2))));
//cout<<" ff2 = "<<ff2<<endl;
  

  // hole:
  PndDrcSurfQuadFlatDiff hole_side1(lens_side1);
  hole_side1.SetPrintColor(1);
  hole_side1.SetName("hole_side1");
  PndDrcSurfQuadFlatDiff hole_side2(lens_side2);
  hole_side2.SetPrintColor(1);
  hole_side2.SetName("hole_side2");
  PndDrcSurfQuadFlatDiff hole_side3(lens_side3);
  hole_side3.SetPrintColor(1);
  hole_side3.SetName("hole_side3");
  PndDrcSurfQuadFlatDiff hole_side4(lens_side4);
  hole_side4.SetVerbosity(0);
  hole_side4.SetPrintColor(1);
  hole_side4.SetName("hole_side4");
  hole_side4.SetNameSurface1("hole_base");
  hole_side4.SetNameSurface2("hole_sphere");

  PndDrcSurfPolySphere hole_sphere(lens_sphere);
  hole_sphere.SetVerbosity(0);
  hole_sphere.SetPrintColor(1);
  hole_sphere.SetName("hole_sphere");
  // hole2:
  PndDrcSurfQuadFlatDiff hole2_side1(lens2_side1);
  hole2_side1.SetPrintColor(1);
  hole2_side1.SetName("hole2_side1");
  PndDrcSurfQuadFlatDiff hole2_side2(lens2_side2);
  hole2_side2.SetPrintColor(1);
  hole2_side2.SetName("hole2_side2");
  PndDrcSurfQuadFlatDiff hole2_side3(lens2_side3);
  hole2_side3.SetPrintColor(1);
  hole2_side3.SetName("hole2_side3");
  PndDrcSurfQuadFlatDiff hole2_side4(lens2_side4);
  hole2_side4.SetPrintColor(1);
  hole2_side4.SetName("hole2_side4");
  PndDrcSurfPolySphere hole2_sphere2(lens2_sphere2);
  hole2_sphere2.SetVerbosity(0);
  hole2_sphere2.SetPrintColor(1);
  hole2_sphere2.SetName("hole2_sphere2");


  XYZPoint p1(-300.0, +300.0, 0);  //            p5----------p8
  XYZPoint p2(-300.0, -300.0, 0);  //           /|          /|
  XYZPoint p3(+300.0, -300.0, 0);  //          / |         / |
  XYZPoint p4(+300.0, +300.0, 0);  //         /  p6-------/--p7
  //                                              /  /        /  /
  //                                             /  /        /  /
  //                                            /  /        /  /
  //                                           /  /        /  /
  XYZPoint p5(-300.0, +300.0, +288); // p1-s1--s2---p4 /
  XYZPoint p6(-300.0, -300.0, +288); // | /s5--s6   | /
  XYZPoint p7(+300.0, -300.0, +288); // |/ s7--s8   |/
  XYZPoint p8(+300.0, +300.0, +288); // p2-s3--s4--p3 
  //cout<<" f="<<f<<endl;
  //XYZPoint p5(-300.0, +300.0, f); // p1-s1--s2---p4 /
  //XYZPoint p6(-300.0, -300.0, f); // | /s5--s6   | /
  //XYZPoint p7(+300.0, -300.0, f); // |/ s7--s8   |/
  //XYZPoint p8(+300.0, +300.0, f); // p2-s3--s4--p3 
  XYZPoint s1(-slab_width/2, +300, 0);
  XYZPoint s2(+slab_width/2, +300, 0);
  XYZPoint s3(-slab_width/2, -300, 0);
  XYZPoint s4(+slab_width/2, -300, 0);
  XYZPoint s5(-slab_width/2, +slab_height/2, 0);
  XYZPoint s6(+slab_width/2, +slab_height/2, 0);
  XYZPoint s7(-slab_width/2, -slab_height/2, 0);
  XYZPoint s8(+slab_width/2, -slab_height/2, 0);

  PndDrcSurfPolyFlat box_side1;
  box_side1.AddPoint(p1);
  box_side1.AddPoint(p2);
  box_side1.AddPoint(s3);
  box_side1.AddPoint(s1);
  box_side1.SetName("box_side1");
  PndDrcSurfPolyFlat box_side2;
  box_side2.AddPoint(s2);
  box_side2.AddPoint(p4);
  box_side2.AddPoint(p3);
  box_side2.AddPoint(s4);
  box_side2.SetName("box_side2");
  PndDrcSurfPolyFlat box_side3;
  box_side3.AddPoint(s1);
  box_side3.AddPoint(s2);
  box_side3.AddPoint(s6);
  box_side3.AddPoint(s5);
  box_side3.SetName("box_side3");
  PndDrcSurfPolyFlat box_side4;
  box_side4.AddPoint(s7);
  box_side4.AddPoint(s8);
  box_side4.AddPoint(s4);
  box_side4.AddPoint(s3);
  box_side4.SetName("box_side4");

  PndDrcSurfPolyFlat box_side5;
  box_side5.AddPoint(p1);
  box_side5.AddPoint(p2);
  box_side5.AddPoint(p6);
  box_side5.AddPoint(p5);
  box_side5.SetReflectivity(refl_none);
  box_side5.SetName("box_side5");
  PndDrcSurfPolyFlat box_side6;
  box_side6.AddPoint(p2);
  box_side6.AddPoint(p3);
  box_side6.AddPoint(p7);
  box_side6.AddPoint(p6);
  box_side6.SetReflectivity(refl_none);
  box_side6.SetName("box_side6");
  PndDrcSurfPolyFlat box_side7;
  box_side7.AddPoint(p3);
  box_side7.AddPoint(p4);
  box_side7.AddPoint(p8);
  box_side7.AddPoint(p7);
  box_side7.SetReflectivity(refl_none);
  box_side7.SetName("box_side7");
  PndDrcSurfPolyFlat box_side8;
  box_side8.AddPoint(p1);
  box_side8.AddPoint(p4);
  box_side8.AddPoint(p8);
  box_side8.AddPoint(p5);
  box_side8.SetReflectivity(refl_none);
  box_side8.SetName("box_side8");
  PndDrcSurfPolyFlat box_side9;
  box_side9.AddPoint(p5);
  box_side9.AddPoint(p6);
  box_side9.AddPoint(p7);
  box_side9.AddPoint(p8);
  box_side9.SetPixel();
  box_side9.SetName("box_side9");

  //PndDrcSurfPolyFlat box_side10(box_side9);
  //box_side10.SetName("box_side10");
  //Transform3D trans10(XYZVector(0,0,-50));
  //box_side10.AddTransform(trans10);
  //box_side10.SetReflectivity(refl_none);


  PndDrcOptVol box;
  box.SetVerbosity(0);
  box.AddSurface(box_side1);
  box.AddSurface(box_side2);
  box.AddSurface(box_side3);
  box.AddSurface(box_side4);
  box.AddSurface(box_side5);
  box.AddSurface(box_side6);
  box.AddSurface(box_side7);
  box.AddSurface(box_side8);
  box.AddSurface(box_side9);
  //box.AddSurface(box_side10);
  box.AddSurface(hole_side1);
  box.AddSurface(hole_side2);
  box.AddSurface(hole_side3);
  box.AddSurface(hole_side4);
  box.AddSurface(hole2_sphere2);
  box.AddSurface(hole2_side1);
  box.AddSurface(hole2_side2);
  box.AddSurface(hole2_side3);
  box.AddSurface(hole2_side4);
  //box.SetOptMaterial(vacuum);
  box.SetOptMaterial(quartz);
  box.SetName("box");

  XYZPoint t5(-slab_width/2, +slab_height/2, -1000);
  XYZPoint t6(+slab_width/2, +slab_height/2, -1000);
  XYZPoint t7(-slab_width/2, -slab_height/2, -1000);
  XYZPoint t8(+slab_width/2, -slab_height/2, -1000);
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

  //slab_side1.SetReflectivity(refl_perfect);
  slab_side2.SetReflectivity(refl_perfect);
  slab_side3.SetReflectivity(refl_perfect);
  slab_side4.SetReflectivity(refl_perfect);
  slab_side5.SetReflectivity(refl_perfect);
  slab_side6.SetReflectivity(refl_perfect);

  PndDrcOptVol slab;
  slab.SetVerbosity(0);
  slab.AddSurface(slab_side1);
  slab.AddSurface(slab_side2);
  slab.AddSurface(slab_side3);
  slab.AddSurface(slab_side4);
  slab.AddSurface(slab_side5);
  slab.AddSurface(slab_side6);
  slab.SetOptMaterial(quartz);
  //slab.SetOptMaterial(vacuum);
  slab.SetName("slab");




  PndDrcOptDevSys opt_system;
  opt_system.SetNameCopyNumber("opt_system");
  opt_system.AddDevice(box);
  opt_system.AddDevice(slab);
  opt_system.AddDevice(lens);
  opt_system.AddDevice(lens2);

  // couple surface 1 of device 1 with surface 2 of device 2
  //                       dev1   dev2    surf1     surf2
  opt_system.CoupleDevice("lens","lens2","lens_sphere","lens2_sphere1");
  opt_system.CoupleDevice("lens2","box","lens2_sphere2","hole2_sphere2");
  opt_system.CoupleDevice("lens2","box","lens2_side1", "hole2_side1");
  opt_system.CoupleDevice("lens2","box","lens2_side2", "hole2_side2");
  opt_system.CoupleDevice("lens2","box","lens2_side3", "hole2_side3");
  opt_system.CoupleDevice("lens2","box","lens2_side4", "hole2_side4");
  opt_system.CoupleDevice("lens","box","lens_side1", "hole_side1");
  opt_system.CoupleDevice("lens","box","lens_side2", "hole_side2");
  opt_system.CoupleDevice("lens","box","lens_side3", "hole_side3");
  opt_system.CoupleDevice("lens","box","lens_side4", "hole_side4");
  opt_system.CoupleDevice("slab","lens","slab_side1", "lens_base");




  PndDrcOptDevManager* manager = new PndDrcOptDevManager();
  manager->AddDeviceSystem(opt_system);


  fstream geo;
  geo.open("Geo.C",std::ios::out);
  geo<<"{"<<endl;
  geo<<"    TCanvas *c1 = new TCanvas(\"c1\"); "<<endl;
  if ( ((TROOT*)gROOT)->GetVersionInt() < 51600)
    {
      geo<<"    TView *view = new TView(1);"<<endl;
    }
  else
    {  
      geo<<"    TView *view = TView::CreateView(1);"<<endl;
    }
  //geo<<"    view->SetRange(-450,-450,-300,450,450,500);"<<endl;
  geo<<"    view->SetRange(-450,-450,-450,450,450,450);"<<endl;
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
  manager->Print(geo);
  

  //geo<<"}"<<endl;
  //exit(0);

  list<PndDrcPhoton> list_photon; // get list

  PndDrcPhoton ph;


  bool photons_exist = false;
  if (ioption==1)  
    {
      slab_width*=0.5;
      slab_height*=0.5;

      for (double xx=-slab_width/2; xx<=slab_width/2; xx+=slab_width/4)
	{
	  for (double yy=-slab_height/2; yy<=slab_height/2; yy+=slab_height/4)
	    {
	      ph.SetPosition(XYZPoint(xx,yy,dist-500));
	      ph.SetDirection(XYZVector(0,0,1));
	      ph.SetWavelength(lambda);
	      list_photon.push_back(ph);
	    }
	}
      photons_exist = true;
      manager->SetPhotonList(list_photon,"slab","opt_system",0,0);
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
      manager->SetPhotonList(list_photon,"slab","opt_system");
    }
  if (ioption==3)
    {
      XYZPoint pos(0,-10,-45.0);
      XYZVector dir(0.0,1,-3); 
      double   beta = 0.80;
      photons_exist = manager->Cerenkov(pos,dir,beta,5000,200); // generate photons
    }
  if (ioption==4)  
    {
      TRandom ran;
      for (int ii=0; ii<10; ii++)
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
      manager->SetPhotonList(list_photon,"slab","opt_system");
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
      manager->SetPhotonList(list_photon,"slab","opt_system");
    }
  if (ioption==6)  
    {
      // straight lines.
      TRandom ran;
      for (double angle=5; angle<=30.5; angle+=5)
	{

	  cout<<" angle = "<<angle<<endl;
	  for (double lambda1=630; lambda1>329; lambda1-=10)
	    {
	      //double lambda1=lambda;
	      // go in x dir
	      double y1 = tan(angle*pi/180);
	      double x1 = y1;
	      double scale = 3/angle;
	      for (double y=-y1; y<= y1; y+= 2*y1/50*scale)
		{
		  //double xx=ran.Uniform(-0.5*slab_width,0.5*slab_width);
		  //double yy=ran.Uniform(-0.5*slab_height,0.5*slab_height);
		  ph.SetPosition(XYZPoint(0,0,-0.5));
		  double x = x1;
		  ph.SetDirection(XYZVector(x,y,1).Unit());
		  ph.SetWavelength(lambda1);
		  list_photon.push_back(ph);
		  ph.SetDirection(XYZVector(-x,y,1).Unit());
		  ph.SetWavelength(lambda1);
		  list_photon.push_back(ph);
		}
	      for (double x=-x1; x<= x1; x+= 2*y1/50*scale)
		{
		  //double xx=ran.Uniform(-0.5*slab_width,0.5*slab_width);
		  //double yy=ran.Uniform(-0.5*slab_height,0.5*slab_height);
		  ph.SetPosition(XYZPoint(0,0,-0.5));
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

  fstream scr;
  scr.open("Screen.C",std::ios::out);
  scr<<"{"<<endl;
  scr<<"    TCanvas *c1 = new TCanvas(\"c1\"); "<<endl;
  scr<<"    TH1F *hgr = new TH1F(\"hgr\",\"spatial position [mm]\",600,-300,300);"<<endl;
  scr<<"    hgr->SetMarkerStyle(7);"<<endl;
  scr<<"    hgr->SetMarkerSize(0.5);"<<endl;
  scr<<"    hgr->SetMinimum(-300);"<<endl;
  scr<<"    hgr->SetMaximum(+300);"<<endl;
  scr<<"    hgr->Draw(\"POL\");"<<endl;
  int icnt_measured = 0;
  int icnt_flying   = 0;
  int icnt_lost     = 0;
  int icnt_absorbed = 0;

  //TRotation rotInv_screen = rot_screen.Inverse();
  list<PndDrcPhoton>::iterator iph;
  PndDrcPhoton ph_old;

  double dista,distb;
  for(iph=list_photon.begin(); iph != list_photon.end(); ++iph) 
    {


      //(*iph).print();
      if ((*iph).Fate()==Drc::kPhotMeasured)
	{
	  icnt_measured++;
	  XYZPoint pos = (*iph).Position();
	  //pos -= shift_screen;
	  //pos.Transform(rotInv_screen);

	  double x     = pos.X();
	  double y     = pos.Y();

	  //cout<<" i,p,d="<<icnt_measured<<" "
	  //<<(*iph).position()<<" "<<(*iph).direction()<<endl;
	  //--------------------
	  list<const PndDrcSurfAbs*> list_surfaces = (*iph).SurfaceList();
	  list<const PndDrcSurfAbs*>::iterator isurf;
	  cout<<"---------"<<endl;
	  for(isurf=list_surfaces.begin(); isurf != list_surfaces.end(); ++isurf) 
	    {
	      cout<<(*isurf)->Name()<<endl;  
	    }
	  //--------------------

	  //--------------------
	  list<double> list_posx = (*iph).PositionXlist();
	  list<double> list_posy = (*iph).PositionYlist();
	  list<double> list_posz = (*iph).PositionZlist();
	  list<double>::iterator iposx;
	  list<double>::iterator iposy;
	  list<double>::iterator iposz;
	  cout<<"---------"<<endl;
	  for(
	      iposx=list_posx.begin(),
		iposy=list_posy.begin(),
		iposz=list_posz.begin(); 
	      iposx != list_posx.end(); 
	      ++iposx,++iposy,++iposz) 
	    {
	      cout<<(*iposx)<<" "
		  <<(*iposy)<<" "
		  <<(*iposz)<<endl;  
	    }
	  //--------------------







	  scr<<"    TMarker* t = new TMarker("<<x<<","<<y<<",20);"<<endl;
	  scr<<"    t->SetMarkerStyle(7);"<<endl;
	  scr<<"    t->SetMarkerColor("
		<<(*iph).ColorNumber((*iph).Wavelength())
		<<");"<<endl;
	  scr<<"    t->SetMarkerSize(0.7);"<<endl;
	  scr<<"    t->Draw();"<<endl;
	  ph_old = (*iph);
	}
      else if ((*iph).Fate()==Drc::kPhotFlying)   icnt_flying++; // should never happen.
      else if ((*iph).Fate()==Drc::kPhotAbsorbed) icnt_absorbed++;
      else                                       
	{
	  icnt_lost++;
	  //XYZPoint pos = (*iph).position();
	  //cout<<pos<<endl;
	}
    }
  cout<<" ratio = "<<dista/distb<<endl;
  scr<<"}"<<endl;
  scr.close();

  int icnt = icnt_measured+icnt_flying+icnt_lost+icnt_absorbed;
  cout<<" generated photons: "<<icnt<<endl;
  cout<<" measured  photons: "<<icnt_measured<<endl;
  cout<<" absorbed  photons: "<<icnt_absorbed<<endl;
  cout<<" lost      photons: "<<icnt_lost<<endl;
  delete manager;

  return EXIT_SUCCESS;

}
