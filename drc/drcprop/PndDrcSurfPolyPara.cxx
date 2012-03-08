// ----------------------------------------------------
// This file belongs to the ray tracing framework
// for the use with Cherenkov detectors
//
// created 2007
//-----------------------------------------------------
#include "PndDrcEffiAbs.h"
#include "PndDrcSurfPolyPara.h"

#include "PndDrcPhoton.h"
//#include "PndDrcOptReflAbs.h"
//#include "PndDrcSurfAbs.h"
//#include "PndDrcSurfPolyFlat.h"
//#include "PndDrcSurfPolySphere.h"
//#include "PndDrcOptMatAbs.h"
//#include "PndDrcOptDev.h"
//
//#include "TVector3.h"
//#include "TRotation.h"
//#include "TRandom.h"
//#include "TMatrixD.h"
//#include "TVectorD.h"
//#include "TDecompSVD.h"
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
PndDrcSurfPolyPara::PndDrcSurfPolyPara()
{
	fRadius  = -999;
	fChecked = false;
}
//----------------------------------------------------------------------
PndDrcSurfPolyPara* PndDrcSurfPolyPara::Clone() const
{
	return new PndDrcSurfPolyPara(*this);
}
//----------------------------------------------------------------------
void PndDrcSurfPolyPara::Copy(const PndDrcSurfPolyPara& s)
{
	fRadius   = s.fRadius;
	fChecked  = s.fChecked;
	fTrans    = s.fTrans;
	fTransInv = s.fTransInv;
}
//----------------------------------------------------------------------
PndDrcSurfPolyPara::PndDrcSurfPolyPara(const PndDrcSurfPolyPara& s) : PndDrcSurfPolyFlat(s)
{
	if (s.fVerbosity>=1) cout<<"  PndDrcSurfPolyPara::PndDrcSurfPolyPara"
		<<"(const PndDrcSurfPolyPara&) name,copy: "
		<<s.fName<<" "<<s.fCopyNumber<<endl;
	Copy(s);
}
//----------------------------------------------------------------------
PndDrcSurfPolyPara&  PndDrcSurfPolyPara::operator=(const PndDrcSurfPolyPara& s)
{
	if (s.fVerbosity>=1) cout<<"  PndDrcSurfPolyPara::operator="
		<<"(const PndDrcSurfPolyPara&) name,copy: "
		<<s.fName<<" "<<s.fCopyNumber<<endl;
	if (&s != this)
	{
		static_cast<PndDrcSurfPolyFlat&>((*this)) = s; // assignment of base class part.
		Copy(s);
	}

	return *this;
}
//----------------------------------------------------------------------
XYZVector PndDrcSurfPolyPara::Normal(const XYZPoint& point) const
{
	if (!fChecked) fChecked = Check1();// check dimensions

  // transform point back, where paraboloid was defined.
	XYZPoint p1(fTransInv*point);

  // unit vector from focal point to point
	XYZVector v1 = (p1 - XYZPoint(0,0,fRadius/2)).Unit();
  // unit vector parallel to z-axis hitting point
	XYZVector v2 = XYZVector(0,0,fabs(p1.Z())).Unit();

	XYZVector pn(v1+v2);

	return (fTrans*pn).Unit();
}
//----------------------------------------------------------------------
bool PndDrcSurfPolyPara::SurfaceHit(PndDrcPhoton& ph,
                                    XYZPoint&  pos_new,
                                    double&    path_length) const
{
  //cout<<" PndDrcSurfPolyPara::surfaceHit"<<endl;//###

	if (!fChecked) fChecked = Check1();// check dimensions

  // transform back in sphere definitions space
	XYZPoint  pos(fTransInv*ph.Position());
	XYZVector dir(fTransInv*ph.Direction());


  // compute hit position
  //
  // line : x = pos + lambda*dir
  //
  // paraboloid: z = p - 1/2p * (x^2+y^2)
  //
  // z = pz + lam*dz = p - 1/2p *
  // (px2 + lam2 dx2 + 2 px lam dx  + py2 + lam2 dy2 + 2 py lam dy)
  //
  // -2p* (pz + lam*dz -p) =
  // (px2 + lam2 dx2 + 2 px lam dx  + py2 + lam2 dy2 + 2 py lam dy)
  //
  // 2p* (p - pz - lam*dz) =
  // (px2 + lam2 dx2 + 2 px lam dx  + py2 + lam2 dy2 + 2 py lam dy)
  //
  // 0 = lam2 (dx2+dy2) + lam (2px dx + 2 py dy + 2p dz) + (px2 + py2 +2p pz - 2pp)

	double p = fRadius;
	double dx2dy2 = dir.X()*dir.X() + dir.Y()*dir.Y();
	double lambda;
	const double kEps = 0.001;

	if (fabs(dx2dy2)<1.0e-12)// otherwise the too small dx2dy2 will cause trouble in
    //                        the next clause
	{
      // 0 = lam2 (dx2+dy2) + lam (2px dx + 2 py dy + 2p dz) + (px2 + py2 +2p pz - 2pp)
      // 0 = lam (2px dx + 2 py dy + 2p dz) + (px2 + py2 +2p pz - 2pp)
		double pp = (2*pos.X()*dir.X()+2*pos.Y()*dir.Y()+2*p*dir.Z());
		double qq =  (pos.X()*pos.X() + pos.Y()*pos.Y() + 2*p*pos.Z() - 2*p*p);
		lambda = -qq/pp;
		if (lambda<kEps)
		{
			return false;
		}
	}
	else
	{
		double pp = (2*pos.X()*dir.X()+2*pos.Y()*dir.Y()+2*p*dir.Z())/dx2dy2;
		double qq =  (pos.X()*pos.X() + pos.Y()*pos.Y() + 2*p*pos.Z() - 2*p*p)/dx2dy2;

		long double root2 = pp*pp/4 - qq;
		if (root2<0)
		{
			return false; // photon does not hit parabola
	  // eg. photon behind parabola perpendicular to z-axis.
			cerr<<" *** PndDrcSurfPolyPara::surfaceHit: root<0 : "<<root2<<endl;
	  //exit(EXIT_FAILURE);
		}
		double lambda1 = -pp/2 + sqrt(root2);
		double lambda2 = -pp/2 - sqrt(root2);

		if (lambda1>kEps && lambda2>kEps)
		{ // check both solutions
			XYZPoint pos_new1     = pos + lambda1*dir;
			XYZPoint pos_check(pos_new);
			pos_check.SetZ(0);
			bool hit1 = WithinSurface(pos_check);
			XYZPoint pos_new2     = pos + lambda2*dir;
			pos_check = pos_new2;
			pos_check.SetZ(0);
			bool hit2 = WithinSurface(pos_check);
			if (hit1 && hit2)
			{
	      // take closest
				lambda = (lambda1<lambda2) ? lambda1 : lambda2;
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
			lambda = lambda1;
		}
		else if (lambda1<=kEps && lambda2>kEps)
		{
			lambda = lambda2;
		}
		else
		{
	  //cout<<" both lambdas <0"<<endl;//###
			return false;
		}


	}

	path_length = lambda;
	pos_new     = pos + path_length*dir;
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
	return false;
}
//----------------------------------------------------------------------
bool PndDrcSurfPolyPara::Check1() const
{
	if (fRadius<=0)
	{
		cerr<<" ***  PndDrcSurfPolyPara::check: "
			<<fName<<": wrong or not set radius="
			<<fRadius<<endl;
		exit(EXIT_FAILURE);
	}

	for (unsigned int i=0; i<fP.size(); i++)
	{
		if (fP[i].Z() != 0)
		{
			cerr<<" ***  PndDrcSurfPolyPara::check: "
				<<fName<<": z component of point != 0 "
				<<" fP["<<i<<"]="
				<<fP[i].X()<<" "
				<<fP[i].Y()<<" "
				<<fP[i].Z()<<endl;
			return false;
		}
	}

	return true;
}
//----------------------------------------------------------------------
void PndDrcSurfPolyPara::AddTransform(const Transform3D& trans)
{
  if (Verbosity()>=3) cout<<"    PndDrcSurfPolyPara::addTransform() name="<<fName<<endl;
  fTrans    = trans * fTrans;
  fTransInv  = fTrans.Inverse();
  fPixelPoint = trans*fPixelPoint;

}
//----------------------------------------------------------------------
XYZPoint PndDrcSurfPolyPara::LimitingPoint(unsigned int i)
{
	if (i>(fP.size()-1) || i<0)
	{
		cerr<<" *** PndDrcSurfPolyPara::limitingPoint: index out of range, i="<<i<<endl;
		exit(EXIT_FAILURE);
	}
	XYZPoint p1(fP[i]);

	double z = fRadius
		- p1.Mag2()/(2*fRadius); // z comp = 0
	p1.SetZ(z);

	return fTrans*p1;


}
//----------------------------------------------------------------------
XYZPoint PndDrcSurfPolyPara::CenterPoint()
{
	return fTrans*XYZPoint(0,0,fRadius);
}
//----------------------------------------------------------------------
void PndDrcSurfPolyPara::Print(fstream& stream) const
{
	if (!fChecked) fChecked = Check1();// check dimensions

  // print points
	unsigned int isize = fP.size();

	if (isize < 3)
	{
		cerr<<" *** PndDrcSurfPolyPara::surfaceHit: surface has only "<<fP.size()
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


				fP1 = fP[i]*frac + j/20.0*(fP[next]-fP[i])*frac;
				double z = fRadius
					- fP1.Mag2()/(2*fRadius); // z comp = 0
				fP1.SetZ(z);

				fP1 = fTrans * fP1;

				stream<<" l->SetPoint("<<icnt<<","
					<<fP1.X()<<","
					<<fP1.Y()<<","
					<<fP1.Z()<<");"<<endl;

				l->SetPoint( icnt, fP1.X(), fP1.Y(), fP1.Z() ); // for an opened canvas
				icnt++;
			}
		}
		fP1 = fP[0]*frac;
		double z = fRadius - fP1.Mag2()/(2*fRadius); // z comp = 0
		fP1.SetZ(z);

		fP1 = fTrans * fP1;

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
void PndDrcSurfPolyPara::Print() const
{
	cout<<" Coordinates of PndDrcSurfPolyPara "<<Name()<<endl;
	static_cast<PndDrcSurfPolyFlat>((*this)).Print();
	cout<<" r  = "<<fRadius<<endl;
}
