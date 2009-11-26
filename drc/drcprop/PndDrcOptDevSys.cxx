// ----------------------------------------------------
// This file belongs to the ray tracing framework
// for the use with Cherenkov detectors
// 
// created 2007
//-----------------------------------------------------
#include "PndDrcOptDevSys.h"

//#include "PndDrcPhoton.h"
//#include "PndDrcOptReflAbs.h"
#include "PndDrcSurfAbs.h"
//#include "PndDrcOptMatAbs.h"
#include "PndDrcOptDev.h"
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
//#include <fstream>
//using std::fstream;
//
//#include <string>
//using std::string;
//
//#include <list>
//using std::list;
//
//#include <map>
//using std::multimap;
//
//#include <cmath>

//----------------------------------------------------------------------
PndDrcOptDevSys::PndDrcOptDevSys()
{
  fCopyNumber  = 0;
  fName        = "unnamed";
  fLongName    = "";
  fVerbosity   = 0;

  fListDev.clear();
  fListSysOri.clear();
  fListSysOriCopy.clear();
  fListSysOri1Coupling.clear();
  fListSysOri2Coupling.clear();
  fListSysOri1CopyCoupling.clear();
  fListSysOri2CopyCoupling.clear();
  fListDev1Coupling.clear();
  fListDev2Coupling.clear();
  fListDev1CopyCoupling.clear();
  fListDev2CopyCoupling.clear();
  fListSurf1Coupling.clear();
  fListSurf2Coupling.clear();
  fListSurf1CopyCoupling.clear();
  fListSurf2CopyCoupling.clear();
}
//----------------------------------------------------------------------
void PndDrcOptDevSys::Copy(const PndDrcOptDevSys& d)
{
  fCopyNumber        = d.fCopyNumber;
  fName              = d.fName;
  fLongName          = d.fLongName;
  fVerbosity         = d.fVerbosity;

  list<PndDrcOptDev*>::const_iterator kDev;
  list<string>::const_iterator        kSys_ori;
  list<int>::const_iterator           kSys_ori_copy;
  list<string>::const_iterator        kSys_ori1;
  list<string>::const_iterator        kSys_ori2;
  list<int>::const_iterator           kSys_ori1_copy;
  list<int>::const_iterator           kSys_ori2_copy;
  list<string>::const_iterator        kDev1;
  list<string>::const_iterator        kDev2;
  list<int>::const_iterator           kDev1_copy;
  list<int>::const_iterator           kDev2_copy;
  list<string>::const_iterator        kSurf1;
  list<string>::const_iterator        kSurf2;
  list<int>::const_iterator           kSurf1_copy;
  list<int>::const_iterator           kSurf2_copy;

  for(kDev=d.fListDev.begin(),
	kSys_ori=d.fListSysOri.begin(), 
	kSys_ori_copy=d.fListSysOriCopy.begin(); 
      kDev != d.fListDev.end(); 
      ++kDev,
	++kSys_ori,
	++kSys_ori_copy) 
    {
      const PndDrcOptDev* tmp = (*kDev);
      PndDrcOptDev* tmp1 = tmp->Clone();
      fListDev.push_back(tmp1);
      fListSysOri.push_back((*kSys_ori));
      fListSysOriCopy.push_back((*kSys_ori_copy));
    }
  
  // copy couplings
  for(kDev1            = d.fListDev1Coupling.begin(),  
	kDev2          = d.fListDev2Coupling.begin(),
	kDev1_copy     = d.fListDev1CopyCoupling.begin(),  
	kDev2_copy     = d.fListDev2CopyCoupling.begin(),
	kSurf1         = d.fListSurf1Coupling.begin(), 
	kSurf2         = d.fListSurf2Coupling.begin(),
	kSurf1_copy    = d.fListSurf1CopyCoupling.begin(), 
	kSurf2_copy    = d.fListSurf2CopyCoupling.begin(),
	kSys_ori1      = d.fListSysOri1Coupling.begin(),
	kSys_ori2      = d.fListSysOri2Coupling.begin(),
	kSys_ori1_copy = d.fListSysOri1CopyCoupling.begin(),
	kSys_ori2_copy = d.fListSysOri2CopyCoupling.begin();
      kDev1 != d.fListDev1Coupling.end();
      ++kDev1,
	++kDev2,
	++kDev1_copy,
	++kDev2_copy,
	++kSurf1,
	++kSurf2,
	++kSurf1_copy,
	++kSurf2_copy,
	++kSys_ori1,
	++kSys_ori2,
	++kSys_ori1_copy,
	++kSys_ori2_copy) 
    {
      CoupleDevice1((*kDev1),(*kDev2),
		    (*kSurf1),(*kSurf2),
		    (*kSys_ori1),(*kSys_ori2),
		    (*kDev1_copy),(*kDev2_copy),
		    (*kSurf1_copy),(*kSurf2_copy),
		    (*kSys_ori1_copy),(*kSys_ori2_copy));
    }
  

}
//----------------------------------------------------------------------
PndDrcOptDevSys::PndDrcOptDevSys(const PndDrcOptDevSys& d)
{
  if (d.fVerbosity>=1) cout<<"  PndDrcOptDevSys::PndDrcOptDevSys"
			    <<"(const PndDrcOptDevSys&) name,copy: "
			    <<d.fName<<" "<<d.fCopyNumber<<endl;
  Copy(d);
}
//----------------------------------------------------------------------
PndDrcOptDevSys& PndDrcOptDevSys::operator=(const PndDrcOptDevSys& d)
{
  if (d.fVerbosity>=1) cout<<"  PndDrcOptDevSys::operator="
			    <<"(const PndDrcOptDevSys&) name,copy: "
			    <<d.fName<<" "<<d.fCopyNumber<<endl;
  if (&d != this) Copy(d);
  return *this;

}
//----------------------------------------------------------------------
PndDrcOptDevSys::~PndDrcOptDevSys()
{
  list<PndDrcOptDev*>::const_iterator kDev;

  for(kDev=fListDev.begin(); kDev != fListDev.end(); ++kDev) 
    {
      delete (*kDev);
   }
}
//----------------------------------------------------------------------
PndDrcOptDevSys* PndDrcOptDevSys::Clone() const
{
  return new PndDrcOptDevSys(*this);
}
//----------------------------------------------------------------------
void PndDrcOptDevSys::SetNameCopyNumber(string name1, int icopy)
{

  string str = name1 + "_" + itoa(icopy);

  if (fLongName == "")
    {
      fLongName = str;
    }
  else
    {
      fLongName += "_" + str;
    }
  



  list<PndDrcOptDev*>::iterator idev;
  list<string>::iterator isys;
  list<int>::iterator isys_copy;


  // replace in content lists
  for(idev=fListDev.begin(),
	isys=fListSysOri.begin(), 
	isys_copy=fListSysOriCopy.begin(); 
      idev != fListDev.end(); 
      ++idev,
	++isys, 
	++isys_copy) 
    {
      if ((*isys)==Name() && (*isys_copy)==CopyNumber())
	{
	  (*isys_copy) = icopy; 
	  (*isys)      = name1; 
	}    
    }
  
  // replace name in coupling lists

  list<string>::iterator        isys_ori1;
  list<string>::iterator        isys_ori2;
  list<int>::iterator           isys_ori1_copy;
  list<int>::iterator           isys_ori2_copy;

  for(isys_ori1      = fListSysOri1Coupling.begin(),
	isys_ori2      = fListSysOri2Coupling.begin(),
	isys_ori1_copy = fListSysOri1CopyCoupling.begin(),
	isys_ori2_copy = fListSysOri2CopyCoupling.begin();
      isys_ori1 != fListSysOri1Coupling.end();
	++isys_ori1,
	++isys_ori2,
	++isys_ori1_copy,
	++isys_ori2_copy) 
    { 
      if ((*isys_ori1)==Name() && (*isys_ori1_copy)==CopyNumber())
	{
	  (*isys_ori1_copy) = icopy;
	  (*isys_ori1)      = name1;
	}    
      if ((*isys_ori2)==Name() && (*isys_ori2_copy)==CopyNumber())
	{
	  (*isys_ori2_copy) = icopy;
	  (*isys_ori2)      = name1;
	}
    }

  fCopyNumber = icopy;
  fName       = name1;

}
//----------------------------------------------------------------------
void PndDrcOptDevSys::AddDevice(const PndDrcOptDev& dev)
{
  AddDevice(dev,Name(),CopyNumber());
}
//----------------------------------------------------------------------
void PndDrcOptDevSys::AddDevice(const PndDrcOptDev& dev, string sys_ori, int sys_ori_copy)
{
  if (fVerbosity>=1) cout<<"  PndDrcOptDevSys::addDevice:"
			    <<" name,copy = "
			    <<dev.Name()<<" "<<dev.CopyNumber()<<endl;
  PndDrcOptDev* tmp = dev.Clone();
  fListDev.push_back(tmp);
  fListSysOri.push_back(sys_ori);
  fListSysOriCopy.push_back(sys_ori_copy);
  

}
//----------------------------------------------------------------------
void PndDrcOptDevSys::AddDeviceSystem(const PndDrcOptDevSys& sys)
{
  if (fVerbosity>=1) cout<<"  PndDrcOptDevSys::addDeviceSystem: "
			    <<" name,copy = "
			    <<sys.Name()<<" "<<sys.CopyNumber()<<endl;
  

  


  PndDrcOptDevSys* tmp = sys.Clone();
  

  list<PndDrcOptDev*>::const_iterator kDev;
  for (kDev =  tmp->DeviceList().begin(); 
       kDev != tmp->DeviceList().end();
       ++kDev)
    {
      AddDevice(*(*kDev),sys.Name(),sys.CopyNumber());
    }
  
  // copy couplings
  list<string>::const_iterator        kSys_ori1;
  list<string>::const_iterator        kSys_ori2;
  list<int>::const_iterator           kSys_ori1_copy;
  list<int>::const_iterator           kSys_ori2_copy;
  list<string>::const_iterator        kDev1;
  list<string>::const_iterator        kDev2;
  list<int>::const_iterator           kDev1_copy;
  list<int>::const_iterator           kDev2_copy;
  list<string>::const_iterator        kSurf1;
  list<string>::const_iterator        kSurf2;
  list<int>::const_iterator           kSurf1_copy;
  list<int>::const_iterator           kSurf2_copy;


  for(kDev1            = tmp->fListDev1Coupling.begin(),  
	kDev2          = tmp->fListDev2Coupling.begin(),
	kDev1_copy     = tmp->fListDev1CopyCoupling.begin(),  
	kDev2_copy     = tmp->fListDev2CopyCoupling.begin(),
	kSurf1         = tmp->fListSurf1Coupling.begin(), 
	kSurf2         = tmp->fListSurf2Coupling.begin(),
	kSurf1_copy    = tmp->fListSurf1CopyCoupling.begin(), 
	kSurf2_copy    = tmp->fListSurf2CopyCoupling.begin(),
	kSys_ori1      = tmp->fListSysOri1Coupling.begin(),
	kSys_ori2      = tmp->fListSysOri2Coupling.begin(),
	kSys_ori1_copy = tmp->fListSysOri1CopyCoupling.begin(),
	kSys_ori2_copy = tmp->fListSysOri2CopyCoupling.begin();
      kDev1 != tmp->fListDev1Coupling.end();
      ++kDev1,
	++kDev2,
	++kDev1_copy,
	++kDev2_copy,
	++kSurf1,
	++kSurf2,
	++kSurf1_copy,
	++kSurf2_copy,
	++kSys_ori1,
	++kSys_ori2,
	++kSys_ori1_copy,
	++kSys_ori2_copy) 
    {
      CoupleDevice1((*kDev1),(*kDev2),
		    (*kSurf1),(*kSurf2),
		    (*kSys_ori1),(*kSys_ori2),
		    (*kDev1_copy),(*kDev2_copy),
		    (*kSurf1_copy),(*kSurf2_copy),
		    (*kSys_ori1_copy),(*kSys_ori2_copy));
    }
  


}

//----------------------------------------------------------------------
void PndDrcOptDevSys::EmbedDevice(PndDrcOptDev& d_daughter, PndDrcOptDev& d_mother)
{
  if (fVerbosity>=1) cout<<"  PndDrcOptDevSys::embedDevice:  name,copy = "
			  <<d_daughter.Name()<<" "<<d_daughter.CopyNumber()<<" in "
			  <<d_mother.Name()<<" "<<d_mother.CopyNumber()<<endl;;


  // add daughter
  AddDevice(d_daughter);

  // create clones of surfaces of daughter and add them to mother
  // and connect them

  list<PndDrcSurfAbs*>::const_iterator kSurf;
  for (kSurf=d_daughter.SurfaceList().begin(); 
       kSurf != d_daughter.SurfaceList().end(); ++kSurf)
    {

      
      if (!(*kSurf)->Internal()) // internal surfaces are not connected.
	{
	  const PndDrcSurfAbs* tmp  = (*kSurf);
	  PndDrcSurfAbs*       tmp1 = tmp->Clone();
	  // Append an "_embedded" to the new surface name which is 
	  // in moment that of the surface name of the daughter
	  tmp1->SetName(d_daughter.Name()+"_"+tmp->Name()+"_embedded");

	  // remove reflectivity, other wise you get the product of reflectivities
	  // of mother and daughter.
	  tmp1->ClearReflectivity();

	  // find d_mother which has after adding a different pointer...
	  
	  PndDrcOptDev* d_mother1=0;
	  
	  list<PndDrcOptDev*>::const_iterator kDev;
	  for(kDev=fListDev.begin(); kDev != fListDev.end(); ++kDev) 
	    {
	      if ((*kDev)->Name() == d_mother.Name()) 
		{
		  d_mother1=(*kDev);
		} 
	    }
	  if (d_mother1==0)
	    {
	      cerr<<"*** PndDrcOptDevSys::embedDevice: device not found."<<endl;
	      exit(EXIT_FAILURE); 
	    }
	  d_mother1->AddSurface(*tmp1);

	  // Connect both
	  CoupleDevice(d_mother.Name(),d_daughter.Name(),tmp1->Name(),tmp->Name());	  
	}
    }
}

//----------------------------------------------------------------------
void PndDrcOptDevSys::CoupleDevice(string dev1,       string dev2,
				string surf1,      string surf2,
				int    dev_copy1,  int    dev_copy2,
				int    surf_copy1, int    surf_copy2)
{
  CoupleDevice1(dev1,dev2,surf1,surf2,Name(),Name(),dev_copy1,dev_copy2,surf_copy1,surf_copy2,0,0);
}

//----------------------------------------------------------------------
void PndDrcOptDevSys::CoupleDevice1(string dev1,       string dev2,
				 string surf1,      string surf2,
				 string sys1,       string sys2,
				 int    dev_copy1,  int    dev_copy2,
				 int    surf_copy1, int    surf_copy2,
				 int    sys_copy1,  int    sys_copy2)
{
  list<PndDrcOptDev*>::const_iterator kDev;
  list<string>::const_iterator        kSys_ori;
  list<int>::const_iterator           kSys_ori_copy;
  

  PndDrcOptDev*  d1=0;
  PndDrcOptDev*  d2=0;
  PndDrcSurfAbs* s1=0;
  PndDrcSurfAbs* s2=0;



  for(kDev=fListDev.begin(),
	kSys_ori=fListSysOri.begin(),
	kSys_ori_copy=fListSysOriCopy.begin(); 
      kDev != fListDev.end(); 
      ++kDev,
	++kSys_ori,
	++kSys_ori_copy) 
    {

      //cout<<" PndDrcOptDevSys::coupleDevice1: check "<<endl;
      //cout<<" dev1 "<<(*kDev)->name()<<" "<<(*kDev)->copyNumber()<<" to "<<dev1<<" "<<dev_copy1<<endl;
      //cout<<" sys1 "<<(*kSys_ori)<<" "<<(*kSys_ori_copy)<<" to "<<sys1<<" "<<sys_copy1<<endl;
      
      if ((*kDev)->Name() == dev1 && (*kDev)->CopyNumber() == dev_copy1 &&
	  (*kSys_ori) == sys1 && (*kSys_ori_copy) == sys_copy1) 
	{
	  d1=(*kDev);
	}
      
      //cout<<" PndDrcOptDevSys::coupleDevice1: check "<<endl;
      //cout<<" dev2 "<<(*kDev)->name()<<" "<<(*kDev)->copyNumber()<<" to "<<dev2<<" "<<dev_copy2<<endl;
      //cout<<" sys2 "<<(*kSys_ori)<<" "<<(*kSys_ori_copy)<<" to "<<sys2<<" "<<sys_copy2<<endl;

      if ((*kDev)->Name() == dev2 && (*kDev)->CopyNumber() == dev_copy2 &&
	  (*kSys_ori) == sys2 && (*kSys_ori_copy) == sys_copy2) 
	{
	  d2=(*kDev);
	}
    }

  //cout<<" pointers = "<<(long int)(d1)<<" "<<(long int)(d2)<<endl;

  if (d1 && d2)
    {
      list<PndDrcSurfAbs*>::const_iterator kSurf;
      for(kSurf=d1->SurfaceList().begin(); kSurf != d1->SurfaceList().end(); ++kSurf) 
	{	  
	  //cout<<" test s1="<<(*kSurf)->name()<<endl;
	  
	  if ((*kSurf)->Name()== surf1 && (*kSurf)->CopyNumber() == surf_copy1) s1=(*kSurf); 
	}
      for(kSurf=d2->SurfaceList().begin(); kSurf != d2->SurfaceList().end(); ++kSurf) 
	{
	  //cout<<" test s2="<<(*kSurf)->name()<<endl;
	  if ((*kSurf)->Name()== surf2 && (*kSurf)->CopyNumber() == surf_copy2) s2=(*kSurf); 
	}
      if (s1 && s2)
	{
	  if (fVerbosity>=1)
	    {
	      cout<<" PndDrcOptDevSys::coupleDevice "
		  <<dev1<<"/"<<surf1<<" with "
		  <<dev2<<"/"<<surf2<<endl;
	    }
	  s1->SetCoupled(d2,s2);
	  s2->SetCoupled(d1,s1);
	  fListDev1Coupling.push_back(dev1);
	  fListDev2Coupling.push_back(dev2);
	  fListSurf1Coupling.push_back(surf1);
	  fListSurf2Coupling.push_back(surf2);
	  fListSysOri1Coupling.push_back(sys1);
	  fListSysOri2Coupling.push_back(sys2);
	  fListDev1CopyCoupling.push_back(dev_copy1);
	  fListDev2CopyCoupling.push_back(dev_copy2);
	  fListSurf1CopyCoupling.push_back(surf_copy1);
	  fListSurf2CopyCoupling.push_back(surf_copy2);
	  fListSysOri1CopyCoupling.push_back(sys_copy1);
	  fListSysOri2CopyCoupling.push_back(sys_copy2);
	  
	}
      else
	{
	  cerr<<" *** PndDrcOptDevSys::coupleDevice: surface not found"<<endl;
	  if (!s1) cerr<<"     "<<surf1<<" missing"<<endl;
	  if (!s2) cerr<<"     "<<surf2<<" missing"<<endl;
	  exit(EXIT_FAILURE);
	}
    }
  else
    {
      cerr<<" *** PndDrcOptDevSys::coupleDevice: device not found"<<endl;
      if (!d1) cerr<<"     "<<dev1<<" missing"<<endl;
      if (!d2) cerr<<"     "<<dev2<<" missing"<<endl;
      exit(EXIT_FAILURE);
    }

}
//----------------------------------------------------------------------
void PndDrcOptDevSys::Print(fstream& stream) const
{
  list<PndDrcOptDev*>::const_iterator kDev;

  for(kDev=fListDev.begin(); kDev != fListDev.end(); ++kDev) 
    {
      if (fVerbosity>=3) cout<<" PndDrcOptDevSys::print: set print flag for "
			      <<(*kDev)->Name()<<" "<<(*kDev)->CopyNumber()<<endl;
      (*kDev)->Print(stream);
   }
}
//----------------------------------------------------------------------
void PndDrcOptDevSys::SetPrintColor(int col)
{
  list<PndDrcOptDev*>::const_iterator kDev;

  for(kDev=fListDev.begin(); kDev != fListDev.end(); ++kDev) 
    {
      (*kDev)->SetPrintColor(col);
   }
}
//----------------------------------------------------------------------
void PndDrcOptDevSys::SetFresnel(bool flag)
{
  list<PndDrcOptDev*>::const_iterator kDev;

  for(kDev=fListDev.begin(); kDev != fListDev.end(); ++kDev) 
    {
      (*kDev)->SetFresnel(flag);
   }
}
//----------------------------------------------------------------------
void PndDrcOptDevSys::AddTransform(const Transform3D& trans)
{

  list<PndDrcOptDev*>::const_iterator kDev;
	      
  for (kDev=fListDev.begin(); kDev != fListDev.end(); ++kDev)
    {
      const_cast<PndDrcOptDev*>(*kDev)->AddTransform(trans);
    }

}
//----------------------------------------------------------------------
void PndDrcOptDevSys::Print() const
{
  list<PndDrcOptDev*>::const_iterator kDev;
	  
  cout<<"     PndDrcOptDevSys::print name= "<<fName<<endl;
  cout<<"                         entries= "<<fListDev.size()<<endl;
  
  for (kDev=fListDev.begin(); kDev != fListDev.end(); ++kDev)
    {
  //    cout<<"\t\t"<<std::hex<<int(*i)<<std::dec<<" "<<(*i)->name()<<endl;
   //   (*i)->print();
    }

  cout<<"                         couplings= "<<fListDev1Coupling.size()<<endl;

  list<string>::const_iterator        kSys_ori1;
  list<string>::const_iterator        kSys_ori2;
  list<int>::const_iterator           kSys_ori1_copy;
  list<int>::const_iterator           kSys_ori2_copy;
  list<string>::const_iterator        kDev1;
  list<string>::const_iterator        kDev2;
  list<int>::const_iterator           kDev1_copy;
  list<int>::const_iterator           kDev2_copy;
  list<string>::const_iterator        kSurf1;
  list<string>::const_iterator        kSurf2;
  list<int>::const_iterator           kSurf1_copy;
  list<int>::const_iterator           kSurf2_copy;


  for(kDev1            = fListDev1Coupling.begin(),  
	kDev2          = fListDev2Coupling.begin(),
	kDev1_copy     = fListDev1CopyCoupling.begin(),  
	kDev2_copy     = fListDev2CopyCoupling.begin(),
	kSurf1         = fListSurf1Coupling.begin(), 
	kSurf2         = fListSurf2Coupling.begin(),
	kSurf1_copy    = fListSurf1CopyCoupling.begin(), 
	kSurf2_copy    = fListSurf2CopyCoupling.begin(),
	kSys_ori1      = fListSysOri1Coupling.begin(),
	kSys_ori2      = fListSysOri2Coupling.begin(),
	kSys_ori1_copy = fListSysOri1CopyCoupling.begin(),
	kSys_ori2_copy = fListSysOri2CopyCoupling.begin();
      kDev1 != fListDev1Coupling.end();
      ++kDev1,
	++kDev2,
	++kDev1_copy,
	++kDev2_copy,
	++kSurf1,
	++kSurf2,
	++kSurf1_copy,
	++kSurf2_copy,
	++kSys_ori1,
	++kSys_ori2,
	++kSys_ori1_copy,
	++kSys_ori2_copy) 
    { 
      cout<<endl;
      cout<<"      sys1  = "<<(*kSys_ori1)<<"\t"<<(*kSys_ori1_copy)
	  <<" \t with sys2  = "<<(*kSys_ori2)<<"\t"<<(*kSys_ori2_copy)<<endl;
      cout<<"      dev1  = "<<(*kDev1)<<"\t"<<(*kDev1_copy)
	  <<" \t with dev2  = "<<(*kDev2)<<"\t"<<(*kDev2_copy)<<endl;
      cout<<"      surf1 = "<<(*kSurf1)<<"\t"<<(*kSurf1_copy)
	  <<" \t with surf2 = "<<(*kSurf2)<<"\t"<<(*kSurf2_copy)<<endl;
      
    }

}



