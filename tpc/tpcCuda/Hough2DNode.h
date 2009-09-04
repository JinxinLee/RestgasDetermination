// NOTE: requires NORMALIZED root parameter space with side length 1.

#ifndef HOUGH2DNODE_H
#define HOUGH2DNODE_H
#include <iostream>
class Hough2DNode {

  
public:
  // Constructors/Destructors ---------
  Hough2DNode();
  Hough2DNode(float* center, int level, int nHyperplanes); 

  ~Hough2DNode();

  float* getCenter() {return _center;}
  int getLevel() {return _level;}
  float* getSonArray();  //return center positions of sons
  float* getCorners() { return _corners;}
  float  getSideLength() {return _length;}
  bool* getHitList() {return _hitList;}
  int getVote() {return _votes;}

  float* getProjection0() {return _proj0;} //return the two coordinates of the 
  float* getProjection1() {return _proj1;} //corners projected onto one 
    
  void setHit(int j) {
    if(j<_nPlanes){
    _hitList[j] = true;
    }else{
      std::cerr << "trying to set hit on hyperplane " << j << ", _nPlanes is "<<_nPlanes<<" out of range"
	    << std::endl;
      throw;
    }
  } //hyperplane was a hit
  void setVotes(int votes)  {_votes = votes;}
  void vote() {_votes++;}
  bool checkHit(int j) {return _hitList[j];}
  void removeHit(int j) ;
  void print();
  int getNplanes(){return _nPlanes;}
  

private:

  // Private Data Members ------------
  int _nPlanes;
  bool* _hitList;
  float _length;
  float* _center;				
  float* _corners;

  float* _proj0;
  float* _proj1;

  int _votes;
  int _level;
  // Private Methods -----------------


};

#endif

