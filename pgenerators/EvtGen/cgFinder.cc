//--------------------------------------------------------------------------
//
// Environment:
//      This software is part of the EvtGen package developed jointly
//      for the BaBar and CLEO collaborations.  If you use all or part
//      of it, please give an appropriate acknowledgement.
//
// Copyright Information: See EvtGen/COPYRIGHT
//      Copyright (C) 1998      Caltech, UCSB
//
// Module: cgFinder.cc
//
//  Description:
//
// Modification history:
//
//  August 11,2000
//------------------------------------------------------------------------
//
//
//
#include "EvtGenBase/EvtPatches.hh"
#include <stdio.h>
#include <fstream>
#include <math.h>
#include <time.h>
#include <string.h>
#include <vector>
#include "EvtGenBase/EvtOrthogVector.hh"
#include "EvtGenBase/EvtCGCoefSingle.hh"
#include "EvtGenBase/EvtdFunction.hh"
#include "EvtGenBase/EvtReport.hh"
using std::endl;
using std::fstream;

int main(int argc, char* argv[]){


  report(INFO,"EvtGen") << "First test the Vector..." << endl;
  std::vector<double> tempT;
  tempT.push_back(1.0);
  report(INFO,"EvtGen") << tempT[0] << endl;
  tempT.push_back(2.2);
  report(INFO,"EvtGen") << tempT[0] << " " << tempT[1]  << endl;
  tempT.push_back(3.0);
  report(INFO,"EvtGen") << tempT[0] << " " << tempT[1] << " "<< tempT[2] << endl;

  report(INFO,"EvtGen") << "EvtdFunction:"<<EvtdFunction::d(6,2,0,acos(0.9999))<<endl;
  report(INFO,"EvtGen") << "EvtdFunction:"<<EvtdFunction::d(6,2,0,acos(0.5))<<endl;
  report(INFO,"EvtGen") << "EvtdFunction:"<<EvtdFunction::d(6,2,0,acos(0.0))<<endl;


  double temp[5];
  int dim=5;

  temp[0]=1.0; temp[1]=2.0; temp[2]=1.0; temp[3]=0.0;temp[4]=3.0;
  int i;
  std::vector<double> vect0;
  for (i=0; i<dim; i++) vect0.push_back(temp[i]);

  temp[0]=3.0; temp[1]=2.0; temp[2]=1.0; temp[3]=0.0;temp[4]=1.0;
  std::vector<double> vect1;
  for (i=0; i<dim; i++) vect1.push_back(temp[i]);


  temp[0]=6.0; temp[1]=7.0; temp[2]=8.0; temp[3]=5.0;temp[4]=1.0;
  std::vector<double> vect2;
  for (i=0; i<dim; i++) vect2.push_back(temp[i]);

  temp[0]=3.0; temp[1]=1.0; temp[2]=2.0; temp[3]=1.0;temp[4]=0.0;
  std::vector<double> vect3;
  for (i=0; i<dim; i++) vect3.push_back(temp[i]);

  std::vector<double> vectors[4]={vect0,vect1,vect2,vect3};

  EvtOrthogVector getOrth(dim,vectors);

  std::vector<double> orth=getOrth.getOrthogVector();
  report(INFO,"EvtGen") << "orth 0="<<orth[0]<<endl;
  report(INFO,"EvtGen") << "orth 1="<<orth[1]<<endl;
  report(INFO,"EvtGen") << "orth 2="<<orth[2]<<endl;
  report(INFO,"EvtGen") << "orth 3="<<orth[3]<<endl;
  report(INFO,"EvtGen") << "orth 4="<<orth[4]<<endl;

  EvtCGCoefSingle cg1(4,4);
  EvtCGCoefSingle cg2(2,2);
  EvtCGCoefSingle cg3(1,1);
  EvtCGCoefSingle cg4(4,3);
  EvtCGCoefSingle cg5(8,9);
  
  report(INFO,"EvtGen") <<"cg1.coef(6,6,4,4,4,2):"<<cg1.coef(6,6,4,4,4,2)<<endl;
  report(INFO,"EvtGen") <<"cg1.coef(6,6,4,4,2,4):"<<cg1.coef(6,6,4,4,2,4)<<endl<<endl;

  report(INFO,"EvtGen") <<"cg1.coef(4,4,4,4,4,0):"<<cg1.coef(4,4,4,4,4,0)<<endl;
  report(INFO,"EvtGen") <<"cg1.coef(4,4,4,4,2,2):"<<cg1.coef(4,4,4,4,2,2)<<endl;
  report(INFO,"EvtGen") <<"cg1.coef(4,4,4,4,0,4):"<<cg1.coef(4,4,4,4,0,4)<<endl<<endl<<endl;


  report(INFO,"EvtGen") <<"cg2.coef(2,2,2,2,2,0):"<<cg2.coef(2,2,2,2,2,0)<<endl;
  report(INFO,"EvtGen") <<"cg2.coef(2,2,2,2,0,2):"<<cg2.coef(2,2,2,2,0,2)<<endl<<endl<<endl;


  report(INFO,"EvtGen") <<"cg3.coef(0,0,1,1,1,-1):"<<cg3.coef(0,0,1,1,1,-1)<<endl;
  report(INFO,"EvtGen") <<"cg3.coef(0,0,1,1,-1,1):"<<cg3.coef(0,0,1,1,-1,1)<<endl<<endl<<endl;

  report(INFO,"EvtGen") <<"cg4.coef(1,1,4,3,4,-3):"<<cg4.coef(1,1,4,3,4,-3)<<endl;
  report(INFO,"EvtGen") <<"cg4.coef(1,1,4,3,2,-1):"<<cg4.coef(1,1,4,3,2,-1)<<endl;
  report(INFO,"EvtGen") <<"cg4.coef(1,1,4,3,0,1):"<<cg4.coef(1,1,4,3,0,1)<<endl;
  report(INFO,"EvtGen") <<"cg4.coef(1,1,4,3,-2,3):"<<cg4.coef(1,1,4,3,-2,3)<<endl<<endl<<endl;

  report(INFO,"EvtGen") <<"cg5.coef(1,1,8,9,4,-3):"<<cg5.coef(1,1,8,9,4,-3)<<endl;

  return 1;
}






