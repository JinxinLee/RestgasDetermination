// ----------------------------------------------------
// This file belongs to the ray tracing framework
// for the use with Cherenkov detectors
// 
// created 2011
//-----------------------------------------------------
#include "PndDrcOptReflGeffcken.h"
#include "PndDrcPhoton.h"

#include "matrix.h"


//----------------------------------------------------------------------
PndDrcOptReflGeffcken* PndDrcOptReflGeffcken::Clone() const
{
  return new PndDrcOptReflGeffcken(*this);
}
//----------------------------------------------------------------------
const Drc::Reflectivity PndDrcOptReflGeffcken::Query(const PndDrcPhoton&    ph,
						     const XYZVector        normal,
						     const double           n_next,
						     const Drc::ReflDir     direction) const
{

  double lambda = ph.Wavelength();
  double pi = 3.1415926535;
  
  // go from n0 (air to n1)

  double n0 = 1.0;
  double n1 = 1.38;
  
  double ns = 1.5;
  //double lambda = ph.Wavelength();
  

  double theta_i1  = 0 * pi/180;
  double theta_i2  = asin(n0/n1*sin(theta_i1));
  double theta_t2  = asin(n1/ns*sin(theta_i2));

  double e0bymu0 = 1.0;//????

  double k0 = 2*pi/lambda;  // nm-1
  double d  = 54.35;//300.0 / n1 / cos(theta_i2) /4; 
  double h  = d * n1 / cos(theta_i2);
  

  //cout<<k0*h<<endl;

  double Y0 = e0bymu0 * n0 * cos(theta_i1);
  double Y1 = e0bymu0 * n1 * cos(theta_i2);
  double Ys = e0bymu0 * ns * cos(theta_t2);
  

  complex <double> m11(cos(k0*h) , 0);
  complex <double> m12(0         , sin(k0*h)/Y1);
  complex <double> m21(0         , sin(k0*h)*Y1);
  complex <double> m22(cos(k0*h) , 0);
  

  Matrix M(m11,m12,m21,m22);
  
  complex <double> in1(1.0,0);
  complex <double> in2(1.0*Ys,0);
  complex <double> out1;
  complex <double> out2;
  
  M.product(out1,out2,in1,in2);
  
  //cout<< in1<<" "<< in2<<endl;
  //cout<<out1<<" "<<out2<<endl;

  complex <double> er1 = (out1-out2/Y0)/2;
  complex <double> ei1 = (out1+out2/Y0)/2;
  
  complex <double> r = er1/ei1;

  
  if (fRan.Uniform()< r*conj(r)) 
    {
      return Drc::ReflReflected;
      
    }
  else
    {
      return Drc::ReflTransmitted;
    }
  

  //std::cout<<lambda<<" "<<r<<" "<<r*conj(r)<<std::endl;


  //complex <double> help = Y0*m11+Y0*Ys*m12-m21-Ys*m22;
  //help /= Y0*m11+Y0*Ys*m12+m21+Ys*m22;
  //cout<<help<<" "<<help*conj(help)<<endl;



      return Drc::ReflAbsorbed;
}
