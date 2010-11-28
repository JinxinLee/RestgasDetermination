//-----------------------------------------------------------
//
// Description:
//     Sub-volume of a 4 dimensional UNIT Hough Space
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


#ifndef HOUGH4DNODE_H
#define HOUGH4DNODE_H


class Hough4DNode {

  
public:
  // Constructors/Destructors ---------
  Hough4DNode();
  Hough4DNode(const double* center, int level, int nHyperplanes); 

  ~Hough4DNode();

  const double* getCenter() const {return _center;}
  int getLevel() const {return _level;}
  const double* getSonArray() const ;  //return center positions of sons
  const double* getCorners() const { return _corners;}
  double  getSideLength() const {return _length;}
  const bool* getHitList() const {return _hitList;}
  int getVote() {return _votes;}

  const double* getProjection0() const {return _proj0;} 
  const double* getProjection1() const {return _proj1;}
  const double* getProjection2() const {return _proj2;}
  const double* getProjection3() const {return _proj3;}

  // Modifiers ----------------
  void setHit(int j) {
    if(j<_nPlanes)
      _hitList[j] = true;
  }
    
  void setVotes(int votes)  {_votes = votes;}
  void vote() {_votes++;}
  
  void removeHit(int j);
  bool checkHit(int j) {return _hitList[j];}
  void print(); //TODO: redesign (to not depend on <iostream>

private:

  // Private Data Members ------------
  int _nPlanes;
  bool* _hitList;
  double _length;
  double* _center;				
  double* _corners;

  double* _proj0;
  double* _proj1;
  double* _proj2;
  double* _proj3;
  
  double* _sons;

  int _votes;
  int _level;
  // Private Methods -----------------


};

#endif
