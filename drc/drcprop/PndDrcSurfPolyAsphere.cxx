// ----------------------------------------------------
// This file belongs to the ray tracing framework
// for the use with Cherenkov detectors
//
// created 2007
//-----------------------------------------------------
#include "PndDrcEffiAbs.h"
#include "PndDrcSurfPolyAsphere.h"

#include "PndDrcPhoton.h"
//#include "PndDrcOptReflAbs.h"
//#include "PndDrcSurfAbs.h"
//#include "PndDrcSurfPolyFlat.h"
//#include "PndDrcOptMatAbs.h"
//#include "PndDrcOptDev.h"
//#include "PndDrcUtil.h"
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
//#include "Math/RotationX.h"
//using ROOT::Math::RotationX;
//#include "Math/RotationY.h"
//using ROOT::Math::RotationY;
//#include "Math/RotationZ.h"
//using ROOT::Math::RotationZ;
//#include "Math/Rotation3D.h"
//using ROOT::Math::Rotation3D;
//
//#include "Math/Transform3D.h"
//using ROOT::Math::Transform3D;
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
//
//#include <utility>
//using std::pair;

#include "TPolyLine3D.h"


//----------------------------------------------------------------------
PndDrcSurfPolyAsphere::PndDrcSurfPolyAsphere()
{
	fRadius   = -999;
	fChecked  = false;
	fConConst = 0;        // sphere
}
//----------------------------------------------------------------------
PndDrcSurfPolyAsphere* PndDrcSurfPolyAsphere::Clone() const
{
	return new PndDrcSurfPolyAsphere(*this);
}
//----------------------------------------------------------------------
void PndDrcSurfPolyAsphere::Copy(const PndDrcSurfPolyAsphere& s)
{
	fRadius       = s.fRadius;
	fConConst     = s.fConConst;
	fChecked      = s.fChecked;
	fTrans        = s.fTrans;
	fTransInv     = s.fTransInv;
}
//----------------------------------------------------------------------
PndDrcSurfPolyAsphere::PndDrcSurfPolyAsphere(const PndDrcSurfPolyAsphere& s) : PndDrcSurfPolyFlat(s)
{
	if (s.fVerbosity>=1) cout<<"  PndDrcSurfPolyAsphere::PndDrcSurfPolyAsphere"
		<<"(const PndDrcSurfPolyAsphere&) name,copy: "
		<<s.fName<<" "<<s.fCopyNumber<<endl;
	Copy(s);
}
//----------------------------------------------------------------------
PndDrcSurfPolyAsphere&  PndDrcSurfPolyAsphere::operator=(const PndDrcSurfPolyAsphere& s)
{
	if (s.fVerbosity>=1) cout<<"  PndDrcSurfPolyAsphere::operator="
		<<"(const PndDrcSurfPolyAsphere&) name,copy: "
		<<s.fName<<" "<<s.fCopyNumber<<endl;
	if (&s != this)
	{
		static_cast<PndDrcSurfPolyFlat&>((*this)) = s; // assignment of base class part.
		Copy(s);
	}

	return *this;
}
//----------------------------------------------------------------------
XYZVector PndDrcSurfPolyAsphere::Normal(const XYZPoint& point) const
{
	if (!fChecked) fChecked = Check1();// check dimensions

  // transform point back, where sphere was defined.
	XYZPoint p1(fTransInv(point));
  // p1 point on surface in unrotated and unshifted system

	double h2   = p1.X()*p1.X()+p1.Y()*p1.Y();
	double h,root,dzdh,dhdx,dhdy;
	if (h2>1e-12)
	{
		h    = sqrt(h2);
		root = sqrt(fRadius*fRadius - (1.0+fConConst)*h2);

		dzdh = (2*h * (fRadius+root) +
		        (1.0+fConConst)*h/root * h2)/((fRadius+root)*(fRadius+root));
		dhdx = p1.X()/h;
		dhdy = p1.Y()/h;
	}
	else
	{
		dhdx=0; // eg axis parallel beam throug zero
		dhdy=0;
		dzdh=0;
	}


  // real z is R-z
	dzdh *= -1;


	XYZVector pa(1,0,dzdh*dhdx); // vector along slope in x dir.
	XYZVector pb(0,1,dzdh*dhdy); // vector along slope in y dir.

	XYZVector pc(pa.Cross(pb));


	pc = fTrans(pc);

	return -pc.Unit();
}
//----------------------------------------------------------------------
bool PndDrcSurfPolyAsphere::SurfaceHit(PndDrcPhoton& ph,
                                       XYZPoint&  pos_new,
                                       double&    path_length) const
{
	const double kEps = 0.001;
	if (fVerbosity>=4) cout<<" PndDrcSurfPolyAsphere::surfaceHit: test of "
		<<Name()<<endl;

	if (!fChecked) fChecked = Check1();// check dimensions

  // transform back in sphere definitions space
	XYZPoint  pos(ph.Position());
	XYZVector dir(ph.Direction());

	if (fVerbosity>=4) cout<<" PndDrcSurfPolyAsphere::surfaceHit: original photon: \n"
		<<" pos: "<<pos<<" dir: "<<dir<<endl;

	dir = fTransInv*dir;
	pos = fTransInv*pos;

	if (fVerbosity>=4) cout<<" PndDrcSurfPolyAsphere::surfaceHit: transformed photon: \n"
		<<" pos: "<<pos<<" dir: "<<dir<<endl;

  // compute hit position
  //
  // line : x = pos + lambda*dir
  //
  // h2 = x*x+y*y = (posx+lambda dirx)^2 + (posy+lambda diry)^2
  //
  // asphere:
  //
  // R - h2/(R+sqrt[R2-(1+k)h2]) = pz + lambda dz
  //
  // R - h2*(R-sqrt[])/(R2-sqrt[]^2) = pz + lambda dz
  //
  // R - pz = h2*(R-sqrt[])/(R2-R2+(1+k)h2) + lambda dz
  //
  // R - pz = (R-sqrt[])/(1+k) + lambda dz
  //
  // (1+k)(R - pz - lambda dz) = R-sqrt
  //
  // kR - (1+k) pz - (1+k) lambda dz = -sqrt
  //
  // with a=kR - (1+k) pz and b=(1+k) dz
  //
  // a - b lambda = -sqrt[]
  //
  // b2 lambda2 - 2 ab lambda + a2 = R2 - (1+k)h2
  //
  // insert h2
  //
  // b2 lambda2 - 2ab lambda + a2 - R2 =
  // -(1+k)*([px2 + 2 lambda px dx + lambda2 dx2]+[py2 + 2 lambda py dy + lambda2 dy2])
  //
  //
  // with c1 = -(1+k)px2
  //      c2 = -(1+k)2 px dx
  //      c3 = -(1+k)dx2
  // with d1 = -(1+k)py2
  //      d2 = -(1+k)2 py dy
  //      d3 = -(1+k)dy2
  //
  // b2 lambda2 - 2ab lambda + a2 - R2 =
  // c1 + lambda c2 + lambda2 c3 + d1 + lambda d2 + lambda2 d3
  //
  // lambda2 (b2-c3-d3) - lambda (2ab+c2+d2) +(a2-R2-c1-d1) = 0
  //

	long double a  = fConConst*fRadius - (1.0+fConConst)*pos.Z();
	long double b  = (1.0+fConConst)*dir.Z();
	long double c1 = -(1.0+fConConst)*pos.X()*pos.X();
	long double c2 = -(1.0+fConConst)*2*pos.X()*dir.X();
	long double c3 = -(1.0+fConConst)*dir.X()*dir.X();
	long double d1 = -(1.0+fConConst)*pos.Y()*pos.Y();
	long double d2 = -(1.0+fConConst)*2*pos.Y()*dir.Y();
	long double d3 = -(1.0+fConConst)*dir.Y()*dir.Y();

	if (fVerbosity>=4)
	{
		cout<<" PndDrcSurfPolyAsphere::surfaceHit: a  ="<<a<<endl;
		cout<<" PndDrcSurfPolyAsphere::surfaceHit: b  ="<<b<<endl;
		cout<<" PndDrcSurfPolyAsphere::surfaceHit: c1 ="<<c1<<endl;
		cout<<" PndDrcSurfPolyAsphere::surfaceHit: c2 ="<<c2<<endl;
		cout<<" PndDrcSurfPolyAsphere::surfaceHit: c3 ="<<c3<<endl;
		cout<<" PndDrcSurfPolyAsphere::surfaceHit: d1 ="<<d1<<endl;
		cout<<" PndDrcSurfPolyAsphere::surfaceHit: d2 ="<<d2<<endl;
		cout<<" PndDrcSurfPolyAsphere::surfaceHit: d3 ="<<d3<<endl;
	}

  //
  // for k=-1 , parabola: c1,c2,c3,d1,d2,d3,b=0 a=-R
  //
  // R - h2/(R+sqrt[R2]) = pz + lambda dz
  // 2R2 - h2 = 2R pz + lamda 2R dz
  // 2R2 - [(posx+lambda dirx)^2 + (posy+lambda diry)^2] = 2R pz + lambda 2R dz
  // 2R2 - [px2+py2 + lambda 2 (pxdx+pydy) + lambda2 (dx2 + dy2) ]
  //                      = 2R pz + lambda 2R dz
  // [px2+py2 + lambda 2 (pxdx+pydy) + lambda2 (dx2 + dy2) ]
  //                      = -2R pz - lambda 2R dz +2R2
  //
  // lambda2 (dx2+dy2) + lambda 2 [(pxdx+pydy)+R dz] + px2+py2+2Rpz-2R2 = 0
  //            h2                    a                  b

	long double lambda;

  // special treatment for parabola
	if (fabs(fConConst+1)<1.0e-4)
	{// parabola
		long double h2 = dir.X()*dir.X()+dir.Y()*dir.Y();
		a = 2*(pos.X()*dir.X()+pos.Y()*dir.Y()+fRadius*dir.Z());
		b = 2*fRadius*(pos.Z()-fRadius) + pos.X()*pos.X() + pos.Y()*pos.Y();
		if (fabs(h2)<1.0e-9)
		{
			if (a==0)
			{
				if (fVerbosity>=4) cout<<" PndDrcSurfPolyAsphere::surfaceHit no lambda"
					<<endl;
				return false;
			}
			else
			{
				lambda = -b/a;
				XYZPoint pos_new1     = pos + lambda*dir;
				XYZPoint pos_check(pos_new1);
				pos_check.SetZ(0);
				bool hit = WithinSurface(pos_check);

				if (hit && lambda>0)
				{
					pos_new = pos_new1;
					path_length = lambda;
					if (!OnSurface(pos_new))
					{
						if (fVerbosity>=4) cout<<" PndDrcSurfPolyAsphere::surfaceHit"
							<<" calc. point not on surface,"
							<<" wrong lambda"<<endl;
						return false; // wrong lambda
					}
					pos_new = fTrans*pos_new;
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
					if (fVerbosity>=4)
						cout<<" PndDrcSurfPolyAsphere::surfaceHit hit lambda="
						<<lambda<<endl;
					return true;
				}
				else
				{
					if (fVerbosity>=4)
						cout<<" PndDrcSurfPolyAsphere::surfaceHit hit,lambda="
						<<hit<<" "<<lambda<<endl;
					return false;;
				}
			}
		}
		else
		{// h2>0
			long double p = a/h2;
			long double q = b/h2;
			long double root2 = p*p/4-q;

			if (fVerbosity>=4)
				cout<<" PndDrcSurfPolyAsphere::surfaceHit p,q,root2: "
				<<p<<" "<<q<<" "<<root2<<endl;
			if (root2<0)
			{
				if (fVerbosity>=4)
					cout<<" PndDrcSurfPolyAsphere::surfaceHit root2<0"<<endl;
				return false;
			}
			else
			{// root2>0
				long double lambda1 = -p/2 + sqrt(root2);
				long double lambda2 = -p/2 - sqrt(root2);
				if (fVerbosity>=4) cout<<" PndDrcSurfPolyAsphere::surfaceHit lambda_1,2 ="
					<<lambda1<<" "<<lambda2<<endl;
				if (lambda1>kEps && lambda2>kEps)
				{ // check both solutions
					XYZPoint pos_check     = pos + lambda1*dir;
					pos_check.SetZ(0);
					bool hit1 = WithinSurface(pos_check);
					pos_check              = pos + lambda2*dir;
					pos_check.SetZ(0);
					bool hit2 = WithinSurface(pos_check);
					if (hit1 && hit2)
					{
		      // take closest
						lambda = (lambda1<lambda2) ? lambda1 : lambda2;
						pos_new = pos + lambda*dir;
						if (!OnSurface(pos_new))
						{
							if (fVerbosity>=4) cout<<" PndDrcSurfPolyAsphere::surfaceHit"
								<<" calc. point not on surface,"
								<<" wrong lambda"<<endl;
							return false; // wrong lambda
						}
						path_length = lambda;
						pos_new = fTrans*pos_new;
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
						if (fVerbosity>=4)
						{
							cout<<" PndDrcSurfPolyAsphere::surfaceHit lambda_1,2="
								<<lambda1<<" "<<lambda2<<endl;
							cout<<"                                took lambda="
								<<lambda<<endl;
						}
						return true;
					}
					else if (hit1)
					{
						pos_new = pos + lambda1*dir;
						if (!OnSurface(pos_new))
						{
							if (fVerbosity>=4) cout<<" PndDrcSurfPolyAsphere::surfaceHit"
								<<" calc. point not on surface,"
								<<" wrong lambda"<<endl;
							return false; // wrong lambda
						}
						path_length = lambda1;
						pos_new = fTrans*pos_new;
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
						if (fVerbosity>=4)
							cout<<" PndDrcSurfPolyAsphere::surfaceHit hit1 lambda_1="
							<<lambda1<<endl;
						return true;
					}
					else if (hit2)
					{
						pos_new = pos + lambda2*dir;
						if (!OnSurface(pos_new))
						{
							if (fVerbosity>=4) cout<<" PndDrcSurfPolyAsphere::surfaceHit"
								<<" calc. point not on surface,"
								<<" wrong lambda"<<endl;
							return false; // wrong lambda
						}
						path_length = lambda2;
						pos_new=fTrans*pos_new;
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
						if (fVerbosity>=4)
							cout<<" PndDrcSurfPolyAsphere::surfaceHit hit2 lambda_1="
							<<lambda2<<endl;
						return true;
					}
					else
					{
						if (fVerbosity>=4)
							cout<<" PndDrcSurfPolyAsphere::surfaceHit "
							<<"no hits for both lambdas"<<endl;
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
					if (fVerbosity>=4) cout<<" PndDrcSurfPolyAsphere::surfaceHit "
						<<"both lambdas < 0 "
						<<lambda1<<" "<<lambda2<<endl;
					return false;
				}
			}
		}
	} // end parabola
	else if (fabs(b*b-c3-d3)<1.0e-12)
	{ // otherwise the too small argument will cause trouble in
      //                        the next clause

		lambda = (a*a-fRadius*fRadius-c1-d1)/(2*a*b+c2+d2);
		if (lambda<kEps)
		{
			if (fVerbosity>=4) cout<<" PndDrcSurfPolyAsphere::surfaceHit lambda="
				<<lambda<<" <kEps no hit"<<endl;
			return false;
		}
		if (fVerbosity>=4) cout<<" PndDrcSurfPolyAsphere::surfaceHit lambda="
			<<lambda<<endl;
	}
	else
	{// most general case
		long double p  = -(2*a*b+c2+d2)/(b*b-c3-d3);
		long double q  = (a*a-fRadius*fRadius-c1-d1)/(b*b-c3-d3);
		long double root2 = p*p/4 - q;
		if (root2<0)
		{
			return false; // photon does not hit asphere
	  // eg. photon behind asphere perpendicular to z-axis.
			cerr<<" *** PndDrcSurfPolyAsphere::surfaceHit: root<0 : "<<root2<<endl;
		}
		long double lambda1 = -p/2 + sqrt(root2);
		long double lambda2 = -p/2 - sqrt(root2);

		if (fVerbosity>=4)
		{
			cout<<" PndDrcSurfPolyAsphere::surfaceHit lambda_1,2="
				<<lambda1<<" "<<lambda2<<endl;
			cout<<"                     dir1 = "<< lambda1*dir<<endl;
			cout<<"                     dir2 = "<< lambda2*dir<<endl;
		}


		if (lambda1>kEps && lambda2>kEps)
		{ // check both solutions
			XYZPoint pos_check   = pos + lambda1*dir;
			pos_check.SetZ(0);
			bool hit1 = WithinSurface(pos_check);
			pos_check            = pos + lambda2*dir;
			pos_check.SetZ(0);
			bool hit2 = WithinSurface(pos_check);
			if (hit1 && hit2)
			{
	      // take closest
				lambda = (lambda1<lambda2) ? lambda1 : lambda2;
				pos_new = pos + lambda*dir;
				if (!OnSurface(pos_new))
				{
					if (fVerbosity>=4) cout<<" PndDrcSurfPolyAsphere::surfaceHit"
						<<" calc. point not on surface,"
						<<" wrong lambda"<<endl;
					return false; // wrong lambda
				}
				path_length = lambda;
				pos_new = fTrans*pos_new;
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
				if (fVerbosity>=4) cout<<" PndDrcSurfPolyAsphere::surfaceHit lambda_1,2="
					<<lambda1<<" "<<lambda2<<endl;
				return true;
			}
			else if (hit1)
			{
				pos_new = pos + lambda1*dir;
				if (!OnSurface(pos_new))
				{
					if (fVerbosity>=4) cout<<" PndDrcSurfPolyAsphere::surfaceHit"
						<<" calc. point not on surface,"
						<<" wrong lambda"<<endl;
					return false; // wrong lambda
				}
				path_length = lambda1;
				pos_new = fTrans*pos_new;
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
				if (fVerbosity>=4) cout<<" PndDrcSurfPolyAsphere::surfaceHit lambda_1,2="
					<<lambda1<<" "<<lambda2<<endl;
				return true;
			}
			else if (hit2)
			{
				pos_new = pos + lambda2*dir;
				if (!OnSurface(pos_new))
				{
					if (fVerbosity>=4) cout<<" PndDrcSurfPolyAsphere::surfaceHit"
						<<" calc. point not on surface,"
						<<" wrong lambda"<<endl;
					return false; // wrong lambda
				}
				path_length = lambda2;
				pos_new = fTrans*pos_new;
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
				if (fVerbosity>=4) cout<<" PndDrcSurfPolyAsphere::surfaceHit lambda_1,2="
					<<lambda1<<" "<<lambda2<<endl;
				return true;
			}
			else
			{
				if (fVerbosity>=4) cout<<" PndDrcSurfPolyAsphere::surfaceHit "
					<<"no hits for both lambdas"<<endl;
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
			if (fVerbosity>=4) cout<<" PndDrcSurfPolyAsphere::surfaceHit "
				<<"both lambdas < 0 "
				<<lambda1<<" "<<lambda2<<endl;
			return false;
		}
	}

	path_length = lambda;
	pos_new     = pos + path_length*dir;

	if (fVerbosity>=4) cout<<" PndDrcSurfPolyAsphere::surfaceHit:"<<Name()
		<<" only one lambda survived"
		<<endl;

	if (fVerbosity>=4) cout<<" PndDrcSurfPolyAsphere::surfaceHit pos_new before trans. ="
		<<pos_new<<endl;
	if (fVerbosity>=4) cout<<" PndDrcSurfPolyAsphere::surfaceHit zVal of pos_new ="
		<<ZVal(pos_new.X(),pos_new.Y())<<endl;



	if (!OnSurface(pos_new))
	{
		if (fVerbosity>=4) cout<<" PndDrcSurfPolyAsphere::surfaceHit"
			<<" calc. point not on surface,"
			<<" wrong lambda"<<endl;
		return false; // wrong lambda
	}


  // check if pos_new is within polynom
	XYZPoint pos_check(pos_new);
	pos_check.SetZ(0);

	if (WithinSurface(pos_check))
	{
		pos_new = fTrans*pos_new;
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
		if (fVerbosity>=4) cout<<" PndDrcSurfPolyAsphere::surfaceHit: lambda -> hit"
			<<endl;
		return true;
	}

	if (fVerbosity>=4) cout<<" PndDrcSurfPolyAsphere::surfaceHit "
		<<Name()<<": false, not within surface bounds"<<endl;

	return false;

}
//----------------------------------------------------------------------
bool PndDrcSurfPolyAsphere::Check1() const
{
	if (fRadius<=0)
	{
		cerr<<" ***  PndDrcSurfPolyAsphere::check: "
			<<fName<<": wrong or not set radius="
			<<fRadius<<endl;
		exit(EXIT_FAILURE);
	}
	for (unsigned int i=0; i<fP.size(); i++)
	{
		if (ZVal(fP[i].X(),fP[i].Y())<0)
		{
			cerr<<" ***  PndDrcSurfPolyAspher::check: "
				<<fName<<": point outside sphere projection:"
				<<" fP["<<i<<"]="
				<<fP[i].X()<<" "
				<<fP[i].Y()<<" "
				<<fP[i].Z()<<endl;
			cerr<<"      z = "<<ZVal(fP[i].X(),fP[i].Y())<<endl;
			exit(EXIT_FAILURE);
	  //  return false;
		}
	}
	for (unsigned int i=0; i<fP.size(); i++)
	{
		if (fP[i].Z() != 0)
		{
			cerr<<" ***  PndDrcSurfPolyAsphere::check: "
				<<fName<<": z component of point != 0 "
				<<" fP["<<i<<"]="
				<<fP[i].X()<<" "
				<<fP[i].Y()<<" "
				<<fP[i].Z()<<endl;
			exit(EXIT_FAILURE);
	  //return false;
		}
	}

	return true;
}
//----------------------------------------------------------------------
XYZPoint PndDrcSurfPolyAsphere::LimitingPoint(unsigned int i)
{
	if (i>(fP.size()-1) || i<0)
	{
		cerr<<" *** PndDrcSurfPolyAsphere::limitingPoint: index out of range, i="<<i<<endl;
		exit(EXIT_FAILURE);
	}
	XYZPoint p1(fP[i]);

	p1.SetZ(ZVal(p1.X(),p1.Y()));

	p1 = fTrans*p1;
	return p1;
}
//----------------------------------------------------------------------
XYZPoint PndDrcSurfPolyAsphere::CenterPoint()
{
	return fTrans*XYZPoint(0,0,fRadius);
}
//----------------------------------------------------------------------
void PndDrcSurfPolyAsphere::Print(fstream& stream) const
{
	if (!fChecked) fChecked = Check1();// check dimensions

  // print points
	unsigned int isize = fP.size();

	if (isize < 3)
	{
		cerr<<" *** PndDrcSurfPolyAsphere::surfaceHit: surface has only "<<fP.size()
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
				fP1.SetZ(ZVal(fP1.X(),fP1.Y())); // fP1.Z()=0...

				fP1 = fTrans*fP1;

				stream<<" l->SetPoint("<<icnt<<","
					<<fP1.X()<<","
					<<fP1.Y()<<","
					<<fP1.Z()<<");"<<endl;

				l->SetPoint( icnt, fP1.X(), fP1.Y(), fP1.Z()); // for an opened canvas
				icnt++;
			}
		}

		fP1 = fP[0]*frac;
		fP1.SetZ(ZVal(fP1.X(),fP1.Y())); // fP1.Z()=0...

		fP1 = fTrans*fP1;
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

}
//----------------------------------------------------------------------
void PndDrcSurfPolyAsphere::Print() const
{
	cout<<" Coordinates of PndDrcSurfPolyAsphere "<<Name()<<endl;
	static_cast<PndDrcSurfPolyFlat>((*this)).Print();
	cout<<" r  = "<<fRadius<<endl;
	cout<<" c  = "<<fConConst<<endl;
}
//----------------------------------------------------------------------
double PndDrcSurfPolyAsphere::ZVal(double x, double y) const
{
	double h2  = x*x + y*y;
	double den = fRadius*(1.0+sqrt(1.0-(1.0+fConConst)*h2/(fRadius*fRadius)));
	return fRadius - (h2/den);
}
//----------------------------------------------------------------------
bool PndDrcSurfPolyAsphere::OnSurface(const XYZPoint& p) const
{
	double x = p.x();
	double y = p.y();
	double z = p.z();

	double zz = ZVal(x,y);

	return (fabs(z-zz)<0.1) ? true : false;

}
//----------------------------------------------------------------------
void PndDrcSurfPolyAsphere::AddTransform(const Transform3D& trans)
{
  if (Verbosity()>=3) cout<<"    PndDrcSurfCyl::addTransform() name="<<fName<<endl;
  fTrans    = trans * fTrans;
  fTransInv = fTrans.Inverse();
  fPixelPoint = trans*fPixelPoint;
}
