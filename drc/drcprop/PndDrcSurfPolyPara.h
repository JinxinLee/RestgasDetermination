/*! 
  \brief Representation of a parabolic surface

  It is limited by a polygon made up by points. The "radius" of the parabolic surface
  can be given. The radius is twice the focal length. The definition of the 
  parabola is given by \f$ p-z = \frac{x^2+y^2}{2p}\f$, where p plays 
  the role of the radius.
 
  The definition is very close to that of \sa DrcSurfPolySphere

  The borders of the parabola are 
  defined by a projection on the x-y plane. This projection is defined by a polygone
  with points which you can add. 

  \sa AddPoint

  The z-components of these points have to be zero.
  Such defined, the paraboloid can be positioned and rotated by
  \sa Shift \sa Rotate
  The rotation is performed before the shift operation.

*/


#ifndef PNDDRCSURFPOLYPARA_H
#define PNDDRCSURFPOLYPARA_H


#include "PndDrcSurfPolyFlat.h"

class PndDrcSurfPolyPara : public PndDrcSurfPolyFlat
{
 public:

  PndDrcSurfPolyPara();                    //!< Empty constructor.

  /*! \brief Copy constructor.
    \param s Object to copy.
  */
  PndDrcSurfPolyPara(const PndDrcSurfPolyPara& s);

  /*! \brief Assignment operator.
    \param s Object to assign.
  */
  PndDrcSurfPolyPara& operator=(const PndDrcSurfPolyPara& s);

  virtual ~PndDrcSurfPolyPara(){};


  /*! Point limiting the sphere.

  The paraboloid construction points are not the same as the limiting points 
  of the sphere itself. This function returns the limiting points.
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
  virtual  PndDrcSurfPolyPara* Clone() const;
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
  void Copy(const PndDrcSurfPolyPara& s);         

  /*! \brief Check dimensions of parabolic surface.

  The radius has to be given and the points of the polygon have to be such that
  the projection is within the half parabolic shape.
  /return true if ok.
  */
  bool Check1() const;

};
#endif
