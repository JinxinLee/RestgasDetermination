//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      PndTpc readout pad
//      Geometry definition for PndTpc Digitization
//
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//      Cristoforo Simonetto TUM
//
//
//-----------------------------------------------------------

#ifndef TPCPAD_HH
#define TPCPAD_HH

// Base Class Headers ----------------


// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op
#include <vector>

// Collaborating Class Declarations --
class PndTpcAbsPadShape;


class PndTpcPad {
public:

  // Constructors/Destructors ---------
  PndTpcPad();
  PndTpcPad(const double x, const double y, const double angle,
	 PndTpcAbsPadShape* const shape,
	 const unsigned int sectorID,
	 const unsigned int ID);
  ~PndTpcPad(){;}

  // Operators
  friend bool operator== (const PndTpcPad& lhs, const PndTpcPad& rhs);
  friend bool operator< (const PndTpcPad& lhs, const PndTpcPad& rhs);
  friend std::ostream& operator<< (std::ostream& s, const PndTpcPad& me);

  // Accessors -----------------------
  double x() const {return _x;}
  double y() const {return _y;}
  double angle() const {return _angle;}
  double pos(const int i) const {
    if(i==0) return _x;
    else if(i==1) return _y;
    else if(i==2) return _angle;
    else throw;}
  PndTpcAbsPadShape* shape() const {return _shape;}
  unsigned int sectorId() const {return _sectorId;};
  unsigned int padId() const {return _id;}
  unsigned int id() const{return _id;} 
  // Modifiers -----------------------
  void setx(const double x){_x=x;}
  void sety(const double y){_y=y;}
  void setangle(const double angle){
    _angle=angle;
    EvalBoundingRect();}
  void setshape(PndTpcAbsPadShape* const shape){
    _shape = shape;
    EvalBoundingRect();}
  void setsectorId(const unsigned int id){_sectorId=id;}
  void addNeighbour(const unsigned int& id){_neighbourIds.push_back(id);}
  unsigned int nNeighbours() const {return _neighbourIds.size();}
  unsigned int getNeighbour(const unsigned int& id) const {return _neighbourIds[id];}

  // Operations ----------------------
  bool Contains(const double x, const double y) const;
  bool CircleIntersection(const double x, 
			  const double y, 
			  const double r) const;
  double GetValue(const double x, const double y) const;
  //returns the number of points, you can get with GetBoundaryPoint
  int GetNBoundaryPoints() const;
  //returns those points on the boundary, which describe the form
  void GetBoundaryPoint(const int index,
			double& x, double& y) const;
  void Draw(int c=1) const;

private:

  // Private Data Members ------------
  // Center of gravity
  double _x;
  double _y;
  //angle (in math. direction of rotation, in rad) between padplane and shape
  double _angle;
  //The minimal sizes for a bounding box with the origin in the centre
  double _width;
  double _height;

  PndTpcAbsPadShape* _shape;
  unsigned int _sectorId;
  unsigned int _id;

  std::vector<unsigned int> _neighbourIds;

  // Private Methods -----------------
  void ToShapeCoord(double& xP, double& yP) const;
  void ToPlaneCoord(double& xP, double& yP) const;
  void EvalBoundingRect();
};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
