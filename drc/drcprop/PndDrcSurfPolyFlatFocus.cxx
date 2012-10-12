// ----------------------------------------------------
// This file belongs to the ray tracing framework
// for the use with Cherenkov detectors
//
// created 2007
//-----------------------------------------------------
#include "PndDrcEffiAbs.h"
#include "PndDrcSurfPolyFlatFocus.h"

#include "PndDrcPhoton.h"
//#include "PndDrcOptReflAbs.h"
//#include "PndDrcSurfAbs.h"
//#include "PndDrcOptMatAbs.h"
//#include "PndDrcOptDev.h"
//
//#include "TObject.h"
#include "TVector3.h"
#include "TRandom.h"
//#include "TMatrixD.h"
#include "TVectorD.h"
#include "TDecompSVD.h"
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
PndDrcSurfPolyFlatFocus::PndDrcSurfPolyFlatFocus()
{
  fAxisPoint   = XYZPoint(0,0,0);
  fFocalLength = 0.0;
  
  
}
//----------------------------------------------------------------------
PndDrcSurfPolyFlatFocus* PndDrcSurfPolyFlatFocus::Clone() const
{
	return new PndDrcSurfPolyFlatFocus(*this);
}
//----------------------------------------------------------------------
void PndDrcSurfPolyFlatFocus::Copy(const PndDrcSurfPolyFlatFocus& s)
{
	fP           = s.fP;
	fNormal      = s.fNormal;
	fRadius      = s.fRadius;
	fConConst    = s.fConConst;
	fFocalLength = s.fFocalLength;
	fAxisPoint   = s.fAxisPoint;
	fPhotonDir   = s.fPhotonDir;
	
}
//----------------------------------------------------------------------
PndDrcSurfPolyFlatFocus::PndDrcSurfPolyFlatFocus(const PndDrcSurfPolyFlatFocus& s) : PndDrcSurfAbs(s)
{
	if (s.fVerbosity>=1) cout<<"  PndDrcSurfPolyFlatFocus::PndDrcSurfPolyFlatFocus"
		<<"(const PndDrcSurfPolyFlatFocus&) name,copy: "
		<<s.fName<<" "<<s.fCopyNumber<<endl;
	Copy(s);
}
//----------------------------------------------------------------------
PndDrcSurfPolyFlatFocus&  PndDrcSurfPolyFlatFocus::operator=(const PndDrcSurfPolyFlatFocus& s)
{
	if (s.fVerbosity>=1) cout<<"  PndDrcSurfPolyFlatFocus::operator="
		<<"(const PndDrcSurfPolyFlatFocus&) name,copy: "
		<<s.fName<<" "<<s.fCopyNumber<<endl;
	if (&s != this)
	{
		static_cast<PndDrcSurfAbs&>((*this)) = s; // assignment of base class part.
		Copy(s);
	}

	return *this;
}
//----------------------------------------------------------------------
void PndDrcSurfPolyFlatFocus::SetFocalPoint(double focal_length, XYZPoint axis_point)
{
  fFocalLength = focal_length;
  fAxisPoint   = axis_point;
}

//----------------------------------------------------------------------
void PndDrcSurfPolyFlatFocus::AddPoint(XYZPoint point)
{
	fP.push_back(point);
	int isize = fP.size();


  // search normal vector from point differences with largest absolute
  // cross product.
	if (isize > 2)
	{
		double length2 = 0;
		for (int i=2; i<isize; i++)
		{
			XYZVector normal = ((fP[i-1]-fP[i-2]).Cross(fP[i]-fP[i-1]));

			if (normal.Mag2() > length2)
			{
				length2 = normal.Mag2();
				fNormal = normal.Unit();
			}
		}
	}




	if (Verbosity()>=5)
	{
		cout<<"      PndDrcSurfPolyFlatFocus::addPoint: vector size = "<<fP.size()<<endl;
		for (int i=0; i<isize; i++)
		{
			cout<<" i="<<i<<" x y z = "
				<<fP[i].X()<<" "
				<<fP[i].Y()<<" "
				<<fP[i].Z()<<endl;
		}
	}
}
//----------------------------------------------------------------------
XYZVector PndDrcSurfPolyFlatFocus::Normal(const XYZPoint& point) const
{
  // normal vector is a constant.
  if (fAxisPoint.Mag2()==0 || fFocalLength==0)
    {
      cerr<<"      PndDrcSurfPolyFlatFocus::surfaceHit: focal length or axis  point not set, abort. "<<endl;
      exit(EXIT_FAILURE);
    }


  XYZPoint  fp        = fAxisPoint - fFocalLength*fNormal;
  XYZVector refl_dir  = fPhotonDir - 2*(fPhotonDir.Dot(fNormal))*fNormal;
  double    lambda    = (fAxisPoint-fp).Dot(fNormal) / refl_dir.Dot(fNormal);
  XYZPoint  dest      = fAxisPoint - lambda * refl_dir;
  //dest = -1*dest;
  
  XYZVector n1(dest-point);
  n1 = n1.Unit();
  
  //double a1 = n1.X()-fPhotonDir.X();
  //double a2 = n1.Y()-fPhotonDir.Y();
  //double a3 = n1.Z()-fPhotonDir.Z();
  
  
  XYZVector u(n1-fPhotonDir);
  //u = u.Unit();
  

  // cout<<" test"<<endl;
  
  return u.Unit();
  
  //if (fNormal.Dot(fFocalPoint-point) >0)
    //{
    //return ( fNormal + (fFocalPoint-point).Unit()).Unit();
    //}
  //else
    //{
      //  return (-fNormal + (fFocalPoint-point).Unit()).Unit();
    //}

    //return fNormal;
  



}
//----------------------------------------------------------------------
bool PndDrcSurfPolyFlatFocus::LineCross(const XYZPoint& pa, const XYZPoint& pb,
                                   const XYZPoint& qa, const XYZPoint& qb) const
{
	const double kEps = 1.0e-9;

	TVector3 p((pb-pa).x(), (pb-pa).y(), (pb-pa).z());
	TVector3 q((qb-qa).x(), (qb-qa).y(), (qb-qa).z());


  // Mag instead of Mag2 otherwise this contraint cannot be true
	if (fabs(p.Dot(q)/(p.Mag2()*q.Mag2())) > 1.0L-kEps) return false; // directions parallel

  // calculate closest distance
	TVector3 n0   = (p.Cross(q)).Unit();
	double   dist = fabs((pb-qb).Dot(n0)); // ??? n0 is only perpendicular to pb-qb if pa is in the plane (qb,qa) and hence also the coord. origin
	if (dist>kEps) return false; // return if pb-qb is not perpendicular to n0

	TVector3 pp(pa.x(),pa.y(),pa.z());
	TVector3 qq(qa.x(),qa.y(),qa.z());
  // solve
  // P + p*s = Q + q*t
  //
  // d**2 = ((P+ps) - (Q+qt))**2
  //
  // d/ds d**2 = d/dt d**2 => (P-Q) + (sp-tq) = 0
  //
  // This linear equation is for the minimum distance between both lines
  // We prefer minimum distance over equality of both lines due to numerical
  // uncertainties.
  //
  // Solve linear equation
  //
  //    px  -qx   s     Qx-Px
  //    py  -qy   t  =  Qy-Py
  //    pz  -qz         Qz-Pz
  //
	TMatrixD mat_a(3,2);

	mat_a(0,0) =  p.X();
	mat_a(1,0) =  p.Y();
	mat_a(2,0) =  p.Z();
	mat_a(0,1) = -q.X();
	mat_a(1,1) = -q.Y();
	mat_a(2,1) = -q.Z();

	TVectorD vec_b(3);

	vec_b(0) = qq.X()-pp.X();
	vec_b(1) = qq.Y()-pp.Y();
	vec_b(2) = qq.Z()-pp.Z();

	TDecompSVD svd(mat_a);

	if (!svd.Solve(vec_b)) return false;
  //cout<<" crossing "<<B[0]<<" "<<B[1]<<endl;//###

	if (vec_b[0]>-kEps && vec_b[0]<1+kEps && vec_b[1]>-kEps && vec_b[1]<1+kEps) return true;
	return false;

}
//----------------------------------------------------------------------
bool PndDrcSurfPolyFlatFocus::SurfaceHit(PndDrcPhoton& ph,
                                    XYZPoint&        pos_new,
                                    double&          path_length) const
{
  if (Verbosity()>=4)
    {
      cout<<"      PndDrcSurfPolyFlatFocus::surfaceHit: name="<<Name()<<endl;
    }

  static const double kEps = 1.0e-9;

  unsigned int isize = fP.size();

  fPhotonDir = ph.Direction(); // save for usage in Normal(). (This is not the cleanest programming style...)
  

  if (isize < 3)
    {
      cerr<<" *** PndDrcSurfPolyFlatFocus::surfaceHit: surface has only "<<fP.size()
	  <<" points. Aborting. Name="<<Name()<<endl;
      exit(EXIT_FAILURE);
    }


  XYZPoint  point = ph.Position();
  XYZVector dir   = ph.Direction().Unit();

  // calculate hits with area
  // (point+lambda*dir - fP[0]) * fNormal = 0
  //
  // point*fNormal + lambda*dir*fNormal - fP[0]*fNormal = 0
  //
  // lamda = (fP[0]-point)*fNormal  / dir*fNormal
  //
  // dir*fNormal > 0 , see condition above

  double h = dir.Dot(fNormal);
  if (h==0)
    {
      if (Verbosity()>=4) cout<<"      PndDrcSurfPolyFlatFocus::surfaceHit: dir perp norm\n";
      return false;
    }
  double   lambda = (fP[0]-point).Dot(fNormal) / dir.Dot(fNormal);
  pos_new = point + lambda*dir;

  if (Verbosity()>=4)
    {
      cout<<"      PndDrcSurfPolyFlatFocus::surfaceHit:"<<endl;
      for (unsigned int i=0; i<isize; i++) cout<<"         fP["<<i<<"] = "<<fP[i]<<endl;
      cout<<"         point = "<<ph.Position()<<endl;
      cout<<"         dir   = "<<ph.Direction()<<endl;
      cout<<"         normal="<<fNormal<<endl;
      cout<<"         (p0-p).norm = "<<(fP[0]-point).Dot(fNormal)<<endl;
      cout<<"         dir.norm    = "<<dir.Dot(fNormal)<<endl;
      cout<<"         lambda="<<lambda<<endl;
      cout<<"         pos_new="<<pos_new<<endl;
    }
  if (lambda<-kEps) // flying in wrong direction.
    {
      if (Verbosity()>=4) cout<<"      PndDrcSurfPolyFlatFocus::surfaceHit: wrong dir "
			      <<" lambda par ="<<lambda<<endl;
      return false;
    }

  path_length = fabs(lambda);
	
  if (WithinSurface(pos_new))
    {
      if (fPixel)
	{ 
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
      return true;
    }
	
  return false;
}
//----------------------------------------------------------------------
bool PndDrcSurfPolyFlatFocus::WithinSurface(XYZPoint& point) const
{
  // check if pos_new is within area.
  //
  //
  //
  //         p3-------------------------p2
  //         /                          /
  //        /                         /
  //       /                        /
  //      /                a------/--------- b
  //     /                      /
  //    /                     /
  //   p4-------------------p1         normal vector to top
  //
  // if the line ab intersects the borders an equal number the point is inside
  // for odd numbers it is outside.

  unsigned int isize = fP.size();

  // find a point outside.
  vector<XYZVector> diff(isize);

  diff[isize-1] = (fP[0] - fP[isize-1]);

  TRandom ran;
  

  // the random addition prevents for rectangles that the middle point of the rectangle
  // is just going throug an edge point towards the outer point.
  // The middle point of a rectangle is often set deliberately for photons for debugging
  // purposes.
  // it is not important that random gets a good seed, results do not depend on outside
  // point.


  XYZPoint p_out(diff[isize-1]);
  for (unsigned int i=0; i<isize-1; i++)
    {
      diff[i] = (fP[i+1] - fP[i])*(1.0+ran.Uniform());
      if (diff[i].Dot(diff[isize-1])<0) diff[i] *= -1;
      p_out += diff[i];
    }
  p_out += XYZVector(fP[0]); // = p0 + differences

  //cout<<" p_out="<<p_out.X()<<" "<<p_out.Y()<<" "<<p_out.Z()<<endl;//###
  //cout<<" 0 "<<fP[0].X()<<" "<<fP[0].Y()<<" "<<fP[0].Z()<<endl;
  //cout<<" 1 "<<fP[1].X()<<" "<<fP[1].Y()<<" "<<fP[1].Z()<<endl;
  //cout<<" 2 "<<fP[2].X()<<" "<<fP[2].Y()<<" "<<fP[2].Z()<<endl;
  //cout<<" 3 "<<fP[3].X()<<" "<<fP[3].Y()<<" "<<fP[3].Z()<<endl;
  
  // determine number of intersections.
  int intersections = 0;
  if (LineCross(p_out,point,fP[isize-1],fP[0])) intersections++;
  //cout<<" 1st "<<intersections<<endl; //###
  for (unsigned int i=0; i<isize-1; i++)
    {
		if (LineCross(p_out,point,fP[i],fP[i+1])) intersections++;
		/*
		  cout<<" nst "<<i<<" "<<intersections<<" "
		  <<fP[i].X()<<" "
		  <<fP[i].Y()<<" "
		  <<fP[i].Z()<<" "
		  <<fP[i+1].X()<<" "
		  <<fP[i+1].Y()<<" "
		  <<fP[i+1].Z()<<endl;
		*/
	}

	if (intersections%2 == 0)
	{
		if (Verbosity()>=4)
		{
			cout<<"      PndDrcSurfPolyFlatFocus::withinSurface:"<<Name()<<" no hit "
				<<"intersections="<<intersections<<endl;
			cout<<"      pout = "<<p_out<<endl;
			cout<<"      pin  = "<<point<<endl;

		}
		return false;
	}
	if (Verbosity()>=4)
	{
		cout<<"      PndDrcSurfPolyFlatFocus::withinSurface:"<<Name()<<" hit intersections="
			<<intersections<<endl;
	}
	return true;
}
//----------------------------------------------------------------------
void PndDrcSurfPolyFlatFocus::Print(fstream& stream) const
{
	int isize = fP.size();


	if (isize < 3)
	{
		cerr<<" *** PndDrcSurfPolyFlatFocus::surfaceHit: surface has only "<<fP.size()
			<<" points. Aborting. Name="<<Name()<<endl;
		exit(EXIT_FAILURE);
	}

	stream<<" TPolyLine3D *l = new TPolyLine3D("<<isize+1<<");"<<endl;

	for (int i=0; i<isize; i++)
	{
		stream<<" l->SetPoint("<<i<<","
			<<fP[i].X()<<","
			<<fP[i].Y()<<","
			<<fP[i].Z()<<");"<<endl;
	}
	stream<<" l->SetPoint("<<isize<<","
		<<fP[0].X()<<","
		<<fP[0].Y()<<","
		<<fP[0].Z()<<");"<<endl;
	stream<<" l->SetLineColor("<<fPrintColor<<");"<<endl;
	stream<<" l->Draw();"<<endl;

  // for a opened canvas
	TPolyLine3D *l = new TPolyLine3D(isize+1);

	for (int i=0; i<isize; i++)
		l->SetPoint(i, fP[i].X(), fP[i].Y(), fP[i].Z());

	l->SetPoint(isize, fP[0].X(), fP[0].Y(), fP[0].Z());
	l->SetLineColor(fPrintColor);
	l->Draw();
}
//----------------------------------------------------------------------
void PndDrcSurfPolyFlatFocus::Print() const
{
	unsigned int isize = fP.size();
	cout<<" Coordinates of surface "<<Name()<<endl;
	for (unsigned int i=0; i<isize; i++)
	{
		cout<<" p["<<i<<"] = "<<fP[i]<<endl;
	}
	cout<<" norm= "<<fNormal<<endl;
}
//----------------------------------------------------------------------
void PndDrcSurfPolyFlatFocus::AddTransform(const Transform3D& trans)
{
  if (Verbosity()>=3) cout<<"    PndDrcSurfPolyFlatFocus::addTransform() name="<<fName<<endl;
  //fTrans    *= trans;
  //fTransInv  = fTrans.Inverse();
  
  for (unsigned int i=0; i<fP.size(); i++)
    {
      fP[i] = trans*fP[i];
    }
  fNormal = trans * fNormal;
  fPixelPoint = trans*fPixelPoint;
  fAxisPoint  = trans*fAxisPoint;
  
}
