// ----------------------------------------------------
// This file belongs to the ray tracing framework
// for the use with Cherenkov detectors
//
// created 2007
//-----------------------------------------------------
#include "PndDrcEffiAbs.h"
#include "PndDrcSurfCyl.h"

#include "PndDrcPhoton.h"
//#include "PndDrcOptReflAbs.h"
//#include "PndDrcSurfAbs.h"
//#include "PndDrcOptMatAbs.h"
//#include "PndDrcOptDev.h"
//
#include "TVector3.h"
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

#include "TPolyLine3D.h"


//----------------------------------------------------------------------
PndDrcSurfCyl::PndDrcSurfCyl()
{
}
//----------------------------------------------------------------------
PndDrcSurfCyl* PndDrcSurfCyl::Clone() const
{
	return new PndDrcSurfCyl(*this);
}
//----------------------------------------------------------------------
void PndDrcSurfCyl::Copy(const PndDrcSurfCyl& s)
{
	fP1           = s.fP1;
	fP2           = s.fP2;
	fRadius       = s.fRadius;
}
//----------------------------------------------------------------------
PndDrcSurfCyl::PndDrcSurfCyl(const PndDrcSurfCyl& s) : PndDrcSurfAbs(s)
{
	if (s.fVerbosity>=1) cout<<"  PndDrcSurfCyl::PndDrcSurfCyl"
				<<"(const PndDrcSurfCyl&) name,copy: "
				<<s.fName<<" "<<s.fCopyNumber<<endl;
	Copy(s);
}
//----------------------------------------------------------------------
PndDrcSurfCyl&  PndDrcSurfCyl::operator=(const PndDrcSurfCyl& s)
{
	if (s.fVerbosity>=1) cout<<"  PndDrcSurfCyl::operator="
				<<"(const PndDrcSurfCyl&) name,copy: "
				<<s.fName<<" "<<s.fCopyNumber<<endl;
	if (&s != this)
	{
		static_cast<PndDrcSurfAbs&>((*this)) = s; // assignment of base class part.
		Copy(s);
	}

	return *this;
}
//----------------------------------------------------------------------
void PndDrcSurfCyl::Set(XYZPoint r1, XYZPoint r2, double radius)
{
	fP1 = r1;
	fP2 = r2;
	fRadius = radius;



}
//----------------------------------------------------------------------
XYZVector PndDrcSurfCyl::Normal(const XYZPoint& point) const
{
  // normal vector is perpendicular to vector fP1-fP2

	XYZVector diff(fP2-fP1);
	XYZVector cross1 = (point-fP1).Cross(diff);

	return (diff.Cross(cross1)).Unit();
}
//----------------------------------------------------------------------
bool PndDrcSurfCyl::SurfaceHit(PndDrcPhoton& ph,
							   XYZPoint&  pos_new,
							   double&    path_length) const
{
  //static const double eps = 1.0e-9;

  // for the methode look up
  // http://www.matheraum.de/read?t=77538&v=t
  // but watch out: the results are wrong.

	XYZPoint  a(ph.Position());
	XYZVector u((ph.Direction()).Unit()); //  photon: a + mu*u;
	XYZPoint  b(fP1);
	XYZVector n(fP2-fP1);

	XYZVector c = a-b;
	XYZVector p = u.Dot(n)/n.Mag2()*n;
	XYZVector m = n.Dot(c)/n.Mag2()*n;

  // mu2 * s + mu * p + q = 0;

	double s = u.Dot(u) - 2*u.Dot(p) + p.Dot(p);
	double pp = 2*(c.Dot(u) - c.Dot(p) - u.Dot(m) + p.Dot(m));
	double qq = c.Dot(c) - 2*c.Dot(m) + m.Dot(m) - fRadius*fRadius;


  //cout<<" s P Q "<<s<<" "<<P<<" "<<Q<<endl;//###
	if (s != 0)
	{
		pp /= s;
		qq /= s;
	}
	else
	{
		return false;
	}

	double root2 = pp*pp/4 - qq;
  //cout<<" root2="<<root2<<endl;//###


	if (root2<0) return false;

	double mu1 = -pp/2 + sqrt(root2);
	double mu2 = -pp/2 - sqrt(root2);

  //cout<<" mu1/2 = "<<mu1<<" "<<mu2<<endl;//###
	double mu_low,mu_high;
	if (mu1<mu2)
	{
		mu_low  = mu1;
		mu_high = mu2;
	}
	else
	{
		mu_low  = mu2;
		mu_high = mu1;
	}

	double mu=-1;
	if (mu_low>0)
	{
		mu = mu_low;
	}
	else if (mu_high>0)
	{
		mu = mu_high;
	}
	else
	{
		return false;
	}

	if (mu>0)
	{
		double lambda = (n.Dot(c)+mu*n.Dot(u))/n.Mag2();
      //cout<<" lambda = "<<lambda<<endl;//###
		if (lambda>0 && lambda<1)
		{
			pos_new = a + mu*u;
			path_length = mu;
          if (fPixel){
	  if (fEffiCathode
	      ->EffiFlag(ph.Wavelength(),
			 ph.Direction().Dot(Normal(pos_new))))
	    { 
	      ph.SetFate(Drc::kPhotMeasured);
	      //if (fPixelCorr) ph.SetPosition1(fPixelPoint);
		if (fPixelCorr) 
		  {
		    ph.SetPosition1(fPixelPoint);
		    ph.SetSurface1(this);
		  }

	    }
	  else
	    { 
	      ph.SetFate(Drc::kPhotAbsorbed);
	    }
	  }
	  
			//if (fPixel) ph.SetFate(Drc::kPhotMeasured);
			return true;
		}

	}

	return false;

}
//----------------------------------------------------------------------
void PndDrcSurfCyl::Print(fstream& stream) const
{
	const double kPi=3.1415926535;

	TVector3 diff(fP2.X()-fP1.X(), fP2.Y()-fP1.Y(), fP2.Z()-fP1.Z());
	TVector3 norm = fRadius * (diff.Orthogonal()).Unit();
	stream<<" TPolyLine3D *l1 = new TPolyLine3D(49);"<<endl;
	stream<<" TPolyLine3D *l2 = new TPolyLine3D(49);"<<endl;
	TPolyLine3D *l1 = new TPolyLine3D(49); // for an opened canvas
	TPolyLine3D *l2 = new TPolyLine3D(49); // for an opened canvas
	TVector3 r1_null;
	TVector3 r2_null;
	TVector3 r1;
	TVector3 r2;

	for (int i=0; i<48; i++)
	{
		norm.Rotate((2*kPi/360)*360/48, diff);
		stream<<" TPolyLine3D *l = new TPolyLine3D(2);"<<endl;
		r1 = TVector3(fP1.X()+norm.X(), fP1.Y()+norm.Y(), fP1.Z()+norm.Z());
		stream<<" l->SetPoint("<<0<<","
				<<r1.X()<<","
				<<r1.Y()<<","
				<<r1.Z()<<");"<<endl;
		r2 = TVector3(fP2.X()+norm.X(), fP2.Y()+norm.Y(), fP2.Z()+norm.Z());
		stream<<" l->SetPoint("<<1<<","
				<<r2.X()<<","
				<<r2.Y()<<","
				<<r2.Z()<<");"<<endl;
		stream<<" l->SetLineColor("<<fPrintColor<<");"<<endl;
		stream<<" l->Draw();"<<endl;

		// for an opened canvas
		TPolyLine3D *l = new TPolyLine3D(2);
		l->SetPoint( 0, r1.X(), r1.Y(), r1.Z() );
		l->SetPoint( 1, r2.X(), r2.Y(), r2.Z() );
		l->SetLineColor(fPrintColor);
		l->Draw();

		if (i==0)
		{
			r1_null = r1;
			r2_null = r2;
		}
		stream<<" l1->SetPoint("<<i<<","
				<<r1.X()<<","
				<<r1.Y()<<","
				<<r1.Z()<<");"<<endl;

		stream<<" l2->SetPoint("<<i<<","
				<<r2.X()<<","
				<<r2.Y()<<","
				<<r2.Z()<<");"<<endl;

		// for an opened canvas
		l1->SetPoint( i, r1.X(), r1.Y(), r1.Z() );
		l2->SetPoint( i, r2.X(), r2.Y(), r2.Z() );
	}
	stream<<" l1->SetPoint("<<48<<","
			<<r1_null.X()<<","
			<<r1_null.Y()<<","
			<<r1_null.Z()<<");"<<endl;

	stream<<" l2->SetPoint("<<48<<","
			<<r2_null.X()<<","
			<<r2_null.Y()<<","
			<<r2_null.Z()<<");"<<endl;

	stream<<" l1->SetLineColor("<<fPrintColor<<");"<<endl;
	stream<<" l1->Draw();"<<endl;
	stream<<" l2->SetLineColor("<<fPrintColor<<");"<<endl;
	stream<<" l2->Draw();"<<endl;

	l1->SetPoint( 48, r1_null.X(), r1_null.Y(), r1_null.Z() );
	l2->SetPoint( 48, r2_null.X(), r2_null.Y(), r2_null.Z() );
	l1->SetLineColor(fPrintColor);
	l2->SetLineColor(fPrintColor);
	l2->Draw();
}
//----------------------------------------------------------------------
void PndDrcSurfCyl::Print() const
{
	cout<<" Coordinates of PndDrcSurfCyl "<<Name()<<endl;
	cout<<" p1 = "<<fP1<<endl;
	cout<<" p2 = "<<fP2<<endl;
	cout<<" r  = "<<fRadius<<endl;
}
//----------------------------------------------------------------------
void PndDrcSurfCyl::AddTransform(const Transform3D& trans)
{
	if (Verbosity()>=3) cout<<"    PndDrcSurfAbs::addTransform() name="<<fName<<endl;
  //fTrans    *= trans;
  //fTransInv  = fTrans.Inverse();

	fP1 = trans * fP1;
	fP2 = trans * fP2;
	fPixelPoint = trans*fPixelPoint;

}
