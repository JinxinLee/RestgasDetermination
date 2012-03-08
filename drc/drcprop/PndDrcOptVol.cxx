// ----------------------------------------------------
// This file belongs to the ray tracing framework
// for the use with Cherenkov detectors
//
// created 2007
//-----------------------------------------------------
#include "PndDrcOptVol.h"

//#include "PndDrcPhoton.h"
#include "PndDrcOptReflAbs.h"
#include "PndDrcSurfAbs.h"
#include "PndDrcOptMatAbs.h"
//#include "PndDrcOptDev.h"
//#include "PndDrcUtil.h"
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
#include <limits>
using std::numeric_limits;

#include <TMath.h>

#include <stdlib.h> // system
#include <TString.h>

//----------------------------------------------------------------------
PndDrcOptVol::PndDrcOptVol()
{
  fOptMat      = 0;
  fRadiator = true;
}
//----------------------------------------------------------------------
PndDrcOptVol::~PndDrcOptVol()
{
  delete fOptMat;
}
//----------------------------------------------------------------------
PndDrcOptVol* PndDrcOptVol::Clone() const
{
  return new PndDrcOptVol(*this);
}
//----------------------------------------------------------------------
void PndDrcOptVol::Copy(const PndDrcOptVol& d)
{
  if (!(&(d.OptMaterial())))
  {
    cerr<<" ***  PndDrcOptVol::copy: optMaterial undefined."<<endl;
    cerr<<"                          vol name =  "<<Name()<<endl;
    exit(EXIT_FAILURE);
  }

  if (&(d.OptMaterial())) fOptMat = (d.OptMaterial()).Clone();

  fRadiator = d.fRadiator;
}
//----------------------------------------------------------------------
PndDrcOptVol::PndDrcOptVol(const PndDrcOptVol& d) : PndDrcOptDev(d)
{
  if (d.fVerbosity>=1) cout<<"  PndDrcOptVol::PndDrcOptVol"
        <<"(const PndDrcOptVol&) name,copy: "
        <<d.fName<<" "<<d.fCopyNumber<<endl;
  Copy(d);
}
//----------------------------------------------------------------------
PndDrcOptVol& PndDrcOptVol::operator=(const PndDrcOptVol& d)
{
  if (d.fVerbosity>=1) cout<<"  PndDrcOptVol::operator="
        <<"(const PndDrcOptVol&) name,copy: "
        <<d.fName<<" "<<d.fCopyNumber<<endl;
  if (&d != this)
  {
    static_cast<PndDrcOptDev&>((*this)) = d; // assignment of base class part.
    Copy(d);
  }
  return *this;
}
//----------------------------------------------------------------------
void PndDrcOptVol::SetOptMaterial(const PndDrcOptMatAbs& mat)
{
  if (&mat)
    {
      delete fOptMat;
      fOptMat = mat.Clone();
    }
}
//----------------------------------------------------------------------
void PndDrcOptVol::Propagate(PndDrcPhoton& ph)
{
  //   fVerbosity=4;
  static const double kEps = 1.0e-9;

  if (Verbosity()>=4)
    {
      cout<<"    PndDrcOptVol::propagate, you are in "<<Name()<<endl;
      cout<<"    photon is"<<endl;
      ph.Print();
    }
  list<PndDrcSurfAbs*>::const_iterator kSurf;


  while (ph.Fate() == Drc::kPhotFlying)
    {// while

      if(Verbosity()>=4)
	{
	  cout << "<<<<< Photon Reflection: " << ph.Reflections() << endl;
	  ph.Print();
	}

      if (ph.Reflections() > ph.ReflectionLimit())
	{
	  ph.SetFate(Drc::kPhotAbsorbed);
	  if (Verbosity()>=4) cout<<"     reflection limit was reached (absorbed)" << endl;
	  break; // leave while loop
	}


      ph.SetDevice(this);
      //---------------
      // search the closest surface which is not the surface where the photon is
      // right now.
      PndDrcSurfAbs* surf_closest=0;
      double path_length_min = numeric_limits<double>::max();
      for(kSurf=fListSurf.begin(); kSurf != fListSurf.end(); ++kSurf)
	{
	  if (Verbosity()>=4) cout<<"     check "<<(*kSurf)->Name()
				  <<" (surface coupling = "
				  <<(*kSurf)->Coupled()<<")"<<endl;
	  XYZPoint pos_new;
	  double   path_length;
	  if ((*kSurf)->SurfaceHit(ph,pos_new,path_length))
	    {
	      if (Verbosity()>=4) cout<<"     hit for "<<(*kSurf)->Name()<<endl
				      <<"     path_length "<<path_length<<endl;
	      if (path_length<path_length_min && path_length>kEps)
		{
		  path_length_min = path_length;
		  surf_closest    = (*kSurf);
		}
	    }
	  else
	    {
	      if (Verbosity()>=4) cout<<"     no hit "<<(*kSurf)->Name()<<" of "<<Name()<<endl;
	    }
	}
      if (surf_closest==0)
	{
	  if (ph.Fate() == Drc::kPhotFlying)
	    {
	      if (Verbosity()>=4) cout<<"     no hit with any surface ->edge hit->lost"<<endl;
	      if (fPhotonTrace) ph.Print(*fPhotonTraceStream);
	      ph.SetFate(Drc::kPhotLost);
	      break;
	    }
	}
      // reset photon, that was only a search...
      ph.SetFate(Drc::kPhotFlying);
      //---------------

      if (Verbosity()>=4) cout<<"     surface coupling = "
			      <<surf_closest->Coupled()
			      <<" for "
			      <<Name()<<" "
			      <<CopyNumber()<<" "
			      <<surf_closest->Name()<<endl;


      // Now the closest surface is found.

      if (Verbosity()>=4) cout<<"     closest "<<surf_closest->Name()<<endl;

      XYZPoint pos_new;
      double   path_length;

      if (!surf_closest->SurfaceHit(ph,pos_new,path_length))
	{
	  cerr<<" *** PndDrcOptVol::Propagate: forbidden clause! Stop."<<endl;
	  exit(EXIT_FAILURE);
	}
      else
	{// surf_closest->SurfaceHit...
	  if (Verbosity()>=4)
            {
              cout<<"     hit, set pos from "<<ph.Position()<<endl;
              cout<<"                  to   "<<pos_new<<endl;
            }
	  ph.SetPosition1(pos_new);
	  if (fPhotonTrace) ph.Print(*fPhotonTraceStream);
	  if (Verbosity()>=4)
            {
              cout<<"     trace was called"<<endl;
            }

	  // Step 1   ------------ Check absorption -----------

	  if (OptMaterial().AbsorptionFlag(ph.Wavelength(),path_length))
            {
              ph.SetFate(Drc::kPhotAbsorbed);
              if (Verbosity()>=4) cout<<"     absorption by material" << endl;
              break; // leave while loop
            }
	  else
            {
              if (Verbosity()>=4) cout<<"     new position set."<<endl;
              if (ph.Fate()!=Drc::kPhotFlying) break;//###1 // measured photons
            }

	  //  Step 2 ------------ Check reflectivity -------------
	  XYZVector norm = surf_closest->Normal(ph.Position());
	  Drc::Reflectivity refl;
	  //ph.SetDevice(this);


	  // NOT COUPLED
	  if (! surf_closest->Coupled())
	    {
	      if (&(surf_closest->Reflectivity())) // Reflectivity defined
		{
		  if (Verbosity()>=4) cout<<"     PndDrcOptVol::reflectivity1a clause"<<endl;

		  //refl = Drc::ReflReflected; // needless ???
		  refl = surf_closest->Reflectivity().Query(ph,norm);

		  if (refl == Drc::ReflAbsorbed)
		    {

		      if (Verbosity()>=4)
			cout<<"     PndDrcOptVol::propagate: absorbed"<<endl;
		      ph.SetFate(Drc::kPhotAbsorbed);
		      return;
		    }
		  if (refl == Drc::ReflTransmitted)
		    {
		      ph.SetFate(Drc::kPhotLost); // Photon ransmitted in nirvana.
		      if (Verbosity()>=4) cout<<"     Photon lost"<<endl;
		      break; // while loop
		      // do nothing
		    }
		  if (refl == Drc::ReflReflected)
		    {
		      ph.Reflect(norm);
		      continue; // while loop
		    }
		  if (refl == Drc::ReflRefracted)
		    {
		      bool refr = ph.Refract(norm,
					     OptMaterial().RefIndex(ph.Wavelength()),
					     OptMaterial().Extinction(ph.Wavelength()),
					     surf_closest->Fresnel());
		      // after this routine the photon has already the right direction!
		      if (refr)
			{
			  ph.SetFate(Drc::kPhotLost); // Photon refracted in nirvana.
			  if (Verbosity()>=4) cout<<"     Photon lost"<<endl;
			  break; // while loop
			}
		    }
		}
	      else 
		{ // no reflectivity
		  // Step 3a ----------- Refraction (no reflectivity defined, no couplings)
		  if (Verbosity()>=4) cout<<"     PndDrcOptVol::reflectivity1b clause";
		
		  bool refr = ph.Refract(norm,
					 OptMaterial().RefIndex(ph.Wavelength()),
					 OptMaterial().Extinction(ph.Wavelength()),
					 surf_closest->Fresnel());
		  // after this routine the photon has already the right direction!

		  if (Verbosity()>=4) cout<<"  refr="<<refr<<endl;


		  if (refr)
		    {
		      if ( ph.Fate() == Drc::kPhotFlying) // otherwise different fate alread assigned
			{
			  ph.SetFate(Drc::kPhotLost); // Photon refracted in nirvana.
			  if (Verbosity()>=4) cout<<"     Photon lost"<<endl;
			  break;
			}
		    }
		  else
		    {
		      continue; // while loop
		    }
		}
	    } 
	  else // COUPLED
	    {
	      // Step 3b ------------ Coupled volumes
		


	      // find coupled device and surface
	      PndDrcOptDev*  dev_coupled  = 0;
	      PndDrcSurfAbs* surf_coupled = 0;
              list<PndDrcOptDev*>::const_iterator  kDev_coupled;
              list<PndDrcSurfAbs*>::const_iterator kSurf_coupled;
              for (kDev_coupled  = (surf_closest->CoupledDeviceList()).begin(),
		     kSurf_coupled = (surf_closest->CoupledSurfaceList()).begin();
                   kDev_coupled != (surf_closest->CoupledDeviceList()).end();
                   ++kDev_coupled, ++kSurf_coupled)
		{
		  if (Verbosity()>=4) cout<<"     coupling to dev,copy,surf= "
					  <<(*kDev_coupled)->Name()<<","
					  <<(*kDev_coupled)->CopyNumber()<<","
					  <<(*kSurf_coupled)->Name()<<endl;
		  PndDrcPhoton ph1(ph);
		  ph1.SetPosition1(ph.Position()-ph.Direction()*0.1);
		  bool hit = (*kSurf_coupled)->SurfaceHit(ph1,pos_new,path_length);
		  if (hit)
		    {
		      dev_coupled  = (*kDev_coupled);
		      surf_coupled = (*kSurf_coupled);
		    }
		}//loop



	      // now coupled device and surface is known
	      if (dev_coupled && surf_coupled)
		{
		  // ------- check if you get out
		  if (&(surf_closest->Reflectivity())) // Reflectivity defined
		    {
		      if (Verbosity()>=4) cout<<"     PndDrcOptVol::reflectivity3a clause"<<endl;
		      refl = Drc::ReflReflected; // needless ???
		      refl = surf_closest
			->Reflectivity().Query(ph,
					       norm,
					       dev_coupled->
					       OptMaterial().RefIndex(ph.Wavelength()),
					       Drc::ReflOut);		      
		      if (refl == Drc::ReflAbsorbed)
			{
			  if (Verbosity()>=4)
			    cout<<"     PndDrcOptVol::propagate: absorbed"<<endl;
			  ph.SetFate(Drc::kPhotAbsorbed);
			  return;
			}
		      if (refl == Drc::ReflTransmitted)
			{
			  PndDrcOptMatAbs* opt_mat = &((dev_coupled)->OptMaterial());
			  double n1  = OptMaterial().RefIndex(ph.Wavelength());
			  double ex1 = OptMaterial().Extinction(ph.Wavelength());
			  double n2  = opt_mat->RefIndex(ph.Wavelength());
			  double ex2 = opt_mat->Extinction(ph.Wavelength());
			  bool iref = ph.Refract(surf_closest->Normal(ph.Position()),
						 n1, ex1,
						 false, /*surf_closest->Fresnel(),*/
						 n2, ex2);
			  // after this routine the photon has already the right direction!
			  (dev_coupled)->Propagate(ph);
			  //cout<<" transmission in DrcOptVol "<<fName<<endl;
			  // do nothing
			}
		      if (refl == Drc::ReflReflected)
			{
			  ph.Reflect(norm); // photon was not reflected by Query
			  continue; // while loop
			}
		      if (refl == Drc::ReflRefracted)
			{
			  bool refr = ph.Refract(norm,
						 OptMaterial().RefIndex(ph.Wavelength()),
						 OptMaterial().Extinction(ph.Wavelength()),
						 surf_closest->Fresnel());
			  // after this routine the photon has already the right direction!
			  if (refr)
			    {
			      ph.SetFate(Drc::kPhotLost); // Photon refracted in nirvana.
			      if (Verbosity()>=4) cout<<"     Photon lost"<<endl;
			      //cout<<"     ### PndDrcOptVol::propagate: lost"<<endl;
			      return; // while loop
			    }
			  else
			    {
			      // do nothing
			    }
			}
		    }
		  // -------  check if you get in
		  if (&(surf_coupled->Reflectivity())) // Reflectivity defined
		    {
		      if (Verbosity()>=4) cout<<"     PndDrcOptVol::reflectivity3b clause"<<endl;
		      // check reflectivity
		      norm = (surf_coupled)->Normal(ph.Position());
		      refl = Drc::ReflReflected;
		      refl = (surf_coupled)->
			Reflectivity().Query(ph,norm,
					     dev_coupled->
					     OptMaterial().RefIndex(ph.Wavelength()),
					     Drc::ReflIn);


		      if (refl == Drc::ReflAbsorbed)
			{
			  if (Verbosity()>=4)
			    cout<<"     PndDrcOptVol::propagate: mirror absorbed"<<endl;
			  ph.SetFate(Drc::kPhotAbsorbed);
			  return;
			}
		      if (refl == Drc::ReflTransmitted)
			{
			  PndDrcOptMatAbs* opt_mat = &((dev_coupled)->OptMaterial());
			  double n1  = OptMaterial().RefIndex(ph.Wavelength());
			  double ex1 = OptMaterial().Extinction(ph.Wavelength());
			  double n2  = opt_mat->RefIndex(ph.Wavelength());
			  double ex2 = opt_mat->Extinction(ph.Wavelength());
			  bool iref = ph.Refract(surf_closest->Normal(ph.Position()),
						 n1, ex1,
						 false, /*surf_closest->Fresnel(),*/
						 n2, ex2);
			  // after this routine the photon has already the right direction!
			  (dev_coupled)->Propagate(ph);
			}
		      if (refl == Drc::ReflReflected)
			{
			  ph.Reflect(norm); // Query has not reflected 
			  continue;
			}
		      if (refl == Drc::ReflRefracted)
			{
			  // do nothing (no Fresnel due to reflectivity ??? )
			}
		    } // end of reflectivity
		  else
		    { // no reflectivity defined
		      PndDrcOptMatAbs* opt_mat = &((dev_coupled)->OptMaterial());
		      if (Verbosity()>=4) cout<<"     opt_mat="<<opt_mat->Name()<<endl;
		      if (opt_mat)
			{
			  double n1  = OptMaterial().RefIndex(ph.Wavelength());
			  double ex1 = OptMaterial().Extinction(ph.Wavelength());
			  double n2  = opt_mat->RefIndex(ph.Wavelength());
			  double ex2 = opt_mat->Extinction(ph.Wavelength());
			  // cout << "VOLCHECK: " << n1 << " " << ex1 << " " << n2 << " " << ex2 << endl;

			  bool iref = ph.Refract(surf_closest->Normal(ph.Position()),
						 n1, ex1,
						 surf_closest->Fresnel(),
						 n2, ex2);
		  // after this routine the photon has already the right direction!

			  //               if( surf_closest->Name() == "slab_side1" && !iref )
			  //                 cout << "wrong" << endl;

			  if (Verbosity()>=4) 
			    {
			      if (iref) {cout<<" refract in new volume"<<endl;}
			      else      {cout<<" no refraction in new volume"<<endl;}
			    }
			  if (iref) // refraction in new volume
			    {
			      if (Verbosity()>=4) cout<<"     go into new volume "
						      <<(dev_coupled)->Name()<<endl;
			      //cout<<" ### leave by propagate to next vol ..."<<endl;
			      (dev_coupled)->Propagate(ph);
			    }
			  else
			    {
			      continue; //  photon is (already) reflected.
			    }
			} // opt_mat
		      //else // screen or mirror...
		      //{
		      //  // do not bring photons inside flat objects
		      //  // ph.setPosition(ph.position()+2*kEps*ph.direction());
		      //  (dev_coupled)->Propagate(ph);
		      //}

		    } // no reflectivity defined

		} // dev_coupled && surf_coupled


	      //} // for loop
		
            } // surf_closest coupled
	  if (ph.Fate() != Drc::kPhotFlying) break; // while loop
	} // if surf_closest...
    } // while
  

  // if the photon got from a surface hit the attribute Drc::kPhotMeasured
  // it should pass this location.

  // has moved into the surfaces->surfaceHit
  //  if (ph.Fate() == Drc::kPhotMeasured)
  //PositionCorrection(ph);
}
//----------------------------------------------------------------------
//void PndDrcOptVol::PositionCorrection(PndDrcPhoton& ph)
//{
// do nothing
// might be implemented in detectors derived from this class.
//}
