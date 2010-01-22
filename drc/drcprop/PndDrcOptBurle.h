

#ifndef PNDDRCOPTBURLE_H
#define PNDDRCOPTBURLE_H

/*! \brief Class representing a BURLE MultiChannelPlate-PMT. 

   \image html PndDrcBurle.png

   However the front plate is considered to consist completely out of the fused silica 
   window. The detectors coordinate system is such that the origin is in the middle 
   of the front window and is pointing in +z direction.

   For coupling issues: the name of the surface of the front window is "front", 
   the name of the volume is "housing".
 
*/

#include "PndDrcOptDevSys.h"

class PndDrcOptBurle : public PndDrcOptDevSys
{
  
  public:
  
  PndDrcOptBurle();                     //!< Empty constructor.

};





#endif
