/*! 
  \brief Representation of a spherical surface

  limited by a polygon made up by points. The radius of the sphere can be given.
  The spherical surface is defined as follows: The sphere is a half sphere centered 
  at the coordinate origin (0,0,0) with a radius. 

  \image html PndDrcSurfPolySphere.png

  The borders of the sphere are 
  defined by a projection on the x-y plane. This projection is defined by a polygone
  with points which you can add. 

  \sa AddPoint

  The z-components of these points have to be zero.
  Such defined, the spherical surface can be positioned and rotated by
  \sa PndDrcSurfAbs::AddTransform()
  The rotation is performed before the shift operation.

*/


#ifndef PNDDRCSURFPOLYSPHERE_H
#define PNDDRCSURFPOLYSPHERE_H

#include "PndDrcSurfPolyFlat.h"

class PndDrcSurfPolySphere : public PndDrcSurfPolyFlat
{
 public:

  PndDrcSurfPolySphere();                    //!< Empty constructor.

  /*! \brief Copy constructor.
    \param s Object to copy.
  */
  PndDrcSurfPolySphere(const PndDrcSurfPolySphere& s);

  /*! \brief Assignment operator.
    \param s Object to assign.
  */
  PndDrcSurfPolySphere& operator=(const PndDrcSurfPolySphere& s);

  virtual ~PndDrcSurfPolySphere(){};



  /*! Point limiting the sphere.

  The spheres construction points are not the same as the limiting points 
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
  virtual  PndDrcSurfPolySphere* Clone() const;
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
  void Copy(const PndDrcSurfPolySphere& s);         

  /*! \brief Check dimensions of spherical surface.

  The radius has to be given and the points of the polygon have to be such that
  the projection is within the half sphere.
  /return true if ok.
  */
  bool Check() const;


};
#endif
