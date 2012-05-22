// ----------------------------------------------------
// This file belongs to the ray tracing framework
// for the use with Cherenkov detectors
//
// created 2007
//-----------------------------------------------------
#include "PndDrcSurfAbs.h"
#include "PndDrcOptDevManager.h"
#include "PndDrcPhoton.h"

#include "PndDrcOptMatAbs.h"
#include "PndDrcOptDev.h"

#include <iostream>
#include <iomanip>
using std::cout;
//using std::cerr;
//using std::cin;
using std::endl;

#include <TMath.h>
using namespace TMath;
#include <TComplex.h>



//----------------------------------------------------------------------
PndDrcPhoton::PndDrcPhoton()
{
  fParticleIDnumber = 0;
  fLambda           = 0;
  fPosition         = XYZPoint(0,0,0);
  fPositionOld      = XYZPoint(0,0,0);
  fDirection        = XYZPoint(0,0,0);
  fOriginDirection  = XYZPoint(0,0,0);
  fFate             = Drc::kPhotFlying;
  fReflections      = 0;
  fVerbosity        = 0;
  fThetaC           = 0;
  fPhiC             = 0;
  fTime             = 0;
  fDev              = 0;
  fReflectionLimit  = 1000;
  fPrintFlag        = true;
}
//----------------------------------------------------------------------
void PndDrcPhoton::Copy(const PndDrcPhoton& ph)
{
  fParticleIDnumber = ph.fParticleIDnumber;
  fLambda           = ph.fLambda;
  fPosition         = ph.fPosition;
  fPositionOld      = ph.fPositionOld;
  fPositionXlist    = ph.fPositionXlist;
  fPositionYlist    = ph.fPositionYlist;
  fPositionZlist    = ph.fPositionZlist;
  fSurfaceList      = ph.fSurfaceList;
  fDirection        = ph.fDirection;
  fOriginDirection  = ph.fOriginDirection;
  fFate             = ph.fFate;
  fReflections      = ph.fReflections;
  fVerbosity        = ph.fVerbosity;
  fThetaC           = ph.fThetaC;
  fPhiC             = ph.fPhiC;
  fTime             = ph.fTime;
  fDev              = ph.fDev;
  fReflectionLimit  = ph.fReflectionLimit;
  fPrintFlag        = ph.fPrintFlag;
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
void PndDrcPhoton::SetPosition1(const XYZPoint& pos)
{
  fPositionOld = fPosition;

  if (fDev)
    {
      double n = (fDev->OptMaterial()).RefIndex(fLambda);
      double dndl = (fDev->OptMaterial()).RefIndexDeriv(fLambda);
      double len = sqrt((pos-fPosition).Mag2()); // mm
      //     double v_phase = 299.792/n; // mm/ns
      double n_group = n - fLambda*dndl;
      double v_group = 299.792/n_group;
      double time = len/v_group;
      fTime += time;          // pos in mm time in ns
    }

  SetPosition(pos);

  if( fPrintFlag )
    {
      fPositionXlist.push_back(pos.X());
      fPositionYlist.push_back(pos.Y());
      fPositionZlist.push_back(pos.Z());
    }
  
}

//----------------------------------------------------------------------
bool PndDrcPhoton::Refract(XYZVector normal,
			   double n1, double ex1,
			   bool fresnelFlag,
			   double n2, double ex2,
			   double diffuseProb)
{

  //   cout << "VOLCHECK2: " << n1 << " " << ex1 << " "
  // << n2 << " " << ex2 << "  flag: " << fresnelFlag << " "
  // << "diffuseProb: " << diffuseProb << endl;
  //     fVerbosity =4;
  //     cout << "******************** REFRACT ******************" << endl;


  static const double kEps = 1.0e-9;

  bool refract_flag;
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

  bool reflect;

  if( fresnelFlag )
    reflect = Fresnel( normal, n1, ex1, n2, ex2 );
  else
    {
      if (sin(alpha1) * n1/n2 > 1) // only reflected for total internal reflection
	reflect = true;
      else
	reflect = false;
    }

  //cout<<" Photon::Refract reflect: "<<reflect<<endl;
  
  if( reflect ) // reflect photon
    {
      double random = gRandom->Uniform(0.0,1.0);

      refract_flag = false;

      if( random <= diffuseProb && diffuseProb > 0 )
	Diffuse( normal );
      else
	Reflect( norm );

      //ph.setDirection(dir1 - 2*(norm.Dot(dir1)*norm)); // new direction ???
      if (Verbosity()>=4) cout<<"     reflection"<<endl;
    }
  else                         // refract photon
    {
      refract_flag = true;

      if (Verbosity()>=4) cout<<"     alpha1,n1,n2 = "<<alpha1<<" "<<n1<<" "<<n2<<endl;

      TComplex nIn    = TComplex( n1, ex1 );
      TComplex nOut   = TComplex( n2, ex2 );

      TComplex refr = TComplex::ASin( nIn / nOut * TMath::Sin( alpha1 ) );
      alpha2 = refr.Re();

      if( alpha2 != alpha2 ) // NaN check
	cout << "PndDrcPhoton::Refract :  alpha2 = " << alpha2
	     << "   Is fDirection (here: dir1) really an unit vector?" << endl;


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
void PndDrcPhoton::Diffuse(const XYZVector& normal)
{
  XYZVector normal1(normal);

    if (fDirection.Dot(normal) < 0) normal1 *= -1;


    // 1. generate any vector (uniformly distributed) around the z-Axis (hemisphere)
    // 2. rotate this vector back in the angle (normal,z-Axis)

    XYZVector zUnit(0,0,1);

    normal1 = normal1.Unit();
    fDirection = fDirection.Unit();

    double angle = ACos( normal1.Dot(zUnit) / ( Sqrt(normal1.Mag2()) * Sqrt(zUnit.Mag2()) ) );
    XYZVector rotAxis = zUnit.Cross(normal1).Unit(); // usually Unit() is not necessary

    double costheta = gRandom->Uniform(0.0,1.0);
    double phi = gRandom->Uniform(0.0,2*Pi());

    XYZVector newf( Cos(phi) * Sqrt(1 - costheta*costheta), Sin(phi) * Sqrt(1 - costheta*costheta), costheta);


    // rotation matrix
    double xx = Cos(angle) + rotAxis.X()*rotAxis.X() * (1-Cos(angle));
    double yx = rotAxis.X()*rotAxis.Y() * (1-Cos(angle)) + rotAxis.Z() * Sin(angle);
    double zx = rotAxis.X()*rotAxis.Z() * (1-Cos(angle)) - rotAxis.Y() * Sin(angle);
    double xy = rotAxis.Y()*rotAxis.X() * (1-Cos(angle)) - rotAxis.Z() * Sin(angle);
    double yy = Cos(angle) + rotAxis.Y()*rotAxis.Y() * (1-Cos(angle));
    double zy = rotAxis.Y()*rotAxis.Z() * (1-Cos(angle)) + rotAxis.X() * Sin(angle);
    double xz = rotAxis.Z()*rotAxis.X() * (1-Cos(angle)) + rotAxis.Y() * Sin(angle);
    double yz = rotAxis.Z()*rotAxis.Y() * (1-Cos(angle)) - rotAxis.X() * Sin(angle);
    double zz = Cos(angle) + rotAxis.Z()*rotAxis.Z() * (1-Cos(angle));

    double newfX = xx * newf.X() + xy * newf.Y() +xz * newf.Z();
    double newfY = yx * newf.X() + yy * newf.Y() +yz * newf.Z();
    double newfZ = zx * newf.X() + zy * newf.Y() +zz * newf.Z();

    fDirection.SetXYZ( -newfX, -newfY, -newfZ ); // negative for reflection direction

    fDirection = fDirection.Unit();

    fReflections++;

}
//----------------------------------------------------------------------
bool PndDrcPhoton::Fresnel(XYZVector normal, double n1, double ex1, double n2, double ex2)
{
  XYZVector dir1   = Direction();

 // norm points outside.
  XYZVector norm = (dir1.Dot(normal)>0) ? normal : -normal;

  if (Verbosity()>=4)
  {
    cout<<"PndDrcPhoton::Fresnel" <<endl;
    cout<<"     norm="<<norm.X()<<" "<<norm.Y()<<" "<<norm.Z()<<endl;
    cout<<"     dir1="<<dir1.X()<<" "<<dir1.Y()<<" "<<dir1.Z()<<endl;
    cout<<"     norm*dir="<<norm.Dot(dir1)<<endl;
  }


  double inci = acos(norm.Dot(dir1));

  TComplex nIn    = TComplex( n1, ex1 );
  TComplex nOut   = TComplex( n2, ex2 );


  TComplex refr = TComplex::ASin( nIn / nOut * TMath::Sin( inci ) );

  TComplex fresnel_s = ( nIn * TComplex::Cos( refr ) - nOut * TMath::Cos( inci )) /
      ( nOut * TMath::Cos( inci ) + nIn * TComplex::Cos( refr ) );
  double refl_s = TMath::Power( TComplex::Abs( fresnel_s ), 2 );

  TComplex fresnel_p = ( nIn * TMath::Cos( inci ) - nOut * TComplex::Cos( refr )) /
      ( nIn * TMath::Cos( inci ) + nOut * TComplex::Cos( refr ) );
  double refl_p = TMath::Power( TComplex::Abs( fresnel_p ), 2 );


  double random = gRandom->Uniform(0.0,1.0);
  double reflProb = refl_s * random + refl_p * (1-random);

  random = gRandom->Uniform(0.0,1.0);

//     cout << "refl. probability: " << reflProb << "  random: " << random << "  inci: " << (inci/TMath::Pi()*180) //<< endl;
//             << "  refr: " << refr << "  refl_s: " << refl_s << "  refl_p: " << refl_p << endl;

  if (random <= reflProb )
  {
      if (Verbosity()>=4)
      {
          cout << "     Fresnel reflected:  "
                  << "refl. probability: " << reflProb << "  random: " << random << "  inci: " << (inci/TMath::Pi()*180) << endl;
      }
    return true;
  }
  else
    return false;
}
//----------------------------------------------------------------------
void PndDrcPhoton::SetFate(Drc::kPhotonFate fate)
{
  fFate=fate;

  //if (fate==Drc::kPhotLost)     cout<<" PndDrcPhoton::SetFate lost-------------------------------"<<endl;
  //if (fate==Drc::kPhotAbsorbed) cout<<" PndDrcPhoton::SetFate abs.-------------------------------"<<endl;

  //if (fate==Drc::kPhotLost) Print();
  

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
