//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Detector plane - a geometric object
//
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef DETPLANE_HH
#define DETPLANE_HH

// Base Class Headers ----------------
#include "TObject.h"

// Collaborating Class Headers -------
#include "TVector3.h"
#include "TPolyMarker3D.h"
#include "TPolyLine3D.h"

// Collaborating Class Declarations --



class DetPlane : public TObject {
public:

  // Constructors/Destructors ---------
  DetPlane();
  DetPlane(const TVector3& o,
	   const TVector3& u,
	   const TVector3& v);
  DetPlane(const TVector3& o,
	   const TVector3& n);
  virtual ~DetPlane(){;}

  // Operators
  

  // Accessors -----------------------
  TVector3 getO() const {return _o;}
  TVector3 getU() const {return _u;}
  TVector3 getV() const {return _v;}

  // Modifiers -----------------------
  void set(const TVector3& o,
	   const TVector3& u,
	   const TVector3& v); 

  void setO(const TVector3& o);
  void setU(const TVector3& u);
  void setV(const TVector3& v);

  // Operations ----------------------
  TVector3 getNormal() const;
  void setNormal(TVector3 n);
  void setNormal(const double& theta, const double& phi);
  // projecting a direction into the plane:
  TVector2 project(const TVector3& x) const;
  // transform from Lab system into plane
  TVector2 LabToPlane(const TVector3& x) const;
  TVector3 toLab(const TVector2& x) const;
  // get vector from point to plane (normal)
  TVector3 dist(const TVector3& point) const;

  void Print() const;
  void getGraphics(double mesh, double length, TPolyMarker3D **pl, TPolyLine3D **u, TPolyLine3D **v, TPolyLine3D **n=NULL);

  friend bool operator== (const DetPlane& lhs, const DetPlane& rhs);
  friend bool operator!= (const DetPlane& lhs, const DetPlane& rhs);

private:

  // Private Data Members ------------
  // origin
  TVector3 _o;
  // Vectors spanning the plane
  TVector3 _u;
  TVector3 _v;

  // Private Methods -----------------

  void sane(); // ensures orthnormal coordinates
public:
  ClassDef(DetPlane,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
