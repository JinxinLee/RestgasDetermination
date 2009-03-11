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
  TMatrixT<double> statePred,covPred,jac;
  double retVal = extrapolate(plane,statePred,covPred,jac);
  setState(statePred);
  setCov(covPred);
  setReferencePlane(plane);
  return retVal;
}

//default implentation might be overwritten, please see the doxy docu
double AbsTrackRep::extrapolate(const DetPlane& plane, TMatrixT<double>& statePred){
  TMatrixT<double> cov,jac;
  return extrapolate(plane,statePred,cov,jac);
}

void AbsTrackRep::Abort(std::string method){
  std::cerr << method <<  " as implemented in " << __FILE__ 
	    << " was called. This means that this feature was used "
	    << "in a track rep which didnt overwrite this method. "
	    << std::endl << "Calling abort()." << std::endl;
  //system call abort
  abort();
}

TVector3 AbsTrackRep::extrapolateToPoca(const TVector3& point, 
			   TMatrixT<double>& statePred,
			   TMatrixT<double>& covPred,
			   DetPlane& planePred){
  Abort("extrapolateToPoca()");
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

DetPlane 
AbsTrackRep::getVirtualDetPlane(const TVector3& hit){
  TMatrixT<double> statePred(5,1);
  TMatrixT<double> covPred(5,5);
  DetPlane plane;
  TVector3 poca=extrapolateToPoca(hit,statePred,covPred,plane);

  /*C. Hoeppner: I am not entirely sure that these two calls are needed, but
    they dont hurt for sure. Something happens here to the orientation of
    of u and v, so keep it.*/
  TVector3 m=plane.getNormal();
  plane.setNormal(m);

  return plane;  
}


ClassImp(AbsTrackRep)
