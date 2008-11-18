// ----------------------------------------------------
// This file belongs to the ray tracing framework
// for the use with Cherenkov detectors
// 
// created 2007
//-----------------------------------------------------
#include "PndDrcPhoton.h"

#include "PndDrcOptMatAbs.h"
#include "PndDrcOptDev.h"

#include <iostream>
using std::cout;
//using std::cerr;
//using std::cin;
using std::endl;

//----------------------------------------------------------------------
PndDrcPhoton::PndDrcPhoton()
{
  fLambda      = 0;
  fPosition    = XYZPoint(0,0,0);
  fPositionOld = XYZPoint(0,0,0);
  fDirection   = XYZPoint(0,0,0);
  fFate        = Drc::kPhotFlying;
  fReflections = 0;
  fVerbosity   = 0;
  fTime        = 0;
  fDev         = 0;
  fReflectionLimit = 1000;
  fPrintFlag   = true;
  
}
//----------------------------------------------------------------------
void PndDrcPhoton::Copy(const PndDrcPhoton& ph)
{
  fLambda      = ph.fLambda;
  fPosition    = ph.fPosition;
  fPositionOld = ph.fPositionOld;
  fDirection   = ph.fDirection;
  fFate        = ph.fFate;
  fReflections = ph.fReflections;
  fVerbosity   = ph.fVerbosity;
  fTime        = ph.fTime;
  fDev         = ph.fDev;
  fReflectionLimit = ph.fReflectionLimit;
  fPrintFlag   = ph.fPrintFlag;
  
}
//----------------------------------------------------------------------
PndDrcPhoton::PndDrcPhoton(const PndDrcPhoton& ph)
{
  if (ph.fVerbosity>=1) cout<<"  PndDrcPhoton::PndDrcPhoton"
			    <<"(const PndDrcPhoton&)"<<endl;
  Copy(ph);
}
//----------------------------------------------------------------------
PndDrcPhoton& PndDrcPhoton::operator=(const PndDrcPhoton& ph)
{
  if (ph.fVerbosity>=1) cout<<"  PndDrcPhoton::operator="
			    <<"(const PndDrcPhoton&) "<<endl;
  if (&ph != this) Copy(ph);
  return *this;

}
//----------------------------------------------------------------------
void PndDrcPhoton::SetPosition(const XYZPoint& pos)
{
  if (fDev && fDev->Radiator()) // no flat device
    {
      double n = (fDev->OptMaterial()).RefIndex(fLambda);
      double dndl = (fDev->OptMaterial()).RefIndexDeriv(fLambda);
      double len = sqrt((pos-fPosition).Mag2()); // mm
      double v_phase = 299.792/n;
      double v_group =  v_phase * ( 1.0 - fLambda/n*dndl);
      double time = len/v_group;
      fTime += time;          // pos in mm time in ns
    }

  fPositionOld = fPosition;
  fPosition    = pos;
};
//----------------------------------------------------------------------
bool PndDrcPhoton::Refract(XYZVector normal, double n1, double n2) 
{
  static const double kEps = 1.0e-9;

  bool      refract_flag;
  XYZVector dir1   = Direction();
 
 // norm points outside.
  XYZVector norm = (dir1.Dot(normal)>0) ? normal : -normal;

  //
  // sin(alpha1)   n2
  // ----------- = --  (Snellius)
  // sin(alpha2)   n1
  //

  // alpaha1, alpha2 > 0

  if (Verbosity()>=4) 
    {
      cout<<"     norm="<<norm.X()<<" "<<norm.Y()<<" "<<norm.Z()<<endl;
      cout<<"     dir1="<<dir1.X()<<" "<<dir1.Y()<<" "<<dir1.Z()<<endl;
      cout<<"     norm*dir="<<norm.Dot(dir1)<<endl;
    }
  

  double alpha1 = acos(norm.Dot(dir1));
  double alpha2;  
  if (sin(alpha1) * n1/n2 > 1) // reflect photon
    {
      refract_flag = false;
      Reflect(norm);
      //ph.setDirection(dir1 - 2*(norm.Dot(dir1)*norm)); // new direction
      if (Verbosity()>=4) cout<<"     reflection"<<endl;
    }
  else                         // refract photon
    {
      refract_flag = true;

      if (Verbosity()>=4) cout<<"     alpha1,n1,n2 = "<<alpha1<<" "<<n1<<" "<<n2<<endl;
      alpha2 = asin(sin(alpha1) * n1/n2);
    
      XYZVector rhelp = norm.Cross(dir1);
      XYZVector diff  = rhelp.Cross(norm);
      diff = diff.Unit();
      
      // diff perp to norm points in direction to the tip of vel.
      //
      // dir2 = norm + x*diff such that L(x*diff)/L(n) = tan(alpha2)
      //
      double x = tan(alpha2);
      if (Verbosity()>=4) cout<<"     x,alpha2 = "<<x<<" "<<alpha2<<endl;
      XYZVector dir2 = norm + x*diff;

      if (Verbosity()>=4) 
	{
	  cout<<"     dir2 before unit = "<<dir2.X()<<" "<<dir2.Y()<<" "<<dir2.Z()<<endl; 
	}
      dir2 = dir2.Unit();
      SetDirection(dir2);
      SetPosition(Position() + 2*kEps*dir2); // bring it in new volume or outside.
      if (Verbosity()>=4) 
	{
	  cout<<"     refraction, brought it inside to "
	      <<Position().X()<<" "
	      <<Position().Y()<<" "
	      <<Position().Z()<<endl;
	  cout<<"     refraction, brought in direction "
	      <<Direction().X()<<" "
	      <<Direction().Y()<<" "
	      <<Direction().Z()<<endl;
	}
    } 

  return refract_flag;

}

//----------------------------------------------------------------------
void PndDrcPhoton::Reflect(const XYZVector& normal)
{
  XYZVector normal1(normal);

  if (fDirection.Dot(normal) < 0) normal1 *= -1;
  fDirection = fDirection-2*(fDirection.Dot(normal1))*normal1;

  fReflections++;

}
//----------------------------------------------------------------------
int PndDrcPhoton::ColorNumber(double lambda) const
{

  // the colors were taken from wikipedia entry about colors.

  if      (lambda< 0.1) return 17; // grey
  if      (lambda> 740) return 50; // dark red
  else if (lambda> 625) return 2;  // red
  else if (lambda> 590) return 42; // orange
  else if (lambda> 565) return 5;  // yellow
  else if (lambda> 520) return 3;  // green
  else if (lambda> 500) return 7;  // cyan
  else if (lambda> 450) return 4;  // blue
  else if (lambda> 430) return 9;  // indigo
  else if (lambda> 380) return 39; // violet

  return 33; // blue grey
}
//----------------------------------------------------------------------
void PndDrcPhoton::Print(fstream& stream) const
{

  if (fPrintFlag)
    {
      stream<<" TPolyLine3D *l = new TPolyLine3D(2);"<<endl;
      
      stream<<" l->SetPoint("<<0<<","
	    <<fPositionOld.X()<<","
	    <<fPositionOld.Y()<<","
	    <<fPositionOld.Z()<<");"<<endl;
      stream<<" l->SetPoint("<<1<<","
	    <<fPosition.X()<<","
	    <<fPosition.Y()<<","
	    <<fPosition.Z()<<");"<<endl;
      stream<<" l->SetLineColor("<<ColorNumber(fLambda)<<");"<<endl;
      stream<<" l->Draw();"<<endl;
    }
  
}
//----------------------------------------------------------------------
void PndDrcPhoton::Print() const
{
  cout<<" --- photon ---"<<endl;
  cout<<"     pos    = "<<fPosition<<endl;
  cout<<"     dir    = "<<fDirection<<endl;
  cout<<"     time   = "<<fTime<<endl;
  cout<<"     lambda = "<<fLambda<<endl;
  cout<<"     refl   = "<<fReflections<<endl;
  string str="unknown";
  if (fFate==Drc::kPhotFlying)   str="flying";
  if (fFate==Drc::kPhotMeasured) str="measured";
  if (fFate==Drc::kPhotLost)     str="lost";
  if (fFate==Drc::kPhotAbsorbed) str="absorbed";
  cout<<"     fate   = "<<str<<endl;
  if (fDev)
    {
      cout<<"     dev    = "<<fDev->Name()<<endl;
    }
  else
    {
      cout<<"     dev    = "<<"zero pointer"<<endl;
    }
  cout<<" --------------"<<endl;
}
