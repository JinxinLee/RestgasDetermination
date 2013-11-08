/**********************************************************
 Description: Class PndAnaCovTool
 Taken from Base class for particles used for Vertex fitting
 Authors: Dipak K. Mishra
          R. Kliemt (taking away the cov converters)
***********************************************************/

#include "PndAnaCovTool.h"

PndAnaCovTool::PndAnaCovTool()
{}

PndAnaCovTool::~PndAnaCovTool()
{}

/*
Convert the Cov matrices from x,y,z,px,py,pz ==> px,py,pz,x,y,z
or vice versa*/
TMatrixD PndAnaCovTool::GetConverted6(TMatrixD tmpMat)const
{
  TMatrixD cMat(6,6);
  for(Int_t i=0; i<6; i++) {
    for(Int_t j=0; j<6; j++) {
      if(i>=3) {
        if(j>=3) { cMat[i-3][j-3] = tmpMat[i][j]; }
        else { cMat[i-3][j+3] = tmpMat[i][j]; }
      } else {
        if(j>=3) { cMat[i+3][j-3] = tmpMat[i][j]; }
        else { cMat[i+3][j+3] = tmpMat[i][j]; }
      }
    }
  }
  return cMat;
}
/*
Convert the Cov matrices from x,y,z,px,py,pz ==> px,py,pz,x,y,z
or vice versa*/
TMatrixDSym PndAnaCovTool::GetConverted6(TMatrixDSym tmpMat)const
{
  TMatrixDSym cMat(6);
  for(Int_t i=0; i<6; i++) {
    for(Int_t j=0; j<6; j++) {
      if(i>=3) {
        if(j>=3) { cMat[i-3][j-3] = tmpMat[i][j]; }
        else { cMat[i-3][j+3] = tmpMat[i][j]; }
      } else {
        if(j>=3) { cMat[i+3][j-3] = tmpMat[i][j]; }
        else { cMat[i+3][j+3] = tmpMat[i][j]; }
      }
    }
  }
  return cMat;
}
//convert cov matrices x,y,z,px,py,pz,E (7x7) ==> px,py,pz,E,x,y,z (7x7)
TMatrixDSym PndAnaCovTool::GetCovMat1(TMatrixD tmpMat)const
{
  TMatrixDSym cMat(7);
  for(Int_t i=0; i<7; i++) {
    for(Int_t j=0; j<7; j++) {
      if(i>=3) {
        if(j>=3) { cMat[i-3][j-3] = tmpMat[i][j]; }
        else { cMat[i-3][j+3] = tmpMat[i][j]; }
      } else {
        if(j>=3) { cMat[i+4][j-3] = tmpMat[i][j]; }
        else { cMat[i+4][j+4] = tmpMat[i][j]; }
      }
    }
  }
  return cMat;
}

// Convert the Cov matrices from px,py,pz,E,x,y,z ==> x,y,z,px,py,pz,E
TMatrixD PndAnaCovTool::GetConverted7(TMatrixD tmpMat)const
{
  TMatrixD cMat(7,7);
  for(Int_t i=0; i<7; i++) {
    for(Int_t j=0; j<7; j++) {
      if(i>=4) {
        if(j>=4) { cMat[i-4][j-4] = tmpMat[i][j]; }
        else { cMat[i-4][j+3] = tmpMat[i][j]; }
      } else {
        if(j>=4) { cMat[i+3][j-4] = tmpMat[i][j]; }
        else { cMat[i+3][j+3] = tmpMat[i][j]; }
      }
    }
  }
  return cMat;
}

// Convert x,y,z,px,py,pz,E (7x7) ==> px,py,pz,x,y,z (6x6)
TMatrixDSym PndAnaCovTool::GetCovMat(TMatrixD tmpMat)const
{
  TMatrixDSym cMat(6);
  for(Int_t i=0; i<6; i++) {
    for(Int_t j=0; j<6; j++) {
      if(i>=3) {
        if(j>=3) { cMat[i-3][j-3] = tmpMat[i][j]; }
        else { cMat[i-3][j+3] = tmpMat[i][j]; }
      } else {
        if(j>=3) { cMat[i+3][j-3] = tmpMat[i][j]; }
        else { cMat[i+3][j+3] = tmpMat[i][j]; }
      }
    }
  }
  return cMat;
}

//converting 7x7 cov matrix to 6x6 matrix
TMatrixDSym PndAnaCovTool::GetFitError(TMatrixDSym e) const
{
  TMatrixDSym err(6);
  for(unsigned i=0; i<3; ++i) {
    for(unsigned j=i; j<3; ++j) {
      err[i][j] = err[j][i] = e[i][j];
      err[3+i][3+j] = err[3+j][3+i] = e[4+i][4+j];
    }
  }
  for(unsigned i=0; i<3; ++i) {
    for(unsigned j=0; j<3; ++j) {
      err[i][3+j] = err[3+j][i]  = e[i][4+j];
    }
  }
  return err;
}

// converting 6x6 cov matrices to 7x7
TMatrixD PndAnaCovTool::GetFitError(TLorentzVector p,TMatrixD e) const
{
//    Error(6x6,e) ==> Error(7x7,output(hsm)) using Momentum(p).
  TMatrixD hsm(7,7);
  for(unsigned i=0; i<3; ++i) {
    for(unsigned j=i; j<3; ++j) {
      hsm[i][j] = hsm[j][i] = e[i][j];
      hsm[4+i][4+j] = hsm[4+j][4+i] = e[3+i][3+j];
    }
  }
  for(unsigned i=0; i<3; ++i) {
    for(unsigned j=0; j<3; ++j) {
      hsm[i][4+j] = hsm[4+j][i] = e[i][3+j];
    }
  }
  double invE = 1./p.E();
  hsm[0][3] = hsm[3][0] = (p.X()*hsm[0][0]+p.Y()*hsm[0][1]+p.Z()*hsm[0][2])*invE;
  hsm[1][3] = hsm[3][1] = (p.X()*hsm[0][1]+p.Y()*hsm[1][1]+p.Z()*hsm[1][2])*invE;
  hsm[2][3] = hsm[3][2] = (p.X()*hsm[0][2]+p.Y()*hsm[1][2]+p.Z()*hsm[2][2])*invE;
  hsm[3][3] = (p.X()*p.X()*hsm[0][0]+p.Y()*p.Y()*hsm[1][1]+p.Z()*p.Z()*hsm[2][2]
               +2.0*p.X()*p.Y()*hsm[0][1]
               +2.0*p.X()*p.Z()*hsm[0][2]
               +2.0*p.Y()*p.Z()*hsm[1][2])*invE*invE;
  hsm[3][4] = hsm[4][3] = (p.X()*hsm[0][4]+p.Y()*hsm[1][4]+p.Z()*hsm[2][4])*invE;
  hsm[3][5] = hsm[5][3] = (p.X()*hsm[0][5]+p.Y()*hsm[1][5]+p.Z()*hsm[2][5])*invE;
  hsm[3][6] = hsm[6][3] = (p.X()*hsm[0][6]+p.Y()*hsm[1][6]+p.Z()*hsm[2][6])*invE;
  return hsm;
}

ClassImp(PndAnaCovTool)
