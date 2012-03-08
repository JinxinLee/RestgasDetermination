// ----------------------------------------------------
// This file belongs to the ray tracing framework
// for the use with Cherenkov detectors
//
// created 2007
//-----------------------------------------------------
#include "PndDrcEffiAbs.h"
#include "PndDrcSurfPolySphere.h"

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


//----------------------------------------------------------------------
PndDrcSurfPolySphere::PndDrcSurfPolySphere()
{
	fRadius  = -999;
	fChecked = false;
}
//----------------------------------------------------------------------
PndDrcSurfPolySphere* PndDrcSurfPolySphere::Clone() const
{
	return new PndDrcSurfPolySphere(*this);
}
//----------------------------------------------------------------------
void PndDrcSurfPolySphere::Copy(const PndDrcSurfPolySphere& s)
{
	fRadius   = s.fRadius;
	fChecked  = s.fChecked;
	fTrans    = s.fTrans;
	fTransInv = s.fTransInv;
}
//----------------------------------------------------------------------
PndDrcSurfPolySphere::PndDrcSurfPolySphere(const PndDrcSurfPolySphere& s) : PndDrcSurfPolyFlat(s)
{
	if (s.fVerbosity>=1) cout<<"  PndDrcSurfPolySphere::PndDrcSurfPolySphere"
		<<"(const PndDrcSurfPolySphere&) name,copy: "
		<<s.fName<<" "<<s.fCopyNumber<<endl;
	Copy(s);
}
//----------------------------------------------------------------------
PndDrcSurfPolySphere&  PndDrcSurfPolySphere::operator=(const PndDrcSurfPolySphere& s)
{
	if (s.fVerbosity>=1) cout<<"  PndDrcSurfPolySphere::operator="
		<<"(const PndDrcSurfPolySphere&) name,copy: "
		<<s.fName<<" "<<s.fCopyNumber<<endl;
	if (&s != this)
	{
		static_cast<PndDrcSurfPolyFlat&>((*this)) = s; // assignment of base class part.
		Copy(s);
	}

	return *this;
}
//----------------------------------------------------------------------
XYZVector PndDrcSurfPolySphere::Normal(const XYZPoint& point) const
{
	if (!fChecked) fChecked = Check1();// check dimensions

  // transform point back, where sphere was defined.
	XYZPoint point0(fTransInv*point);

  // normal vector points from origin (0,0,0) to point of surface.
	XYZVector pn(point0);

  // transform back

	return fTrans(pn).Unit();//p1.Unit();
}
//----------------------------------------------------------------------
bool PndDrcSurfPolySphere::SurfaceHit(PndDrcPhoton& ph,
                                      XYZPoint&  pos_new,
                                      double&    path_length) const
{
	if (!fChecked) fChecked = Check1();// check dimensions

  // transform back in sphere definitions space
	XYZPoint  pos(fTransInv*ph.Position());
	XYZVector dir(fTransInv*ph.Direction());

  //if (/*pos.Z()<0 && */ dir.Z()<0) return false; // wrong direction###

  // check if the photon hits the full sphere
  // double costh = sqrt((dir.X()*dir.X()+dir.Y()*dir.Y())/dir.Mag2());
  // double theta = fabs(acos(costh));
  // if (tan(theta)* -dir.Z() >= fRadius) return false; // angle too steep

  // compute hit position
  //
  // line : x = pos + lambda*dir
  //
  // sphere: x^2 = r^2
  //
  // pos^2 + lambda^2*dir^2 + 2 pos lambda dir = r^2
  //
  //
  // lambda^2 + lambda*2*pos*dir + pos^2-r^2 = 0;
  //
  // lambda12 = -pos*dir + sqrt((pos*dir)^2 - (pos^2-r^2)
  //
  // the other (-) solution is the wrong one.
  //
  // path_length = lambda since dir is normalized

	double posdir = pos.Dot(dir);
	double diff   = pos.Mag2() - fRadius*fRadius;

	double root2 = posdir*posdir - diff;
	if (fVerbosity>=4) cout<<" PndDrcSurfPolySphere::surfaceHit root2="<<root2<<endl;
	if (root2<0) return false; // no hit
	double lambda1 = -posdir + sqrt(root2);
	double lambda2 = -posdir - sqrt(root2);
	const double kEps = 0.001;

	if (fVerbosity>=4) cout<<" PndDrcSurfPolySphere::surfaceHit lambda_1,2="
		<<lambda1<<" "<<lambda2<<endl;

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
	if (fVerbosity>=4) cout<<" PndDrcSurfPolySphere::surfaceHit pos_new_z="
			       <<pos_new.Z()<<endl;
	if (pos_new.Z()<0) return false; // wrong half sphere.


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

	if (fVerbosity>=4) cout<<" PndDrcSurfPolySphere::surfaceHit false"<<endl;

	return false;

}
//----------------------------------------------------------------------
bool PndDrcSurfPolySphere::Check1() const
{
	if (fRadius<=0)
	{
		cerr<<" ***  PndDrcSurfPolySphere::check: "
			<<fName<<": wrong or not set radius="
			<<fRadius<<endl;
		exit(EXIT_FAILURE);
	}
	for (unsigned int i=0; i<fP.size(); i++)
	{
		if (fP[i].Mag2()>fRadius*fRadius)
		{
			cerr<<" ***  PndDrcSurfPolySphere::check: "
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
			cerr<<" ***  PndDrcSurfPolySphere::check: "
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
XYZPoint PndDrcSurfPolySphere::LimitingPoint(unsigned int i)
{
	unsigned int isize=fP.size();
	if (i>(isize-1) || i<0)
	{
		cerr<<" *** PndDrcSurfPolySphere::limitingPoint: index out of range, i="<<i<<endl;
		exit(EXIT_FAILURE);
	}
  //TVector3 p1(fP[i]);

	XYZPoint p1(fP.at(i));

	double x2y2 = p1.X()*p1.X()+p1.Y()*p1.Y();
	p1.SetZ(sqrt(fRadius*fRadius - x2y2)); // fP1.Z()=0...

	return fTrans*p1;


}
//----------------------------------------------------------------------
XYZPoint PndDrcSurfPolySphere::CenterPoint()
{
	return fTrans*XYZPoint(0,0,fRadius);
}
//----------------------------------------------------------------------
void PndDrcSurfPolySphere::AddTransform(const Transform3D& trans)
{
  if (Verbosity()>=3) cout<<"    PndDrcSurfPolySphere::addTransform() name="<<fName<<endl;
  fTrans    = trans * fTrans;
  fTransInv = fTrans.Inverse();
  fPixelPoint = trans*fPixelPoint;
}
//----------------------------------------------------------------------
void PndDrcSurfPolySphere::Print(fstream& stream) const
{
	if (!fChecked) fChecked = Check1();// check dimensions

  // print points
	unsigned int isize = fP.size();

	if (isize < 3)
	{
		cerr<<" *** PndDrcSurfPolySphere::surfaceHit: surface has only "<<fP.size()
			<<" points. Aborting. Name="<<Name()<<endl;
		exit(EXIT_FAILURE);
	}



	for (double frac=1.0; frac>0.19; frac-=0.2)
	{// several (5) lines


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
				double x2y2 = fP1.X()*fP1.X()+fP1.Y()*fP1.Y();
				fP1.SetZ(sqrt(fRadius*fRadius - x2y2)); // fP1.Z()=0...

				fP1 = fTrans(fP1);
	      //fP1.Transform(fRot);
	      //fP1 += fShift;

				stream<<" l->SetPoint("<<icnt<<","
					<<fP1.X()<<","
					<<fP1.Y()<<","
					<<fP1.Z()<<");"<<endl;

				l->SetPoint( icnt, fP1.X(), fP1.Y(), fP1.Z() ); // for an opened canvas
				icnt++;
			}
		}

		fP1 = fP[0]*frac;
		double x2y2 = fP1.X()*fP1.X()+fP1.Y()*fP1.Y();
		fP1.SetZ(sqrt(fRadius*fRadius - x2y2));

		fP1 = fTrans(fP1);
      //fP1.Transform(fRot);
      //fP1 += fShift;

		stream<<" l->SetPoint("<<icnt<<","
			<<fP1.X()<<","
			<<fP1.Y()<<","
			<<fP1.Z()<<");"<<endl;
		stream<<" l->SetLineColor("<<fPrintColor<<");"<<endl;
		stream<<" l->Draw();"<<endl;

		// for an opened canvas
		l->SetPoint( icnt, fP1.X(), fP1.Y(), fP1.Z());
		l->SetLineColor(fPrintColor);
		l->Draw();
		icnt++;
	}

}
//----------------------------------------------------------------------
void PndDrcSurfPolySphere::Print() const
{
	cout<<" Coordinates of PndDrcSurfPolySphere "<<Name()<<endl;
	static_cast<PndDrcSurfPolyFlat>((*this)).Print();
	cout<<" r  = "<<fRadius<<endl;
}
