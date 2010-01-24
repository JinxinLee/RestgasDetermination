

#ifndef PNDDRCOPTBURLE_H
#define PNDDRCOPTBURLE_H

/*! \brief Class representing a BURLE MultiChannelPlate-PMT. 

   \image html PndDrcBurle.png

   The front plate is considered to consist completely out of the fused silica 
   window. The detectors coordinate system is such that the origin is in the middle 
   of the front window and is pointing in +z direction.

   For coupling issues: the name of the surface of the front window is "front", 
   the name of the volume is "housing".

   The cathode efficiency is by default "bialkali". It can be changed by \sa SetEffi().
 
*/

#include "PndDrcOptDevSys.h"
#include "PndDrcEffiAbs.h"

class PndDrcOptBurle : public PndDrcOptDevSys
{
  
  public:

  PndDrcOptBurle();                     //!< Empty constructor.

  /*! \brief    Set cathode efficiency.

  By default a bialkali cathode is assumed. With this routine you can change this.

  \param effi The efficiency.
  */
  void SetEffi(PndDrcEffiAbs& effi);

  /*! \brief    Set position correction for measured photons.

  It is turned on by default. Here, you can turn it off.

  \param pos_corr The flag.
  */
  void SetPosCorr(bool pos_corr=true);
};





#endif
