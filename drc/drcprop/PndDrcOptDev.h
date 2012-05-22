/** \class PndDrcOptDev
  \brief Class for optical device representation. 

  An device can be a optical volume, a mirror, or eg. a measuring plane.
  consistng out of surfaces, which can be coupled.
**/



#ifndef PNDDRCOPTDEV_H
#define PNDDRCOPTDEV_H


#include <list>
#include <string>
using std::list;
using std::string;

#include "PndDrcSurfAbs.h"
#include "PndDrcPhoton.h"
#include "Math/Transform3D.h"
using ROOT::Math::Transform3D;
 
//#include "Math/Vector3D.h"
//using ROOT::Math::XYZVector;
//#include "Math/Point3D.h"
//using ROOT::Math::XYZPoint;

//class PndDrcSurfAbs;
class PndDrcOptMatAbs;

class PndDrcOptDev
{
 public:
  //!                                              Constructor
  PndDrcOptDev();                          
  //!                                              Destructor.
  virtual ~PndDrcOptDev();                 

  /*! \brief Virtual copy constructor.
    \return Pointer to new allocated memory.
  */
  virtual PndDrcOptDev* Clone() const=0;

  /*! \brief Copy constructor.
    \param d The object to copy.
  */
  PndDrcOptDev(const PndDrcOptDev& d);

  /*! \brief Assignment operator.
    \param d The object to assign.
  */
  PndDrcOptDev& operator=(const PndDrcOptDev& d);

  //! Print out all surface names and pointers.
  void Print();


  /*! \brief Set the copy number for this object.

  This serves for the purpos to distinguisch between several objects
  generated with a copy constructor.
  \param i The copy number.
  */
  void SetCopyNumber(int i);

  /*! \brief Copy number for this object.
  \return The copy number.
  \sa SetCopyNumber
  */
  int CopyNumber() const {return fCopyNumber;};

  /*! \brief Set the name of the volume.
    \param name The name.
  */
  void SetName(string name){fName=name;};
  /*! \brief The name of the volume.
    \return The name.
  */
  string Name() const {return fName;};



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

  /*! \brief Add surface.

    \param surf The surface.

  */
  virtual void AddSurface(const PndDrcSurfAbs& surf);

  /*! \brief List of surfaces 

  the device is composed of.
  \return List.
  */
  list<PndDrcSurfAbs*>& SurfaceList(){return fListSurf;};

  /*! \brief Propagate one photon.
  \param ph The photon.
  */
  virtual void Propagate(PndDrcPhoton& ph) = 0;

  /*! \brief Write volume coordinates to stream in root syntax.
    \param stream The stream.
  */
  virtual void Print(fstream& stream) const;

  /*! \brief Optical material.
    \return Pointer to material.
  */
  virtual PndDrcOptMatAbs& OptMaterial() const;


  /*! \brief Add transformation

  The tranform from the root framework is a rotation followed by a shift 
  and is represented by a 4x4 matrix, rotation + shift vector.

  \param trans The transformation.

  */
  virtual void AddTransform(const Transform3D& trans);

  /*! \brief Flag if object is radiator.
    \return The flag.
  */
  virtual bool Radiator() const = 0;    

  /*! \brief Set print color for root objects.

  All subobjects of this object will get the chosen color
  for printing with root.

  \param col The chosen color.
  \sa PndDrcSurfAbs::SetPrintColor()
  */
  void SetPrintColor(int col);

  /*! \brief Enable Fresnel reflections at all surfaces

  Surfaces which are added later are not affected by this command.
    
  \param flag The flag.
  \sa PndDrcOptDevSys::SetFresnel()
  \sa PndDrcSurfAbs::SetFresnel()
  */
  void SetFresnel(bool flag);

  friend class PndDrcOptDevManager;

  protected: 
  int               fCopyNumber;                  //!<  Copy number for "copies".
  string            fName;                        //!<   Name of volume.   
  int               fVerbosity;                   //!<    Verbosity from 0 to 5.
  mutable bool      fPhotonTrace;                 //!<    Flag for photon tracing.
  mutable fstream*  fPhotonTraceStream;           //!< Stream for photon tracing.
  list<PndDrcSurfAbs*> fListSurf;                 //!<  List of surface pointers.  
  XYZVector         fKx;                          //!<  Direction vector of volumina.
  XYZVector         fKy;                          //!<  Direction vector of volumina.
  XYZVector         fKz;                          //!<  Direction vector of volumina.
  


 private:
  /*! \brief Auxiliary function for assignment operator and copy constructor..
    \param d The object to copy.
  */
  void Copy(const PndDrcOptDev& d);


  /*! \brief X-direction vector of volume.
 
  \return The vector.
  */
  const XYZVector DirectionX(){return fKx;};
  /*! \brief Y-direction vector of volume.
 
  \return The vector.
  */
  const XYZVector DirectionY(){return fKy;};
  /*! \brief Z-direction vector of volume.
 
  \return The vector.
  */
  const XYZVector DirectionZ(){return fKz;};
  
  
    



};
#endif
