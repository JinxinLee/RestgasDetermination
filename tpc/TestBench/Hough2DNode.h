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


class Hough2DNode {

  
public:
  // Constructors/Destructors ---------
  Hough2DNode();
  Hough2DNode(const double* center, int level, int nHyperplanes); 

  ~Hough2DNode();


  const double* getCenter() const {return _center;}
  int getLevel() const {return _level;}
  const double* getSonArray() const ;  //return center positions of sons
  const double* getCorners() const { return _corners;}
  double  getSideLength() const {return _length;}
  const bool* getHitList() const {return _hitList;}
  int getVote() {return _votes;}

  const double* getProjection0() const 
  {return _proj0;} //return the two coordinates of the 
  
  const double* getProjection1() const 
  {return _proj1;} //corners projected onto one 
    
  bool checkHit(int j) {return _hitList[j];}

  int getNplanes() {return _nPlanes;}
  
  // Modifiers ----------------
  void setHit(int j) {
    if(j<_nPlanes)
      _hitList[j] = true;
  }
  
  
  void setVotes(int votes)  {_votes = votes;}
  void vote() {_votes++;}
  
  void removeHit(int j) ;
  //void print(); TODO: redesign (to not depend on <iostream>
  
  

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

