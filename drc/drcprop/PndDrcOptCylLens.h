/*! 
  \brief Class for optical device representation of a rectangular lens. 

  The naming of the surfaces is as shown here:

  \image html PndDrcOptCylLens1.png
*/

#ifndef PNDDRCOPTCYLLENS_H
#define PNDDRCOPTCYLLENS_H

#include "PndDrcOptVol.h"
#include "PndDrcSurfPolyFlat.h"

class PndDrcOptCylLens : public PndDrcOptVol
{
  public:


  PndDrcOptCylLens();                         //!< Empty constructor

  /*! \brief Construct brick.
    \param dx half-length of the box along the x-axis.
    \param dy half-length of the box along the y-axis.
    \param dz half-length of the box along the z-axis.
    \param r1 Radius of lens side1
    \param r6 Radius of lens side2
  */
  PndDrcOptCylLens(double dx, double dy, double dz,
		double r1=9999, double r6=9999); 


  

  /*! \brief Pointer to surface.

  If surface is not found the null pointer is returned. If the surface name occurs
  more than once, the program stops. 
    \param name of surface.
    \return pointer to surface.
  */
  PndDrcSurfAbs* const Surface(string name);
  
  /*! \brief Create downstream surface
    \param surf6 The created surface.
    \param r6 Radius
  */
  void Make_surf6(PndDrcSurfPolyFlat& surf6,double r6);

  /*! \brief Create upstream surface
    \param surf1 The created surface.
    \param r1 Radius
  */
  void Make_surf1(PndDrcSurfPolyFlat& surf1,double r1);


 private:

  XYZPoint           fP1; //!< Aux. point.
  XYZPoint           fP2; //!< Aux. point.
  XYZPoint           fP3; //!< Aux. point.
  XYZPoint           fP4; //!< Aux. point.
  double             fDz; //!< Z half ength.
  PndDrcSurfPolyFlat fA0; //!< Aux. surface.

};

#endif
