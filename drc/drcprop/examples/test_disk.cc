

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

#include <list>
using std::list;

#include <map>
using std::map;

#include <utility>
using std::pair;

#include "TROOT.h"
#include "TRint.h"
#include "TVector3.h"
#include "TRandom.h"
#include "TRotation.h"
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
#include "PndDrcOptReflPerfect.h"
#include "PndDrcOptMatLithotecQ0.h"
#include "PndDrcOptDevSys.h"
#include "PndDrcOptVol.h"
#include "PndDrcOptDevManager.h"


int main(int argc, char *argv[])
{

  // Example for a simple bar with screen (photon detection) and mirror.

  //int number_photons = 50;

  int verbosity = 0; // 0=quiet, 1=constructors,2=member,3=functionality
  //                    4=photons, 5=everything.


  // create one segment of a disk and reprocuce the other.
  //
  //                                        y
  //                                        |
  //                                        |
  //                                        |
  //              8                  7      |
  //           p4-----------------p3        |
  //            \                /          0-----------x
  //             \              /          /
  //              \            /          / 
  //               \          /          /  
  //                \  5     / 6        z   
  //                 p1-----p2              
  //                                        
  
  const int nsegs      = 12;           // Number of segments
  double    dist_z1    = 200;//mm      // distance from beam axis (=z)
  double    dist_z2    = 700;//mm      // outer distance from beam.
  double    angle      = 2*3.1415926/nsegs; 


  XYZPoint p1(-dist_z1*tan(angle/2), dist_z1, 200); 
  XYZPoint p2(+dist_z1*tan(angle/2), dist_z1, 200); 
  XYZPoint p3(+dist_z2*tan(angle/2), dist_z2, 200); 
  XYZPoint p4(-dist_z2*tan(angle/2), dist_z2, 200); 
  XYZPoint p5(-dist_z1*tan(angle/2), dist_z1, 210); 
  XYZPoint p6(+dist_z1*tan(angle/2), dist_z1, 210); 
  XYZPoint p7(+dist_z2*tan(angle/2), dist_z2, 210); 
  XYZPoint p8(-dist_z2*tan(angle/2), dist_z2, 210); 

  PndDrcSurfPolyFlat pfront;
  //pfront.SetVerbosity(4);
  pfront.AddPoint(p1);  
  pfront.AddPoint(p2);
  pfront.AddPoint(p3);
  pfront.AddPoint(p4);
  pfront.SetName("pfront");

  PndDrcSurfPolyFlat pback(pfront); // construct from pfront
  pback.SetVerbosity(verbosity);
  Transform3D trans1(XYZVector(0,0,+10));
  pback.AddTransform(trans1);// and shift it
  pback.SetName("pback");


  PndDrcOptReflPerfect refl;

  PndDrcSurfPolyFlat pbottom;
  pbottom.SetReflectivity(refl);
  pbottom.AddPoint(p1);  
  pbottom.AddPoint(p2);
  pbottom.AddPoint(p6);
  pbottom.AddPoint(p5);
  pbottom.SetName("pbottom");

  PndDrcSurfPolyFlat ptop;
  ptop.AddPoint(p4);  
  ptop.AddPoint(p3);
  ptop.AddPoint(p7);
  ptop.AddPoint(p8);
  ptop.SetPixel();
  ptop.SetPrintColor(2);
  ptop.SetName("ptop");

  PndDrcSurfPolyFlat pleft;
  pleft.SetVerbosity(verbosity);
  pleft.AddPoint(p1);  
  pleft.AddPoint(p5);
  pleft.AddPoint(p8);
  pleft.AddPoint(p4);
  pleft.SetName("pleft");

  PndDrcSurfPolyFlat pright;
  pright.SetVerbosity(verbosity);
  pright.AddPoint(p2);  
  pright.AddPoint(p6);
  pright.AddPoint(p7);
  pright.AddPoint(p3);
  pright.SetName("pright");

  PndDrcOptVol psegment;
  PndDrcOptMatLithotecQ0 quartz;
  psegment.SetOptMaterial(quartz);
  psegment.AddSurface(pfront);
  psegment.AddSurface(pback);
  psegment.AddSurface(pbottom);
  psegment.AddSurface(ptop);
  psegment.AddSurface(pleft);
  psegment.AddSurface(pright);
  psegment.SetVerbosity(verbosity);


  PndDrcOptDevSys psystem;
  psystem.AddDevice(psegment);

  // reproduce the system nsegs times (system = segment + mirror)
  PndDrcOptDevSys     psys[nsegs];
  PndDrcOptDevSys     total_sys;
  total_sys.SetNameCopyNumber("total",0);



  for (int i=0; i<nsegs; i++)
  {
    psys[i] = psystem; // copy
    psys[i].SetNameCopyNumber("segment",i);
    Transform3D trans2(RotationZ(-angle*i));
    psys[i].AddTransform(trans2);
    total_sys.AddDeviceSystem(psys[i]);
  }

  for (int i=0; i<nsegs; i++)
  {
    cout<<i<<" "<<psys[i].Name()<<endl;
    
    total_sys.AddDeviceSystem(psys[i]);
  }
  

  // couple systems side at side
    for (int i=1; i<nsegs; i++)
      {
	total_sys.CoupleDevice("segment","segment","pleft","pright",i,(i-1)); 
      }
    // connect last with first.
    total_sys.CoupleDevice("segment","segment","pleft","pright",0,nsegs-1); 



  PndDrcOptDevManager manager;
  manager.AddDeviceSystem(total_sys);


	
  fstream geo;
  geo.open("Geo.C",std::ios::out);
  geo<<"{"<<endl;
  geo<<"    TCanvas *c1 = new TCanvas(\"c1\"); "<<endl;
  if ( ((TROOT*)gROOT)->GetVersionInt() < 51600)
    {
      geo<<"    TView *view = new TView(1);"<<endl;
    }
  else
    {  
      geo<<"    TView *view = TView::CreateView(1);"<<endl;
    }
  geo<<"    view->SetRange(-500,-500,-500,500,500,500);"<<endl;
  geo<<"    Int_t i;"<<endl;
  geo<<"    view->SetView(0,90,90,i);"<<endl;
  
  // the following command sets a flag within the manager and all photons from
  // now on will be traced and can be plotted by calling within root
  // .x Geo.C 
  // .x Screen.C
  // 
  manager.Print(geo);
  //
  // the intention is to play around with routines.

  // photons

  // get volume and positions for intersection with particle.
  // A intersection with the volume produces on complex shapes 2n hits
  // with a volume name and a copy number.

  /*
  list<PndDrcPhoton> list_photon;
  PndDrcPhoton ph;
  ph.SetPosition(XYZPoint(0,300,205));
  ph.SetDirection(XYZVector(-2,1,0));
  ph.SetWavelength(500);
  list_photon.push_back(ph);
  bool photons_exist = true;
  manager.SetPhotonList(list_photon,"segment",0);
  */

  
    XYZPoint  pos(0,0,0);
    XYZVector dir(0,2,2);
    bool photons_exist = manager.Cerenkov(pos,dir.Unit(),0.99); // generate photons
  
  //bool photons_exist = (photons_exist1 || photons_exist2 || 
  //		photons_exist3 || photons_exist4); 

  if (photons_exist)
    {
      fstream scr;
      scr.open("Screen.C",std::ios::out);
      scr<<"{"<<endl;
      scr<<"    TCanvas *c1 = new TCanvas(\"c1\"); "<<endl;
      scr<<"    TH1F *hgr = new TH1F(\"hgr\",\"a simple graph\",100,-100,100);"<<endl;
      scr<<"    hgr->SetMarkerStyle(20);"<<endl;
      scr<<"    hgr->SetMinimum(-100);"<<endl;
      scr<<"    hgr->SetMaximum(+100);"<<endl;
      scr<<"    hgr->Draw(\"POL\");"<<endl;



      int icnt_measured = 0;
      int icnt_absorbed = 0;
      int icnt_lost     = 0;

      manager.Propagate();                           // propagate photons

      list<PndDrcPhoton> list_photon = manager.PhotonList(); // get list

      list<PndDrcPhoton>::iterator iph;
      for (iph = list_photon.begin(); iph != list_photon.end(); ++iph)
	{
	  //cout<<(*iph).fate()<<endl;
	  if ((*iph).Fate()==Drc::kPhotMeasured)
	    {
	      icnt_measured++;
	      double x     = (*iph).Direction().X();
	      double y     = (*iph).Direction().Y();
	      double z     = (*iph).Direction().Z();
	      double theta = atan2(sqrt(x*x+y*y),fabs(z));
	      double phi   = atan2(y,x);
	      double r     = theta;
	      double xx = r*cos(phi)*180/3.1415;
	      double yy = r*sin(phi)*180/3.1415;
	      scr<<"    TMarker* t = new TMarker("<<xx<<","<<yy<<",20);"<<endl;
	      scr<<"    t->SetMarkerColor("
		 <<(*iph).ColorNumber((*iph).Wavelength())
		 <<");"<<endl;
	      scr<<"    t->SetMarkerSize(0.7);"<<endl;
	      scr<<"    t->Draw();"<<endl;
	    }
	  if ((*iph).Fate()==Drc::kPhotAbsorbed) icnt_absorbed++;
	  if ((*iph).Fate()==Drc::kPhotLost)     icnt_lost++;

	  
	}
      scr<<"}"<<endl;
      scr.close();
      int icnt = icnt_measured+icnt_absorbed+icnt_lost;
      cout<<" Total photons: "<<icnt<<endl;
      cout<<"      measured: "<<icnt_measured<<endl;
      cout<<"      absorbed: "<<icnt_absorbed<<endl;
      cout<<"          lost: "<<icnt_lost<<endl;
    }



  geo<<"}"<<endl;
  geo.close();
  
  
  return 0;
}
