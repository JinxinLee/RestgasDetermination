/*! 
  \brief Representation of an optical volume.

  The volume consists out of surfaces. 
*/

#ifndef PNDDRCOPTVOL_H
#define PNDDRCOPTVOL_H

#include "PndDrcOptDev.h"

class PndDrcOptVol : public PndDrcOptDev
{
 public:

  //!                                             Constructor.
  PndDrcOptVol();                          
  //!                                             Destructor.
  virtual ~PndDrcOptVol();                 

  /*! \brief Virtual copy constructor.
    \return Pointer to new allocated memory.
  */
  PndDrcOptVol* Clone() const;


  /*! \brief Copy constructor.
    \param d The object to copy.
  */
  PndDrcOptVol(const PndDrcOptVol& d);

  /*! \brief Assignment operator.
    \param d The object to assign.
  */
  PndDrcOptVol& operator=(const PndDrcOptVol& d);

  /*! \brief Position correction for measured photons.

  This function is for detectors derived from this class. Eg. In a multipixel
  detector the photon position will be corrected to be the middle of the pixel. 
  This routine is called in \sa Propagate() when the photon gets the attribute 
  \sa Drc::PhotMeasured

  \param ph The photon to be corrected.
  */ 
  void PositionCorrection(PndDrcPhoton& ph);
  



  // implements pure virtual base class
  void Propagate(PndDrcPhoton& ph);
  bool Radiator() const {return true;};

  //void transform(const TVector3& shift,const TRotation& rot);

  /*! \brief Set optical material.
    \param mat The material.
  */
  void SetOptMaterial(const PndDrcOptMatAbs& mat);
  /*! \brief Optical material.
    \return Pointer to material.
  */
  PndDrcOptMatAbs& OptMaterial() const 
    {
      return *fOptMat;
    };

 protected:

  PndDrcOptMatAbs*    fOptMat;                    //!< Optical material.          


 private:

  /*! \brief Auxiliary function for assignment operator and copy constructor..
    \param d The object to copy.
  */
  void Copy(const PndDrcOptVol& d);



};

#endif
