//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class BiCubSplineFitter
//      see BiCubSplineFitter.h for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Felix Boehmer        TUM            (original author)
//
//-----------------------------------------------------------


// Panda Headers ----------------------

// This Class' Header ------------------
#include "BiCubSplineFitter.h"


// C/C++ Headers ----------------------
#include <iostream>
#include <algorithm>
#include <cmath>

// Collaborating Class Headers --------
#include "BSpline.h"
#include "TVectorD.h"

// Class Member definitions -----------


/*TODO: redesign & make use of std::maps! write init method 
  instead of huge constructor to for ex. be able to turn 
  off and on error handling after constructing */


BiCubSplineFitter::BiCubSplineFitter(BiCubSpline* BCSP, 
				     const std::vector<std::vector<double>*>* data)
{  
  _BCSP = BCSP;
  _data = std::vector<std::vector<double>*>(data->size(), NULL);
  for(int i=0; i<data->size(); ++i) 
    _data[i] = new std::vector<double>(*(data->at(i)));
  _emptycols=false;
  _newmat=NULL;
   
  unsigned int NData = _data.size();   //# of data points
  unsigned int lSx = _BCSP->_M.size();  //# of knots - 4
  unsigned int lSy = _BCSP->_N.size();
  unsigned int prod = lSx*lSy;          //will be used a lot ...
  _mData = TArrayD(prod*NData);
  
  std::cout<<"\nBiCubSplineFitter: Initialising -------------------------------"
  		   <<"----------"<<std::endl;
  
  //test if data has correct format:
  for(int i=0; i<NData; i++) {
    if(_data.at(i)->size() == 4)
      continue;
    else
      Warning("BiCubSplineFitter:", "Wrong data format! Aborting...");	
  }
  
  //resolve sorting on the data:
  std::cout<<"\nBiCubSplineFitter: Sorting the data... "<<std::endl;
  sortData();
  
  //Building the matrix:
  std::cout<<"\nBiCubSplineFitter: Building Spline Matrix ("<<NData<<" x "
  		   <<prod<<"): "<<std::endl<<"                   ";
  
  int count=0;    //testing
  for(int r=0; r<NData; r++) { //loop over rows
    if(r%(NData/10)==0) {
      std::cout<<"* ";
      std::cout.flush();
    }
    double x_r = _data.at(r)->at(0);
    double y_r = _data.at(r)->at(1);
    double sigma_r = _data.at(r)->at(3);
    if(sigma_r < 1e-10) {
      sigma_r = 1;	
      count++;
    }

    for(int j=0; j<lSy; j++) {        //loop over 
      for(int i=0; i<lSx; i++) {      //columns 
    	int index = (j*lSx+i)+ r*prod;
	//TODO: implement proper error handling
    	_mData[index]=_BCSP->_M[i]->eval(x_r)*_BCSP->_N[j]->eval(y_r)*1/sigma_r;
      }
    }
  }
  
  _matrix = new TMatrixD(NData, prod, _mData.GetArray());
  std::cout<<"\n                   Spline Matrix initialised."<<std::endl;
  std::cout<<"                   "<<count<<" errors below 1e-10"<<std::endl; 
  std::cout.flush();
   
  std::cout<<"\nChecking for empty columns...";
  std::cout.flush();
  //check for empty columns:
  for(int j=0; j<prod; j++) {
    double sum = 0;
    for(int i=0; i<NData; i++)
      sum+=_mData[i*prod + j];
    if(sum==0)
      _rc.push_back(j);      
  }
  std::cout<<"Found "<<_rc.size()<<" empty columns;"<<std::endl;
  std::cout.flush();
    
  if (_rc.size()>0) {
    _emptycols=true;
    std::cout<<"Removing empty columns ... ";
    std::cout.flush();
    
    TArrayD temp = TArrayD((prod-_rc.size())*NData);  //cleaned matrix array
    int counter = 0;
    for(int j=0; j<prod; j++)  {  //loop over columns first for faster execution
      bool empty_col=false;
      for(int c=0; c<_rc.size(); c++)  //check if this column is marked empty 
        if(_rc[c]==j) {
          empty_col=true;
          break;
        }
      if(empty_col==false)
        for(int i=0; i<NData; i++)   //loop over rows
          temp[i*(prod-_rc.size()) + j-counter] =  _mData[i*prod + j];
      else {
    	counter++;
    	continue;
      }    	  	
    } //end cleaning    
        
    //build new matrix without empty columns
    _newmat = new TMatrixD(NData, prod - _rc.size());
    _newmat->SetMatrixArray(temp.GetArray());
    std::cout<<" done."<<std::endl;
    std::cout.flush();
    std::cout<<"\nBiCubSplineFitter: ready "
    		 <<"------------------------------------------------\n\n"<<std::endl;
  } 
}


BiCubSplineFitter::~BiCubSplineFitter() {
  delete _matrix;
  if(_emptycols)
    delete _newmat;
  for(int i=0; i<_data.size(); ++i) 
    delete _data[i];   
  
}


void
BiCubSplineFitter::decompose(double TOL) {
  std::cout<<"\nBiCubSplineFitter::decompose():"
	   <<" performing QR-decomposition... ";
  std::cout.flush();
  if(_emptycols)
    _householder = new TDecompQRH(*_newmat, TOL);
  else
    _householder = new TDecompQRH(*_matrix, TOL);
  if(_householder->Decompose()) {
    std::cout<<"success"<<std::endl;
    std::cout.flush();
  }
  else {
    std::cout<<"failed!"<<std::endl;
    std::cout.flush();
  }
}
  
TVectorD
BiCubSplineFitter::solve() {
  TVectorD val = TVectorD(_data.size());
  for(int k=0; k<_data.size(); k++) {
	double sigma_k = _data.at(k)->at(3);
	if(sigma_k == 0)
	 sigma_k = 1;	
    val[k] = _data.at(k)->at(2)/sigma_k;
  }
  TVectorD copy = TVectorD(val);
  std::cout<<"BiCubSplineFitter::solve(): solving... ";
  std::cout.flush();
  
  if(_householder->Solve(copy)) {
    std::cout<<"found a solution"<<std::endl;
    std::cout.flush();

    if(_emptycols) {
      std::cout<<"BiCubSplineFitter::solve(): "
      		   <<"reconstructing lost coefficients...";
      std::cout.flush();
      int lSol = copy.GetNoElements();
      std::vector<double> sol; 
      int Nfound=0;
      for (int v=0; v<lSol+_rc.size(); v++) {
	//find out if v is listed as a removed column
	bool found=false; 
	for(int i=0; i<_rc.size(); i++) {
	  if(_rc[i]==v) {
	    found = true; break;
	  }
	}
	if(found) {
	  sol.push_back(0);
	  Nfound++;
	}  
	else
	  sol.push_back(copy[v-Nfound]);
      }
      std::cout<<" done."<<std::endl;
      std::cout.flush();
      TArrayD finalsol = TArrayD(sol.size());
      for(int i=0; i<sol.size(); i++)
	finalsol[i] = sol[i];
      
      TVectorD result = TVectorD(sol.size(), finalsol.GetArray());
      return result;
    }
  }
  else {
    std::cout<<"something went wrong"<<std::endl;
    std::cout.flush();
    return val;  // nothing was done.
  }  
}

//custom compare functions for the sorting algorithm
//gives error when made member function...

bool xSort(std::vector<double>* const vec1, std::vector<double>* const vec2) {
  return (vec1->at(0) < vec2->at(0));
}


bool ySort(std::vector<double>* const vec1, std::vector<double>* const vec2) {
  return (vec1->at(1) < vec2->at(1));   //"<=" gives seg fault during execution
}


void
BiCubSplineFitter::sortData() {
  //first sort data in y direction
  std::sort(_data.begin(), _data.end(), ySort);

  //define cuts in y:
  int yCuts = (int)(double)std::sqrt((double)_data.size());
  double yCutWidth = (double) (_data.at((_data.size())-1)->at(1)-(_data.at(0))->at(1))/yCuts;
  
  int startMarker = 0;
  int endMarker = 0;
  
  //sort each y-slice in x
  for(int i=0; i<yCuts; i++) {
	startMarker=endMarker;
	while((_data.at(startMarker+endMarker))->at(1) < (i+1)*yCutWidth)
	  endMarker++;
	std::sort(_data.begin()+startMarker, _data.begin()+endMarker, xSort);
  }
    
  std::cout<<"BiCubSplineFitter::sortData(): "<<yCuts<<" cuts in y have been defined";
  std::cout<<std::endl;
  std::cout.flush();	
}



