/*!
  \brief Class for optical device representation of a rectangular lens.



  The surfaces are named:
  \verbatim
                                                p16---------p26
                                               /| side6     /|
                                              / |          / |
                                             /  p46-------/-p36              Y
                                            p1-----------p2 /                ^  Z             side46
                                           / | /        /| /                 | /        side56     side36
                                          /  |/        / |/                  |/               side26
                                         /   p4-------/--p3            X <---0
                                        p11---------p21 /
                                        | /side1     | /                           side41
                                        |/           |/                      side51       side31
                                        p41---------p31                            side21


  \endverbatim
  Surface names at the very right refer to left right top down...


*/


//  The naming of the surfaces is as shown here:
//  \image html PndDrcOptLens1.png

#ifndef PNDDRCOPTLENS_H
#define PNDDRCOPTLENS_H

#include "PndDrcOptVol.h"
#include "PndDrcSurfPolyFlat.h"

class PndDrcOptLens : public PndDrcOptVol
{
  public:


  PndDrcOptLens();                         //!< Empty constructor

  /*! \brief Construct lens.
    \param dx half-length of the box along the x-axis.
    \param dy half-length of the box along the y-axis.
    \param dz half-length of the box along the z-axis.
    \param r1 Radius of lens side1
    \param r6 Radius of lens side2
    \param c1 Conical constant for lens side1 (0=sphere, -1=parabola, asphere else)
    \param c6 Conical constant for lens side6
  */
  PndDrcOptLens(double dx, double dy, double dz,
		double r1=9999, double r6=9999,
		double c1=0, double c6=0);




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
  double             fDz; //!< Z half length.
  PndDrcSurfPolyFlat fA0; //!< Aux. surface.

};

#endif
