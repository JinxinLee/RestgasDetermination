// ----------------------------------------------------
// This file belongs to the ray tracing framework
// for the use with Cherenkov detectors
// 
// created 2011
//-----------------------------------------------------

#include <iostream>
#include <iomanip>
using std::cout;
using std::cerr;
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
  //double ll;
  //std::cin>>ll;
  double quarterlambda = 450/4; // that is vaccuum wave length
  // normally one has to take the wavelength within the material.
  // This cancels later with h = n*d*cos(th)
  
  // 1st layer is close to object (substrate)
  fLayerThicknessVector.push_back(0.231*quarterlambda);
  fLayerMaterialVector.push_back(fLayerMaterialHigh);
  
  fLayerThicknessVector.push_back(0.431*quarterlambda);
  fLayerMaterialVector.push_back(fLayerMaterialLow);

  fLayerThicknessVector.push_back(2.115*quarterlambda);
  fLayerMaterialVector.push_back(fLayerMaterialHigh);

  fLayerThicknessVector.push_back(1.000*quarterlambda);
  fLayerMaterialVector.push_back(fLayerMaterialLow);
  // last layer is outside layer
  
  fRefIndConst = false;
  
 
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

  fRefIndConst             = s.fRefIndConst;
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

  double ran = gRandom->Uniform();
  double reflprob =  ReflProb(ph,normal,n_next,direction);
  
  //if (ran>=reflprob) cout<<" ran,refl_prob "<<ran<<" "<<reflprob<<endl; //###
  
  if ( ran < reflprob )
    {
      return Drc::ReflReflected;
    }
  else
    {
      //cout<<" transmitted"<<endl;//###
      
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


  

  int fac_orig = 1;
  if (fRefIndConst) fac_orig=-1; // -1: factor to get constant refractive indices for check with Geffckens
  //                                    original curve
  
  double lambda   = ph.Wavelength();   // nm

  // go from n0 (air) to glass ns (substrate)
  // n1 is the AR layer

  double n_this = ph.Device()->OptMaterial().RefIndex(fac_orig*lambda);  // this refractive index



  double n_smallest = (fLayerMaterialLow->RefIndex(fac_orig*lambda) < n_next) ? 
    fLayerMaterialLow->RefIndex(fac_orig*lambda) : n_next;
      
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

      // runs 0...layer-1 
    }
  else
    {// leaving the volume
      istart = fLayerThicknessVector.size()-1;
      iend   = -1;
      istep  = -1;
      // runs layer-1...0
    }
  

  double n0 = n_this;
  double ns = n_next;
  double n1 = 0;

  // electric & magnetic vector
  double theta_s  = asin((n0/ns)*sin(acos(costh)));

  // S stands for "senkrecht". The electric vector is perp. to the plane of incidence. 
  complex <double> EoutS;
  complex <double> HoutS;
  double Y0S = n0  * cos(acos(costh));// Y optical admittance Y=H/E
  double YsS = ns  * cos(theta_s);

  complex <double> Ein(100.0,0);
  complex <double> Hin(100.0*YsS,0);


  for (int ilayer = istart; ilayer != iend; ilayer += istep)
    {
      n1 = fLayerMaterialVector[ilayer]->RefIndex(fac_orig*lambda);
      
      double theta1 = asin((n0/n1)*sin(acos(costh))); 
      double k = 2*pi/lambda;  // nm-1
      double h = fLayerThicknessVector[ilayer] * cos(theta1);
      //* fLayerMaterialVector[ilayer]->RefIndex(fac_orig*lambda); 
      // is already within the layer thickness! See there...
      //double e0bymu0 = 1.0;//????
      double Y = /*e0bymu0 */ n1  * cos(theta1); // Senkrecht
      complex <double> m11_6(cos(k*h) ,                 0);
      complex <double> m12_6(0               , sin(k*h)/Y);
      complex <double> m21_6(0               , sin(k*h)*Y);
      complex <double> m22_6(cos(k*h) ,                 0);
      Matrix M(m11_6,m12_6,m21_6,m22_6);
      M.product(EoutS,HoutS,Ein,Hin); // Senkrecht
      Ein = EoutS; // Senkrecht
      Hin = HoutS; // Senkrecht
    }
  
   // P stands for "parallel". The electric vector is parallel to the plane of incidence. 
  complex <double> EoutP;
  complex <double> HoutP;
  double Y0P = n0  * cos(acos(costh));
  double YsP = ns  * cos(theta_s);

  Ein = complex <double> (100.0,0);
  Hin = complex <double> (100.0*YsP,0);
 

  for (int ilayer = istart; ilayer != iend; ilayer += istep)
    {
      n1 = fLayerMaterialVector[ilayer]->RefIndex(fac_orig*lambda);
      
      double theta1 = asin((n0/n1)*sin(acos(costh))); 
      double k = 2*pi/lambda;  // nm-1
      double h = fLayerThicknessVector[ilayer] * cos(theta1);
      //* fLayerMaterialVector[ilayer]->RefIndex(fac_orig*lambda); 
      // is already within the layer thickness! See there...
      //double e0bymu0 = 1.0;//????
      double Y = /*e0bymu0 */ n1  / cos(theta1); // Parallel
      complex <double> m11_6(cos(k*h) ,                 0);
      complex <double> m12_6(0               , sin(k*h)/Y);
      complex <double> m21_6(0               , sin(k*h)*Y);
      complex <double> m22_6(cos(k*h) ,                 0);
      Matrix M(m11_6,m12_6,m21_6,m22_6);
      M.product(EoutP,HoutP,Ein,Hin); // Parallel
      Ein = EoutP; // Parallel
      Hin = HoutP; // Parallel
    }
  




  complex <double> er1S = (EoutS-HoutS/Y0S)/2;
  complex <double> ei1S = (EoutS+HoutS/Y0S)/2;
  complex <double> er1P = (EoutP-HoutP/Y0P)/2;
  complex <double> ei1P = (EoutP+HoutP/Y0P)/2;
  
  complex <double> rS = er1S/ei1S;
  complex <double> rP = er1P/ei1P;
    
  if      (fPolDir == Drc::PolDirS)
    {
      return real(rS*conj(rS));
    }
  else if (fPolDir == Drc::PolDirP)
    {
      return real(rP*conj(rP));
    }
  else if (fPolDir == Drc::PolDirBoth)
    {
      return real((rP+rS)/2 *conj((rP+rS)/2));
    }
  else if (fPolDir == Drc::PolDirPhot)
    {
      cerr<<" *** PndDrcOptReflGeffcken::ReflProb Drc::PolDirPhot not imlemented"<<endl;
      exit(EXIT_FAILURE);
    }
  else
    {
      cerr<<" *** PndDrcOptReflGeffcken::ReflProb invalid Drc::PolDir"<<endl;
      exit(EXIT_FAILURE);
    }

}
