/*!

  \brief Representation of a photon.


*/

#ifndef PNDDRCPHOTON_H
#define PNDDRCPHOTON_H

#include "Math/Point3D.h"
using ROOT::Math::XYZPoint;

#include "Math/Vector3D.h"
using ROOT::Math::XYZVector;

#include <fstream>
using std::fstream;

#include <list>
using std::list;

#include "TRandom3.h"

namespace Drc
{
  /*! \enum PndDrc::kPhotonFate
    An enum holding the fate of a photon. */
  enum kPhotonFate
    {
      kPhotFlying,                                //!< Photon is still propagating.
      kPhotMeasured,                              //!< Photon has hit measuring device.
      kPhotLost,                                  //!< Photon got lost outside.
      kPhotAbsorbed                               //!< Photon got lost inside.
    };
}

class PndDrcOptDev;


/*!
  \brief Representation of a photon.
*/
class PndDrcPhoton
{
 public:

  //! Empty constructor.
  PndDrcPhoton();
  /*! \brief Copy constructor
    \param ph The photon
  */
  PndDrcPhoton(const PndDrcPhoton& ph);

  /*! \brief Assignment operator.
    \param ph The object to assign.
  */
  PndDrcPhoton& operator=(const PndDrcPhoton& ph);

  //! Destructor.
  virtual ~PndDrcPhoton(){};

  




  /*! \brief Verbosity

  Set the verbosity. Range is 0 to 5.
  \verbatim
  0=quiet,
  1=constructors,destructors,
  2=member functions,
  3=functionality
  4=photons
  5=everything
  \endverbatim
  \param level The verbosity level
  */
  void SetVerbosity(int level){fVerbosity=level;};

  /*! \brief Verbosity.
    \return Verbosity
    \sa SetVerbosity()
  */
  const int Verbosity() const {return fVerbosity;};


  /*! \brief Set fate of photon
    \param fate The fate
    \sa PndDrc::kPhotonFate
  */
  void SetFate(Drc::kPhotonFate fate);

  /*! \brief Fate of photon
    \return The fate
    \sa PndDrc::kPhotonFate
  */
  const Drc::kPhotonFate Fate() const {return fFate;};


  /*! \brief Particle ID number
    \return Particle ID number.
  */
  int ParticleIDnumber() const {return fParticleIDnumber;};

  /*! \brief Set associated particle ID number
    \param particleIDnumber Particle ID number.
  */
  void SetParticleIDnumber(int particleIDnumber) {fParticleIDnumber=particleIDnumber;};


  /*! \brief Reflections
    \return Reflections.
  */
  int Reflections() const {return fReflections;};

  /*! \brief Reflection limit

  in order to kill infinite number of reflections eg. in
  \sa  PndDrcOptVol::Propagate()

  \return The limit
  */
  int ReflectionLimit() const {return fReflectionLimit;};

  /*! \brief Set reflection limit

  in order to kill infinite number of reflections eg. in
  \sa  PndDrcOptVol::Propagate()

  \param limit The limit
  */
  void SetReflectionLimit(int limit) {fReflectionLimit=limit;};


  /*! \brief Position
    \return Position.
  */
  XYZPoint Position() const {return fPosition;};

  /*! \brief Set position used by propagating routines.

  The position is also set in a 
  \sa  PositionXlist(),
  \sa  PositionYlist(),
  \sa  PositionZlist()

  If you would like to set simply the postion of a photon use \sa SetPosition()
  
  \param pos Position.
  */
  void SetPosition1(const XYZPoint& pos);


  /*! \brief Set position
     \param pos Position
  */
  void SetPosition(const XYZPoint& pos){fPosition=pos;};
  

  /*! \brief Surface list
    \param surf Surface pointer to set.
  */
  void SetSurface1(const PndDrcSurfAbs* surf);
  
  
  /*! \brief Position X list
    \return Position X list.
  */
  list<double>& PositionXlist(){return fPositionXlist;};

  /*! \brief Position Y list
    \return Position Y list.
  */
  list<double>& PositionYlist(){return fPositionYlist;};

  /*! \brief Position Z list
    \return Position Z list.
  */
  list<double>& PositionZlist(){return fPositionZlist;};


  /*! \brief Surface list
    \return Surface list.
  */
  list<const PndDrcSurfAbs*>& SurfaceList() {return fSurfaceList;};
  
  
  /*! \brief Direction
    \return Normalized direction.
  */
  XYZVector Direction() const {return fDirection;};

  /*! \brief Set normalized direction
    \param dir Direction.
  */
  void SetDirection(const XYZVector& dir) {fDirection=dir.Unit();};

  /*! \brief Origin direction

  A device gets a directional coordinate system a creation.   The three directional vectors are
  (1,0,0) (0,1,0) (0,0,1). These directions change with each device transformation. The generated photons
  contain the original direction, a vector containing three direction cosines from the scalar product of
  the photons direction with the three direction vectors of the volume.

  \image html origin_direction.png


    \return Normalized origin direction.
   */
  XYZVector OriginDirection() const {return fOriginDirection;};


  /*! \brief Cherenkov angle
  \return Cherenkov angle.
  */
  double ThetaC() const {return fThetaC;};


  /*! \brief Cherenkov (polar) angle
  \return Cherenkov (polar) angle
  */
  double PhiC() const {return fPhiC;};


 /*! \brief Set wavelength
  \param lambda Wavelength in nm.
  */
  void SetWavelength(double lambda){fLambda=lambda;};

  /*! \brief Wavelength
    \return Wavelength in nm.
  */
  double Wavelength() const {return fLambda;};


  /*! \brief Device where photon is
    \return Device.
  */
  PndDrcOptDev* Device() const {return fDev;};
 /*! \brief Set device where photon is
  \param dev The device.
  */
  void SetDevice(PndDrcOptDev* dev){fDev=dev;};


  /*! \brief Time of flight for photon
    \return Time of flight in [ns].
  */
  double Time() const {return fTime;};


  /*! \brief Handles refraction and reflection of photon.
  After this routine, the photons direction, position and fate is set.
  \param normal The normal vector of the surface of the volume the photon is in.
  \param n_in The refractive index of the current medium
  \param ex_in The extinction coefficient of the current medium
  \param n_out The refractive index of or the next volume or the vacuum outside.
  \param ex_out The extinction coefficient of or the next volume or the vacuum outside.
  \param fresnelFlag Fresnel reflection.
  \param diffuseProb Diffused reflection probability.
  \return True if refraction occured, false if reflection occured.
  */
  bool Refract(XYZVector normal,
	       double n_in,
	       double ex_in,
	       bool fresnelFlag=true,
	       double n_out=1.0,
	       double ex_out=0.0,
	       double diffuseProb = 0);

  /*! \brief Reflect the photon
    \param normal The normal vector of the surface.
    \sa Refract()
  */
  void Reflect(const XYZVector& normal);



  /*! \brief Write photon coordinates to stream in root syntax.
    \param stream The stream.
  */
  void Print(fstream& stream) const;

  //! Write photon properties to cout.
  void Print() const;

  /*! \brief Color number for usage in root
    \param lambda The wavelength in nm.
    \return Color number.
  */
  int ColorNumber(double lambda) const;


  /*! \brief Max. number of list entries.
    \sa  SurfaceList()
    \sa  PositionXlist()
    \sa  PositionYlist()
    \sa  PositionZlist()

    \return Max number of list entries
  */
  int ListLimit() const {return fListLimit;};

  /*! \brief Set max. number of list entries.
    \sa  SurfaceList()
    \sa  PositionXlist()
    \sa  PositionYlist()
    \sa  PositionZlist()
    \sa  PrintFlag()

    \pa lim Max number of list entries
  */
  void SetListLimit(unsigned int lim) {fListLimit=lim;};

  /*! \brief Print flag of photon to write its way to the data stream
    \sa  Print(fstream& stream)
    \return print flag.
  */
  bool PrintFlag() const {return fPrintFlag;};

  /*! \brief Set print flag of photon to write its way to the data stream. Also the
    generation of lists is enabled or disabled.
    \sa  Print(fstream& stream)
    \param flg Print flag.
  */
  void SetPrintFlag(bool flg){fPrintFlag=flg;};



  friend class PndDrcOptDevManager;

  
 private:
  int                  fParticleIDnumber;              //!< Associated particle
  double               fThetaC;                        //!< Cherenkov angle
  double               fPhiC;                          //!< Cherenkov (azimuthal) angle
  double               fLambda;                        //!< Wavelength in nm.
  XYZPoint             fPosition;                      //!< Actual position of photon.
  XYZPoint             fPositionOld;                   //!< Old position of photon.
  list<double>         fPositionXlist;                 //!< List of position X
  list<double>         fPositionYlist;                 //!< List of position Y
  list<double>         fPositionZlist;                 //!< List of position Z
  list<const PndDrcSurfAbs*> fSurfaceList;             //!< List of hit surface pointers.
  XYZVector            fDirection;                     //!< Normalized direction of photon.
  XYZVector            fOriginDirection;               //!< Normalized origin direction of photon.
  Drc::kPhotonFate     fFate;                          //!< The fate of the photon.
  int                  fReflections;                   //!< Number of suffered reflections.
  int                  fVerbosity;                     //!< Verbosity level 0-5.
  PndDrcOptDev*        fDev;                           //!< Pointer to device where photon is.
  double               fTime;                          //!< Time of flight.
  int                  fReflectionLimit;               //!< Reflection limit.
  bool                 fPrintFlag;                     //!< Print flag, by default true
  unsigned int         fListLimit;                     //!< Max number of list entries. 


  /*! \brief Copy function for assigment and copy operator.
    \param ph The photon
  */
  void Copy(const PndDrcPhoton& ph);

  /*! \brief Fresnel reflection
    \param normal The normal vector of the surface.
    \param n_in The refractive index of the current medium
    \param ex_in The extinction coefficient of the current medium
    \param n_out The refractive index of or the next volume or the vacuum outside.
    \param ex_out The extinction coefficient of or the next volume or the vacuum outside.
  */
  bool Fresnel(XYZVector normal, double n_in, double ex_in, double n_out=1.0, double ex_out=0.0);

  /*! \brief Reflect diffusely the photon
    \param normal The normal vector of the surface.
  */
  void Diffuse(const XYZVector& normal);

  /*! \brief Set Cherenkov angle
  \param thetac Cherenkov angle.
  */
  void SetThetaC(double thetac){fThetaC=thetac;};

  /*! \brief Set Cherenkov (polar) angle
  \param phic Cherenkov (polar) angle.
  */
  void SetPhiC(double phic){fPhiC=phic;};

  /*! \brief Set normalized origin direction (at photon production)
  \param org Origin direction.
  */
  void SetOriginDirection(const XYZVector& org) {fOriginDirection=org.Unit();};


 /*! \brief Set time of photon
  \param tim The time.
  */
  void SetTime(double tim){fTime=tim;};



};
#endif
