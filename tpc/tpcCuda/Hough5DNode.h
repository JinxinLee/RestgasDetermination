// NOTE: requires NORMALIZED root parameter space with side length 1.

#ifndef HOUGH5DNODE_H
#define HOUGH5DNODE_H



class Hough5DNode {

  
public:

  // Constructors/Destructors ---------
  Hough5DNode();
  Hough5DNode(float* center, int level, int nHyperplanes); 

  float* getCenter() {return _center;}
  int getLevel() {return _level;}
  float* getSonArray();  //return center positions of sons
  float* getCorners() { return _corners;}
  float  getSideLength() {return _length;}
  bool* getHitList() {return _hitList;}
  int getVote() {return _votes;}
  
  
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

  int _votes;
  int _level;
  

  // Private Methods -----------------


};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
