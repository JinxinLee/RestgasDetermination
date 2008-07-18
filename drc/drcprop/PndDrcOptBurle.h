

#ifndef PNDDRCOPTBURLE_H
#define PNDDRCOPTBURLE_H

/*! \brief Class representing a BURLE MultiChannelPlate-PMT. The default size is
  8 x 8 pixels, which one can adjust by \sa set setPixelNumber(). The dimensions are
  like in the followin image. 

   \image html PndDrcBurle.png

   However the front plate is considered to consist completely out of the fused silica 
   window. The detectors coordinate system is such that the origin is in the middle 
   of the front window. The thickness of the whole detector is 25.4 mm. 
*/

class PndDrcOptBurle
{
  private:
  //!                                             Verbosity
  int               fVerbosity;
  
  public:
  
  PndDrcOptBurle();                     //!< Empty constructor.


  




};





#endif
