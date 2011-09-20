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


  // electric & magnetic vector
  double theta_s  = asin((n0/ns)*sin(acos(costh)));
  double Ys       = ns  * cos(theta_s);
  complex <double> Ein(100.0,0);
  complex <double> Hin(100.0*Ys,0);
  complex <double> Eout;
  complex <double> Hout;

  double Y0 = n0  * cos(acos(costh));


  complex <double> out1;
  complex <double> out2;

  for (int ilayer = istart; ilayer != iend; ilayer += istep)
    {
      n1 = fLayerMaterialVector[ilayer]->RefIndex(lambda);
      
      double theta1 = asin((n0/n1)*sin(acos(costh))); 
      double k = 2*pi/lambda;  // nm-1
      double h = fLayerThicknessVector[ilayer] * cos(theta1) / n1;
      double e0bymu0 = 1.0;//????
      double Y = e0bymu0 * n1  * cos(theta1);
      complex <double> m11_6(cos(k*h) ,                 0);
      complex <double> m12_6(0               , sin(k*h)/Y);
      complex <double> m21_6(0               , sin(k*h)*Y);
      complex <double> m22_6(cos(k*h) ,                 0);
      Matrix M(m11_6,m12_6,m21_6,m22_6);
      
      M.product(Eout,Hout,Ein,Hin);
     
      Ein = Eout;
      Hin = Hout;
      //complex <double> er1 = (Eout-Hout/Y0)/2;
      //complex <double> ei1 = (Eout+Hout/Y0)/2;
  
      //complex <double> r = er1/ei1;
      //cout<<" ilayer,r,n ="<<ilayer<<" "<<real(r*conj(r))<<" "<<n1<<endl;
    }
  
  //cout<< in1<<" "<< in2<<endl;
  //cout<<out1<<" "<<out2<<endl;
  
  complex <double> er1 = (Eout-Hout/Y0)/2;
  complex <double> ei1 = (Eout+Hout/Y0)/2;
  
  complex <double> r = er1/ei1;
    
  return real(r*conj(r));
  


}
