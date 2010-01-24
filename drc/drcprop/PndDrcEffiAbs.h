/*!
  \brief Abstract base class for photo kathode efficiency

  The efficiciency is a function of the photon wavelength 

*/

#ifndef PNDDRCEFFIABS_H
#define PNDDRCEFFIABS_H


#include <string>
using std::string;

class PndDrcEffiAbs
{
 public:

  //!                                             Empty constructor.
  PndDrcEffiAbs();


  /*! \brief Virtual copy constructor.
    \return Pointer to new allocated memory.
  */
  virtual PndDrcEffiAbs* Clone() const = 0;

  /*! Copy constructor.
    /param Object to copy.
  */
  PndDrcEffiAbs(const PndDrcEffiAbs& mat);

  /*! Assignment operator.
    /param Object to assign.
  */
  PndDrcEffiAbs& operator=(const PndDrcEffiAbs& mat);


  //! Destructor.
  virtual ~PndDrcEffiAbs();



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


  /*! \brief Set the name of the kathode material.
    \param name The name.
  */
  void SetName(string name){fName=name;};

  /*! \brief The name of the kathode material.
    \return The name.
  */
  virtual string Name() const = 0;




  /*! \brief Efficiency flag

  for measuring the photon. The measured photon returns true, the absorbed one 
  returns false.

  \param lambda The wavelength in [nm].
  \param cosine Cosine between photon direction and surface normal vector.
  \return Absorption flag.
  */
  virtual bool EffiFlag(double lambda, double cosine=1) const = 0;

 protected:

  string            fName;                        //!< Name of surface.
  int               fVerbosity;                   //!< Verbosity from 0 to 5.

};

#endif
