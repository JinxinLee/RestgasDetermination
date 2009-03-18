#include "AbsTrackRep.h"
#include <iostream>

AbsTrackRep::AbsTrackRep() : dimension(5),state(5,1), cov(5,5), startState(5,1), startCov(5,5),chiSqu(0), statusFlag(0), inverted(false)
{
  
}

AbsTrackRep::AbsTrackRep(int dim) : dimension(dim), state(dim,1), cov(dim,dim), startState(dim,1), startCov(dim,dim), chiSqu(0), statusFlag(0), inverted(false)
{
 
}

AbsTrackRep::~AbsTrackRep() {}

double AbsTrackRep::extrapolate(const DetPlane& plane){
  TMatrixT<double> statePred(dimension,1);
  TMatrixT<double> covPred(dimension,dimension);
  double retVal = extrapolate(plane,statePred,covPred);
  setState(statePred);
  setCov(covPred);
  setReferencePlane(plane);
  return retVal;
}

//default implentation might be overwritten, please see the doxy docu
double AbsTrackRep::extrapolate(const DetPlane& plane, TMatrixT<double>& statePred){
  TMatrixT<double> cov(dimension,dimension);
  return extrapolate(plane,statePred,cov);
}

void AbsTrackRep::Abort(std::string method){
  std::cerr << method <<  " as implemented in " << __FILE__ 
	    << " was called. This means that this feature was used "
	    << "in a track rep which didnt overwrite this method. "
	    << std::endl << "C++ throw;" << std::endl;
  //system call abort
  throw;
}

void AbsTrackRep::extrapolateToPoca(const TVector3& point,
				    TVector3& poca,
				    TVector3& dirInPoca){
  Abort("extrapolateToPoca()");
}

void AbsTrackRep::extrapolateToLine(const TVector3& point1, 
				    const TVector3& point2,
				    TVector3& poca,
				    TVector3& dirInPoca){
  Abort("extrapolateToLine()");
}
  

void AbsTrackRep::stepalong(double h){
  Abort("stepalong()");
}

void AbsTrackRep::setReferencePlane(const DetPlane& pl){
  _refPlane=pl;
}

void
AbsTrackRep::reset(){
  std::cout<<"AbsTrackRep::reset"<<std::endl;
  TVector3 nullVec(0.,0.,0.);
  _refPlane.set(nullVec,nullVec,nullVec);
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
