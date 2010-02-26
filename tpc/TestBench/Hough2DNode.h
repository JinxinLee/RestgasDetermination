//-----------------------------------------------------------
//
// Description:
//     Sub-volume of a 2 dimensional UNIT Hough Space
//     C-like implementation for easy transition to CUDA
//      
//      
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Felix Boehmer      TU Munich       (original author)
//
//
//-----------------------------------------------------------


#ifndef HOUGH2DNODE_H
#define HOUGH2DNODE_H


//includes
#include <iostream>



class Hough2DNode {

  
public:
  // Constructors/Destructors ---------
  Hough2DNode();
  Hough2DNode(double* center, int level, int nHyperplanes); 

  ~Hough2DNode();


  double* getCenter() {return _center;}
  int getLevel() {return _level;}
  double* getSonArray();  //return center positions of sons
  double* getCorners() { return _corners;}
  double  getSideLength() {return _length;}
  bool* getHitList() {return _hitList;}
  int getVote() {return _votes;}

  double* getProjection0() {return _proj0;} //return the two coordinates of the 
  double* getProjection1() {return _proj1;} //corners projected onto one 
    
  void setHit(int j) {
    if(j<_nPlanes)
      _hitList[j] = true;
  }
  
  
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
  double _length;
  double* _center;				
  double* _corners;

  double* _proj0;
  double* _proj1;

  int _votes;
  int _level;
  // Private Methods -----------------


};

#endif

