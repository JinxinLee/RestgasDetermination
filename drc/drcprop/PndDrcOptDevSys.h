/*! 
  \brief Class for representation of a system of optical devices.

  Optical devices can be combined (coupled) to a system. This system, 
  then can be also multiplicated by the copy constructor. A copy number
  helps to distingish between all generated systems. Since one can add a 
  system to another and also combine the surfaces of two different systems, which 
  were added to s system, one should make sure, that every single system gets a 
  different copy number when the names are the same.

  \anchor originating
  The notion "originating system" refers to the system in which the device was added.
  After adding of two system into one, this allows to distinguish between two devices.
*/

#ifndef PNDDRCOPTDEVSYS_H
#define PNDDRCOPTDEVSYS_H

#include <string>
#include <list>
#include <fstream>
using std::string;
using std::fstream;
using std::list;

#include "Math/Transform3D.h"
using ROOT::Math::Transform3D;

class PndDrcOptDev;
//class fstream;



class PndDrcOptDevSys
{
 public:
  //!                                             Constructor 
  PndDrcOptDevSys();                          
  //!                                             Destructor.
  virtual ~PndDrcOptDevSys();                 


  /*! \brief Copy constructor.
    \param d The object to copy.
  */
  PndDrcOptDevSys(const PndDrcOptDevSys& d);

  /*! \brief Assignment operator.
    \param d The object to assign.
  */
  PndDrcOptDevSys& operator=(const PndDrcOptDevSys& d);

  /*! \brief Virtual copy constructor.
    \return Pointer to new allocated memory.
  */
  virtual PndDrcOptDevSys* Clone() const;
  
  /*! \brief List accessor \return List of devices */
  const list<PndDrcOptDev*>& DeviceList() const     {return fListDev;}; 

  /*! \brief List accessor \return List of originating system*/
  const list<string>& ListSysOri()           const {return fListSysOri;}; 
  /*! \brief List accessor \return List of originating system copy*/
  const list<int>& ListSysOriCopy()          const {return fListSysOriCopy;}; 
  /*! \brief List accessor \return List of originating system  */
  const list<string>& ListSysOri1Coupling()  const {return fListSysOri1Coupling;};
  /*! \brief List accessor \return List of originating system2 for coupling  */
  const list<string>& ListSysOri2Coupling()  const {return fListSysOri2Coupling;}; 
  /*! \brief List accessor \return List of device 1 for coupling  */
  const list<string>& ListDev1Coupling()     const {return fListDev1Coupling;};
  /*! \brief List accessor \return List of device 2 for coupling */
  const list<string>& ListDev2Coupling()     const {return fListDev2Coupling;};
  /*! \brief List accessor \return List of device 1 for coupling  */
  const list<int>& ListDev1CopyCoupling()    const {return fListDev1CopyCoupling;};
  /*! \brief List accessor \return List of device 2 for coupling */
  const list<int>& ListDev2CopyCoupling()    const {return fListDev2CopyCoupling;};
  /*! \brief List accessor \return List of surface 1 for coupling */
  const list<string>& ListSurf1Coupling()    const {return fListSurf1Coupling;};
  /*! \brief List accessor \return List of surface 2 for coupling */
  const list<string>& ListSurf2Coupling()    const {return fListSurf2Coupling;}; 
  /*! \brief List accessor \return List of surface 1 for coupling */
  const list<int>& ListSurf1CopyCoupling()   const {return fListSurf1CopyCoupling;};
  /*! \brief List accessor \return List of surface 2 for coupling */
  const list<int>& ListSurf2CopyCoupling()   const {return fListSurf2CopyCoupling;}; 
  /*! \brief List accessor \return List of originating system copy 1 for coupling */
  const list<int>& ListSysOri1CopyCoupling() const {return fListSysOri1CopyCoupling;}; 
  /*! \brief List accessor \return List of originating system copy 2 for coupling */
  const list<int>& ListSysOri2CopyCoupling() const {return fListSysOri2CopyCoupling;};



  /*! \brief Set the name and copy number of the volume.

  If there are already added device systems with different names, those names will not be changed.
  

    \param name1 The name.
    \param icopy The copy number.
  */
  void SetNameCopyNumber(string name1, int icopy=0);
  
  /*! Name
    \return The name.
  */
  string Name() const {return fName;}
  /*! Copy number
    \return The copy number.
  */
  int CopyNumber() const {return fCopyNumber;}
  

  /*! \brief Long name of system
    \return The long name.
  */
  string LongName() const {return fLongName;};
  


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
  /*! \brief Add device.
    \param dev The device.
  */
  void AddDevice(const PndDrcOptDev& dev);

  /*! \brief Embed device.

  Use carefully...

  An embedded device is a device completely part of another device.
  This function creates the necessary surfaces of the mother and couples those
  to the surfaces of the daughter.

  If the volume to embed contains internal surfaces, make sure that those are
  marked as internal. \sa PndDrcSurfAbs::SetInternal() 

  Two surfaces of two embedded volumes are not allowed to 
  touch each other (zero distance problem).

    \param dev_daughter The daughter device to embed .
    \param dev_mother   The mother device to embed in.
  */
  void EmbedDevice(PndDrcOptDev& dev_daughter, PndDrcOptDev& dev_mother);

  /*! \brief Add whole device system
    \param sys The device system.
  */
  void AddDeviceSystem(const PndDrcOptDevSys& sys);
  

  /*! \brief Couple two devices such that photons can propagate from
    one to the other.
    \param dev1       The first  optical device to couple.
    \param dev2       The second optical device to couple.
    \param surf1      The surface of dev1 to couple. 
    \param surf2      The surface of dev2 to couple.
    \param dev_copy1  The copy number of the dev1 to couple. 
    \param dev_copy2  The copy number of the dev2 to couple. 
    \param surf_copy1 The copy number of the surf1 to couple. 
    \param surf_copy2 The copy number of the surf2 to couple. 
  */
  void CoupleDevice(string dev1,         string dev2,
		    string surf1,        string surf2,
		    int    dev_copy1=0,  int    dev_copy2=0,
		    int    surf_copy1=0, int    surf_copy2=0);

  /*! \brief Couple two devices such that photons can propagate from
    one to the other.

    This routine couples devices which are from the same optical device system.
    For coupling of devices from different systems use \sa CoupleDeviceSystems.

    \param sys1       The optical system for dev1 to couple.
    \param sys2       The optical system for dev2 to couple.
    \param dev1       The first  optical device to couple.
    \param dev2       The second optical device to couple.
    \param surf1      The surface of dev1 to couple. 
    \param surf2      The surface of dev2 to couple.
    \param sys_copy1  The copy number of the device system #1 to couple. 
    \param sys_copy2  The copy number of the device system #2 to couple. 
    \param dev_copy1  The copy number of the dev1 to couple. 
    \param dev_copy2  The copy number of the dev2 to couple. 
    \param surf_copy1 The copy number of the surf1 to couple. 
    \param surf_copy2 The copy number of the surf2 to couple. 
  */
  void CoupleDeviceSystem(string sys1,         string sys2,
			  string dev1,         string dev2,
			  string surf1,        string surf2,
			  int    sys_copy1 = 0, int    sys_copy2 = 0,
			  int    dev_copy1 = 0, int    dev_copy2 = 0,
			  int    surf_copy1= 0, int    surf_copy2= 0);

  /*! \brief Write volume coordinates to stream in root syntax.
    \param stream The stream.
  */
  void Print(fstream& stream) const;

  //! Print out pointers and names of volumes, surfaces...
  void Print() const;

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
  \sa PndDrcOptDev::SetFresnel()
  \sa PndDrcSurfAbs::SetFresnel()

  */
  void SetFresnel(bool flag);
  


  /*! \brief Add transformation

  The tranform from the root framework is a rotation followed by a shift 
  and is represented by a 4x4 matrix, rotation + shift vector.

  \param trans The transformation.

  */
  virtual void AddTransform(const Transform3D& trans);

 protected: 
  list<string>        fListSysOri;                //!< List of system origin for devices.
  list<int>           fListSysOriCopy;            //!< List of copy number of system origin for devices.
  int                 fCopyNumber;                //!< Copy number for "copies".   
  string              fName;                      //!< Name of volume.  
  string              fLongName;                  //!< Long unique name of system.
  int                 fVerbosity;                 //!< Verbosity from 0 to 5.
  mutable bool        fPhotonTrace;               //!< Flag for photon tracing.   
  mutable fstream*    fPhotonTraceStream;         //!< Stream for photon tracing.
  list<PndDrcOptDev*> fListDev;                   //!< List of devices.
  list<string>        fListSysOri1Coupling;       //!< List of originating systems for couplings.
  list<string>        fListSysOri2Coupling;       //!< List of originating systems for couplings.
  list<int>           fListSysOri1CopyCoupling;   //!< List of originating system copies for couplings.
  list<int>           fListSysOri2CopyCoupling;   //!< List of originating system copies for couplings.
  list<string>        fListDev1Coupling;          //!< List of devices for couplings.
  list<string>        fListDev2Coupling;          //!< List of devices for couplings.
  list<int>           fListDev1CopyCoupling;      //!< List of devices for couplings.
  list<int>           fListDev2CopyCoupling;      //!< List of devices for couplings.
  list<string>        fListSurf1Coupling;         //!< List of surfaces for couplings.
  list<string>        fListSurf2Coupling;         //!< List of surfaces for couplings.
  list<int>           fListSurf1CopyCoupling;     //!< List of surfaces for couplings.
  list<int>           fListSurf2CopyCoupling;     //!< List of surfaces for couplings.


 private:
  /*! \brief Auxiliary function for assignment operator and copy constructor..
    \param d The object to copy.
  */
  void Copy(const PndDrcOptDevSys& d);


  /*! \brief Add device

  to the system. This private member assigns to that device also the orinating system name and a 
  copy number. 

    \param dev The device.
    \param sys_ori The system name where the device si originating from
    \param sys_ori_copy The system copy number where the device is orinating from.
  */
  void AddDevice(const PndDrcOptDev& dev, string sys_ori, int sys_ori_copy);
  




};
#endif
