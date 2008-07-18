/*! 
  \brief Abstract class for flat surface representation, defined by 2 points 
  of one surface and 2 points of another surface. The 4 points have to be 
  within a plane. The purpose of this class is to generate surfaces between 
  one surface and a non flat surface. Eg. the surfaces between 
  a flat surface (one side) and a spherical surface (the other side) 
  to construct the sides of a lens.
  \image html PndDrcSurfQuadFlatDiff.png
  Since you connect to non planar surfaces which are created in an unshifted and
  unrotated coordinate system, you should generate the difference surface in
  the same system and rotate the whole construct after the generation.
*/


#ifndef PNDDRCSURFQUADFLATDIFF_H
#define PNDDRCSURFQUADFLATDIFF_H



#include "PndDrcSurfAbs.h"

#include "PndDrcSurfPolyFlat.h"

class PndDrcSurfQuadFlatDiff : public PndDrcSurfAbs
{
  public:

  //! Empty constructor.
  PndDrcSurfQuadFlatDiff();                    

  //! Destructor.
  virtual ~PndDrcSurfQuadFlatDiff();

  /*! \brief Copy constructor.

  Since the names of both surfaces are copied, there is the possibilty 
  to set those names.
  \sa SetNameSurface1()
  \sa SetNameSurface2()

    \param s Object to copy.
  */
  PndDrcSurfQuadFlatDiff(const PndDrcSurfQuadFlatDiff& s);

  /*! \brief Assignment operator.
    \param s Object to assign.
  */
  PndDrcSurfQuadFlatDiff& operator=(const PndDrcSurfQuadFlatDiff& s);
  

  /*! \brief Add surface to connect to and 2 points

  for construction of difference surface. You have to this exatcly twice.
  The 1st point of the first surface is connected to the 1st point of the second surface.
  Analog procedure for the 2nd point.
  \param surf The surface to connect to.
  \param p1   The 1st point of the surface to connect to.
  \param p2   The 2nd point of the surface to connect to.
  \sa PndDrcSurfPolySphere::LimitingPoint
  \sa PndDrcSurfPolyPara::LimitingPoint

  */
  void AddSurface(const PndDrcSurfAbs& surf, XYZPoint p1, XYZPoint p2);
  
  /*! \brief Set the name of the first surface
    \sa PndDrcSurfQuadFlatDiff(const PndDrcSurfQuadFlatDiff&)
    \param s The name.
  */
  void SetNameSurface1(string s){fS1->SetName(s);};

  /*! \brief Set the name of the second surface
    \sa PndDrcSurfQuadFlatDiff(const PndDrcSurfQuadFlatDiff&)
    \param s The name.
  */
  void SetNameSurface2(string s){fS2->SetName(s);};

  /*! \brief Name of first surface.
    \return The name.
  */
  string NameSurface1() const {return fS1->Name();};
  /*! \brief Name of second surface.
    \return The name.
  */
  string NameSurface2() const {return fS2->Name();};



  // implementation of pure virtual functions.
  virtual  PndDrcSurfQuadFlatDiff* Clone() const;
  XYZVector Normal(const XYZPoint& point)   const; 
  bool     SurfaceHit(PndDrcPhoton& ph, 
		      XYZPoint&  pos_new, 
		      double&    path_length) const;
  void     Print(fstream& stream)          const;
  void     Print() const;
  void AddTransform(const Transform3D& trans);

  virtual bool IsFlat() const {return true;};



  private:

  XYZPoint           fS1p1;             //!< Point of 1st surface.
  XYZPoint           fS1p2;             //!< Point of 1st surface.
  XYZPoint           fS2p1;             //!< Point of 2nd surface.
  XYZPoint           fS2p2;             //!< Point of 2nd surface.
  PndDrcSurfAbs*     fS1;               //!< 1st surface.
  PndDrcSurfAbs*     fS2;               //!< 2st surface.
  XYZVector          fNormal;           //!< Normal vector.
  PndDrcSurfPolyFlat fSurfAux;          //!< Auxiliary surface.
  
  /*! \brief Auxiliary function for assignment operator and copy constructor..
    \param s The object to copy.
  */
  void Copy(const PndDrcSurfQuadFlatDiff& s);         



};

#endif
