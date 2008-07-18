// ----------------------------------------------------
// This file belongs to the ray tracing framework
// for the use with Cherenkov detectors
// 
// created 2007
//-----------------------------------------------------
#include "PndDrcConicSection.h"

//#include "TVector3.h"
//#include "TRotation.h"
//#include "TRandom.h"
#include "TMatrixD.h"
#include "TVectorD.h"
#include "TDecompSVD.h"
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
#include <iostream>
using std::cout;
//using std::cerr;
//using std::cin;
using std::endl;
//
//#include <valarray>
//using std::valarray;
//
//#include <fstream>
//using std::fstream;
//
//#include <string>
//using std::string;
//
//#include <list>
//using std::list;
//
//#include <vector>
//using std::vector;

#include <cmath>

//#include <cassert>
#include "assert.h"
//using std::assert;

//----------------------------------------------------------------------
PndDrcConicSection::PndDrcConicSection()
{
  fVerbosity = 0;
  fZ_plane   = 0.0;

  fTheta = 0;
  fPhi   = 0;
  fPsi   = 0;
  fX0    = 0;
  fY0    = 0;
}
//----------------------------------------------------------------------
void PndDrcConicSection::Copy(const PndDrcConicSection& s)
{
  fVerbosity = s.fVerbosity;
  fZ_plane   = s.fZ_plane;
  fTheta = s.fTheta;
  fPhi   = s.fPhi;
  fPsi   = s.fPsi;
  fX0    = s.fX0;
  fY0    = s.fY0;
  
}
//----------------------------------------------------------------------
PndDrcConicSection::PndDrcConicSection(const PndDrcConicSection& s)
{
  if (s.fVerbosity>=1) 
    cout<<"  PndDrcConicSection::PndDrcConicSection(const PndDrcConicSection&)"<<endl;
  Copy(s);
}
//----------------------------------------------------------------------
PndDrcConicSection& PndDrcConicSection::operator=(const PndDrcConicSection& s)
{
  if (s.fVerbosity>=1) 
    cout<<"  PndDrcConicSection::PndDrcConicSection& operator=(const PndDrcConicSection&)"
	<<endl;

    if (&s != this)
    {
      Copy(s);
    }

  return *this;  
}
//----------------------------------------------------------------------
PndDrcConicSection::~PndDrcConicSection()
{
  
}
//----------------------------------------------------------------------
bool PndDrcConicSection::ConicSection(double z0, vector<double>& x, vector<double>& y)
{
  
  assert(x.size()==5);
  assert(y.size()==5);

  static const double kPi=3.1415926535;

  double a,b,c,d,e,f,lam2;
  double phi1;



  {
    TMatrixD mat_a(5,5); 
    TVectorD vec_b(5);
 
    for (int i=0; i<5; i++)
      {   
	mat_a(i,0) =  x[i]*x[i];
	mat_a(i,1) =  x[i]*y[i];
	mat_a(i,2) =  x[i];
	mat_a(i,3) =  y[i];
	mat_a(i,4) =  1;
	vec_b(i)   = -y[i]*y[i];
      }    
    
    TDecompSVD svd(mat_a);
    bool ok;
    TVectorD s = svd.Solve(vec_b,ok);
    if (ok) 
      {
	a    = s[0];
	b    = 0.5*s[1];
	c    = 1; //y*y
	d    = 0.5*s[2];
	e    = 0.5*s[3];
	f    = s[4];
	
	phi1 = 0.5*atan2(-2*b,-(a-c));//+1*pi/2;

	if (fVerbosity>0) 
	  {
	    cout<<a<<" "<<b<<" "<<c<<" "<<d<<" "<<e<<" "<<f<<endl;
	    lam2 = a*c-b*b;
	    if (lam2>0)
	      {
		cout<<" ellipse   "<<" "<<lam2<<endl;
	      }
	    else if (lam2==0)
	      {
		cout<<" parabola  "<<" "<<lam2<<endl;
	      }
	    else if (lam2<0)
	      {
		cout<<" hyperbola "<<" "<<lam2<<endl;
	      }
	    
	    double delta = a*f*c - a*e*e -c*d*d -f*b*b +2*b*e*d;
	    cout<<" Delta = "<<delta<<endl;
	    cout<<" phi1 = "<<phi1*180/kPi<<endl;
	  }
	
	




      }  
  }


  for (int i=0; i<5; i++) Rotate(x[i],y[i],-phi1);


  {
    TMatrixD mat_a(5,5); 
    TVectorD vec_b(5);
    for (int i=0; i<5; i++)
      {   
	mat_a(i,0) =  x[i]*x[i];
	mat_a(i,1) =  x[i]*y[i];
	mat_a(i,2) =  x[i];
	mat_a(i,3) =  y[i];
	mat_a(i,4) =  1;
	vec_b(i)   = -y[i]*y[i];
      }
    TDecompSVD svd(mat_a);
    bool ok;
    TVectorD s = svd.Solve(vec_b,ok);
    if (ok) 
      {
	a    = s[0];
	b    = 0.5*s[1];
	c    = 1; //y*y
	d    = 0.5*s[2];
	e    = 0.5*s[3];
	f    = s[4];
	if (fVerbosity>0) cout<<a<<" "<<b<<" "<<c<<" "<<d<<" "<<e<<" "<<f<<endl;
      }  
  }

  
  if (fVerbosity>0) cout<<" d/a = "<<d/a<<endl;
  if (fVerbosity>0) cout<<" e/c = "<<e/c<<endl;

  if (fVerbosity>0) cout<<" correction: "<<endl;
  
  if (lam2 >0) // ellipse
    {
      if (d/a>0) 
	{
	  phi1+=kPi;
	  for (int i=0; i<5; i++) Rotate(x[i],y[i],-kPi);      
	  if (fVerbosity>0) cout<<" changed phi by to "<<phi1*180/kPi<<endl;
	}
    }
  else // hyperbola
    {
      if (fabs(a)<1)
	{
	  if (d/a<0) 
	    {
	      phi1+=kPi;
	      for (int i=0; i<5; i++) Rotate(x[i],y[i],-kPi);      
	      if (fVerbosity>0) cout<<" changed phi to "<<phi1*180/kPi<<endl;
	    }
	}
      else
	{
	  if (e/c>0)
	    {
	      phi1+=kPi/2;
	      for (int i=0; i<5; i++) Rotate(x[i],y[i],-kPi/2);      
	      if (fVerbosity>0) cout<<" changed phi to "<<phi1*180/kPi<<endl;
	    }
	  if (e/c<0)
	    {
	      phi1-=kPi/2;
	      for (int i=0; i<5; i++) Rotate(x[i],y[i],+kPi/2);      
	      if (fVerbosity>0) cout<<" changed phi to "<<phi1*180/kPi<<endl;
	    }
	  
	  
	}
      
    }
  
  {
    TMatrixD mat_a(5,5); 
    TVectorD vec_b(5);
    for (int i=0; i<5; i++)
      {   
	mat_a(i,0) =  x[i]*x[i];
	mat_a(i,1) =  x[i]*y[i];
	mat_a(i,2) =  x[i];
	mat_a(i,3) =  y[i];
	mat_a(i,4) =  1;
	vec_b(i)   = -y[i]*y[i];
      }
    TDecompSVD svd(mat_a);
    bool ok;
    TVectorD s = svd.Solve(vec_b,ok);
    if (ok) 
      {
	a    = s[0];
	b    = 0.5*s[1];
	c    = 1; //y*y
	d    = 0.5*s[2];
	e    = 0.5*s[3];
	f    = s[4];
	//cout<<a<<" "<<b<<" "<<c<<" "<<d<<" "<<e<<" "<<f<<endl;
      }  
  }


  if (fVerbosity>0) cout<<" d/a = "<<d/a<<endl;
  if (fVerbosity>0) cout<<" e/c = "<<e/c<<endl;

  double x_shift_old=d/a;
  double y_shift_old=e/c;

  for (int i=0; i<5; i++) {x[i] += d/a; y[i] += e/c;}



  {
    TMatrixD mat_a(5,5); 
    TVectorD vec_b(5);
 
    for (int i=0; i<5; i++)
      {   
	mat_a(i,0) =  x[i]*x[i];
	mat_a(i,1) =  x[i]*y[i];
	mat_a(i,2) =  x[i];
	mat_a(i,3) =  y[i];
	mat_a(i,4) =  1;
	vec_b(i)   = -y[i]*y[i];
      }    
    
    TDecompSVD svd(mat_a);
    bool ok;
    TVectorD s = svd.Solve(vec_b,ok);
    if (ok) 
      {
	a    = s[0];
	b    = 0.5*s[1];
	c    = 1; //y*y
	d    = 0.5*s[2];
	e    = 0.5*s[3];
	f    = s[4];
	
	if (fVerbosity>0) cout<<a<<" "<<b<<" "<<c<<" "<<d<<" "<<e<<" "<<f<<endl;
	
	

	if (fVerbosity>0) cout<<" -f/a -f/c "<<-f/a<<" "<<-f/c<<endl;

	if (fabs(f/a)<fabs(f/c))
	{
	  double tmp=a;
	  a=c;
	  c=tmp;
	  a/=c;
	  f/=c;
	  c=1;
	  //phi1 = kPi/2+phi1;
	}
	if (fVerbosity>0) cout<<a<<" "<<b<<" "<<c<<" "<<d<<" "<<e<<" "<<f<<endl;


	double aa=sqrt(fabs(f/a));
	double bb=sqrt(fabs(f/c));

	z0 *=-1;
	
	if (fVerbosity>0) cout<<" Halbachsen="<<aa<<" "<<bb<<endl;





	if (fVerbosity>0) cout<<" BB/AA "<<bb*bb/aa/aa<<endl;
	if (fVerbosity>0) cout<<" BB/A  "<<bb*bb/aa<<endl;

	
	double psi_calc = atan(bb*bb/(aa*(-z0)));
	if (fVerbosity>0) cout<<" psi   = "<<psi_calc*180/kPi<<endl;

	
	double theta_calc = acos(sqrt((a+tan(psi_calc)*tan(psi_calc))/
				      (1+tan(psi_calc)*tan(psi_calc))));
	//double theta_calc = acos(sqrt((B*B/A/A+tan(psi_calc)*tan(psi_calc))/
	//		      (1+tan(psi_calc)*tan(psi_calc))));
	if (fVerbosity>0) cout<<" theta = "<<theta_calc*180/kPi<<endl;

	double mx = -aa-z0*(tan(theta_calc)+tan(psi_calc-theta_calc));
	//double my = 0;
	
	// construct x0,y0

	double x0c = mx - (-z0)*tan(theta_calc);
	double y0c = 0;
	
	if (fVerbosity>0) cout<<" x0 = "<<x0c<<endl;
	if (fVerbosity>0) cout<<" y0 = "<<y0c<<endl;
	
	if (fVerbosity>0) cout<<" shift old x= "<<x_shift_old<<endl;
	if (fVerbosity>0) cout<<" shift old y= "<<y_shift_old<<endl;
	
	

	x0c -= x_shift_old;
	y0c -= y_shift_old;
	

	if (lam2<0) x0c+=2*aa;

	if (fVerbosity>0) cout<<" x0 = "<<x0c<<endl;
	if (fVerbosity>0) cout<<" y0 = "<<y0c<<endl;

	if (fVerbosity>0) cout<<" phi = "<<phi1*180/kPi<<endl;
	

	Rotate(x0c,y0c,phi1);
	
	
	if (fVerbosity>0) cout<<" x0 = "<<x0c<<endl;
	if (fVerbosity>0) cout<<" y0 = "<<y0c<<endl;
	//rotate(mx,my,-phi1);
	

	fX0 = x0c;
	fY0 = y0c;
	fPhi = phi1;
	fTheta = theta_calc;
	fPsi = psi_calc;
	
      }  
  }


  return true;
  

  
}
//----------------------------------------------------------------------
void PndDrcConicSection::Rotate(double& x, double& y, double phi)
{
  double xx = x;
  double yy = y;
  //phi=0;
  //cout<<" int1 "<<x<<" "<<y<<" "<<phi<<endl;
  x = (xx) * cos(phi) - (yy) * sin(phi) ;
  y = (xx) * sin(phi) + (yy) * cos(phi) ;
  //cout<<" int2 "<<x<<" "<<y<<" "<<phi<<endl;

}
