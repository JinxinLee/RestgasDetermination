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
            ph.SetPosition(pos_new);

	  // Step 1   ------------ Check absorption -----------

            if (OptMaterial().AbsorptionFlag(ph.Wavelength(),path_length))
            {
	      //ph.setPosition(ph.position()+path_length*ph.direction());
              if (fPhotonTrace) ph.Print(*fPhotonTraceStream);
              ph.SetFate(Drc::kPhotAbsorbed);
              if (Verbosity()>=4) cout<<"     absorption by material" << endl;
              break; // leave while loop
            }
            else
            {
              if (Verbosity()>=4) cout<<"     new position set."<<endl;
              if (fPhotonTrace) ph.Print(*fPhotonTraceStream);
              if (ph.Fate()!=Drc::kPhotFlying) break;//###1 // measured photons
            }

	  //  Step 2 ------------ Check reflectivity -------------
            XYZVector norm = surf_closest->Normal(ph.Position());
            Drc::Reflectivity refl;

            if (&(surf_closest->Reflectivity())) // Reflectivity defined
            {
              if (Verbosity()>=4) cout<<"     PndDrcOptVol::reflectivity1a clause"<<endl;
              ph.SetDevice(this);

              refl = Drc::ReflReflected; // needless ???
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
                if (refr)
                {
                  ph.SetFate(Drc::kPhotLost); // Photon refracted in nirvana.
                  if (Verbosity()>=4) cout<<"     Photon lost"<<endl;
                  break; // while loop
                }
              }
            }
            else if (!  surf_closest->Coupled())
            {
	      // Step 3a ----------- Refraction (no reflectivity defined, no couplings)
              if (Verbosity()>=4) cout<<"     PndDrcOptVol::reflectivity1b clause";

              bool refr = ph.Refract(norm,
                                     OptMaterial().RefIndex(ph.Wavelength()),
                                     OptMaterial().Extinction(ph.Wavelength()),
                                     surf_closest->Fresnel());

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
                continue; // while loop
            }



	  // Step 3b ------------ Coupled volumes

            if (Verbosity()>=4) cout<<"     surface coupling = "
                  <<surf_closest->Coupled()
                  <<" for "
                  <<Name()<<" "
                  <<CopyNumber()<<" "
                  <<surf_closest->Name()<<endl;

            if (surf_closest->Coupled())
            {// if surf_closest coupled
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

		  // deal with kEps problem in sphere and parabolid
                PndDrcPhoton ph1(ph);
		  //if (! (*kSurf_coupled)->isFlat())
		  // taken out 27.02.08 after problems in test_barrel1
                {
                  ph1.SetPosition(ph.Position()-ph.Direction()*0.1);
                  if (Verbosity()>=4)
                  {
                    cout<<"     bring temporarily back from "
                        <<ph.Position()<<" to "
                        <<ph1.Position()<<endl;
                    cout<<"       ( ph has still old value...)"<<endl;

			//cout<<"           direction "<<ph1.Direction()<<endl;
                  }
                }
		  //(*kSurf_coupled)->setVerbosity(5);//###
                bool hit = (*kSurf_coupled)->SurfaceHit(ph1,pos_new,path_length);

                if (Verbosity()>=4)
                {
                  cout<<"     hit="<<hit<<" with "
                      <<(*kSurf_coupled)->Name()
                      <<" of "
                      <<(*kDev_coupled)->Name()
                      <<"/"
                      <<(*kDev_coupled)->CopyNumber()<<endl;
                  cout<<"     for ph x,xdir :"<<ph.Position().X()<<" "<<ph.Direction().X()<<endl;
                  cout<<"     for ph y,ydir :"<<ph.Position().Y()<<" "<<ph.Direction().Y()<<endl;
                  cout<<"     for ph z,zdir :"<<ph.Position().Z()<<" "<<ph.Direction().Z()<<endl;
                  cout<<"     path_length "<<path_length<<endl;
                }


                if (hit)
                {
                  if (&((*kSurf_coupled)->Reflectivity())) // Reflectivity defined
                  {
                    if (Verbosity()>=4) cout<<"     PndDrcOptVol::reflectivity2a clause"<<endl;
			  // check reflectivity
                    norm = (*kSurf_coupled)->Normal(ph.Position());
                    refl = Drc::ReflReflected;
                    refl = (*kSurf_coupled)->Reflectivity().Query(ph,norm);


                    if (refl == Drc::ReflAbsorbed)
                    {
                      if (Verbosity()>=4)
                        cout<<"     PndDrcOptVol::propagate: mirror absorbed"<<endl;
                      ph.SetFate(Drc::kPhotAbsorbed);
                      return;
                    }
                    if (refl == Drc::ReflTransmitted)
                    {
			      // do nothing
                    }
                    if (refl == Drc::ReflReflected)
                    {
                      ph.Reflect(norm);
                    }
                    if (refl == Drc::ReflRefracted)
                    {
			      // do nothing
                    }
                  } // end of reflectivity


		      // bring photon inside coupled volume
		      // to prevent infinite recursion of coupled surface hits.
		      // The factor 2 comes from comparisons with same kEps in
		      // surfaceHit.

                  PndDrcOptMatAbs* opt_mat = &((*kDev_coupled)->OptMaterial());
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

			  //               if( surf_closest->Name() == "slab_side1" && !iref )
			  //                 cout << "wrong" << endl;

                    if (Verbosity()>=4) cout<<" refract in new volume flag = "
                          <<iref<<endl;

                    if (iref) // refraction in new volume
                    {
                      if (Verbosity()>=4) cout<<"     go into new volume "
                            <<(*kDev_coupled)->Name()<<endl;
                      (*kDev_coupled)->Propagate(ph);
                    }
                    else
                    {
                      break; // coupled surface loop, since phot. is reflected.
                    }
                  }
                  else // screen or mirror...
                  {
			  // do not bring photons inside flat objects
			  // ph.setPosition(ph.position()+2*kEps*ph.direction());
                    (*kDev_coupled)->Propagate(ph);
                  }
                  if (ph.Fate() != Drc::kPhotFlying) break;
                }
              }
            }
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
