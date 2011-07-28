/*! 
  \brief Representation of a cylindrical surface with a curvature in the x-z plane.

  it is limited by a polygon made up by points. The radius of the cylinder can be given.
  The cylindrical surface is defined as follows: The cylinder is a half cylinder centered 
  around the y-axis with a certain radius

  \image html PndDrcSurfPolyCyl.png

  The borders of the cylnder are 
  defined by a projection on the x-y plane. This projection is defined by a polygone
  with points which you can add. 

  \sa AddPoint

  The z-components of these points have to be zero.
  Such defined, the cylindrical surface can be positioned and rotated by
  \sa Shift \sa Rotate
  The rotation is performed before the shift operation.

  The surfaces are named:
  \verbatim
                                                *------------*
                                               /| side6     /|
                                              / |          / |
                                             /  *---------/--*               Y
                                            p1-----------p2 /                ^  Z
                                           / | /        /| /                 | /
                                          /  |/        / |/                  |/
                                         /   p4-------/--p3            X <---0
                                        *------------*  /
                                        | /side1     | /
                                        |/           |/
                                        *------------* 


  \endverbatim
  For other surfaces look into the code...
*/


#ifndef PNDDRCSURFPOLYCYL_H
#define PNDDRCSURFPOLYCYL_H

#include "PndDrcSurfPolyFlat.h"

class PndDrcSurfPolyCyl : public PndDrcSurfPolyFlat
{
 public:

  PndDrcSurfPolyCyl();                    //!< Empty constructor.

  /*! \brief Copy constructor.
    \param s Object to copy.
  */
  PndDrcSurfPolyCyl(const PndDrcSurfPolyCyl& s);

  /*! \brief Assignment operator.
    \param s Object to assign.
  */
  PndDrcSurfPolyCyl& operator=(const PndDrcSurfPolyCyl& s);

  virtual ~PndDrcSurfPolyCyl(){};   //!< Destructor.



  /*! Point limiting the cylinder.

  The cylinder construction points are not the same as the limiting points 
  of the cylinder itself. This function returns the limiting points.
    \param i Index of point starting with 0
    \return The point.
  */
  XYZPoint LimitingPoint(unsigned int i);
  
  /*! Center point of the surface.

  Like the /sa limitingPoint this function returns the point on the z-axis 
  at construction after applying transformations.
  \return The transformed center point.
  */
  XYZPoint CenterPoint();

  // implemetation of pure virtual functions.
  virtual  PndDrcSurfPolyCyl* Clone() const;
  XYZVector Normal(const XYZPoint& point)   const; 
  bool     SurfaceHit(PndDrcPhoton& ph, 
		      XYZPoint&  pos_new, 
		      double&    path_length) const;
  void     Print(fstream& stream)          const;
  void     Print() const;
  void     AddTransform(const Transform3D& trans);
  virtual bool IsFlat() const {return false;};

 private:

  mutable bool      fChecked;           //!< Flag if dimensions are checked.
  Transform3D       fTrans;             //!< Transform.
  Transform3D       fTransInv;          //!< Inverse transform.

  /*! \brief Auxiliary function for assignment operator and copy constructor..
    \param s The object to copy.
  */
  void Copy(const PndDrcSurfPolyCyl& s);         

  /*! \brief Check dimensions of spherical surface.

  The radius has to be given and the points of the polygon have to be such that
  the projection is within the half sphere.
  /return true if ok.
  */
  bool Check() const;



};
#endif
