/*! 
  \brief Class for optical device representation of a trapezoid 

  with both x and y dimensions varying along z. 
  The naming of the surfaces is as shown here:

  \image html PndDrcOptBrik1.png
*/

#ifndef PNDDRCOPTTRD2_H
#define PNDDRCOPTTRD2_H

#include "PndDrcOptVol.h"

class PndDrcOptTrd2 : public PndDrcOptVol
{

  public:


  PndDrcOptTrd2();                         //!< Empty constructor

  /*! \brief Construct brick.
    \param dx1 half-length of the box along the x-axis at side1 (-z-side).
    \param dx2 half-length of the box along the x-axis at side6 (+z-side).
    \param dy1 half-length of the box along the y-axis at side1 (-z-side).
    \param dy2 half-length of the box along the y-axis at side6 (+z-side).
    \param dz  half-length of the box along the z-axis at side6.
  */
  PndDrcOptTrd2(double dx1, double dx2,
	     double dy1, double dy2, 
	     double dz); 


  /*! \brief Set size of brick.
    \param dx1 half-length of the box along the x-axis at side1 (-z-side).
    \param dx2 half-length of the box along the x-axis at side6 (+z-side).
    \param dy1 half-length of the box along the y-axis at side1 (-z-side).
    \param dy2 half-length of the box along the y-axis at side6 (+z-side).
    \param dz  half-length of the box along the z-axis at side6.
  */
  void SetSize(double dx1, double dx2,
	       double dy1, double dy2, 
	       double dz); 
  

  /*! \brief Pointer to surface.

  If surface is not found the null pointer is returned. If the surface name occurs
  more than once, the program stops. 
    \param name of surface.
    \return pointer to surface.
  */
  PndDrcSurfAbs* const Surface(string name);
  
  



};

#endif
