#ifndef FITPARAMS_H
#define FITPARAMS_H

#include<map>
#include<vector>
#include<iostream>

#include "TVectorT.h"
#include "TMatrixT.h"

class FitParams{
 private:

  //prefix f means forward. Forseeing that I might implement 
  //COMPASS-like smooting later with backward Kalman pass
  std::map<int, TMatrixT<double> > fStatePred;
  std::map<int, TMatrixT<double> > fStateFilt;
  std::map<int, TMatrixT<double> > stateFin;

  std::map<int, TMatrixT<double> > fCovPred;
  std::map<int, TMatrixT<double> > fCovFilt;
  std::map<int, TMatrixT<double> > covFin;

  std::map<int, TMatrixT<double> > fJacobian;

  TMatrixT<double> MCtruth;
  double           MCtruth_s;

  TMatrixT<double> startState;
  double           startState_s;

 public:
  FitParams();
  virtual ~FitParams();
  
  int getN() const {return fStatePred.size();}

  inline void setMCtruth(const TMatrixT<double> & mat) {
	MCtruth.ResizeTo(mat.GetNrows(),mat.GetNcols());
	MCtruth = mat; 
  }
  inline void setStartState(const TMatrixT<double> & mat) {
	startState.ResizeTo(mat.GetNrows(),mat.GetNcols());
	startState = mat; 
  }
  inline void setMCtruth_s(const double _s) {
	MCtruth_s = _s; 
  }
  inline void setStartState_s(const double _s) {
	startState_s = _s; 
  }

  inline TMatrixT<double> getMCtruth() {
	return MCtruth;
  }
  inline TMatrixT<double> getStartState() {
	return startState;
  }
  inline double getMCtruth_s() {
	return MCtruth_s;
  }
  inline double getStartState_s() {
	return startState_s;
  }

  inline bool addfStatePred(int index, const TMatrixT<double>& mat) {
	if(fStatePred.count(index) != 0) {
	  return false;
	}
	else {
	  fStatePred[index].ResizeTo(mat.GetNrows(),mat.GetNcols());
	  fStatePred[index] = mat;
	  return true;
	}
  }

  inline bool addfStateFilt(int index, const TMatrixT<double>& mat) {
	if(fStateFilt.count(index) != 0) {
	  return false;
	}
	else {
	  fStateFilt[index].ResizeTo(mat.GetNrows(),mat.GetNcols());
	  fStateFilt[index] = mat;
	  return true;
	}
  }

  inline bool addStateFin(int index, const TMatrixT<double>& mat) {
	if(stateFin.count(index) != 0) {
	  return false;
	}
	else {
	  stateFin[index].ResizeTo(mat.GetNrows(),mat.GetNcols());
	  stateFin[index] = mat;
	  return true;
	}
  }

  inline bool addfCovPred(int index, const TMatrixT<double>& mat) {
	if(fCovPred.count(index) != 0) {
	  return false;
	}
	else {
	  fCovPred[index].ResizeTo(mat.GetNrows(),mat.GetNcols());
	  fCovPred[index] = mat;
	  return true;
	}
  }

  inline bool addfCovFilt(int index, const TMatrixT<double>& mat) {
	if(fCovFilt.count(index) != 0) {
	  return false;
	}
	else {
	  fCovFilt[index].ResizeTo(mat.GetNrows(),mat.GetNcols());
	  fCovFilt[index] = mat;
	  return true;
	}
  }

  inline bool addCovFin(int index, const TMatrixT<double>& mat) {
	if(covFin.count(index) != 0) {
	  return false;
	}
	else {
	  covFin[index].ResizeTo(mat.GetNrows(),mat.GetNcols());
	  covFin[index] = mat;
	  return true;
	}
  }

  inline bool addfJacobian(int index, const TMatrixT<double>& mat) {
	if(fJacobian.count(index) != 0) {
	  return false;
	}
	else {
	  fJacobian[index].ResizeTo(mat.GetNrows(),mat.GetNcols());
	  fJacobian[index] = mat;
	  return true;
	}
  }

  inline bool getfStatePred(int index, TMatrixT<double>& mat) {
	if(fStatePred.count(index) == 0) {
	  return false;
	}
	else {
	  mat.ResizeTo(fStatePred[index].GetNrows(),fStatePred[index].GetNcols());
	  mat = fStatePred[index];
	  return true;
	}
  }

  inline bool getfStateFilt(int index, TMatrixT<double>& mat) {
	if(fStateFilt.count(index) == 0) {
	  return false;
	}
	else {
	  mat.ResizeTo(fStateFilt[index].GetNrows(),fStateFilt[index].GetNcols());
	  mat = fStateFilt[index];
	  return true;
	}
  }

  inline bool getStateFin(int index, TMatrixT<double>& mat) {
	if(stateFin.count(index) == 0) {
	  return false;
	}
	else {
	  mat.ResizeTo(stateFin[index].GetNrows(),stateFin[index].GetNcols());
	  mat = stateFin[index];
	  return true;
	}
  }

  inline bool getfCovPred(int index, TMatrixT<double>& mat) {
	if(fCovPred.count(index) == 0) {
	  return false;
	}
	else {
	  mat.ResizeTo(fCovPred[index].GetNrows(),fCovPred[index].GetNcols());
	  mat = fCovPred[index];
	  return true;
	}
  }

  inline bool getfCovFilt(int index, TMatrixT<double>& mat) {
	if(fCovFilt.count(index) == 0) {
	  return false;
	}
	else {
	  mat.ResizeTo(fCovFilt[index].GetNrows(),fCovFilt[index].GetNcols());
	  mat = fCovFilt[index];
	  return true;
	}
  }

  inline bool getCovFin(int index, TMatrixT<double>& mat) {
	if(covFin.count(index) == 0) {
	  return false;
	}
	else {
	  mat.ResizeTo(covFin[index].GetNrows(),covFin[index].GetNcols());
	  mat = covFin[index];
	  return true;
	}
  }

  inline bool getfJacobian(int index, TMatrixT<double>& mat) {
	if(fJacobian.count(index) == 0) {
	  return false;
	}
	else {
	  mat.ResizeTo(fJacobian[index].GetNrows(),fJacobian[index].GetNcols());
	  mat = fJacobian[index];
	  return true;
	}
  }

  inline bool modfStatePred(int index, const TMatrixT<double>& mat) {
	if(fStatePred.count(index) == 0) {
	  return false;
	}
	else {
	  fStatePred[index] = mat;
	  return true;
	}
  }

  inline bool modfStateFilt(int index, const TMatrixT<double>& mat) {
	if(fStateFilt.count(index) == 0) {
	  return false;
	}
	else {
	  fStateFilt[index] = mat;
	  return true;
	}
  }

  inline bool modStateFin(int index, const TMatrixT<double>& mat) {
	if(stateFin.count(index) == 0) {
	  return false;
	}
	else {
	  stateFin[index] = mat;
	  return true;
	}
  }
  
  inline bool modfCovPred(int index, const TMatrixT<double>& mat) {
	if(fCovPred.count(index) == 0) {
	  return false;
	}
	else {
	  fCovPred[index] = mat;
	  return true;
	}
  }

  inline bool modfCovFilt(int index, const TMatrixT<double>& mat) {
	if(fCovFilt.count(index) == 0) {
	  return false;
	}
	else {
	  fCovFilt[index] = mat;
	  return true;
	}
  }

  inline bool modCovFin(int index, const TMatrixT<double>& mat) {
	if(covFin.count(index) == 0) {
	  return false;
	}
	else {
	  covFin[index] = mat;
	  return true;
	}
  }

  inline bool modfJacobian(int index, const TMatrixT<double>& mat) {
	if(fJacobian.count(index) == 0) {
	  return false;
	}
	else {
	  fJacobian[index] = mat;
	  return true;
	}
  }


  ClassDef(FitParams,1)

};





#endif //FITPARAMS
