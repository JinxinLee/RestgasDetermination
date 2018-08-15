// ******************************************************
// DecayTreeFitter Package
// We thank the original author Wouter Hulsbergen 
// (BaBar, LHCb) for providing the sources. 
// http://arxiv.org/abs/physics/0503191v1 (2005)
// Adaptation & Development for PANDA: Ralf Kliemt (2015)
// ******************************************************
//
//  LineTool.h
//  TreeFitter
//
//  Created by Ralf Kliemt on 26/01/15.
//  Copyright (c) 2015 Ralf Kliemt. All rights reserved.
//

#ifndef TreeFitter_LineTool_h
#define TreeFitter_LineTool_h 1

#include "TVector3.h"
#include <iostream>
#include <cmath>
#include "Rtypes.h"

namespace DecayTreeFitter
{
  // ==========================================================================
  class Line
  {
  public:
    Line() {}
    Line ( const TVector3& p0 , const TVector3& v0 ) : m_p0 ( p0 ) , m_v0 ( v0  ) {}
    virtual ~Line(){};

    const TVector3&  beginPoint() const { return m_p0 ; }
    const TVector3& direction()  const { return m_v0 ; }
    TVector3 position ( const double mu ) const
    { return beginPoint() + direction() * (float)mu ; }
    TVector3 operator() ( const double mu ) const
    { return beginPoint() + direction() * (float)mu ; }
  public:
    inline std::ostream& fillStream ( std::ostream& os ) const
    {
      os << "\np0 ("
      << m_p0.x() << " " << m_p0.y() << " " << m_p0.z()
      << ") direction ("
      << m_v0.x()<< " " << m_v0.y() << " " << m_v0.z() << ")\n"
      << std::endl;
      return os;
    }
  private:
    TVector3  m_p0; // the start point on the line
    TVector3 m_v0; // the direction vector of the line
	ClassDef(Line,1);
  };

  // =========================================================================
  inline std::ostream& operator<<
  (std::ostream&                      os  ,
   const Line& rhs )
  {
    return rhs.fillStream(os);
  }

  // ==========================================================================
  // ==========================================================================
  inline bool closestPointParams
  (const Line& line0 ,
   const Line& line1 ,
   double&      mu0   ,
   double&      mu1   )
  {
    // lhs:

    bool OK = true;

    // the matrix:
    const double a00 =  line0.direction().Mag2() ;
    const double a10 =  line0.direction().Dot( line1.direction() ) ;
    const double a01 = -a10 ;
    const double a11 = -line1.direction().Mag2() ;

    // the inverse determinant:
    const double det =  ( a00 * a11 - a01 * a10 ) ; // det = -sin^2(angle(line0.dir,line1.dir))
    if ( std::fabs(det) < 1e-10 )
    {
      OK = false; // parallel
    }
    else
    {

      const double detinv = 1.0 / det;

      // rhs:
      const TVector3 p1_p0 = line1.beginPoint() - line0.beginPoint() ;

      const double b0  = p1_p0.Dot ( line0.direction() ) ;
      const double b1  = p1_p0.Dot ( line1.direction() ) ;

      // get the Kramer solutions:

      mu0 =  ( b0  * a11 - b1  * a01 ) * detinv ;
      mu1 =  ( a00 * b1  - a10 * b0  ) * detinv ;

    }

    return OK ;
  }

  // ==========================================================================
  // ==========================================================================

} // 
#endif
