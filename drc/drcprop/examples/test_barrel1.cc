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



void cross(PndDrcPhoton ph1, PndDrcPhoton ph2, double& x, double& y, double& z)
{
  XYZPoint  pos1 = ph1.Position();
  XYZPoint  pos2 = ph2.Position();
  XYZVector dir1 = ph1.Direction();
  XYZVector dir2 = ph2.Direction();
  
  // y is zero
  // pos1_x + f1 * dir1_x = pos2_x + f2 *dir2_x
  // f1 = (pos2_x - pos1_x)/dir1_x + f2 * dir2_x/dir1_x
  // now for z
  // pos1_z + ((pos2_x - pos1_x)/dir1_x + f2 * dir2_x/dir1_x)*dir1_z = pos2_z + f2 *dir2_z
  // pos1_z + (pos2_x-pos1_x)* dir1_z/dir1_x + f2*dir2_x * dir1_z/dir1_x = pos2_z + f2 *dir2_z
  // pos1_z-pos2_z + (pos2_x-pos1_x)*dir1_z/dir1_x = f2 * (dir2_z -dir2_x* dir1_z/dir1_x)

  double f2 = pos1.Z()-pos2.Z() +  (pos2.X()-pos1.X())*dir1.Z()/dir1.X();

  f2 /= (dir2.Z()-dir2.X()*dir1.Z()/dir1.X());

  XYZPoint ppp = pos2 + f2*dir2;

  x = ppp.X();
  y = ppp.Y();
  z = ppp.Z();
  

  
}






int main(int argc, char *argv[]) 
{


  if (argc<2) 
    {
      cout<<" usage: test_barrel1 xxx            (xxx=pad plane distance mm)"<<endl;
      exit(1);
    }
  
  double dist_plane=200;
  
  char* ccc = argv[1];
  
  cout<<" dist plane int: ";
  //cin>>ccc;
  dist_plane = atof(ccc);
  cout<<dist_plane<<endl;
  

  const double pi=3.1415926535;
  

  // good values with 70mm FL: 19.8 / 22

  double radius_sio2     = atof(argv[2]);//-19;//-14.3; //19.8; //0.85*23.25;//70.5;

  double radius_nlak33a  = atof(argv[3]);//43; //;//41.2;//0.4460*93.62;// 0.4477 0.55
  
  double conical =1;  
  conical = atof(argv[4]);

  double thick_lens_nlak33a = 4.5;//5.58;//4;//4
  thick_lens_nlak33a =  atof(argv[5]);
  

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

  double thick_lens_sio2=5;
  PndDrcOptLens lens_sio2(q2.X(),q2.Y(),thick_lens_sio2/2,-radius_sio2,9999,
			  conical,conical);
  lens_sio2.SetName("lens_sio2");
  lens_sio2.AddTransform(Transform3D(XYZVector(0,0,-400-thick_lens_sio2/2)));
  lens_sio2.SetOptMaterial(PndDrcOptMatLithotecQ0());
  //lens_sio2.SetOptMaterial(PndDrcOptMatNLAK33A());
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
  lens_nlak33a.SetOptMaterial(PndDrcOptMatNLAK33A());
  //lens_nlak33a.SetOptMaterial(PndDrcOptMatLithotecQ0());
  lens_nlak33a.SetPrintColor(4);
  opt_system.AddDevice(lens_nlak33a);
  opt_system.CoupleDevice("lens_sio2","lens_nlak33a","side1","side6");


  double thick_lens_air = 5;//1;//6; 
  PndDrcOptLens lens_air(q2.X(),q2.Y(),
			 thick_lens_air/2,9999,-radius_nlak33a,
			 conical,conical);
  lens_air.SetName("lens_air");
  lens_air.AddTransform(Transform3D(XYZVector(0,0,
					       -400
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

  string sfile = "Geo"+string(ccc)+".C";
  

  fstream geo;
  geo.open(sfile.c_str(),std::ios::out);
  geo<<"{"<<endl;
  geo<<"    TCanvas *c1 = new TCanvas(\"c1\"); "<<endl;
  cout<<" root version: "<<((TROOT*)gROOT)->GetVersionInt()<<endl;
  
  if ( ((TROOT*)gROOT)->GetVersionInt() < 51600)
    {
      geo<<"    TView *view = new TView(1);"<<endl;
    }
  else
    {  
      geo<<"    TView *view = TView::CreateView(1);"<<endl;
    }
  geo<<"    view->SetRange(-400,-500,-1050,600,500,-50);"<<endl;
  //geo<<"    view->SetRange(-500,-500,-1100,500,500,-100);"<<endl;
  geo<<"    Int_t i;"<<endl;
  geo<<"    view->SetView(90,90,90,i);"<<endl;
  for (int i=0; i<8/*12+7*/;i++) // 3 or 12
    geo<<"    view->Zoom();"<<endl;
  
  // the following command sets a flag within the manager and all photons from
  // now on will be traced and can be plotted by calling within root
  // .x Geo.C 
  // .x Screen.C
  // 
  manager->Print(geo);

  // create a list of photons in bar

  //XYZPoint  pos(0,-10,0);
  //XYZVector dir(0,1,-1); 
  //double   beta = 0.99;
  //bool photons_exist = manager->cerenkov(pos,dir,beta,2000,1e16,400,600,50); // generate photons
  //if (photons_exist) manager->propagate();              // propagate photons

  PndDrcPhoton ph;
  ph.SetReflectionLimit(200);
  
  list<PndDrcPhoton> list_photon; // get list
  
  /*
  
  {
      
    // straight lines.
    TRandom ran;
    for (double angle=5; angle<=40.5; angle+=5)
      {
	double slab_width = q2.X()*1.9;
	double slab_height =q2.Y()*1.9;

	//cout<<" angle = "<<angle<<endl;
	for (double lambda1=630; lambda1>329; lambda1-=30)
	  {
	    //double lambda1=lambda;
	    // go in x dir
	    double y1 = tan(angle*pi/180);
	    double x1 = y1;
	    double scale = 3/angle;
	    for (double y=-y1; y<= y1; y+= 2*y1/20*scale)
	      {
		double xx=ran.Uniform(-0.5*slab_width,0.5*slab_width);
		double yy=ran.Uniform(-0.5*slab_height,0.5*slab_height);
		ph.SetPosition(XYZPoint(xx,yy,0));
		double x = x1;
		ph.SetDirection(XYZVector(x,y,-1).Unit());
		ph.SetWavelength(lambda1);
		list_photon.push_back(ph);
		ph.SetDirection(XYZVector(-x,y,-1).Unit());
		ph.SetWavelength(lambda1);
		list_photon.push_back(ph);
	      }
	    for (double x=-x1; x<= x1; x+= 2*y1/20*scale)
	      {
		double xx=ran.Uniform(-0.5*slab_width,0.5*slab_width);
		double yy=ran.Uniform(-0.5*slab_height,0.5*slab_height);
		ph.SetPosition(XYZPoint(xx,yy,0));
		double y = y1;
		ph.SetDirection(XYZVector(x,y,-1).Unit());
		ph.SetWavelength(lambda1);
		list_photon.push_back(ph);
		ph.SetDirection(XYZVector(x,-y,-1).Unit());
		ph.SetWavelength(lambda1);
		list_photon.push_back(ph);
	      }
	     
	  }
      }
    // photons_exist = true;
    //manager->print();
  
    cout<<" list size = "<<list_photon.size()<<endl;
  
    //manager->setPhotonList(list_photon,"bar","optsys");
  }
  */
  /*
  
  
  ph.SetReflectionLimit(200);
  for (double angle=0; angle<40.5; angle+=1)
    {
      for (int i=0; i<2; i++)
	{
	  ph.SetDirection(XYZVector(sin(pi/180*angle),0,-cos(pi/180*angle)));
	  if (i==0) ph.SetPosition(XYZPoint((q0.X()+q1.X())/2-2,0,-399.9));
	  if (i==1) ph.SetPosition(XYZPoint((q0.X()+q1.X())/2+2,0,-399.9));
	  ph.SetWavelength(630);
	  list_photon.push_back(ph);
	}
      for (int i=0; i<2; i++)
	{
	  ph.SetDirection(XYZVector(-sin(pi/180*angle),0,-cos(pi/180*angle)));
	  if (i==0) ph.SetPosition(XYZPoint((q0.X()+q1.X())/2-2,0,-399.9));
	  if (i==1) ph.SetPosition(XYZPoint((q0.X()+q1.X())/2+2,0,-399.9));
	  ph.SetWavelength(630);
	  list_photon.push_back(ph);
	}
    }
  
  */
  
    ph.SetReflectionLimit(200);
   double angle=0;
   
    
      for (int i=0; i<30; i++)
	{
	  ph.SetDirection(XYZVector(sin(pi/180*angle),0,-cos(pi/180*angle)));
	  ph.SetPosition(XYZPoint(q0.X()+(q1.X()-q0.X())*i/31.0,0,-399));
	  ph.SetWavelength(630);
	  list_photon.push_back(ph);
	}
      
   angle=30;
   
    
      for (int i=0; i<30; i++)
	{
	  ph.SetDirection(XYZVector(sin(pi/180*angle),0,-cos(pi/180*angle)));
	  ph.SetPosition(XYZPoint(q0.X()+(q1.X()-q0.X())*i/31.0,0,-399));
	  ph.SetWavelength(630);
	  list_photon.push_back(ph);
	}

   angle=15;
   
    
      for (int i=0; i<30; i++)
	{
	  ph.SetDirection(XYZVector(sin(pi/180*angle),0,-cos(pi/180*angle)));
	  ph.SetPosition(XYZPoint(q0.X()+(q1.X()-q0.X())*i/31.0,0,-399));
	  ph.SetWavelength(630);
	  list_photon.push_back(ph);
	}
    
  
  // propagate writes to geo, that has finished, therefore, close geo
  


  manager->SetPhotonList(list_photon,"bar","optsys");
  manager->Propagate();  
  

  geo<<"}"<<endl;
  geo.close();


  //exit(0);
  


  list_photon = manager->PhotonList();  // get list
  
  sfile = "Screen"+string(ccc)+".C";
  

  fstream scr;
  scr.open(sfile.c_str(),std::ios::out);
  scr<<"{"<<endl;
  scr<<"    TCanvas *c1 = new TCanvas(\"c1\"); "<<endl;
  scr<<"    TH1F *hgr = new TH1F(\"hgr\",\"test_barrel1\",500,-500,500);"<<endl;
  scr<<"    hgr->SetStats(0);"<<endl;
  scr<<"    hgr->SetMarkerStyle(20);"<<endl;
  scr<<"    hgr->SetMinimum(-500);"<<endl;
  scr<<"    hgr->SetMaximum(500);"<<endl;
  scr<<"    hgr->Draw(\"POL\");"<<endl;





  // analyse list


  double z0 = 900;//300+400+2+200;
  

  int icnt_measured = 0;
  int icnt_flying   = 0;
  int icnt_lost     = 0;
  int icnt_absorbed = 0;

  double xxx[100000];
  double yyy[100000];

  
  int isize=0;


  fstream out_opt;
  out_opt.open("optic.dat",std::ios::out);
  

  
  list<PndDrcPhoton>::iterator iph;
  int i=0;
  bool second=false;
  PndDrcPhoton ph_tmp;
  
  double th[1000],zz[1000];
  for (int iii=0;iii<1000; iii++) 
    {
      th[iii]=-999;
      zz[iii]=-999;
    }
  
  int iii=0;
  for(iph=list_photon.begin(); iph != list_photon.end(); ++iph) 
    {
      
      /*
	if (i%2 ==0)
	{
	ph_tmp = (*iph);
	}
	else
	{
	if (ph_tmp.Fate() == Drc::kPhotMeasured && (*iph).Fate() == Drc::kPhotMeasured)
	{ 
	double x,y,z;
	cross(ph_tmp,(*iph),x,y,z);
	double theta = atan2(ph_tmp.Direction().X(),ph_tmp.Direction().Z())*180/pi;
	
	if (theta<0) 
	{
	theta+=180;
	}
	else 
	{
	theta-=180;
	}
	
	
	
	out_opt<<theta<<" "<<x<<" "<<z<<endl;
	th[iii] = theta;
	zz[iii] = z+400;
	iii++;
	}
	}
	i++;
      
      */
      
      
    



      if ((*iph).Fate()==Drc::kPhotMeasured)
	{
	  //xxx[isize] = (*iph).position().X();
	  //yyy[isize] = (*iph).position().Y();
	  
	  isize++;
	  
	  double xx=(*iph).Position().X();
	  double yy=(*iph).Position().Y();
	  scr<<"    TMarker* t = new TMarker("<<xx<<","<<yy<<",20);"<<endl;
	  scr<<"    t->SetMarkerColor("
		<<(*iph).ColorNumber((*iph).Wavelength())
		<<");"<<endl;
	  scr<<"    t->SetMarkerSize(0.2);"<<endl;
	  scr<<"    t->Draw();"<<endl;
	}
      

      if      ((*iph).Fate()==Drc::kPhotMeasured) icnt_measured++;
      else if ((*iph).Fate()==Drc::kPhotFlying)   icnt_flying++; // should never happen.
      else if ((*iph).Fate()==Drc::kPhotAbsorbed) icnt_absorbed++;
      else                                       icnt_lost++;
}

  
  double th1=999,th2=999,zz1,zz2;
  for (int iii=0; iii<1000 ;iii++)
    {
      if (fabs(th[iii]-00.00)<fabs(th1-00.00))
	{
	  th1 = th[iii];
	  zz1 = zz[iii];
	}
      if (fabs(th[iii]-20.00)<fabs(th2-20.00))
	{
	  th2 = th[iii];
	  zz2 = zz[iii];
	}
    }
  cout<<" 1,2, % "<<zz1<<" "<<zz2<<" "<<fabs(zz1-zz2)/zz1*100<<endl;
  

  out_opt.close();
  
      
    
  scr<<"}"<<endl;
  scr.close();

  int icnt = icnt_measured+icnt_flying+icnt_lost+icnt_absorbed;
  cout<<" generated photons: "<<icnt<<endl;
  cout<<" measured  photons: "<<icnt_measured<<endl;
  cout<<" absorbed  photons: "<<icnt_absorbed<<endl;
  cout<<" lost      photons: "<<icnt_lost<<endl;
  
  delete manager;
  
  exit(0);
  
  
  // Hough
  
  TFile* hfile = (TFile*)gROOT->FindObject("test_barrel1.root"); 
  if (hfile) hfile->Close();
  hfile = new TFile("test_barrel1.root","RECREATE","Hough transform test file");
  
  //TH2D* hxy        = new TH2D("hxy",       "y vs x", 150,-300,300,100,-200,200);
  TH1D* hpsi       = new TH1D("hpsi",      "psi",    720,-360,360);
  TH1D* hphi       = new TH1D("hphi",      "phi",    720,-360,360);
  TH1D* htheta     = new TH1D("htheta",    "theta",  720,-360,360);
  TH1D* hx0        = new TH1D("hx0",       "x0",     500,-500,500);
  TH1D* hy0        = new TH1D("hy0",       "y0",     500,-500,500);
  

 



  PndDrcConicSection conic;
  
  //int isize = vph.size();
  if (isize>=5)
    {
      for (int i1=1;    i1<isize-4; i1++){
	for (int i2=i1+1; i2<isize-3; i2++){
	  for (int i3=i2+1; i3<isize-2; i3++){
	    for (int i4=i3+1; i4<isize-1; i4++){
	      for (int i5=i4+1; i5<isize  ; i5++){
		vector<double> x(5);
		vector<double> y(5);
		x[0] = xxx[i1];
		x[1] = xxx[i2];
		x[2] = xxx[i3];
		x[3] = xxx[i4];
		x[4] = xxx[i5];
		y[0] = yyy[i1];
		y[1] = yyy[i2];
		y[2] = yyy[i3];
		y[3] = yyy[i4];
		y[4] = yyy[i5];
		//for (int i=0;i<5;i++) cout<<x[0]<<" "; cout<<endl;
		//for (int i=0;i<5;i++) cout<<y[0]<<" "; cout<<endl;
		//cout<<"----"<<endl;
		
		
		if (conic.ConicSection(z0,x,y))
		  {
		    hpsi->Fill(conic.Psi()*180/pi);
		    hphi->Fill(conic.Phi()*180/pi);
		    htheta->Fill(conic.Theta()*180/pi);
		    hx0->Fill(conic.X0());
		    hy0->Fill(conic.Y0());
		  }
	      }
	    }
	  }
	}
      }
      
    }
  

  hfile->Write();
  


  return EXIT_SUCCESS;

}
