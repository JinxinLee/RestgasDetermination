#include "AbsTrackRep.h"
#include <iostream>

AbsTrackRep::AbsTrackRep() : state(5,1), cov(5,5), startState(5,1), startCov(5,5),chiSqu(0), statusFlag(0), inverted(false)
{
  
}

AbsTrackRep::AbsTrackRep(int dim) : state(dim,1), cov(dim,dim), startState(dim,1), startCov(dim,dim), chiSqu(0), statusFlag(0), inverted(false)
{
 
}

AbsTrackRep::~AbsTrackRep() {}

double AbsTrackRep::extrapolate(const DetPlane& plane){
  TMatrixT<double> statePred,covPred,jac;
  double retVal = extrapolate(plane,statePred,covPred,jac);
  setState(statePred);
  setCov(covPred);
  setReferencePlane(plane);
  return retVal;
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
