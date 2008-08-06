// ----------------------------------------------------
// This file belongs to the ray tracing framework
// for the use with Cherenkov detectors
// 
// created 2007
//-----------------------------------------------------

#include "PndDrcUtil.h"
#include <algorithm>
#include "TVector3.h"
//
//#include "Math/Vector3D.h"
//using ROOT::Math::XYZVector;
//
//#include "Math/Point3D.h"
//using ROOT::Math::XYZPoint;
//
//#include "Math/Transform3D.h"
//using ROOT::Math::Transform3D;
//
//#include "Math/RotationX.h"
//using ROOT::Math::RotationX;
//#include "Math/RotationY.h"
//using ROOT::Math::RotationY;
//#include "Math/RotationZ.h"
//using ROOT::Math::RotationZ;
//#include "Math/Rotation3D.h"
//using ROOT::Math::Rotation3D;
//
//
//#include <iostream>
//using std::cout;
//using std::cerr;
//using std::cin;
//using std::endl;
//
//#include <valarray>
//using std::valarray;
//
//#include <vector>
//using std::vector;
//
//#include <string>
//using std::string;
//
//#include <list>
//using std::list;
//
//#include <map>
//using std::map;
//
//#include <limits>
//
//#include <fstream>
//using std::fstream;
//using std::ostream;
//using std::istream;
//
//#include <utility>
//using std::pair;

string itoa1(int i)
{
  if (i<0 || i>9)
    {
      std::cerr<<" *** itoa: i="<<i<<" is larger 9 ... aborting"<<std::endl;
      exit(EXIT_FAILURE);
    }

  if (i<0 ) return "#";
  if (i==0) return "0";
  if (i==1) return "1";
  if (i==2) return "2";
  if (i==3) return "3";
  if (i==4) return "4";
  if (i==5) return "5";
  if (i==6) return "6";
  if (i==7) return "7";
  if (i==8) return "8";
  if (i==9) return "9";

  return "X"; // dummy
  
}      

string itoa(int i, int idigits)
{
  using std::string;
  
  // if you increase imax, adjust the following lines
  //const int    imax    =  99999999;
 const int    kDigMax =  8;
  //int iarr[kDigMax-1]  = {-1,-1,-1,-1,-1};
  // up to here

  const int kTen = 10;
  

  std::string sresult = "";
  
 
  // get number of digits
  int idigits1 = 0;
  int i1 = abs(i);
  while (idigits1++<=kDigMax)
    {
      i1/=10;
      if (i1==0) break;
    }
  
  // too many digits (idigits>0) or i<0
  if (idigits!=0 && (idigits1>idigits || i<0))
    {
      int idigits2 = idigits;
      while ((idigits2--)>0)
	{
	  sresult += "#";
	}
      return sresult;
    }

  // too many digits (idigits>0) or i<0
  if (idigits==0 && (idigits1>kDigMax || i<0))
    {
      //idigits1 = kDigMax;
      
      while ((idigits1--)>0)
	{
	  sresult += "#";
	}
      return sresult;
    }

  // free number of digits
  if (idigits==0)
    {
      i1 = i;
      while ((idigits1--)>0)
	{
	  int i2 = i1/(int)pow(kTen,idigits1);
	  sresult += itoa1(i2);
	  i1 -= i2*(int)pow(kTen,idigits1);
	}
      return sresult;      
    }
  // fixed number of digits
  else
    {
      i1 = i;
      int idigits2 = idigits;
      while ((idigits2--)>0)
	{
	  int i2 = i1/(int)pow(kTen,idigits2);
	  sresult += itoa1(i2);
	  i1 -= i2*(int)pow(kTen,idigits2);
	}
      return sresult;      
    }
}

//---------------------------------------------------------
const XYZVector angle_tra(const XYZVector &p1,
			  const double    &the,
			  const double    &phi)
{

  double px = p1.X();
  double py = p1.Y();
  double pz = p1.Z();
  
  double p_abs,the_00,phi_00,sp,cp,st,ct,arg;
  double px_1,py_1,pz_1;
  
  XYZVector p2;

  p_abs = sqrt(px*px+py*py+pz*pz);          // calculate angles of p_in
  arg   = pz/p_abs;
  //if (arg >  1.0) arg =  1.0;
  //if (arg < -1.0) arg = -1.0;
  the_00 = acos(arg);
  //if (px==0 && py==0) {
  //phi_00 = 0.0;
  //}
  //else {
  phi_00 = atan2(px,py);
  //}

  //std::cout<<" inne = "<<the_00<<std::endl;
  
  px_1 = sin(the)*sin(phi)*p_abs;          
  py_1 = sin(the)*cos(phi)*p_abs;
  pz_1 = cos(the)         *p_abs;

  sp = sin(phi_00);                         // angles of p_in
  cp = cos(phi_00);
  st = sin(the_00);
  ct = cos(the_00);

  p2.SetX( px_1*cp  + py_1*ct*sp + pz_1*st*sp);  // rot vec by theta_00,phi_00
  p2.SetY(-px_1*sp  + py_1*ct*cp + pz_1*st*cp);
  p2.SetZ(          - py_1*st    + pz_1*ct   );

  return p2;
}

ostream& operator << (ostream &stream, const TVector3 &t)
{
  stream<<" X="<<t.X()<<" Y="<<t.Y()<<" Z= "<<t.Z();
  return stream;
}

ostream& operator << (ostream &stream, const XYZVector &t)
{
  stream<<" X="<<t.X()<<" Y="<<t.Y()<<" Z= "<<t.Z();
  return stream;
}
ostream& operator << (ostream &stream, const XYZPoint &t)
{
  stream<<" X="<<t.X()<<" Y="<<t.Y()<<" Z= "<<t.Z();
  return stream;
}
