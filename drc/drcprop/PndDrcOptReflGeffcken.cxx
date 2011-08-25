// ----------------------------------------------------
// This file belongs to the ray tracing framework
// for the use with Cherenkov detectors
// 
// created 2011
//-----------------------------------------------------
#include <iostream>
#include <iomanip>
using std::cout;
//using std::cerr;
//using std::cin;
using std::endl;
using std::hex;


#include "PndDrcOptMatAbs.h"
#include "PndDrcOptMatMgF2.h"
#include "PndDrcOptReflGeffcken.h"
#include "PndDrcPhoton.h"

#include "matrix.h"

#include "PndDrcOptDev.h"

//----------------------------------------------------------------------
PndDrcOptReflGeffcken::PndDrcOptReflGeffcken()
{


  fLayerMaterialLow  = new PndDrcOptMatMgF2();
  fLayerMaterialHigh = new PndDrcOptMatMgF2();


  fLayerThicknessList.push_back(54.35);
  fLayerMaterialList.push_back(fLayerMaterialLow->Clone()); // since list is deleted separately
  
  
}
//----------------------------------------------------------------------
PndDrcOptReflGeffcken::~PndDrcOptReflGeffcken()
{

  if (fLayerMaterialLow)  delete fLayerMaterialLow;
  if (fLayerMaterialHigh) delete fLayerMaterialHigh;

  list<PndDrcOptMatAbs*>::const_iterator kLayerMaterialList;

  for(kLayerMaterialList  = fLayerMaterialList.begin();
    kLayerMaterialList != fLayerMaterialList.end(); 
    ++kLayerMaterialList) 
  {
    delete (*kLayerMaterialList);
  }
}

//----------------------------------------------------------------------
PndDrcOptReflGeffcken* PndDrcOptReflGeffcken::Clone() const
{
  return new PndDrcOptReflGeffcken(*this);
}
//----------------------------------------------------------------------
void PndDrcOptReflGeffcken::Copy(const PndDrcOptReflGeffcken& s)
{
  

  list<PndDrcOptMatAbs*>::const_iterator kLayerMaterialList;

  for(kLayerMaterialList  = s.fLayerMaterialList.begin();
      kLayerMaterialList != s.fLayerMaterialList.end(); 
      ++kLayerMaterialList) 
    {
      const PndDrcOptMatAbs* tmp = (*kLayerMaterialList);
      PndDrcOptMatAbs* tmp1 = tmp->Clone();

      fLayerMaterialList.push_back(tmp1);
    }



  //fLayerMaterialList       = s.fLayerMaterialList;
  fLayerThicknessList      = s.fLayerThicknessList;
  fLayerMaterialLow        = (s.fLayerMaterialLow)->Clone();
  fLayerMaterialHigh       = (s.fLayerMaterialHigh)->Clone();
 
}
//----------------------------------------------------------------------
PndDrcOptReflGeffcken::PndDrcOptReflGeffcken(const PndDrcOptReflGeffcken& s) : PndDrcOptReflAbs(s)
{
  Copy(s);
}
//----------------------------------------------------------------------
PndDrcOptReflGeffcken& PndDrcOptReflGeffcken::operator=(const PndDrcOptReflGeffcken& s)
{


  if (s.fVerbosity>=1) cout<<"  PndDrcOptReflGeffcken::operator="
        <<"(const PndDrcOptReflGeffcken&) "<<endl;
  if (&s != this) Copy(s);
  return *this;

}
//----------------------------------------------------------------------
const Drc::Reflectivity PndDrcOptReflGeffcken::Query(const PndDrcPhoton&    ph,
						     const XYZVector        normal,
						     const double           n_next,
						     const Drc::ReflDir     direction) const
{

  if (fRan.Uniform() < ReflProb(ph,normal,n_next,direction) )
    {
      return Drc::ReflReflected;
    }
  else
    {
      return Drc::ReflTransmitted;
    }
}

//----------------------------------------------------------------------
const double PndDrcOptReflGeffcken::ReflProb(const PndDrcPhoton&    ph,
					     const XYZVector        normal,
					     const double           n_next,
					     const Drc::ReflDir     direction) const
{
  const double pi = 3.1415926535;

  double lambda   = ph.Wavelength();   // nm
  double k0       = 2*pi/lambda;       // nm-1

  // go from n0 (air) to glass ns (substrate)
  // n1 is the AR layer

  double n0 = ph.Device()->OptMaterial().RefIndex(lambda);
  double ns = n_next;


  list<PndDrcOptMatAbs*>::const_iterator  kLayerMaterial=fLayerMaterialList.begin();
  list<double>::const_iterator            kLayerThickness=fLayerThicknessList.begin();


  double n1 = (*kLayerMaterial)->RefIndex(lambda);
  
  

  double costh     = ph.Direction().X()*normal.X();
  costh           += ph.Direction().Y()*normal.Y();
  costh           += ph.Direction().Z()*normal.Z();  

  double theta_i1  = acos(costh) * pi/180;
  double theta_i2  = asin(n0/n1*sin(theta_i1));
  double theta_t2  = asin(n1/ns*sin(theta_i2));

  double e0bymu0 = 1.0;//????

  double d  = (*kLayerThickness);           //54.35;//300.0 / n1 / cos(theta_i2) /4; 
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

  return real(r*conj(r));
  
  

  //std::cout<<lambda<<" "<<r<<" "<<r*conj(r)<<std::endl;


  //complex <double> help = Y0*m11+Y0*Ys*m12-m21-Ys*m22;
  //help /= Y0*m11+Y0*Ys*m12+m21+Ys*m22;
  //cout<<help<<" "<<help*conj(help)<<endl;


}
