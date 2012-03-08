// ----------------------------------------------------
// This file belongs to the ray tracing framework
// for the use with Cherenkov detectors
//
// created 2007
//-----------------------------------------------------
#include "PndDrcOptDevManager.h"

//#include "PndDrcPhoton.h"
//#include "PndDrcOptReflAbs.h"
#include "PndDrcSurfAbs.h"
#include "PndDrcOptMatAbs.h"
#include "PndDrcOptDev.h"
#include "PndDrcOptDevSys.h"
#include "PndDrcUtil.h"
//
//#include "TObject.h"
//#include "TVector3.h"
//#include "TRandom.h"
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
//#include <vector>
//using std::vector;
//
//#include <string>
//using std::string;
//
#include <list>
//using std::list;
//
//#include <map>
//using std::map;
//
#include <limits>
using std::numeric_limits;
//
//#include <fstream>
//using std::fstream;
//
//#include <utility>
using std::pair;

//#include <cmath>


//typedef map< double, pair<XYZPoint,XYZPoint> > HitMapType;
//typedef map< double, pair<string,int> >        VolMapType;
//typedef map< double, pair<string,int> >        SysOriMapType;
//typedef HitMapType::value_type    HitValuePair;
//typedef VolMapType::value_type    VolValuePair;
//typedef SysOriMapType::value_type SysOriValuePair;

// pointer to the object made by Construct()
PndDrcOptDevManager* PndDrcOptDevManager::fgInstance = 0;

//----------------------------------------------------------------------
PndDrcOptDevManager::PndDrcOptDevManager()
{
  if(fgInstance)
  {
    cerr<<" There is only one PndDrcOptDevManager allowed! "<<endl;
    exit(EXIT_FAILURE);
  }
  fgInstance = this;
  fVerbosity = 0;
}
//----------------------------------------------------------------------
PndDrcOptDevManager::PndDrcOptDevManager(const PndDrcOptDevManager& d)
{
  Copy(d);
}
//----------------------------------------------------------------------
PndDrcOptDevManager& PndDrcOptDevManager::operator=(const PndDrcOptDevManager& d)
{
  if (&d != this) Copy(d);
  return *this;
}
//----------------------------------------------------------------------
void PndDrcOptDevManager::Copy(const PndDrcOptDevManager& m)
{
  cerr<<" *** PndDrcOptDevManager::copy() may not be called."<<endl;
  cerr<<"                                 this is a single instanton!"<<endl;
  exit(EXIT_FAILURE);
}
//----------------------------------------------------------------------
PndDrcOptDevManager::~PndDrcOptDevManager()
{
  list<PndDrcOptDevSys*>::const_iterator kSys;

  for(kSys=fListDevSys.begin(); kSys != fListDevSys.end(); ++kSys)
  {
    delete (*kSys);
  }
}
//----------------------------------------------------------------------
PndDrcOptDev* PndDrcOptDevManager::Device(string s,
                                          string sys,
                                          int    icopy,
                                          int    isys_copy)
{
  string     str = s + "_" + itoa(icopy,6) + sys + "_" + itoa(isys_copy,6);
  PndDrcOptDev* dev = 0;

  map<string,PndDrcOptDev*>::iterator idev = fMapDev.find(str);
  if (idev != fMapDev.end())
  {
    dev = (*idev).second;
  }
  else
  {
	  cerr<<" *** PndDrcOptDevManager::device: device not in map: "
    <<str<<endl;
	  cerr<<" map: "<<endl;
	  for (idev= fMapDev.begin(); idev!= fMapDev.end(); idev++)
    {
      cerr<<(*idev).first<<endl;

    }

	  exit(EXIT_FAILURE);
  }
  return dev;
}
//----------------------------------------------------------------------
void PndDrcOptDevManager::AddDeviceSystem(const PndDrcOptDevSys& sys)
{
  PndDrcOptDevSys* tmp = new PndDrcOptDevSys();
  *tmp = sys;
  fListDevSys.push_back(tmp);


  // add for fast finding of devices per name the combined name and copy number to
  // a map;

  list<PndDrcOptDev*>::const_iterator kDev;
  list<string>::const_iterator        kSys;
  list<int>::const_iterator           kSys_copy;


  for (kDev        = tmp->DeviceList().begin(),
       kSys      = tmp->ListSysOri().begin(),
       kSys_copy = tmp->ListSysOriCopy().begin();
       kDev != tmp->DeviceList().end();
       ++kDev,
       ++kSys,
       ++kSys_copy)
  {

    string str = (*kDev)->Name() + "_" + itoa((*kDev)->CopyNumber(),6) // kDev copy number is always the same
    + (*kSys) + "_" + itoa((*kSys_copy),6);


    pair<string,PndDrcOptDev*> p(str,(*kDev));
    pair< map<string,PndDrcOptDev*>::iterator, bool > idev1 = fMapDev.insert(p);

    if (!idev1.second)
    {
      cerr<<" *** PndDrcOptDevManager::addDeviceSystem: device already present \n"
      <<"     name,copy: "<<tmp->Name()<<" "<<tmp->CopyNumber()<<endl;
      cout<<" inserted not "<<str<<endl;//###
      cout<<" long name = "<<tmp->LongName()<<endl;
//       exit(EXIT_FAILURE); // due to kDev copy number (see test_hook.cc)
    }
    else
    {
      //cout<<" inserted "<<str<<endl;//###
    }
  }



}
//----------------------------------------------------------------------
/*
 void PndDrcOptDevManager::coupleDevice(string dev1,    string dev2,
 string surf1,   string surf2,
 int    copy1,   int    copy2)
 {
 list<PndDrcOptDevSys*>::const_iterator isys;
 PndDrcOptDev*  d1=device(dev1,copy1);
 PndDrcOptDev*  d2=device(dev2,copy2);
 PndDrcSurfAbs* s1=0;
 PndDrcSurfAbs* s2=0;


 if (d1 && d2)
 {
 list<PndDrcSurfAbs*>::const_iterator isurf;
 for(isurf=(d1->surfaceList()).begin();
 isurf != (d1->surfaceList()).end(); ++isurf)
 {
 if ((*isurf)->name()== surf1) s1=(*isurf);
 }
 for(isurf=d2->surfaceList().begin(); isurf != d2->surfaceList().end(); ++isurf)
 {
 if ((*isurf)->name()== surf2) s2=(*isurf);
 }
 if (s1 && s2)
 {
 s1->setCoupled(d2,s2);
 s2->setCoupled(d1,s1);
 }
 else
 {
 cerr<<" *** PndDrcOptDevManager::coupleDevice: surface not found"<<endl;
 if (!s1) cerr<<"     "<<surf1<<" missing"<<endl;
 if (!s2) cerr<<"     "<<surf2<<" missing"<<endl;
 exit(EXIT_FAILURE);
 }
 }
 else
 {
 cerr<<" *** PndDrcOptDevManager::coupleDevice: device not found"<<endl;
 if (!d1) cerr<<"     "<<dev1<<" missing (copy="<<copy1<<")"<<endl;
 if (!d2) cerr<<"     "<<dev2<<" missing (copy="<<copy2<<")"<<endl;
 exit(EXIT_FAILURE);
 }
 }
 */
//----------------------------------------------------------------------
void PndDrcOptDevManager::Print(fstream& stream) const
{
  list<PndDrcOptDevSys*>::const_iterator kSys;

  for(kSys=fListDevSys.begin(); kSys != fListDevSys.end(); ++kSys)
  {

    if (fVerbosity>=3) cout<<" PndDrcOptDevManager::print: set print flag for "
      <<(*kSys)->Name()<<" "<<(*kSys)->CopyNumber()<<endl;
    (*kSys)->Print(stream);
  }
}
//----------------------------------------------------------------------
bool PndDrcOptDevManager::Intersect(const XYZPoint&  pos,
                                    const XYZVector& dir,
                                    list<XYZPoint>&  p_in,
                                    list<XYZPoint>&  p_out,
                                    list<string>&    vol_name,
                                    list<string>&    sys_name,
                                    list<int>&       vol_copy,
                                    list<int>&       sys_copy,
                                    double           range) const
{
  const double kEps = 1.0e-9;

  /*
   typedef map< double, pair<XYZPoint,XYZPoint> > HitMapType;
   typedef map< double, pair<string,int> >        VolMapType;
   typedef map< double, pair<string,int> >        SysOriMapType;
   typedef HitMapType::value_type    HitValuePair;
   typedef VolMapType::value_type    VolValuePair;
   typedef SysOriMapType::value_type SysOriValuePair;
   */

  //HitMapType    hit_map;
  //VolMapType    vol_map;
  //SysOriMapType sys_ori_map;

  list< pair<XYZPoint,XYZPoint> > hit_list;
  list< pair<string,int> >        vol_list;
  list< pair<string,int> >        sys_ori_list;




  //HitMapType::iterator    hit_map_iter;
  //VolMapType::iterator    vol_map_iter;
  //SysOriMapType::iterator sys_ori_map_iter;
  list< pair<XYZPoint,XYZPoint> >::iterator hit_list_iter;
  list< pair<string,int> >::iterator        vol_list_iter;
  list< pair<string,int> >::iterator        sys_ori_list_iter;





  list<PndDrcOptDevSys*>::const_iterator kSys;
  list<PndDrcOptDev*>::const_iterator    kDev;
  list<PndDrcSurfAbs*>::const_iterator   kSurf;

  list<string>::const_iterator           kSys_ori;
  list<int>::const_iterator              kSys_ori_copy;

  double path_length;
  bool   hit;

  pair<XYZPoint,XYZPoint> hit_pair;

  if (fVerbosity>=3) cout<<" PndDrcOptDevManager::intersect: start loop ------------------"<<endl;

  for (kSys=fListDevSys.begin(); kSys != fListDevSys.end(); ++kSys)
  {
    PndDrcOptDevSys* sys = (*kSys);
    list<PndDrcOptDev*> list_dev = sys->DeviceList();
    if (fVerbosity>=3) cout<<" PndDrcOptDevManager::intersect: check system "
      <<sys->Name()<<" "<<sys->CopyNumber()<<endl;
    for (kDev=list_dev.begin(),
         kSys_ori=sys->ListSysOri().begin(),
         kSys_ori_copy=sys->ListSysOriCopy().begin();
         kDev != list_dev.end();
         kDev++,
         kSys_ori++,
         kSys_ori_copy++)
    {

      //########################
      bool hit1=true;
      int ihit = 0;
      PndDrcPhoton ph;
      ph.SetPosition(pos);
      ph.SetDirection(dir.Unit());
      while (hit1 && (ph.Position()-pos).Mag2()<range*range )
	    {
	      hit1 = false;
	      double path_shortest = numeric_limits<double>::max();


	      //########################


	      PndDrcOptDev* dev = (*kDev);
	      if (fVerbosity>=3) cout<<" PndDrcOptDevManager::intersect: check device "
          <<dev->Name()<<" "<<dev->CopyNumber()<<endl;
	      list<PndDrcSurfAbs*> list_surf = dev->SurfaceList();
	      XYZPoint pos_new;

	      PndDrcSurfAbs* surf_closest=0;
	      if (dev->Radiator())
        {
          for (kSurf=list_surf.begin(); kSurf != list_surf.end(); kSurf++)
          {
            PndDrcSurfAbs* surf = (*kSurf);
            hit = surf->SurfaceHit(ph,pos_new,path_length);
            if (hit)
            {
              if (path_length < path_shortest)
              {
                path_shortest = path_length;
                surf_closest  = surf;
              }
            }
          }
        }

	      if (surf_closest)
        {
          PndDrcSurfAbs* surf = surf_closest;
          hit = surf->SurfaceHit(ph,pos_new,path_length);
          if (hit) hit1=true;

          if (hit)
          {
            if (fVerbosity>=3) cout<<" PndDrcOptDevManager::intersect: hit# "<<hit<<endl;
            ph.SetPosition(pos_new+ph.Direction()*2*kEps); // bring it in.
            ihit++;
            if (ihit%2 == 1)
            {
              hit_pair.first  = pos_new;
            }
            else if (ihit%2 == 0)
            {
              hit_pair.second = pos_new;


              //hit_map.insert( HitValuePair(path_length,hit_pair) );
              hit_list.push_back(hit_pair);
              pair<string,int> vol_pair;
              vol_pair.first = dev->Name();
              vol_pair.second = dev->CopyNumber();
              //vol_map.insert( VolValuePair(path_length,vol_pair) );
              vol_list.push_back(vol_pair);
              if (fVerbosity>=3)
              {
                cout<<" PndDrcOptDevManager::intersect: hit pair"
                <<" added "
                <<path_length<<" "
                <<vol_pair.first<<" "
                <<vol_pair.second<<endl;
                cout<<" PndDrcOptDevManager::intersect: size of vol_map "
                <<vol_list.size()<<endl;
              }

              pair<string,int> sys_ori_pair;
              sys_ori_pair.first  = (*kSys_ori);
              sys_ori_pair.second = (*kSys_ori_copy);
              //sys_ori_map.insert(SysOriValuePair(path_length,sys_ori_pair));
              sys_ori_list.push_back(sys_ori_pair);
            }
          }
        }
	    }
    }
  }


  if (fVerbosity>=3) cout<<" PndDrcOptDevManager::intersect: stop loop ------------------"<<endl;




  if (fVerbosity>=3) cout<<" PndDrcOptDevManager::intersect: size of vol_map "
    <<vol_list.size()<<endl;

  if (vol_list.size()>0)
  {


    // map is already sorted.

    p_in.clear();
    p_out.clear();
    vol_name.clear();
    vol_copy.clear();
    sys_name.clear();
    sys_copy.clear();


    for (vol_list_iter=vol_list.begin(),
         hit_list_iter=hit_list.begin(),
         sys_ori_list_iter=sys_ori_list.begin();

         vol_list_iter != vol_list.end();

         ++ vol_list_iter,
         ++ hit_list_iter,
         ++ sys_ori_list_iter)
    {
      pair<XYZPoint,XYZPoint> r = (*hit_list_iter);//.second;
      pair<string,int> v = (*vol_list_iter);//.second;
      pair<string,int> s = (*sys_ori_list_iter);//.second;

      if ((r.first-r.second).Mag2() > 1.0e-18) // no flat devices.
	    {

	      if (fVerbosity>=3) cout<<" PndDrcOptDevManager::intersect: added "
          <<v.first<<endl;


	      p_in.push_back(r.first);
	      p_out.push_back(r.second);
	      vol_name.push_back(v.first);
	      vol_copy.push_back(v.second);
	      sys_name.push_back(s.first);
	      sys_copy.push_back(s.second);
	    }
      else
	    {
	      if (fVerbosity>=3) cout<<" PndDrcOptDevManager::intersect: did not add "
          <<v.first<<endl;

	    }

    }

    return (p_in.size() >0) ? true : false;
  }

  return false;
}
//----------------------------------------------------------------------
bool PndDrcOptDevManager::Cerenkov(const XYZPoint& pos, const XYZVector& dir,
                                   double beta, int n, double range, double nu1, double nu2,
                                   int refl_limit, int particleIDnumber)
{

  list<XYZPoint> p_in_list;
  list<XYZPoint> p_out_list;
  list<string>   vol_name_list;
  list<int>      vol_copy_list;
  list<string>   sys_name_list;
  list<int>      sys_copy_list;


  if(Intersect(pos,dir,
               p_in_list,p_out_list,
               vol_name_list,sys_name_list,
               vol_copy_list,sys_copy_list,range))
  {
    list<XYZPoint>::const_iterator kit_p_in;
    list<XYZPoint>::const_iterator kit_p_out;
    list<string>::const_iterator   kit_vol_name;
    list<int>::const_iterator      kit_vol_copy;
    list<string>::const_iterator   kit_sys_name;
    list<int>::const_iterator      kit_sys_copy;

    bool result = false;

    for (kit_p_in = p_in_list.begin(),
         kit_p_out = p_out_list.begin(),
         kit_vol_name = vol_name_list.begin(),
         kit_vol_copy = vol_copy_list.begin(),
         kit_sys_name = sys_name_list.begin(),
         kit_sys_copy = sys_copy_list.begin();
         kit_p_in != p_in_list.end();
         ++kit_p_in,
         ++kit_p_out,
         ++kit_vol_name,
         ++kit_vol_copy,
         ++kit_sys_name,
         ++kit_sys_copy)
    {
      XYZPoint     r1 = (*kit_p_in);
      XYZPoint     r2 = (*kit_p_out);
      string vol_name = (*kit_vol_name);
      int    vol_copy = (*kit_vol_copy);
      string sys_name = (*kit_sys_name);
      int    sys_copy = (*kit_sys_copy);


      //cout<<" r1= "<<r1.X()<<" "<<r1.Y()<<" "<<r1.Z()<<endl;//###
      //cout<<" r2= "<<r2.X()<<" "<<r2.Y()<<" "<<r2.Z()<<endl;//###
      if (fVerbosity>=3) cout<<" PndDrcOptDevManager::cerenkov: vol "<<vol_name<<endl;

      if (Cerenkov(vol_name,sys_name,vol_copy,sys_copy,r1,r2,beta,n,nu1,nu2,refl_limit,particleIDnumber)) result = true;


    }

    return result;
  }

  return true;
}
//----------------------------------------------------------------------
bool PndDrcOptDevManager::Cerenkov(const string& vol_name, const string& sys_name,
                                   int vol_copy, int sys_copy,const XYZPoint& r1, const XYZPoint& r2,
                                   double beta, int inum, double lam1, double lam2,int refl_limit, int particleIDnumber)
{
  static bool first=true;

  //const double cc = 29.9792;      // c in cm/ns
  const double kPi = 3.1415926535;

  static double nu_mean,fn0;


  if (first)
  {
    first = false;
    if (lam1==lam2) lam2=lam1+1;
    nu_mean = (log(lam2)-log(lam1))/(1.0/lam1 - 1.0/lam2);
    if (fVerbosity>=3) cout<<" PndDrcOptDevManager::cerenkov nu_mean = "
      <<nu_mean<<endl;

    double kk   = 370;           // cm-1 eV-1, PRD 50 (1994) 1261
    double c    = 2.99792458e10; // cm/s
    double hbar = 6.5821220e-16; // eV s

    double nu1  = c/(lam1*1.0e-7); // s-1 lower end of visible spectrum
    double nu2  = c/(lam2*1.0e-7); // s-1 upper end

    fn0 = kk*(hbar*2*kPi)*(nu1-nu2); // cm-1
    fn0 *= 0.1;                     // mm-1
    if (fVerbosity>=3)
    {
      cout<<" N0 = "<<fn0<<" mm-1"
      <<" for nu1,nu2 = "<<lam1<<"-"<<lam2<<endl;
      cout<<"                that is dE = "<<(hbar*2*kPi)*(nu1-nu2)<<" eV"<<endl;
    }
  }



  if (beta>1.001)
  {
    cerr<<"*   : PndDrcOptDevManager::cerenkov: beta="<<beta<<endl;
    cerr<<"      reset beta=0.9999"<<endl;
    beta = 0.9999;
  }


  PndDrcOptDev* dev=Device(vol_name,sys_name,vol_copy,sys_copy);
  if( dev->Radiator() )
  {

    // get number of photons to sample
    double ref_index = dev->OptMaterial().RefIndex(nu_mean);
    double costh = 1.0/(ref_index*beta);
    if (fVerbosity>=3) cout << " costh= " << costh << endl;
    if (costh>1) return 0; // particle too slow


    //cout<<"inum="<<fn0<<" "<<(r1-r2).Mag()<<" "<<(1-costh*costh)<<endl;//###


    int inum1 = inum;
    if (inum==0) // 0 = Cherenkov distributed
    {
      double path = sqrt((r1-r2).Mag2());
      inum1 = int(fn0*path*(1-costh*costh) + 0.5);
    }


    if (inum1==0) return false; // no photons.

    double   x1,x2,x,lambda;
    XYZVector diff;
    XYZPoint  r;
    XYZVector beta_ph;
    double    dtheta,dphi;


    //cout<<" generate photons: "<<inum1<<endl;

    for (int i=0; i<inum1; i++) // loop over photons
    {
      do
      {
        // sample a lambda distribution with a 1/lambda^2 shape.
        x1     = 1.0/lam2;               // inv. lambda start at x1
        x2     = 1.0/lam1;               // inv. lambda end at x2
        x      = gRandom->Uniform(x1,x2);  // inv. lambda random range
        lambda = 1.0/x;                        // yields 1/lambda^2 dist.
        //cout<<" x,lambda="<<x<<" "<<lambda<<endl;//###
        double n = dev->OptMaterial().RefIndex(lambda);
        costh = 1.0L/(beta*n);
      } while (costh>1);

      diff = r2 - r1;
      r    = r1 + gRandom->Uniform(1.0)*diff;
      beta_ph   = diff.Unit();      // velocity vector with length 1
      dphi      = gRandom->Uniform(0,2*kPi);


      //cout<<dphi<<endl;//###


      dtheta = acos(costh);

      beta_ph   = angle_tra(beta_ph,dtheta,dphi);

      PndDrcPhoton ph;

      ph.SetPosition(r);
      ph.SetDirection(beta_ph);

      double dirx = beta_ph.Dot(dev->DirectionX());
      double diry = beta_ph.Dot(dev->DirectionY());
      double dirz = beta_ph.Dot(dev->DirectionZ());

      ph.SetOriginDirection(XYZVector(dirx,diry,dirz));
      ph.SetThetaC(dtheta);
      ph.SetPhiC(dphi);
      ph.SetParticleIDnumber(particleIDnumber);
      ph.SetWavelength(lambda);
      ph.SetDevice(dev);
      ph.SetReflectionLimit(refl_limit);
      fListPhoton.push_back(ph);
    }
  }

  return true;

}
//----------------------------------------------------------------------
void PndDrcOptDevManager::SetPhotonList(list<PndDrcPhoton>& photon_list,
                                        string vol_name, string sys_name,
                                        int ivol_copy, int isys_copy)
{
  list<PndDrcPhoton>::iterator iph;
  //for (iph = photon_list.begin(); iph != photon_list.end(); ++iph)
  //{
  //if ( (*iph).PositionXlist().size() != 1)
  //{
  //  cerr << "*** PndDrcOptDevManager::SetPhotonList: "
  //       << " photons in list have position lists with more than one position. Abort."<<endl;
  //  cerr << "    See documentation of PndDrcPhoton::SetPosition1()"<<endl;
  //  
  //  exit(EXIT_FAILURE);
  //}
  //}
  
  fListPhoton = photon_list;
  
  PndDrcOptDev* dev = 0;
  
  for (iph = fListPhoton.begin(); iph != fListPhoton.end(); ++iph)
    {
      if( vol_name == "@@@")
	dev = (*iph).Device();
      else
	{
	  dev =Device(vol_name,sys_name,ivol_copy,isys_copy); // internal device pointer
	  (*iph).SetDevice(dev);
	}
      
      if( dev == 0 )
	{
	  cerr << "*** PndDrcOptDevManager::SetPhotonList: Photon device is not set" << endl;
	  
	  exit(EXIT_FAILURE);
	}  
      double x = (*iph).Direction().Dot(dev->DirectionX());
      double y = (*iph).Direction().Dot(dev->DirectionY());
      double z = (*iph).Direction().Dot(dev->DirectionZ());
      (*iph).SetOriginDirection(XYZVector(x,y,z));     
    }
}
//----------------------------------------------------------------------
void PndDrcOptDevManager::Propagate()
{
  list<PndDrcPhoton>::iterator iph;
  int photonTotal = fListPhoton.size();

  int cnt = 0;

  for (iph = fListPhoton.begin(); iph != fListPhoton.end(); ++iph)
  {
    PndDrcOptDev* dev = (*iph).Device();

    if (fVerbosity==4)
    {
      cnt++;
      cout << "***** Photon ID: " << cnt << " " << dev->Name() << endl;
      dev->SetVerbosity(4);
    }

    dev->Propagate((*iph));
  }
}
//----------------------------------------------------------------------
void PndDrcOptDevManager::AddTransform(const Transform3D& trans)
{
  list<PndDrcOptDevSys*>::const_iterator kSys;

  for (kSys=fListDevSys.begin(); kSys != fListDevSys.end(); ++kSys)
  {
    PndDrcOptDevSys* sys = (*kSys);
    sys->AddTransform(trans);
  }
}
//----------------------------------------------------------------------
void PndDrcOptDevManager::Print()
{

  cout<<"---------------------------- Start PndDrcOptDevManager objects and pointers"
  <<endl;

  list<PndDrcOptDevSys*>::const_iterator kSys;


  for (kSys=fListDevSys.begin(); kSys != fListDevSys.end(); ++kSys)
  {
    cout<<"\n\n"<<std::hex<< (long int)(*kSys)<<std::dec<<" "<<(*kSys)->Name()<<endl;
    (*kSys)->Print();
  }
  cout<<"---------------------------- End  PndDrcOptDevManager objects and pointers"
  <<endl;
}

