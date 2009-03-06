/*  Copyright © 2008, Christian Hoeppner & Sebastian Neubert
    All rights reserved.

    This file is part of Genfit.

    Genfit is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Genfit is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser Public License for more details.

    You should have received a copy of the GNU Lesser Public License
    along with Genfit.  If not, see <http://www.gnu.org/licenses/>. */

// Description:
//      Detector plane - a geometric object

#ifndef DETPLANE_H
#define DETPLANE_H

#include "TObject.h"

#include "TVector3.h"
#include "TPolyMarker3D.h"
#include "TPolyLine3D.h"

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

  double distance(TVector3&) const;

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

