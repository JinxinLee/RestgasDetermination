#include "AbsTrackRep.h"
#include <iostream>

AbsTrackRep::AbsTrackRep() : state(5,1), cov(5,5), startState(5,1), startCov(5,5),startS(0), chiSqu(0), statusFlag(0), inverted(false), params()
{
  
}

AbsTrackRep::AbsTrackRep(int dim) : state(dim,1), cov(dim,dim), startState(dim,1), startCov(dim,dim), startS(0), chiSqu(0), statusFlag(0), inverted(false), params()
{
 
}

AbsTrackRep::~AbsTrackRep() {}

void 
AbsTrackRep::init(const TVector3& startpoint, 
			  const TVector3& startmomentum, 
			  const TVector3& locBField)
{throw;}



// this changes the state of the rep!!! ->
double
AbsTrackRep::extrapolate(const DetPlane& plane){
  TMatrixT<double> newstate=state;
  TMatrixT<double> newcov=cov;
  TMatrixT<double> jac;
  double l=extrapolate(plane,newstate);//,newcov,jac);
  state=newstate;
  cov=newcov;
  setReferencePlane(plane);
  return l;
}

void
AbsTrackRep::reset(){
  std::cout<<"AbsTrackRep::reset"<<std::endl;
  state.Zero();
  cov.Zero();
  startState.Zero();
  startCov.Zero();
}

void
AbsTrackRep::Print() const {
  std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
  std::cout<<"AbsTrackRep::Parameters at reference plane ";
  _refPlane.Print();
  std::cout<<"AbsTrackRep::State"<<std::endl;
  state.Print();
  std::cout<<"AbsTrackRep::Covariances"<<std::endl;
  cov.Print();
  std::cout<<"AbsTrackRep::chi^2"<<std::endl;
  std::cout<<chiSqu<<std::endl;
  std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
}

ClassImp(AbsTrackRep)
