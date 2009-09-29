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
  double x() const {return fx;}
  double y() const {return fy;}
  double angle() const {return fangle;}
  double pos(const int i) const {
    if(i==0) return fx;
    else if(i==1) return fy;
    else if(i==2) return fangle;
    else throw;}
  PndTpcAbsPadShape* shape() const {return fshape;}
  unsigned int sectorId() const {return fsectorId;};
  unsigned int padId() const {return fid;}
  unsigned int id() const{return fid;} 
  // Modifiers -----------------------
  void setx(const double x){fx=x;}
  void sety(const double y){fy=y;}
  void setangle(const double angle){
    fangle=angle;
    EvalBoundingRect();}
  void setshape(PndTpcAbsPadShape* const shape){
    fshape = shape;
    EvalBoundingRect();}
  void setsectorId(const unsigned int id){fsectorId=id;}
  void addNeighbour(const unsigned int& id){fneighbourIds.push_back(id);}
  unsigned int nNeighbours() const {return fneighbourIds.size();}
  unsigned int getNeighbour(const unsigned int& id) const {return fneighbourIds[id];}

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
  double fx;
  double fy;
  //angle (in math. direction of rotation, in rad) between padplane and shape
  double fangle;
  //The minimal sizes for a bounding box with the origin in the centre
  double fwidth;
  double fheight;

  PndTpcAbsPadShape* fshape;
  unsigned int fsectorId;
  unsigned int fid;

  std::vector<unsigned int> fneighbourIds;

  // Private Methods -----------------
  void ToShapeCoord(double& xP, double& yP) const;
  void ToPlaneCoord(double& xP, double& yP) const;
  void EvalBoundingRect();
};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
