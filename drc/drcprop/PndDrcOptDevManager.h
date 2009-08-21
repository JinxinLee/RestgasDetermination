/*!
  \brief Class to manage devices.
  There is only one instance per application allowed.
  Access this instance with \sa Instance().
*/

#ifndef PNDDRCOPTDEVMANAGER_H
#define PNDDRCOPTDEVMANAGER_H


#include "TRandom3.h"
#include "PndDrcOptDev.h"
#include <map>
#include <list>
using std::map;
using std::list;


class PndDrcOptDevSys;
//class PndDrcOptDev;

class PndDrcOptDevManager
{
 public:
  //!                                             Empty constructor.
  PndDrcOptDevManager();

  /*! \brief Copy constructor.

  This function may never be called since the class is a single instanton.
  This function was implemented to satisfy the rule checker.
  \param d The object to copy.
  */
  PndDrcOptDevManager(const PndDrcOptDevManager& d);

  /*! \brief Assignment operator.
  This function may never be called since the class is a single instanton.
  This function was implemented to satisfy the rule checker.
    \param d The object to assign.
  */
  PndDrcOptDevManager& operator=(const PndDrcOptDevManager& d);

  //!                                             Destructor.
  virtual ~PndDrcOptDevManager();

  /*! \brief Instance of manager.
    \return Pointer to manager.
  */
  static PndDrcOptDevManager* Instance() {return fgInstance;};

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

  /*! \brief Register an optical device system to the manager.

    \param sys The system to register.
  */
  void AddDeviceSystem(const PndDrcOptDevSys& sys);

  /* \brief Couple two devices such that photons can propagate from
    one to the other.
    \param dev1  The first optical device to couple.
    \param dev2  The first optical device to couple.
    \param surf1 The surface of dev1 to couple.
    \param surf2 The surface of dev2 to couple.
    \param copy1 Optional copy number if system exists several times.
    \param copy2 Optional copy number if system exists several times.
  */
  //void coupleDevice(string dev1,    string dev2,
  //	    string surf1,   string surf2,
  //	    int    copy1=0, int    copy2=0);



  /*! \brief Pointer to device.
    \param vol The device string.
    \param sys Name of originating device system.
    \param vol_copy Device copy number.
    \param sys_copy Copy number of originating device system.
    \return  Pointer to device.
  */
  PndDrcOptDev* Device(string vol,string sys="unnamed", int vol_copy=0, int sys_copy=0);

  /*! \brief Write volume coordinates to stream in root syntax.
    \param stream The stream.
  */
  void Print(fstream& stream) const;

  /*! \brief Device hit by particle.

  If the particle intersect a volume there should be 2n intersections
  with n>0. A box has 2 intersection points, wheras complicated volumes
  can have more. A tube with a thickness has 2 intersection pairs.
  The intersection pairs are normally not identical. If a flat volume
  with two surfaces (mirror) is hit, the intersection points are identical.

  If the trajectory hits two volumes, adjacent or behind each other, the
  lists contain also different hit volume names and copies.


  This routine fails when surfaces, e.g. mirrors are embedded in a volume.
  Do not hit such surfaces by defining a maximum particle range. An entrance-exit pair
  is not added to the lists, when the exit is beyond the range.

  It works with embedded volumes.


    \param pos      Particles origing.
    \param dir      Particles direction.
    \param p_in     List of entrance vectors in volume.
    \param p_out    List of exit     vectors in volume.
    \param vol_name List of names of the hit volumes.
    \param sys_name List of names of the hit \ref originating "originating" system.
    \param vol_copy List of the copy number of hit volumes.
    \param sys_copy List of the copy number of \ref originating "originating" system..
    \param range    Max. particle range.
    \return         Flag if valid intersections found.
   */
  bool Intersect(const XYZPoint&  pos,
		 const XYZVector& dir,
		 list<XYZPoint>&  p_in,
		 list<XYZPoint>&  p_out,
		 list<string>&    vol_name,
		 list<string>&    sys_name,
		 list<int>&       vol_copy,
		 list<int>&       sys_copy,
		 double           range=1.0e16) const;



  /*! \brief Generate cerenkov radiation

  and store photons in internal photon list. It is assumed that the particle has
  the charge Z=1.

  Some quantities depending on lambda1 and lambda2 are calculated at first call.
  Therefore, do not change lambda1 and lamda2 at successive calls.

  \param pos Particle start position [mm].
  \param dir Particle direction.
  \param beta Particle velocity [c].
  \param n Number of photons to produce, n=0 for automatic (recommended).
  \param range Max range of particle. \sa Intersect()
  \param nu1 Smallest wavelength.
  \param nu2 Largest wavelength.
  \param refl_limit Maximum number of reflections.

  \return Flag if internal photon list has entries.
  */
  bool Cerenkov(const XYZPoint&  pos,
		const XYZVector& dir,
		double           beta,
		int              n=0,
		double           range=1.0e16,
		double           nu1=300,
		double           nu2=700,
		int              refl_limit=1000,
		int              particleIDnumber=0);


  /*! \brief Generate cerenkov radiation

  and store photons in internal photon list. It is assumed that the particle has
  the charge Z=1. This routine is inteded to be used with eg. Geant4, where the
  tracker gives you the sensitive voume name, its copy number and the coordinates
  where the particle is.

  Some quantities depending on lambda1 and lambda2 are calculated at first call.
  Therefore, do not change lambda1 and lamda2 at successive calls.

  \param vol_name Name of hit volume.
  \param sys_name Name of \ref originating "originating" system of the volume.
  \param vol_copy Copy number of volume.
  \param sys_copy Copy number of the \ref originating "originating" system.
  \param r1 Particle start point [mm].
  \param r2 Particle end point [mm].
  \param beta Particle velocity [c].
  \param n Number of photons to produce, n=0 for automatic (recommended).
  \param nu1 Smallest wavelength.
  \param nu2 Largest wavelength.
  \param refl_limit Maximum number of reflections.

  \return Flag if internal photon list has entries.
  */
  bool Cerenkov(const string&   vol_name,
		const string&   sys_name,
		int             vol_copy,
		int             sys_copy,
		const XYZPoint& r1,
		const XYZPoint& r2,
		double          beta,
		int             n=0,
		double          nu1=300,
		double          nu2=700,
		int             refl_limit=1000,
        int             particleIDnumber=0);



  /*! \brief List of generated Cerenkov photons.

  After the generation of Cerenkov photons by the cerenkov routine, this function
  returns the list of th photons.

  \return List.
  */
  list<PndDrcPhoton>& PhotonList(){return fListPhoton;};

  //! Clear photon list
  void ClearPhotonList(){fListPhoton.clear();};


  /*! \brief Set listist of photons.

  This is mainly for debugging. You have to set at least the position and the
  direction of the photon.
  \param photon_list List of photons.
  \param vol_name The name of the volume where the photon is.
  \param sys_name The name of the \ref originating "originating" system.
  \param vol_copy Copy number of the volume where the photon is.
  \param sys_copy Copy number of the \ref originating "originating" system where the photon is.
  */
  //void setPhotonList(list<PndDrcPhoton>& list, string vol_name, int copy=0);
  void SetPhotonList(list<PndDrcPhoton>& photon_list,
		     string vol_name,   string sys_name="unnamed",
		     int    vol_copy=0, int sys_copy=0);

  void Propagate(); //!< Propagate the internal photon list.

  /*! \brief Add transformation

  The tranform from the root framework is a rotation followed by a shift
  and is represented by a 4x4 matrix, rotation + shift vector.

  \param trans The transformation.

  */
  void AddTransform(const Transform3D& trans);

  //! Print out boxes, surfaces and their pointers.
  void Print();

 private:
  static PndDrcOptDevManager* fgInstance;         //!< The instance.
  int                         fVerbosity;         //!< Verbosity from 0 to 5.
  TRandom3                    fRan;               //!< Random generator.
  map<string,PndDrcOptDev*>   fMapDev;            //!< Map of device pointers.
  list<PndDrcOptDevSys*>      fListDevSys;        //!< List of optical devices.
  list<PndDrcPhoton>          fListPhoton;        //!< List of photons.

  /*! \brief Copy function fir assignment operator and copy constructor.
    \param m The manager to copy.
  */
  void Copy(const PndDrcOptDevManager& m);




};
#endif
