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
#include "PndDrcOptMatLithotecQ0.h"
#include "PndDrcOptMatTiO2.h"
#include "PndDrcOptReflGeffcken.h"
#include "PndDrcPhoton.h"

#include "matrix.h"

#include "PndDrcOptDev.h"

//----------------------------------------------------------------------
PndDrcOptReflGeffcken::PndDrcOptReflGeffcken()
{


  fLayerMaterialLow  = new PndDrcOptMatLithotecQ0();
  fLayerMaterialHigh = new PndDrcOptMatTiO2();

  double quarterlambda = 520/4;
  
  // 1st layer is close to object (substrate)
  fLayerThicknessVector.push_back(0.231*quarterlambda);
  fLayerMaterialVector.push_back(fLayerMaterialHigh);
  
  fLayerThicknessVector.push_back(0.431*quarterlambda);
  fLayerMaterialVector.push_back(fLayerMaterialLow);

  fLayerThicknessVector.push_back(0.231*quarterlambda);
  fLayerMaterialVector.push_back(fLayerMaterialHigh);

  fLayerThicknessVector.push_back(1.000*quarterlambda);
  fLayerMaterialVector.push_back(fLayerMaterialLow);
  // last layer is outside layer
  
}
//----------------------------------------------------------------------
PndDrcOptReflGeffcken::~PndDrcOptReflGeffcken()
{

  if (fLayerMaterialLow)  delete fLayerMaterialLow;
  if (fLayerMaterialHigh) delete fLayerMaterialHigh;

  //vector<PndDrcOptMatAbs*>::const_iterator kLayerMaterialVector;

  //for(kLayerMaterialVector  = fLayerMaterialVector.begin();
  //kLayerMaterialVector != fLayerMaterialVector.end(); 
  //++kLayerMaterialVector) 
  //{
  //delete (*kLayerMaterialVector);
  //}
}

//----------------------------------------------------------------------
PndDrcOptReflGeffcken* PndDrcOptReflGeffcken::Clone() const
{
  return new PndDrcOptReflGeffcken(*this);
}
//----------------------------------------------------------------------
void PndDrcOptReflGeffcken::Copy(const PndDrcOptReflGeffcken& s)
{
  

  vector<PndDrcOptMatAbs*>::const_iterator kLayerMaterialVector;

  for(kLayerMaterialVector  = s.fLayerMaterialVector.begin();
      kLayerMaterialVector != s.fLayerMaterialVector.end(); 
      ++kLayerMaterialVector) 
    {
      const PndDrcOptMatAbs* tmp = (*kLayerMaterialVector);
      PndDrcOptMatAbs* tmp1 = tmp->Clone();

      fLayerMaterialVector.push_back(tmp1);
    }



  //fLayerMaterialVector       = s.fLayerMaterialVector;
  fLayerThicknessVector      = s.fLayerThicknessVector;
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

  double n_this = ph.Device()->OptMaterial().RefIndex(lambda);  // this refractive index



  double n_smallest = (fLayerMaterialLow->RefIndex(lambda) < n_next) ? 
    fLayerMaterialLow->RefIndex(lambda) : n_next;
      
  int istart = 0;
  int iend   = 0;
  int istep  = 0;
  
  // check if photons gets internally reflected
  double costh =  fabs(ph.Direction().X()*normal.X()+
		       ph.Direction().Y()*normal.Y()+
		       ph.Direction().Z()*normal.Z());
  if (n_this > n_smallest && acos(costh)*n_this/n_smallest > 1 ) return 1.0;
  
  if (direction == Drc::ReflIn)
    {// entering the volume
      istart = 0;
      iend   = fLayerThicknessVector.size(); // number of elements
      istep  = +1;
    }
  else
    {// leaving the volume
      istart = fLayerThicknessVector.size();
      iend   = 0;
      istep  = -1;
    }
  

  double n0 = n_this;
  double ns = n_next;
  double n1 = 0;
  double Y0,Y1,Ys;
  
  complex <double> out1;
  complex <double> out2;

  for (int ilayer = istart; ilayer != iend; ilayer += istep)
    {
      n1 = fLayerMaterialVector[ilayer]->RefIndex(lambda);
      
      double theta_i1  = acos(costh);
      double theta_i2  = asin(n0/n1*sin(theta_i1));
      double theta_t2  = asin(n1/ns*sin(theta_i2));
      
      double e0bymu0 = 1.0;//????
      
      double d  = fLayerThicknessVector[ilayer];
      double h  = d * n1 * cos(theta_i2);
      
      
      
      //double n1 = 0;// (*kLayerMaterial)->RefIndex(lambda);
      //double n0,ns;
      
      
      

      

      //cout<<k0*h<<endl;
      
      Y0 = e0bymu0 * n0 * cos(theta_i1);
      Y1 = e0bymu0 * n1 * cos(theta_i2);
      Ys = e0bymu0 * ns * cos(theta_t2);
      
      
      complex <double> m11(cos(k0*h) , 0);
      complex <double> m12(0         , sin(k0*h)/Y1);
      complex <double> m21(0         , sin(k0*h)*Y1);
      complex <double> m22(cos(k0*h) , 0);
      
      
      Matrix M(m11,m12,m21,m22);
      
      complex <double> in1(1.0,0);
      complex <double> in2(1.0*Ys,0);
      
      M.product(out1,out2,in1,in2);
      
    }
  
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
