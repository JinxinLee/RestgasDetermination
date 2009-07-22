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

#include "TRandom.h"

//! A namespace for encapsulation.
namespace Drc
{
  /*! \enum PndDrc::PhotonFate
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
  void SetFate(Drc::kPhotonFate fate) {fFate=fate;};
  /*! \brief Fate of photon
    \return The fate
    \sa PndDrc::kPhotonFate
  */
  const Drc::kPhotonFate Fate() const {return fFate;};

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
  /*! \brief Set position
    \param pos Position.
  */
  void SetPosition(const XYZPoint& pos);

  /*! \brief Direction
    \return Normalized direction.
  */
  XYZVector Direction() const {return fDirection;};
  /*! \brief Set normalized direction
    \param dir Direction.
  */
  void SetDirection(const XYZVector& dir) {fDirection=dir.Unit();};

  /*! \brief Wavelength
    \return Wavelength in nm.
  */
  double Wavelength() const {return fLambda;};
 /*! \brief Set wavelength
  \param lambda Wavelength in nm.
  */
  void SetWavelength(double lambda){fLambda=lambda;};

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
 /*! \brief Set time of photon
  \param tim The time.
  */
  void SetTime(double tim){fTime=tim;};

  /*! \brief Handles refraction and reflection of photon.

  After this routine, the photons direction, position and fate is set.
  \param normal The normal vector of the surface of the volume the photon is in.
  \param n_inside The refractive index of the current medium
  \param ex_inside The extinction coefficient of the current medium
  \param n_outside The refractive index of or the next volume or the vacuum outside.
  \param ex_outside The extinction coefficient of or the next volume or the vacuum outside.
  \param fresnel Fresnel reflection.
  \param diffuse Diffused reflection probability. 
  \return True if refraction occured, false if reflection occured.
  */
  bool Refract(XYZVector normal, double n_in, double ex_in, double n_out=1.0, double ex_out=0.0, bool fresnelFlag=true, double diffuseProb = 0);

  /*! \brief Reflect the photon
    \param normal The normal vector of the surface.
    \sa Refract()
  */
  void Reflect(const XYZVector& normal);
             
  void Diffuse(const XYZVector& normal);           
             
  bool Fresnel(XYZVector normal, double n_in, double ex_in, double n_out=1.0, double ex_out=0.0);


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


  /*! \brief Print flag of photon to write its way to the data stream
    \sa  Print(fstream& stream)
    \return print flag.
  */
  bool PrintFlag() const {return fPrintFlag;};
 /*! \brief Set print flag of photon to write its way to the data stream
    \sa  Print(fstream& stream) 
  \param flg Print flag.
  */
  void SetPrintFlag(bool flg){fPrintFlag=flg;};
  

 private:
  double           fLambda;                        //!< Wavelength in nm.           
  XYZPoint         fPosition;                      //!< Actual position of photon.          
  XYZPoint         fPositionOld;                   //!< Old position of photon.      
  XYZVector        fDirection;                     //!< Normalized direction of photon.       
  Drc::kPhotonFate fFate;                          //!< The fate of the photon.              
  int              fReflections;                   //!< Number of suffered reflections.     
  int              fVerbosity;                     //!< Verbosity level 0-5.         
  PndDrcOptDev*    fDev;                           //!< Pointer to device where photon is.              
  double           fTime;                          //!< Time of flight.             
  int              fReflectionLimit;               //!< Reflection limit.
  bool             fPrintFlag;                     //!< Print flag, by default true


  /*! \brief Copy function for assigment and copy operator.
    \param ph The photon
  */
  void Copy(const PndDrcPhoton& ph);
};
#endif
