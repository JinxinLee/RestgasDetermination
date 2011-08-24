/*! 
  \brief Class for optical device representation of a brick. 

  The naming of the surfaces is as shown here:

  \image html PndDrcOptBrick1.png
*/


#ifndef PNDDRCOPTBRIK_H
#define PNDDRCOPTBRIK_H

#include "PndDrcOptVol.h"

class PndDrcOptBrick : public PndDrcOptVol
{

  public:


  PndDrcOptBrick();                         //!< Empty constructor

  /*! \brief Construct brick.
    \param dx half-length of the box along the x-axis.
    \param dy half-length of the box along the y-axis.
    \param dz half-length of the box along the z-axis.
  */
  PndDrcOptBrick(double dx, double dy, double dz); 


  /*! \brief Set size of brick.
    \param dx half-length of the box along the x-axis.
    \param dy half-length of the box along the y-axis.
    \param dz half-length of the box along the z-axis.
  */
  void SetSize(double dx, double dy, double dz);
  

  /*! \brief Pointer to surface.

  If surface is not found the null pointer is returned. If the surface name occurs
  more than once, the program stops. 
    \param name of surface.
    \return pointer to surface.
  */
  PndDrcSurfAbs* const Surface(string name);
  
  
  



};

#endif
