/*! 
  \brief Class for surface representation. 


  The surface is defined by a position dependent normal vector and boundaries. 
  It might be coupled to a surface of another volume. 

  There are internal and external surfaces. Internal surfaces should be marked a such by
  \sa SetInternal() to prevent the coupling to a mother volume surface, when the device 
  containing this surface is embedded as daughter into the mother device. This refers in 
  particular to mirrors and pad planes.
 
*/

#ifndef PNDDRCSURFABS_H
#define PNDDRCSURFABS_H


#include <list>
#include <string>
#include <fstream>
using std::list;
using std::string;
using std::fstream;
#include "Math/Vector3D.h"
using ROOT::Math::XYZVector;
#include "Math/Point3D.h"
using ROOT::Math::XYZPoint;
#include "Math/Transform3D.h"
using ROOT::Math::Transform3D;

//class XYZVector;
//class XYZPoint;

class PndDrcOptDev;
class PndDrcOptReflAbs;
class PndDrcPhoton;
//class Transform3D;

class PndDrcSurfAbs
{
 public:

  //!                                        Empty constructor.
  PndDrcSurfAbs();                               

  /*! \brief Copy constructor.
    \param s The object to copy.
  */
  PndDrcSurfAbs(const PndDrcSurfAbs& s);

  /*! \brief Assignment operator.
    \param s The object to assign.
  */
  PndDrcSurfAbs& operator=(const PndDrcSurfAbs& s);

  //! Destructor.
  virtual ~PndDrcSurfAbs();            

  /*! \brief Virtual copy constructor.
    \return Pointer to new allocated memory.
  */
  virtual PndDrcSurfAbs* Clone() const=0;

  /*! \brief Set the copy number for this object.

  This serves for the purpos to distinguisch between several objects
  generated with a copy constructor.
  \param i The copy number.
  */
  void SetCopyNumber(int i){fCopyNumber=i;};

  /*! \brief Copy number for this object.
  \return The copy number.
  \sa SetCopyNumber
  */
  int CopyNumber() const {return fCopyNumber;};

  /*! \brief Set surface as internal surface

  to prevent coupling to other surfaces when daughter volume with this surface 
  is embedded in a mother volume in \sa PndDrcOptDevSys::EmbedDevice().

  \param flag The flag
  */
  void SetInternal(bool flag=true) {fInternal=flag;}
  
  /*! \brief Flag if surface is an internal surface

  \sa SetInternal()

  \return The flag
  */
  bool Internal() const {return fInternal;}


  /*! \brief Flag for coupling with other devices.
    \return Flag
  */
  bool Coupled() const {return fCoupledFlag;};


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

  /*! \brief Coupling to different surface.

  Tell the surface to which device and surface it is optical coupled. Photons can then
  propagate through this surface to different optical volume.

  \param dev The coupled device.
  \param surf The coupled surface.
  */
  void SetCoupled(PndDrcOptDev*  dev,
		  PndDrcSurfAbs* surf);

  /*! \brief The coupled device list.
    \return The device.
    \sa SetCoupled()
    \sa CoupledSurfaceList()
  */
  const list<PndDrcOptDev*>& CoupledDeviceList()  const {return fCoupledDeviceList;};

  /*! \brief The coupled surface list.
    \return The Surface.
    \sa SetCoupled()
    \sa CoupledDeviceList()
  */
  const list<PndDrcSurfAbs*>& CoupledSurfaceList() const {return fCoupledSurfaceList;};


  /*! \brief Set the name of the volume.
    \param name The name.
  */
  void SetName(string name){fName=name;};
  /*! \brief The name of the volume.
    \return The name.
  */
  string Name() const {return fName;};

  /*! Set reflectivity
    \param refl the reflectivity.
  */
  void SetReflectivity(const PndDrcOptReflAbs& refl);

  /*! Clear reflectivity
  */
  void ClearReflectivity();


  /*! \brief Set the PhotoFate to PhotMeasured 

  after successfully running the surfaceHit function.
  \param flag The flag.
  */
  void SetPixel(bool flag=true){fPixel=flag;};

  /*! Reflectivity
    \return Reflectivity.
  */
  PndDrcOptReflAbs& Reflectivity() { return *fReflectivity; };

  /*! \brief Enable Fresnel reflections at surface 
    
  \param flag The flag.
  \sa PndDrcOptDevSys::SetFresnel()
  \sa PndDrcOptDev::SetFresnel()
  */
  void SetFresnel(bool flag=true){fFresnel=flag;}
  

  /*! \brief Fresnel reflections at surface 
    
  \return Flag if Fresnel reflections are enabled.
  */
  bool Fresnel(){return fFresnel;}
  

  /*! \brief Normal vector

  The normal vector of the surface points outside.
  \param point The point where the normal vector is taken;
  \return Normal vector.
  */
  virtual XYZVector Normal(const XYZPoint& point) const = 0; 

  /*! \brief Intersection vector

  The intersection of a photon with the surface. If the photon has hit
  the area, pos_new is the position on the area, the direction is unchanged.
  The path from the current position to the hit position is returned by path_length.
  If the surface is a screen, the photons fate is set to PhotMeasured.
  \sa kPhotonFate() SetScreen().

  \param ph          The photon. 
  \param pos_new     Hit position on surface (return value).
  \param path_length Distance from photon position to hit position (return value).
  \return            If the photon has hit the area.
  */
  virtual bool SurfaceHit(PndDrcPhoton& ph, 
			  XYZPoint&  pos_new, 
			  double&    path_length) const = 0;


  //! Write coordinates to stdout.
  virtual void Print() const = 0;

  /*! \brief Write surface coordinates to stream in root syntax.

    \param stream The stream.
    \return The stream.
  */
  virtual void Print(fstream& stream) const = 0;

  /*! \brief Set print color for root objects.
    \param col The color
    \verbatim
    17  grey
    50  dark red
    2  red
    42  orange
    5  yellow
    3  green
    7  cyan
    4  blue
    9  indigo
    39  violet
    \endverbatim
  */
  void SetPrintColor(int col){fPrintColor=col;};

  /* \brief The print color for root.
    \return The color number
    \sa SetPrintColor
  */
  //int printColor() const {return fPrintColor;};


  /*! \brief Add transformation

  The tranform from the root framework is a rotation followed by a shift 
  and is represented by a 4x4 matrix, rotation + shift vector.
  Consecutive transforms are multiplied from the left.

  \param trans The transformation.

  */
  virtual void AddTransform(const Transform3D& trans) = 0;


  /*! \brief Flag if surface is flat.
    \return Flag.
  */
  virtual bool IsFlat() const=0;

 protected:
  int                  fCopyNumber;         //!< Copy number for "copies".       
  bool                 fCoupledFlag;        //!< Coupling flag.          
  list<PndDrcSurfAbs*> fCoupledSurfaceList; //!< Coupled surface.
  list<PndDrcOptDev*>  fCoupledDeviceList;  //!< Coupled device.   
  string               fName;               //!< Name of surface.            
  int                  fVerbosity;          //!< Verbosity from 0 to 5.          
  int                  fPrintColor;         //!< Print color for root.         
  PndDrcOptReflAbs*    fReflectivity;       //!< Pointer to reflectivity. 
  bool                 fPixel;              //!< Flag if surface measures photon
  bool                 fInternal;           //!< Flag for internal surfaces.
  bool                 fFresnel;            //!< Flag for Fresnel reflections. 


 private:
  /*! \brief Auxiliary function for assignment operator and copy constructor..
    \param s The object to copy.
  */
  void Copy(const PndDrcSurfAbs& s);         

};

#endif
