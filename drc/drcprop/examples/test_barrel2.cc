#include <iostream>
using std::cout;
using std::cerr;
using std::cin;
using std::endl;
#include <valarray>
using std::valarray;
#include <vector>
using std::vector;
#include <string>
using std::string;
#include <list>
using std::list;
#include <limits>
#include <fstream>
using std::fstream;
#include <utility>
using std::pair;
#include <map>
using std::map;

#include "TROOT.h"
#include "TVector3.h"
#include "TRotation.h"
#include "TRandom.h"
#include "TRandom1.h"
#include "TRotation.h"
#include "TH2F.h"
#include "TFile.h"
#include "TRint.h"
#include "TDecompLU.h"
#include "TDecompSVD.h"

#include "Math/Vector3D.h"
using ROOT::Math::XYZVector;
#include "Math/Point3D.h"
using ROOT::Math::XYZPoint;
#include "Math/Transform3D.h"
using ROOT::Math::Transform3D;
#include "Math/RotationX.h"
using ROOT::Math::RotationX;
#include "Math/RotationY.h"
using ROOT::Math::RotationY;
#include "Math/RotationZ.h"
using ROOT::Math::RotationZ;
#include "Math/Rotation3D.h"
using ROOT::Math::Rotation3D;

#include "PndDrcSurfAbs.h"
#include "PndDrcPhoton.h"
#include "PndDrcSurfPolyFlat.h"
#include "PndDrcSurfQuadFlatDiff.h"
#include "PndDrcSurfPolySphere.h"
#include "PndDrcSurfPolyPara.h"
#include "PndDrcOptReflSilver.h"
#include "PndDrcOptReflNone.h"
#include "PndDrcOptMatLithotecQ0.h"
#include "PndDrcOptMatNLAK33A.h"
#include "PndDrcOptMatLLF1.h"
#include "PndDrcOptMatVacuum.h"
#include "PndDrcOptDevSys.h"
#include "PndDrcOptVol.h"
#include "PndDrcOptDevManager.h"
#include "PndDrcOptBrick.h"
#include "PndDrcOptLens.h"
#include "PndDrcConicSection.h"



// fit routine for finding right lens radii

double chi2(double,double,double,double,double);


int main(int argc, char *argv[]) 
{
  double dist_plane = atof(argv[1]);
  double r1     = atof(argv[2]);
  double r2     = atof(argv[3]);
  double c      = atof(argv[4]);
  double t      = atof(argv[5]);

  //double r1 = -50;
  //double r2 = -150;
  //double c  = 0;
  TRandom ran;
  
  double chi2_min = chi2(r1,r2,c,t,dist_plane);
    
  
  
  // 500 = 900 seconds   0.5 Hz
  double rr1,rr2,cc,tt;
  
  for (int i=0; i<500000; i++)
    {
      rr1=0;
      while (rr1>-30)           rr1 = ran.Gaus(r1,5);
      rr2=0;
      while (rr2>-30)           rr2 = ran.Gaus(r2,5);
      cc=-9;
      while (cc<-2.5 || cc>2.5) cc  = ran.Gaus(c,0.3);
      tt=0;
      double thick_min = (fabs(rr1)-sqrt(rr1*rr1-595)) + (fabs(rr2)-sqrt(rr2*rr2-595));
      //cout<<thick_min<<endl;
      
      while (tt<thick_min || tt>20)     tt  = ran.Gaus(t,0.2);
      

      //cout<<" test r1 r2 c : "<<chi2_min<<" "<<rr1<<" "<<rr2<<" "<<cc<<" "<<i<<endl;
      double chi2_1 = chi2(rr1,rr2,cc,tt,dist_plane);
      
      if (chi2_1<chi2_min)
	{
	  r1 = rr1;
	  r2 = rr2;
	  c = cc;
	  t = tt;
	  
	  chi2_min = chi2_1;
	  cout<<" chi2 r1 r2 c : "<<chi2_min<<" "<<rr1<<" "<<rr2<<" "<<cc<<" "<<tt<<" "<<i<<endl;
	}
      

      
    }
  
  
  
}

double chi2(double radius_sio2,double radius_nlak33a, double conical,double thick_lens_nlak33a,
double dist_plane) 
{
  
  //double dist_plane=300;
  
  

  const double pi=3.1415926535;
  

  // good values with 70mm FL: 19.8 / 22

  //double radius_sio2     = -55;//atof(argv[2]);//-19;//-14.3; //19.8; //0.85*23.25;//70.5;

  //double radius_nlak33a  = -160;//atof(argv[3]);//43; //;//41.2;//0.4460*93.62;// 0.4477 0.55
  
  //double conical =0.5;
  
  //conical = atof(argv[4]);
  
  

  XYZPoint q0(-34/2,-17.5/2,0);
  XYZPoint q1(+34/2,-17.5/2,0);
  XYZPoint q2(+34/2,+17.5/2,0);
  XYZPoint q3(-34/2,+17.5/2,0);

  PndDrcOptBrick bar(q1.X(),q2.Y(),400);
  bar.SetOptMaterial(PndDrcOptMatLithotecQ0());
  bar.SetName("bar");
  bar.Surface("side6")->SetReflectivity(PndDrcOptReflSilver());

  PndDrcOptDevSys opt_system;
  opt_system.SetNameCopyNumber("optsys",0);
  opt_system.AddDevice(bar);

  double thick_lens_sio2=3;
  PndDrcOptLens lens_sio2(q2.X(),q2.Y(),thick_lens_sio2/2,-radius_sio2,9999,
			  conical,conical);
  lens_sio2.SetName("lens_sio2");
  lens_sio2.AddTransform(Transform3D(XYZVector(0,0,-400-thick_lens_sio2/2)));
  //lens_sio2.SetOptMaterial(PndDrcOptMatLithotecQ0());
  lens_sio2.SetOptMaterial(PndDrcOptMatNLAK33A());
  lens_sio2.Surface("side21")->SetReflectivity(PndDrcOptReflNone());
  lens_sio2.Surface("side26")->SetReflectivity(PndDrcOptReflNone());
  lens_sio2.Surface("side31")->SetReflectivity(PndDrcOptReflNone());
  lens_sio2.Surface("side36")->SetReflectivity(PndDrcOptReflNone());
  lens_sio2.Surface("side41")->SetReflectivity(PndDrcOptReflNone());
  lens_sio2.Surface("side46")->SetReflectivity(PndDrcOptReflNone());
  lens_sio2.Surface("side51")->SetReflectivity(PndDrcOptReflNone());
  lens_sio2.Surface("side56")->SetReflectivity(PndDrcOptReflNone());
  opt_system.AddDevice(lens_sio2);
  opt_system.CoupleDevice("bar","lens_sio2","side1","side6");
  
  //double thick_lens_nlak33a = 9;//4;//4 
  PndDrcOptLens lens_nlak33a(q2.X(),q2.Y(),
			     thick_lens_nlak33a/2,radius_nlak33a,radius_sio2,
			     conical,conical);
  lens_nlak33a.Surface("side21")->SetReflectivity(PndDrcOptReflNone());
  lens_nlak33a.Surface("side26")->SetReflectivity(PndDrcOptReflNone());
  lens_nlak33a.Surface("side31")->SetReflectivity(PndDrcOptReflNone());
  lens_nlak33a.Surface("side36")->SetReflectivity(PndDrcOptReflNone());
  lens_nlak33a.Surface("side41")->SetReflectivity(PndDrcOptReflNone());
  lens_nlak33a.Surface("side46")->SetReflectivity(PndDrcOptReflNone());
  lens_nlak33a.Surface("side51")->SetReflectivity(PndDrcOptReflNone());
  lens_nlak33a.Surface("side56")->SetReflectivity(PndDrcOptReflNone());
  lens_nlak33a.SetName("lens_nlak33a");
  lens_nlak33a.AddTransform(Transform3D(XYZVector(0,0,
					       -400
					       -thick_lens_sio2
					       -thick_lens_nlak33a/2)));
  //lens_nlak33a.SetOptMaterial(PndDrcOptMatLLF1());
  //lens_nlak33a.SetOptMaterial(PndDrcOptMatNLAK33A());
  lens_nlak33a.SetOptMaterial(PndDrcOptMatLithotecQ0());
  lens_nlak33a.SetPrintColor(4);
  opt_system.AddDevice(lens_nlak33a);
  opt_system.CoupleDevice("lens_sio2","lens_nlak33a","side1","side6");


  double thick_lens_air = 1;//1;//6; 
  PndDrcOptLens lens_air(q2.X(),q2.Y(),
			 thick_lens_air/2,9999,-radius_nlak33a,
			 conical,conical);
  lens_air.SetName("lens_air");
  lens_air.AddTransform(Transform3D(XYZVector(0,0,					       -400
					       -thick_lens_sio2
					       -thick_lens_nlak33a
					      -thick_lens_air/2)));
  lens_air.SetOptMaterial(PndDrcOptMatVacuum());
  lens_air.SetPrintColor(2);
  opt_system.AddDevice(lens_air);
  opt_system.CoupleDevice("lens_nlak33a","lens_air","side1","side6");


  PndDrcOptBrick box(dist_plane,dist_plane,dist_plane/2);
  box.SetOptMaterial(PndDrcOptMatLithotecQ0());
  //box.SetOptMaterial(PndDrcOptMatVacuum());
  box.SetName("box");
  box.Surface("side1")->SetPixel();
  box.Surface("side1")->SetReflectivity(PndDrcOptReflNone());
  box.Surface("side2")->SetReflectivity(PndDrcOptReflNone());
  box.Surface("side3")->SetReflectivity(PndDrcOptReflNone());
  box.Surface("side4")->SetReflectivity(PndDrcOptReflNone());
  box.Surface("side5")->SetReflectivity(PndDrcOptReflNone());
  
  
  box.AddTransform( Transform3D(XYZVector(0,0,
					  -400
					  -thick_lens_sio2
					  -thick_lens_nlak33a
					  -thick_lens_air
					  -dist_plane/2)) );



  opt_system.AddDevice(box);
  opt_system.CoupleDevice("lens_air","box","side1","side6");
  //opt_system.embedDevice(box,cave);
  




  // The manager must be created as pointer. It is created as singleton, that is only 
  // one manager can exist per application.
  PndDrcOptDevManager* manager = new PndDrcOptDevManager();
  manager->AddDeviceSystem(opt_system);
  //manager->setVerbosity(5);

  

  PndDrcPhoton ph;
  ph.SetReflectionLimit(500);
  
  list<PndDrcPhoton> list_photon; // get list  
      

  //fstream out;
  //out.open("test.dat",std::ios::out);
  

  double chi2=0;
  int nn=0;
  
  // straight lines.
  TRandom ran;
  for (double angle=5; angle<=35.5; angle+=30)
    {
      double slab_width = q2.X()*1.9;
      double slab_height =q2.Y()*1.9;
      for (double lambda1=630; lambda1>329; lambda1-=30)
	{
	  //double lambda1=lambda;
	  // go in x dir
	  double y1 = tan(angle*pi/180);
	  double x1 = y1;
	  double scale = 3.0/angle;
	  for (double y=-y1; y<= y1; y+= 2.*y1/5*scale)
	    {
	      double xx=ran.Uniform(-0.5*slab_width,0.5*slab_width);
	      double yy=ran.Uniform(-0.5*slab_height,0.5*slab_height);
	      ph.SetPosition(XYZPoint(xx,yy,0));
	      double x = x1;
	      ph.SetDirection(XYZVector(x,y,-1).Unit());
	      ph.SetWavelength(lambda1);
	      list_photon.push_back(ph);
	    }
	}// lambda
      manager->SetPhotonList(list_photon,"bar","optsys");
      manager->Propagate();  
      list_photon = manager->PhotonList();  // get list

      double xi=0;
      double xi2=0;
      int n=0;
	
      list<PndDrcPhoton>::iterator iph;
      for(iph=list_photon.begin(); iph != list_photon.end(); ++iph) 
	{
	  if ((*iph).Fate()==Drc::kPhotMeasured)
	    {
	      double xx=fabs((*iph).Position().X());
	      //double yy=(*iph).Position().Y();
	      if (xx>0)
		{
		  //out<<xx<<" "<<yy<<endl;
		  n++;
		  xi+=xx;
		  xi2+=xx*xx;
		}
		
	    }
	}
      if (xi2>0 && xi>0) // nan?
	{  
	  chi2 += (xi2-xi*xi/n)/n;	
	  nn++;
	}
      
      list_photon.clear();
	
    }// angle
      
  //out.close();
    
  
  delete manager;
  

  return chi2/nn;
  

}
