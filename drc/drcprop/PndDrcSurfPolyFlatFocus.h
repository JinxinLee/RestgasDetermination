/*! 
  \brief Abstract class for flat surface representation, 

  defined by a sequence
  of point going arround the surface.

  In addition this surface has focussing properties. Photons which 
  hit the surface perpendicular will hit the focal point (if you stay within
  the same material. It works only for a reflecting surface since the 
  normal vector is adapted. The focal point is where the photons are going to 
  after reflection. 
  
*/


#ifndef PNDDRCSURFPOLYFLATFOCUS_H
#define PNDDRCSURFPOLYFLATFOCUS_H



#include "PndDrcSurfAbs.h"
#include <vector>
using std::vector;
#include "Math/Transform3D.h"
using ROOT::Math::Transform3D;

class PndDrcSurfPolyFlatFocus : public PndDrcSurfAbs
{
 public:

  PndDrcSurfPolyFlatFocus();                    //!< Empty constructor.

  /*! \brief Copy constructor.
    \param s Object to copy.
  */
  PndDrcSurfPolyFlatFocus(const PndDrcSurfPolyFlatFocus& s);

  /*! \brief Assignment operator.
    \param s Object to assign.
  */
  PndDrcSurfPolyFlatFocus& operator=(const PndDrcSurfPolyFlatFocus& s);

  virtual ~PndDrcSurfPolyFlatFocus(){};         //!< Destructor.

  /*! \brief Add a surface boundary point

  The points have to be given in the order going around the surface. Direction doesn't matter.

    \param point The point
  */
  void     AddPoint(XYZPoint point);       

   /*! \brief Set the focal point

   perpendicular photons will be directed to this point. Surface must have reflection!

    \param point The point
  */
  void     SetFocalPoint(XYZPoint point);       

 /*! \brief Test if point within surface.

  \param point Point to test.
  \return true if within surface.
  */
  bool WithinSurface(XYZPoint& point) const;

  // implemetation of pure virtual functions.
  virtual  PndDrcSurfPolyFlatFocus* Clone() const;
  XYZVector Normal(const XYZPoint& point)   const; 
  bool     SurfaceHit(PndDrcPhoton& ph, 
		      XYZPoint&  pos_new, 
		      double&    path_length) const;
  void     Print(fstream& stream)          const; 
  void     Print() const;
  void     AddTransform(const Transform3D& trans);
  virtual bool IsFlat() const {return true;};

  /*! \brief Set radius

  for usage in derived class
  \param r Radius
  */
  void SetRadius(double r){fRadius=r;};

  /*! \brief Set conical constant

  for usage in derived class
  The parameter k determines the shape. For
  \verbatim
  hyperbola k < -1 
  parabola  k = -1 
  ellipse   -1 < k < 0
  sphere    k = 0 
  ellipse   k > 0 
  \endverbatim
  \param c Conical constant
  */
  void SetConicalConstant(double c){fConConst=c;};

  /*! Point limiting the sphere.

  for usage in derived class

  The spheres construction points are not the same as the limiting points 
  of the sphere itself. This function returns the limiting points.
    \param i Index of point starting with 0
    \return The point.
  */
  virtual XYZPoint LimitingPoint(unsigned int i){return XYZPoint(0,0,0);};
 
 protected:

  vector<XYZPoint> fP;                  //!<  Vector of surface boundary points.             
  double fRadius;                       //!< Radius for curved surfaces.
  double fConConst;                     //!< Conial constant for curved surfaces.
  XYZPoint fFocalPoint;                 //!< Focal point.

 private:

  XYZVector        fNormal;             //!< Normal vector.    
  

  /*! \brief Auxiliary function for assignment operator and copy constructor..
    \param s The object to copy.
  */
  void Copy(const PndDrcSurfPolyFlatFocus& s);         

  /*! \brief Line intersection

  between two lines each define by two points. The intersection has to be
  between the points.
  \param p1a 1st point of 1st line
  \param p1b 2nd point of 1st line
  \param p2a 1st point of 2nd line
  \param p2b 2nd point of 2nd line
  \return Flag if intersection exists.
  */
  bool LineCross(const XYZPoint& p1a, const XYZPoint& p1b,
		 const XYZPoint& p2a, const XYZPoint& p2b) const;



};
#endif
