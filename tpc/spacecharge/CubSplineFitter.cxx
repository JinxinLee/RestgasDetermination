//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class CubSplineFitter
//      see CubSplineFitter.h for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Felix Boehmer        TUM            (original author)
//
//
//-----------------------------------------------------------


// Panda Headers ----------------------

// This Class' Header ------------------
#include "CubSplineFitter.h"

// C/C++ Headers ----------------------
#include <iostream>

// Collaborating Class Headers --------
#include "BSpline.h"
#include "TVectorD.h"
#include "TError.h"

// Class Member definitions -----------

CubSplineFitter::CubSplineFitter(CubSpline* CSP,
				 const std::vector<std::vector<double>*>* data)
  
{  
  _CSP = CSP;
  _data = data;
  _Q=NULL;
  _R=NULL;
  _newmat=NULL;
  _decomp=false;
  _emptycols=false;
  
  int NData = _data->size();   //# of data points
  int lSx = _CSP->_M.size();  //# of knots - 4
  _mData = TArrayD(lSx*NData);
  std::cout<<"\n_mData initialised with length "<<_mData.fN<<std::endl;

  //make sure data has the proper format:
  for(int e=0; e<NData; e++)
    if(_data->at(e)->size() != 2)
      Fatal("CubSplineFitter::CubSplineFitter()", "wrong data format!");
  
  //TO DO: resolve sorting on the data:
  
  //Building the matrix:
  std::cout<<"\n\nCubSplineFitter - Building Spline Matrix: "<<std::endl;
  for(int r=0; r<NData; r++) {
    if(r%10==0) {
      std::cout<<"* ";
      std::cout.flush();
    }
    double x_r = _data->at(r)->at(0);  //coordinate is the 0. entry
        
    for(int i=0; i<lSx; i++) {
      _mData[r*lSx + i]=_CSP->_M[i]->eval(x_r);
    }
  }
  
  _matrix = new TMatrixD(NData, lSx);
  _matrix->SetMatrixArray(_mData.GetArray()); //fills the matrix

  _matrix->Print();
  std::cout<<"\nSpline Matrix initialised."<<std::endl;
  std::cout.flush();

 
  std::cout<<"\nChecking for empty rows..."<<std::endl;
  std::cout.flush();
     
  //check for empty rows:
  for(int j=0; j<lSx; j++) {
    double sum = 0;
    for(int i=0; i<NData; i++)
      sum+=(*_matrix)[i][j];
    if(sum==0)
      _rc.push_back(j);
  }
  std::cout<<"\nFound "<<_rc.size()<<" empty rows"<<std::endl;
  
  if (_rc.size()>0) {
    _emptycols=true;
    //build a std::vector out of the matrix data
    std::cout<<"\nRemoving empty rows ... "<<std::endl;
    std::vector<double> temp;
    for(int l=0; l<lSx*NData; l++)
      temp.push_back(_mData[l]);
    
    //remove all columns listed in _rc
    for(int m=0; m<_rc.size(); m++) {
      int index = _rc[m];
      for(int p=NData-1;p>=0; p--) {
	std::vector<double>::iterator the_iterator = temp.begin();
	for(int l=0; l<p*(lSx-m) + index; l++)
	  the_iterator++;
	temp.erase(the_iterator);
      }
    }

    //build new matrix without empty columns:
    TArrayD temp_arr = TArrayD(temp.size());
    for(int i=0; i<temp.size(); i++)
      temp_arr[i] = temp[i];
    _newmat = new TMatrixD(NData, lSx - _rc.size());
    _newmat->SetMatrixArray(temp_arr.GetArray());
  }    
    
}


CubSplineFitter::~CubSplineFitter() {} //TODO: delete stuff


void
CubSplineFitter::decompose(double TOL) {
  
  
  if(_emptycols)
    _householder = new TDecompQRH(*_newmat, TOL);
  else
    _householder = new TDecompQRH(*_matrix, TOL);
  if(_householder->Decompose()) {
    std::cout<<"CubSplineFitter::decompose(): decomposition successful!"
	     <<std::endl;
    _Q = &(_householder->GetQ());
    _R = &(_householder->GetR());
    _Up = &(_householder->GetUp());
    _W = &(_householder->GetW());
  }
  _decomp=true;
}
  
TVectorD
CubSplineFitter::solve(const TVectorD& val) {
  if(_decomp==false) 
    decompose();
  TVectorD copy = TVectorD(val);
  if(_householder->Solve(copy)) {
    std::cout<<"CubSplineFitter::solve(): found a solution"<<std::endl;
    std::cout.flush();
    return copy;
  }
  else {
    std::cout<<"CubSplineFitter::solve(): something went wrong"<<std::endl;
    std::cout.flush();
    return val;
  }  
// manual mode

//   TMatrixD f = TMatrixD(val.fN, 1, val.GetArray());
//   TMatrixD Q_t = TMatrixD(*_Q);
//   Q_t.T();
//   TMatrixD v = Q_t*f;
//   TMatrixD R_i = TMatrixD(*_R);
//   R_i.Invert();
//   TMatrixD gamma = R_i*v; 
//   TArrayD sol = TArrayD(R_i.GetNcols(),gamma.GetMatrixArray());
//   return sol;
}
