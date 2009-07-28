/*!
  \brief Class for optical material representation.

  It is assumed that for all DIRC option radiator material is used
  which is not birefingent.

*/

#ifndef PNDDRCOPTMATABS_H
#define PNDDRCOPTMATABS_H


#include <string>
using std::string;

class PndDrcOptMatAbs
{
 public:

  //!                                             Empty constructor.
  PndDrcOptMatAbs();


  /*! \brief Virtual copy constructor.
    \return Pointer to new allocated memory.
  */
  virtual PndDrcOptMatAbs* Clone() const = 0;

  /*! Copy constructor.
    /param Object to copy.
  */
  PndDrcOptMatAbs(const PndDrcOptMatAbs& mat);

  /*! Assignment operator.
    /param Object to assign.
  */
  PndDrcOptMatAbs& operator=(const PndDrcOptMatAbs& mat);


  //! Destructor.
  virtual ~PndDrcOptMatAbs();



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


  /*! \brief Set the name of the volume.
    \param name The name.
  */
  void SetName(string name){fName=name;};

  /*! \brief The name of the volume.
    \return The name.
  */
  string Name() const {return fName;};


  /*! \brief Refraction index.
    \param lambda wavelength [nm]
  */
  virtual double RefIndex(const double lambda) const = 0;

  /*!\brief  Derivative of refraction index with respect to wavelength.
    \param lambda wavelength [nm]
  */
  virtual double RefIndexDeriv(const double lambda) const = 0;

  /*!\brief  Derivative of extinction with respect to wavelength.
    \param lambda wavelength [nm]
  */
  virtual double Extinction(const double lambda) const = 0;


  /*! \brief Absorption flag

  for pathlength. When return value is true, the pathlength contains the value
  where absorption has occured.

  \param lambda The wavelength in [nm].
  \param length The pathlength in [mm].
  \return Absorption flag.
  */
  virtual bool AbsorptionFlag(double lambda, double length) const = 0;

 protected:

  string            fName;                        //!< Name of surface.
  int               fVerbosity;                   //!< Verbosity from 0 to 5.

};

#endif
