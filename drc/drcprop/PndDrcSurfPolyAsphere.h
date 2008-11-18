/*! 
  \brief Representation of a aspherical surface

  limited by a polygon made up by points. The parameters of the asphere can be given.
  The surface is defined as follows: The asphere is a half asphere centered 
  at the coordinate origin (0,0,0). 

  The asphere is determined by a radius R and a conical constant k. 
  The aspherical parameters turned out to cause to much troubles for the surface 
  hit algorithmus. Therefore they are disabled.

  \image html PndDrcSurfPolyAsphere1.png

  The parameter k determines the shape. For
  \verbatim
  hyperbola k < -1 
  parabola  k = -1 
  ellipse   -1 < k < 0
  sphere    k = 0 
  ellipse   k > 0 
  \endverbatim

  \image html PndDrcSurfPolyAsphere2.png

  You find more explaination in german under
  http://de.wikipedia.org/wiki/Linse_%28Optik%29#Asph.C3.A4rische_Linsen
  and less informative in english under
  http://en.wikipedia.org/wiki/Aspheric_lens

  \image html PndDrcSurfPolySphere.png

  The borders of the asphere are 
  defined by a projection on the x-y plane. This projection is defined by a polygone
  with points which you can add. 

  \sa AddPoint

  The z-components of these points have to be zero.
  Such defined, the aspherical surface can be positioned and rotated by
  \sa Shift \sa Rotate
  The rotation is performed before the shift operation.

The aspherical components of the

*/


#ifndef PNDDRCSURFPOLYASPHERE_H
#define PNDDRCSURFPOLYASPHERE_H

#include "PndDrcSurfPolyFlat.h"

class PndDrcSurfPolyAsphere : public PndDrcSurfPolyFlat
{
 public:

  PndDrcSurfPolyAsphere();                    //!< Empty constructor.

  /*! \brief Copy constructor.
    \param s Object to copy.
  */
  PndDrcSurfPolyAsphere(const PndDrcSurfPolyAsphere& s);

  /*! \brief Assignment operator.
    \param s Object to assign.
  */
  PndDrcSurfPolyAsphere& operator=(const PndDrcSurfPolyAsphere& s);

  virtual ~PndDrcSurfPolyAsphere(){};




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
  virtual  PndDrcSurfPolyAsphere* Clone() const;
  XYZVector Normal(const XYZPoint& point)   const; 
  bool     SurfaceHit(PndDrcPhoton& ph, 
		      XYZPoint&  pos_new, 
		      double&    path_length) const;
  void     Print(fstream& stream)          const;
  void     Print()          const;
  void     AddTransform(const Transform3D& trans);
  virtual bool IsFlat() const {return false;};

 private:
  mutable bool      fChecked;           //!< Flag if dimensions are checked.        
  Transform3D       fTrans;             //!< Transform    
  Transform3D       fTransInv;          //!< Inverse transform   

  /*! \brief Auxiliary function for assignment operator and copy constructor..
    \param s The object to copy.
  */
  void Copy(const PndDrcSurfPolyAsphere& s);         

  /*! \brief Check dimensions of spherical surface.

  The radius has to be given and the points of the polygon have to be such that
  the projection is within the half sphere.
  /return true if ok.
  */
  bool Check1() const;

  /*! The z value in unshifted and unrotated system.
    \param x The x coordinate.
    \param y The y coordinate.
    \return The z coordinate of the surface.
  */
  double ZVal(double x, double y) const;

  /*! Check if point is on the aspherical surface

  with precision eps=0.1 but do not check the limiting points.
  \param p The point to check.
  \return True, if on surface.
  */
  bool OnSurface(const XYZPoint& p) const;


};
#endif
