/*! 

  \brief Representation of a cylinder surface

*/


#ifndef PNDDRCSURFCYL_H
#define PNDDRCSURFCYL_H

/*! 
  \brief Representation of a cylinder surface
*/

#include "PndDrcSurfAbs.h"

class PndDrcSurfCyl : public PndDrcSurfAbs
{
 public:

  PndDrcSurfCyl();                    //!< Empty constructor.

  /*! \brief Copy constructor.
    \param s Object to copy.
  */
  PndDrcSurfCyl(const PndDrcSurfCyl& s);

  /*! \brief Assignment operator.
    \param s Object to assign.
  */
  PndDrcSurfCyl& operator=(const PndDrcSurfCyl& s);

  virtual ~PndDrcSurfCyl(){};  //!< Destructor;

  /*! \brief Add a surface boundary point.
    \param r1 First point of cylinder axis.
    \param r2 Second point of cylinder axis.
    \param radius Radius of cylinder.
  */
  void     Set(XYZPoint r1, XYZPoint r2, double radius);       


  // implemetation of pure virtual functions.
  virtual  PndDrcSurfCyl* Clone() const;
  XYZVector Normal(const XYZPoint& point)   const; 
  bool     SurfaceHit(PndDrcPhoton& ph, 
		      XYZPoint&  pos_new, 
		      double&    path_length) const;
  void     Print(fstream& stream)          const;
  void     Print() const;
  void     AddTransform(const Transform3D& trans);
  virtual bool IsFlat() const {return false;};

 private:

  XYZPoint fP1;                         //!< First point of cylinder axis                   
  XYZPoint fP2;                         //!< Second point of cylinder axis                              
  double   fRadius;                     //!< Radius of cylinder.                        

  /*! \brief Auxiliary function for assignment operator and copy constructor..
    \param s The object to copy.
  */
  void Copy(const PndDrcSurfCyl& s);         




};
#endif
