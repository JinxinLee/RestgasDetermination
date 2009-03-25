//-----------------------------------------------------------
//
// Description:
//      An implementation of AbsField for fields with
//      rotational symmetry.
//      Representation by a 2-D cartesian grid.
//      Linear interpolation between grid points in the x-z-plane.
//
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Cristoforo Simonetto    TUM            (original author)
//      Felix Boehmer           TUM
//
//-----------------------------------------------------------

#ifndef TPCFIELDCYLGRID_H
#define TPCFIELDCYLGRID_H

// Base Class Headers ----------------
#include "PndTpcAbsField.h"

// C++ Headers
#include <vector>


template <class t>
class PndTpcFieldCylGrid : public PndTpcAbsField<t> {

public:

  // Constructors/Destructors ---------
 
  PndTpcFieldCylGrid(const t& nominalValue,
		  const double minR, const double minZ,
		  const double spacingR, const double spacingZ,
		  const TVector3& relPosition = TVector3(0.,0.,0.),
		  std::vector<std::vector<t*>*>* pGrid = 0); 
		  
 
  PndTpcFieldCylGrid(const PndTpcFieldCylGrid& fCG);  
  
  virtual ~PndTpcFieldCylGrid();

  // Operators
  // virtual PndTpcFieldCylGrid& operator=(const PndTpcFieldCylGrid& aFCG);

  virtual void print(std::ostream& s) const;
  
  // Accessors -----------------------
  virtual t value(const TVector3& point) const;   //linear approx.
  virtual bool pointOk(const TVector3& point) const;

//in x,z derivative uses the next grid points => independent of length of direction
 
  double minR() const {return(_minX);}
  double minZ() const {return(_minZ);}
  double spacingR() const {return(_spacingX);}
  double spacingZ() const {return(_spacingZ);}
  double maxR() const {return(_maxX);}
  double maxZ() const {return(_maxZ);}
  
  // Modifiers -----------------------
  void setMinR(const double minR) {
    _minX=minR;
    evalMaxPoint();}
  void setMinZ(const double minZ) {
    _minZ=minZ;
    evalMaxPoint();}
  void setSpacingR(const double spacingR) {
    _spacingX=spacingR;
    evalMaxPoint();}
  void setSpacingZ(const double spacingZ) {
    _spacingZ=spacingZ;
    evalMaxPoint();}

protected:
  // Protected Data Members ------------
  std::vector<std::vector<t*>*>* _pGrid;  //fieldmap
  
  // Protected Methods -----------------

  //retrieves the value at angle phi if valRZ is the value at same r and z,
  //but with phi = 0 (in the x-z-plane)
  //the default is just to return valRZ for each phi
  //but the specialization for t=TVector3 rotates the value wit phi
  virtual t rotate(const t& valRZ, const double phi) const;
  //I don't like this implementaion - next update will change it to void


  void evalMaxPoint();     //evaluates the plane within _pGrid is defined

  int _xBins;
  int _zBins;
  
  double _minX;//min. x (and r) where pGrid is defined (rel. to the origin of the grid)
  double _minZ;//min. z  where pGrid is defined (relative to the origin of the grid)
  double _spacingX;  //the space between two grid points 
  double _spacingZ;
  double _maxX;//max. x (and r) where pGrid is defined (rel. to the origin of the grid)
  double _maxZ;//max. z  where pGrid is defined (relative to the origin of the grid)

private:

  

  // Private Methods -----------------
  t evalValue(const TVector3& point) const;
  t startInterpolation(int&, int&, double, double) const;
  t interpolate(int&, int&, double, double) const;
  
  void deleteGrid();      //deletes the grid properly...
};

#endif
