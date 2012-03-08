// ----------------------------------------------------
// This file belongs to the ray tracing framework
// for the use with Cherenkov detectors
//
// created 2007
//-----------------------------------------------------
#include "PndDrcEffiAbs.h"
#include "PndDrcSurfPolyCyl.h"

#include "PndDrcPhoton.h"
//#include "PndDrcOptReflAbs.h"
//#include "PndDrcSurfAbs.h"
//#include "PndDrcSurfPolyFlat.h"
//#include "PndDrcOptMatAbs.h"
//#include "PndDrcOptDev.h"
//
//#include "TVector3.h"
//#include "TRotation.h"
//#include "TRandom.h"
//#include "TMatrixD.h"
//#include "TVectorD.h"
//#include "TDecompSVD.h"
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
using std::cerr;
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

#include "TPolyLine3D.h"


// bended around y-axis  

//----------------------------------------------------------------------
PndDrcSurfPolyCyl::PndDrcSurfPolyCyl()
{
	fRadius  = -999;
	fChecked = false;
}
//----------------------------------------------------------------------
PndDrcSurfPolyCyl* PndDrcSurfPolyCyl::Clone() const
{
	return new PndDrcSurfPolyCyl(*this);
}
//----------------------------------------------------------------------
void PndDrcSurfPolyCyl::Copy(const PndDrcSurfPolyCyl& s)
{
	fRadius   = s.fRadius;
	fChecked  = s.fChecked;
	fTrans    = s.fTrans;
	fTransInv = s.fTransInv;
}
//----------------------------------------------------------------------
PndDrcSurfPolyCyl::PndDrcSurfPolyCyl(const PndDrcSurfPolyCyl& s) : PndDrcSurfPolyFlat(s)
{
	if (s.fVerbosity>=1) cout<<"  PndDrcSurfPolyCyl::PndDrcSurfPolyCyl"
		<<"(const PndDrcSurfPolyCyl&) name,copy: "
		<<s.fName<<" "<<s.fCopyNumber<<endl;
	Copy(s);
}
//----------------------------------------------------------------------
PndDrcSurfPolyCyl&  PndDrcSurfPolyCyl::operator=(const PndDrcSurfPolyCyl& s)
{
	if (s.fVerbosity>=1) cout<<"  PndDrcSurfPolyCyl::operator="
		<<"(const PndDrcSurfPolyCyl&) name,copy: "
		<<s.fName<<" "<<s.fCopyNumber<<endl;
	if (&s != this)
	{
		static_cast<PndDrcSurfPolyFlat&>((*this)) = s; // assignment of base class part.
		Copy(s);
	}

	return *this;
}
//----------------------------------------------------------------------
XYZVector PndDrcSurfPolyCyl::Normal(const XYZPoint& point) const
{
	if (!fChecked) fChecked = Check();// check dimensions

  // transform point back, where sphere was defined.
	XYZPoint point0(fTransInv*point);

  // normal vector points from origin (0,0,0) to point of surface.
	XYZVector pn(point0);
  // but y-component is zero
	pn.SetY(0);

  // transform back

	return fTrans(pn).Unit();//p1.Unit();
}
//----------------------------------------------------------------------
bool PndDrcSurfPolyCyl::SurfaceHit(PndDrcPhoton& ph,
                                   XYZPoint&  pos_new,
                                   double&    path_length) const
{
  if (!fChecked) fChecked = Check();// check dimensions

  // transform back in sphere definitions space
  XYZPoint  pos(fTransInv*ph.Position());
  XYZVector dir(fTransInv*ph.Direction());

  

  // There are 2 positions for photon to have distance=radius from y-axis
  // (work on projections with y component beeing zero)
  //
  // (pos+lambda*dir)_x^2 + (pos+lanbda*dir)_z^2 = R^2
  //   pos_x^2 + 2 lambda dir_x pos_x + lambda^2 dir_x^2 
  // + pos_z^2 + 2 lambda dir_z pos_z + lambda^2 dir_z^2 = R^2
  // lambda^2 + lambda 2*dir_xz*pos_xz/dir_xz^2 + (pos_xz^2-R2)/dir_xz^2 = 0
  //
  // lambda1/2 = ...

  double pos2,dir2,posdir;
  
  pos2   = pos.X()*pos.X() + pos.Z()*pos.Z();//XYZVector(pos).Dot(XYZVector(pos));
  dir2   = dir.X()*dir.X() + dir.Z()*dir.Z();//dir.Dot(dir);
  posdir = pos.X()*dir.X() + pos.Z()*dir.Z();//dir.Dot(pos);
  



  double p = 2*posdir/dir2;
  double q = (pos2-fRadius*fRadius)/dir2;

  double root2 = p*p/4-q;
  if (root2<0) return false; // no hit


  double lambda1 = -p/2 + sqrt(root2);
  double lambda2 = -p/2 - sqrt(root2);



  const double kEps = 0.001;

  if (fVerbosity>=4 )
      //&& fabs(ph.Direction().X()-0.0122667)<0.0001
      //&& fabs(ph.Direction().Y()-0.185798)<0.0001
      //&& fabs(ph.Direction().Z()+0.982511)<0.0001) 
    {
      cout<<" local coordinates:"<<endl;
      cout<<" radius="<<fRadius<<endl;
      cout<<" pos="<<pos<<endl;
      cout<<" dir="<<dir<<endl;
      cout<<" pos2="<<pos2<<endl;
      cout<<" dir2="<<dir2<<endl;
      cout<<" posdir="<<posdir<<endl;
      cout<<" p,q,root2 "<<p<<" "<<q<<" "<<root2<<endl;
      cout<<" PndDrcSurfPolyCyl::surfaceHit lambda_1,2="
	  <<lambda1<<" "<<lambda2<<endl;
    }
  
  if (lambda1>kEps && lambda2>kEps)
    { // check both lambdas

      XYZPoint pos_new1     = pos + lambda1*dir;
      XYZPoint pos_check(pos_new1);
      bool hit1 = (pos_new1.Z()>0);
      pos_check.SetZ(0);
      if (hit1) hit1 = WithinSurface(pos_check);
      XYZPoint pos_new2     = pos + lambda2*dir;
      pos_check = pos_new2;
      bool hit2 = (pos_new2.Z()>0);
      pos_check.SetZ(0);
      if (hit2) hit2 = WithinSurface(pos_check);
      if (hit1 && hit2)
	{
	  // take closest
	  double lambda = (lambda1<lambda2) ? lambda1 : lambda2;
	  pos_new = pos + lambda*dir;
	  path_length = lambda;
	  pos_new = fTrans * pos_new;
          if (fPixel){
	    if (fEffiCathode
		->EffiFlag(ph.Wavelength(),
			   ph.Direction().Dot(Normal(pos_new))))
	      { 
		ph.SetFate(Drc::kPhotMeasured);
		if (fPixelCorr) ph.SetPosition1(fPixelPoint);
	      }
	    else
	      { 
		ph.SetFate(Drc::kPhotAbsorbed);
	      }
	  }
	  
	  //if (fPixel) ph.SetFate(Drc::kPhotMeasured);
	  return true;
	}
      else if (hit1)
	{
	  pos_new = pos + lambda1*dir;
	  path_length = lambda1;
	  pos_new = fTrans * pos_new;
          if (fPixel){
	    if (fEffiCathode
		->EffiFlag(ph.Wavelength(),
			   ph.Direction().Dot(Normal(pos_new))))
	      { 
		ph.SetFate(Drc::kPhotMeasured);
		if (fPixelCorr) ph.SetPosition1(fPixelPoint);
	      }
	    else
	      { 
		ph.SetFate(Drc::kPhotAbsorbed);
	      }
	  }
	  
	  //if (fPixel) ph.SetFate(Drc::kPhotMeasured);
	  return true;
	}
      else if (hit2)
	{
	  pos_new = pos + lambda2*dir;
	  path_length = lambda2;
	  pos_new = fTrans * pos_new;
          if (fPixel){
	    if (fEffiCathode
		->EffiFlag(ph.Wavelength(),
			   ph.Direction().Dot(Normal(pos_new))))
	      { 
		ph.SetFate(Drc::kPhotMeasured);
		if (fPixelCorr) ph.SetPosition1(fPixelPoint);
	      }
	    else
	      { 
		ph.SetFate(Drc::kPhotAbsorbed);
	      }
	  }
	  
	  //if (fPixel) ph.SetFate(Drc::kPhotMeasured);
	  return true;
	}
      else
	{
	  return false;
	}
    }
  else if (lambda1>kEps && lambda2<=kEps)
    {
      path_length = lambda1;
    }
  else if (lambda1<=kEps && lambda2>kEps)
    {
      path_length = lambda2;
    }
  else
    {
      //cout<<" not taken"<<endl;//###
      return false;

    }
  pos_new     = pos + path_length*dir;
  if (fVerbosity>=4) cout<<" PndDrcSurfPolyCyl::surfaceHit pos_new_z="
			 <<pos_new.Z()<<endl;
  if (pos_new.Z()<0) return false; // wrong half cyl.


  // check if pos_new is within polynom
  XYZPoint pos_check(pos_new);
  pos_check.SetZ(0);

  if (WithinSurface(pos_check))
    {
      pos_new = fTrans * pos_new;
      if (fPixel){
	if (fEffiCathode
	    ->EffiFlag(ph.Wavelength(),
		       ph.Direction().Dot(Normal(pos_new))))
	  { 
	    ph.SetFate(Drc::kPhotMeasured);
	    if (fPixelCorr) ph.SetPosition1(fPixelPoint);
	  }
	else
	  { 
	    ph.SetFate(Drc::kPhotAbsorbed);
	  }
      }
	  
      //if (fPixel) ph.SetFate(Drc::kPhotMeasured);
      return true;
    }

  if (fVerbosity>=4) cout<<" PndDrcSurfPolyCyl::surfaceHit false"<<endl;




  if (fVerbosity>=4) cout<<" PndDrcSurfPolyCyl::surfaceHit false"<<endl;

  return false;

}
//----------------------------------------------------------------------
bool PndDrcSurfPolyCyl::Check() const
{
  if (fRadius<=0)
    {
      cerr<<" ***  PndDrcSurfPolyCyl::check: "
	  <<fName<<": wrong or not set radius="
	  <<fRadius<<endl;
      exit(EXIT_FAILURE);
    }
  for (unsigned int i=0; i<fP.size(); i++)
    {
      if (fP[i].Z()*fP[i].Z()+fP[i].X()*fP[i].X()>fRadius*fRadius)
	{
	  cerr<<" ***  PndDrcSurfPolyCyl::check: "
	      <<fName<<": point outside sphere projection:"
	      <<" fP["<<i<<"]="
	      <<fP[i].X()<<" "
	      <<fP[i].Y()<<" "
	      <<fP[i].Z()<<endl;
	  exit(EXIT_FAILURE);
	}
    }
  for (unsigned int i=0; i<fP.size(); i++)
    {
      if (fP[i].Z() != 0)
	{
	  cerr<<" ***  PndDrcSurfPolyCyl::check: "
	      <<fName<<": z component of point != 0 "
	      <<" fP["<<i<<"]="
	      <<fP[i].X()<<" "
	      <<fP[i].Y()<<" "
	      <<fP[i].Z()<<endl;
	  exit(EXIT_FAILURE);
	}
    }

  return true;
}
//----------------------------------------------------------------------
XYZPoint PndDrcSurfPolyCyl::LimitingPoint(unsigned int i)
{
  unsigned int isize=fP.size();
	if (i>(isize-1) || i<0)
	{
		cerr<<" *** PndDrcSurfPolyCyl::limitingPoint: index out of range, i="<<i<<endl;
		exit(EXIT_FAILURE);
	}
  //TVector3 p1(fP[i]);

	XYZPoint p1(fP.at(i));

	p1.SetZ(sqrt(fRadius*fRadius - p1.X()*p1.X())); // fP1.Z()=0...

	return fTrans*p1;


}
//----------------------------------------------------------------------
XYZPoint PndDrcSurfPolyCyl::CenterPoint()
{
	return fTrans*XYZPoint(0,0,fRadius);
}
//----------------------------------------------------------------------
void PndDrcSurfPolyCyl::AddTransform(const Transform3D& trans)
{
  if (Verbosity()>=3) cout<<"    PndDrcSurfPolyCyl::addTransform() name="<<fName<<endl;
  fTrans    = trans * fTrans;
  fTransInv = fTrans.Inverse();
  fPixelPoint = trans*fPixelPoint;

}
//----------------------------------------------------------------------
void PndDrcSurfPolyCyl::Print(fstream& stream) const
{
  if (!fChecked) fChecked = Check();// check dimensions

  // print points
  unsigned int isize = fP.size();

  if (isize < 3)
    {
      cerr<<" *** PndDrcSurfPolyCyl::surfaceHit: surface has only "<<fP.size()
	  <<" points. Aborting. Name="<<Name()<<endl;
      exit(EXIT_FAILURE);
    }

  double frac=1;

  stream<<" TPolyLine3D *l = new TPolyLine3D("<<20*isize+1<<");"<<endl;
  TPolyLine3D *l = new TPolyLine3D(20*isize+1); // for an opened canvas

  XYZPoint fP1;
  int icnt=0;

  for (unsigned int i=0; i<isize; i++)
    {
      for (unsigned int j=0; j<20; j++)
	{
	  unsigned int next = i+1;
	  if (next==isize) next=0;


	  fP1 = (fP[i]*frac) + j/20.0*(fP[next]-fP[i])*frac;
	  double x2 = fP1.X()*fP1.X();
	  fP1.SetZ(sqrt(fRadius*fRadius - x2)); // fP1.Z()=0...

	  fP1 = fTrans(fP1);
	  //fP1.Transform(fRot);
	  //fP1 += fShift;

	  stream<<" l->SetPoint("<<icnt<<","
		<<fP1.X()<<","
		<<fP1.Y()<<","
		<<fP1.Z()<<");"<<endl;

	  l->SetPoint( icnt, fP1.X(), fP1.Y(), fP1.Z()); // for an opened canvas
	  icnt++;
	}
    }

  fP1 = fP[0]*frac;
  double x2 = fP1.X()*fP1.X();
  fP1.SetZ(sqrt(fRadius*fRadius - x2));

  fP1 = fTrans(fP1);

  stream<<" l->SetPoint("<<icnt<<","
	<<fP1.X()<<","
	<<fP1.Y()<<","
	<<fP1.Z()<<");"<<endl;
  stream<<" l->SetLineColor("<<fPrintColor<<");"<<endl;
  stream<<" l->Draw();"<<endl;

  // for an opened canvas
  l->SetPoint( icnt, fP1.X(), fP1.Y(), fP1.Z() );
  l->SetLineColor(fPrintColor);
  l->Draw();
  icnt++;
}
//----------------------------------------------------------------------
void PndDrcSurfPolyCyl::Print() const
{
	cout<<" Coordinates of PndDrcSurfPolyCyl "<<Name()<<endl;
	static_cast<PndDrcSurfPolyFlat>((*this)).Print();
	cout<<" r  = "<<fRadius<<endl;
}
