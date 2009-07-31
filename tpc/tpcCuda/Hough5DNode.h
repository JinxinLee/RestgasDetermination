// NOTE: requires NORMALIZED root parameter space with side length 1.

#ifndef HOUGH5DNODE_H
#define HOUGH5DNODE_H


class Hough5DNode {

  
public:

  // Constructors/Destructors ---------
  Hough5DNode();
  Hough5DNode(float* center, int level, int nHyperplanes); 
  
  ~Hough5DNode();

  float* getCenter() {return _center;}
  int getLevel() {return _level;}
  float* getSonArray();  //return center positions of sons
  float* getCorners() { return _corners;}
  float  getSideLength() {return _length;}
  bool* getHitList() {return _hitList;}
  int getVote() {return _votes;}
  
  float* getProjection0() {return _proj0;}   //return the two coordinates of the 
  float* getProjection1() {return _proj1;}  //corners projected onto one 
  float* getProjection2() {return _proj2;} //dimension of the parameter space;
  float* getProjection3() {return _proj3;}
  float* getProjection4() {return _proj4;}
  
  
  void setHit(int j) {_hitList[j] = true;} //hyperplane was a hit
  void vote() {_votes++;}

  void print();
  

private:

  // Private Data Members ------------
  int _nPlanes;
  bool* _hitList;
  float _length;
  float* _center;				
  float* _corners;

  float* _proj0;
  float* _proj1;
  float* _proj2;
  float* _proj3;
  float* _proj4;

  int _votes;
  int _level;
  

  // Private Methods -----------------


};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
