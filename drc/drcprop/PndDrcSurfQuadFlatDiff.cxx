// ----------------------------------------------------
// This file belongs to the ray tracing framework
// for the use with Cherenkov detectors
//
// created 2007
//-----------------------------------------------------
#include "PndDrcEffiAbs.h"
#include "PndDrcSurfQuadFlatDiff.h"

#include "PndDrcPhoton.h"
//#include "PndDrcOptReflAbs.h"
//#include "PndDrcSurfAbs.h"
//#include "PndDrcSurfPolyFlat.h"
//#include "PndDrcOptMatAbs.h"
//#include "PndDrcOptDev.h"
//#include "PndDrcUtil.h"
//
//#include "TObject.h"
//#include "TVector3.h"
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
PndDrcSurfQuadFlatDiff::PndDrcSurfQuadFlatDiff()
{
	fS1 = 0;
	fS2 = 0;
}
//----------------------------------------------------------------------
PndDrcSurfQuadFlatDiff::~PndDrcSurfQuadFlatDiff()
{
	if (fS1) delete fS1;
	if (fS2) delete fS2;
}

//----------------------------------------------------------------------
PndDrcSurfQuadFlatDiff* PndDrcSurfQuadFlatDiff::Clone() const
{
	return new PndDrcSurfQuadFlatDiff(*this);
}
//----------------------------------------------------------------------
void PndDrcSurfQuadFlatDiff::Copy(const PndDrcSurfQuadFlatDiff& s)
{
	fS1p1        = s.fS1p1;
	fS1p2        = s.fS1p2;
	fS2p1        = s.fS2p1;
	fS2p2        = s.fS2p2;
	fS1          = (s.fS1)->Clone();
	fS2          = (s.fS2)->Clone();
	fNormal      = s.fNormal;
	fSurfAux     = s.fSurfAux;
}
//----------------------------------------------------------------------
PndDrcSurfQuadFlatDiff::PndDrcSurfQuadFlatDiff(const PndDrcSurfQuadFlatDiff& s) : PndDrcSurfAbs(s)
{
	if (s.fVerbosity>=1) cout<<"  PndDrcSurfQuadFlatDiff::PndDrcSurfQuadFlatDiff"
		<<"(const PndDrcSurfQuadFlatDiff&) name,copy: "
		<<s.fName<<" "<<s.fCopyNumber<<endl;
	Copy(s);
}
//----------------------------------------------------------------------
PndDrcSurfQuadFlatDiff&  PndDrcSurfQuadFlatDiff::operator=(const PndDrcSurfQuadFlatDiff& s)
{
	if (s.fVerbosity>=1) cout<<"  PndDrcSurfQuadFlatDiff::operator="
		<<"(const PndDrcSurfQuadFlatDiff&) name,copy: "
		<<s.fName<<" "<<s.fCopyNumber<<endl;
	if (&s != this)
	{
		static_cast<PndDrcSurfAbs&>((*this)) = s; // assignment of base class part.
		Copy(s);
	}

	return *this;
}
//----------------------------------------------------------------------
void PndDrcSurfQuadFlatDiff::AddSurface(const PndDrcSurfAbs& surf, XYZPoint p1, XYZPoint p2)
{
	if (fS1 && fS2)
	{
		cerr<<" *** PndDrcSurfQuadFlatDiff::addSurface: you can only add two surfaces.\n";
		exit(EXIT_FAILURE);
	}
	else if (!fS1)
	{
		fS1 = surf.Clone();
		fS1p1 = p1;
		fS1p2 = p2;
	}
	else if (!fS2)
	{
		fS2 = surf.Clone();
		fS2p1 = p1;
		fS2p2 = p2;
	}
	else
	{
		cerr<<" *** PndDrcSurfQuadFlatDiff::addSurface: this line should never hit."<<endl;
		exit(EXIT_FAILURE);
	}

	if (fS1 && fS2)
	{
		XYZVector cross1 = (fS1p2-fS1p1).Cross(fS2p1-fS1p1);
		XYZVector cross2 = (fS1p2-fS1p1).Cross(fS2p2-fS1p1);
		fNormal = (cross1.Mag2() > cross2.Mag2()) ? cross1.Unit() : cross2.Unit();
		fSurfAux.AddPoint(fS1p1);
		fSurfAux.AddPoint(fS1p2);
		//fSurfAux.AddPoint(fS2p1);
		//fSurfAux.AddPoint(fS2p2);
		fSurfAux.AddPoint(fS2p2);
		fSurfAux.AddPoint(fS2p1);
	}
}
//----------------------------------------------------------------------
XYZVector PndDrcSurfQuadFlatDiff::Normal(const XYZPoint& point) const
{
  // normal vector is a constant.
	return fNormal;
}
//----------------------------------------------------------------------
bool PndDrcSurfQuadFlatDiff::SurfaceHit(PndDrcPhoton& ph,
                                        XYZPoint&  pos_new,
                                        double&    path_length) const
{
  static const double kEps = 1.0e-9;

  if (Verbosity()>=3) cout<<"      PndDrcSurfQuadFlatDiff::surfaceHit: name="
			  <<Name()<<endl;


  if (!fS1 || !fS2)
    {
      cerr<<" *** PndDrcSurfQuadFlatDiff::surfaceHit: at least one of"
	  <<" both surfaces undefined."<<endl;
      exit(EXIT_FAILURE);
    }



  // find first point

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
      if (Verbosity()>=4) cout<<"      PndDrcSurfQuadFlatDiff::surfaceHit:"
			      <<" dir perp norm, no hit. \n";
      return false;
    }
  double   lambda = (fS1p1-point).Dot(fNormal) / dir.Dot(fNormal);
  pos_new = point + lambda*dir;

  if (Verbosity()>=4) cout<<"      PndDrcSurfQuadFlatDiff::surfaceHit:"<<Name()<<endl
			  <<"      lambda pos_new ="<<lambda<<" "<<pos_new<<endl;


  if (lambda<-kEps) // flying in wrong direction.
    {
      if (Verbosity()>=4) cout<<"      PndDrcSurfQuadFlatDiff::surfaceHit: wrong dir "
			      <<" lambda par ="<<lambda<<" no hit."<<endl;
      return false;
    }

  path_length = fabs(lambda);

  // check surfaces between rectangular partial surface and eventual bended
  // surface borders.
  bool hit_aux_surf = fSurfAux.WithinSurface(pos_new);

  // if both are flat, there was not hit.
  if (fS1->IsFlat() && fS2->IsFlat())
    {
      if (hit_aux_surf)
	{
	  if (fPixel){
	    if (fEffiCathode
		->EffiFlag(ph.Wavelength(),
			   ph.Direction().Dot(Normal(pos_new))))
	      { 
		ph.SetFate(Drc::kPhotMeasured);
		if (fPixelCorr) ph.SetPosition(fPixelPoint);
	      }
	    else
	      { 
		ph.SetFate(Drc::kPhotAbsorbed);
	      }
	  }
		
	  //if (fPixel) ph.SetFate(Drc::kPhotMeasured);
		
	  if (Verbosity()>=4) cout<<"      PndDrcSurfQuadFlatDiff::surfaceHit:"
				  <<" within rectangular partial surface, hit. "
				  <<endl;
	  return true;
	}
      else
	{
	  if (Verbosity()>=4) cout<<"      PndDrcSurfQuadFlatDiff::surfaceHit:"
				  <<" no hit within reactangular partial surface"
				  <<" of flat borders. "
				  <<endl;
	  return false;
	}
    }
	
	
  bool in_between1 = false;
  if (!(fS1->IsFlat()))
    {
      // check if point between curve and line
      // create a fake photon.
      if (Verbosity()>=4) cout<<"      PndDrcSurfQuadFlatDiff::surfaceHit: (1) "
			      <<" check point between line and curve of "
			      <<fS1->Name()<<endl;
      PndDrcPhoton ph1;
      ph1.SetPosition(pos_new);
	    
      XYZPoint ref(0.5*(XYZVector(fS1p1) + XYZVector(fS1p2)));
      //                 no addition between XYZPoints possible
      ref.SetX(1.01*ref.X()); // bend it a little bit outside
      ref.SetY(1.01*ref.Y()); // so that dir points a little bit inside.
      dir = (XYZPoint(pos_new)-ref).Unit();
	    
      ph1.SetDirection(dir);
	    
      if (Verbosity()>=4)
	{
	  cout<<"        with aux. photon "<<endl;
	  cout<<"        ph position "<<ph1.Position()<<endl;
	  cout<<"        & direction "<<ph1.Direction()<<endl;
	}
	    
      XYZPoint v_dummy;
      double dummy;
      if (fS1->SurfaceHit(ph1,v_dummy,dummy))
	{
	  if (Verbosity()>=4) cout<<"      PndDrcSurfQuadFlatDiff::surfaceHit: (1) "
				  <<" between line and curve of "
				  <<fS1->Name()<<", hit."<<endl;
	  in_between1 =  true;
	}
      else
	{
	  if (Verbosity()>=4) cout<<"      PndDrcSurfQuadFlatDiff::surfaceHit: "
				  <<" no hit between line and curve of "
				  <<fS1->Name()<<", check other side."<<endl;
	}
    }
	
	
  //                   convex           concave  cases
  bool in_between2 = (hit_aux_surf && fS2->IsFlat()); // for later logic
  if (! fS2->IsFlat())
    {
      // check if point between curve and line
      // create a fake photon.
      if (Verbosity()>=4) cout<<"      PndDrcSurfQuadFlatDiff::surfaceHit: (2) "
			      <<" check point between line and curve of "
			      <<fS2->Name()<<endl;
      PndDrcPhoton ph1;
      ph1.SetPosition(pos_new);
	    
      XYZPoint ref(0.5*(XYZVector(fS2p1) + XYZVector(fS2p2)));
      //                 no addition between XYZPoints possible
      ref.SetX(1.01*ref.X()); // bend it a little bit outside
      ref.SetY(1.01*ref.Y()); // so that dir points a little bit inside.
      dir = (XYZPoint(pos_new)-ref).Unit();
	    
      ph1.SetDirection(dir);
	    
      if (Verbosity()>=4)
	{
	  cout<<"        with aux. photon "<<endl;
	  cout<<"        ph position "<<ph1.Position()<<endl;
	  cout<<"        & direction "<<ph1.Direction()<<endl;
	}
	    
      XYZPoint v_dummy;
      double dummy;
      // fS2->setVerbosity(4);//###
      if (fS2->SurfaceHit(ph1,v_dummy,dummy))
	{
	  if (Verbosity()>=4) cout<<"      PndDrcSurfQuadFlatDiff::surfaceHit: (2) "
				  <<" between line and curve of "
				  <<fS2->Name()<<", hit."<<endl;
	  in_between2 = true;
	}
      else
	{
	  if (Verbosity()>=4) cout<<"      PndDrcSurfQuadFlatDiff::surfaceHit: "
				  <<" no hit between line and curve of "
				  <<fS2->Name()<<", no hit possibility left."<<endl;
	}
    }
	
	
  if (Verbosity()>=4)
    {
      cout<<"      PndDrcSurfQuadFlatDiff::surfaceHit: "<<endl
	  <<"         hit_aux_surf = "<<hit_aux_surf<<endl;
      if (fS1->IsFlat()) cout<<"         in_between1  = "<<"flat"<<endl;
      if (!fS1->IsFlat()) cout<<"         in_between1  = "<<in_between1<<endl;
      if (fS2->IsFlat()) cout<<"         in_between2  = "<<"flat"<<endl;
      if (!fS2->IsFlat()) cout<<"         in_between2  = "<<in_between2<<endl;
	    
    }
	
	
  // in/exclusion logic
  if (hit_aux_surf)
    {
      if (fPixel){
	if (fEffiCathode
	    ->EffiFlag(ph.Wavelength(),
		       ph.Direction().Dot(Normal(pos_new))))
	  { 
	    ph.SetFate(Drc::kPhotMeasured);
	    if (fPixelCorr) ph.SetPosition(fPixelPoint);
	  }
	else
	  { 
	    ph.SetFate(Drc::kPhotAbsorbed);
	  }
      }
	    
      //if (fPixel) ph.SetFate(Drc::kPhotMeasured); // don' know why this is only implemented for the flat-flat case
	    
      // exclude photons from concave parts
      if ( !(fS1->IsFlat()) && in_between1) return false;
      if ( !(fS2->IsFlat()) && in_between2) return false;
      return true;
    }
  else
    {
      if (in_between1) return true; // surface convex, photon inside
      if (in_between2) return true; // surface convex, photon inside
      return false;
    }
}
//----------------------------------------------------------------------
void PndDrcSurfQuadFlatDiff::Print(fstream& stream) const
{
  stream<<" TPolyLine3D *l = new TPolyLine3D(2);"<<endl;

  stream<<" l->SetPoint("<<0<<","
	<<fS1p1.X()<<","
	<<fS1p1.Y()<<","
	<<fS1p1.Z()<<");"<<endl;
  stream<<" l->SetPoint("<<1<<","
	<<fS2p1.X()<<","
	<<fS2p1.Y()<<","
	<<fS2p1.Z()<<");"<<endl;
  stream<<" l->SetLineColor("<<fPrintColor<<");"<<endl;
  stream<<" l->Draw();"<<endl;

  stream<<" TPolyLine3D *l = new TPolyLine3D(2);"<<endl;

  stream<<" l->SetPoint("<<0<<","
	<<fS1p2.X()<<","
	<<fS1p2.Y()<<","
	<<fS1p2.Z()<<");"<<endl;
  stream<<" l->SetPoint("<<1<<","
	<<fS2p2.X()<<","
	<<fS2p2.Y()<<","
	<<fS2p2.Z()<<");"<<endl;
  stream<<" l->SetLineColor("<<fPrintColor<<");"<<endl;
  stream<<" l->Draw();"<<endl;

  // for an opened canvas
  TPolyLine3D *l1 = new TPolyLine3D(2);
  l1->SetPoint( 0, fS1p1.X(), fS1p1.Y(), fS1p1.Z() );
  l1->SetPoint( 1, fS2p1.X(), fS2p1.Y(), fS2p1.Z() );
  l1->SetLineColor(fPrintColor);
  l1->Draw();
  
  TPolyLine3D *l2 = new TPolyLine3D(2);
  l2->SetPoint( 0, fS1p2.X(), fS1p2.Y(), fS1p2.Z() );
  l2->SetPoint( 1, fS2p2.X(), fS2p2.Y(), fS2p2.Z() );
  l2->SetLineColor(fPrintColor);
  l2->Draw();
}
//----------------------------------------------------------------------
void PndDrcSurfQuadFlatDiff::Print() const
{
	cout<<" Coordinates of PndDrcSurfQuadFlatDiff "<<Name()<<endl;
	cout<<"  s1p1 = "<<fS1p1<<endl;
	cout<<"  s1p2 = "<<fS1p2<<endl;
	cout<<"  s2p1 = "<<fS2p1<<endl;
	cout<<"  s2p2 = "<<fS2p2<<endl;
	cout<<"  surf1= "<<endl;
	fS1->Print();
	cout<<"  surf2= "<<endl;
	fS2->Print();
	cout<<"  norm = "<<fNormal<<endl;
}
//----------------------------------------------------------------------
void PndDrcSurfQuadFlatDiff::AddTransform(const Transform3D& trans)
{
  if (!fS1 || !fS2)
    {
      cerr<<" PndDrcSurfQuadFlatDiff::shift: surface undefined"<<endl;
      exit(EXIT_FAILURE);
    }
  
  if (Verbosity()>=3) cout<<" PndDrcSurfQuadFlatDiff::addTransform() name="<<fName<<endl;
  
  fNormal = trans * fNormal;
  fS1p1   = trans * fS1p1;
  fS1p2   = trans * fS1p2;
  fS2p1   = trans * fS2p1;
  fS2p2   = trans * fS2p2;
  fS1->AddTransform(trans);
  fS2->AddTransform(trans);
  fSurfAux.AddTransform(trans);
  fPixelPoint = trans*fPixelPoint;
}
