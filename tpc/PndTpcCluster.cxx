//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcCluster
//      see PndTpcCluster.h for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndTpcCluster.h"


// C/C++ Headers ----------------------
#include <iostream>
using namespace std;

// Collaborating Class Headers --------
#include "TVectorD.h"

// Class Member definitions -----------



PndTpcCluster::PndTpcCluster()
  : fpos(0,0,0), fsig(0.04,0.04,0.15),fcov(3,3),famp(0), fsize(0), fnPad(0), fnPadX(0), fnPadY(0), findex(0),findexInTrack(-1), fct(0.0)
{}

PndTpcCluster::PndTpcCluster(const PndTpcCluster& clust) 
  : fpos(clust.fpos), fsig(clust.fsig),fcov(clust.fcov), faxis(clust.faxis),famp(clust.famp),
    fhasaxis(clust.fhasaxis), fnPad(clust.fnPad), fnPadX(clust.fnPadX),fnPadY(clust.fnPadY),
    findex(clust.findex), fsize(clust.fsize),findexInTrack(clust.findexInTrack),
    fmcid(clust.fmcid), fct(clust.fct)
{ 
  for(unsigned int i=0; i<(clust.digis).size(); ++i) 
    digis.push_back(PndTpcDigi((clust.digis)[i]));
}

PndTpcCluster::PndTpcCluster(const TVector3& Pos, double Amp, 
		       unsigned int Index, unsigned int Size,double FCT)
  : fpos(Pos), fsig(0.04,0.04,0.15),fcov(3,3), famp(Amp), findex(Index), fsize(Size),findexInTrack(-1),fct(FCT)
{}

PndTpcCluster::PndTpcCluster(const TVector3& Pos, const TVector3& Sig, double Amp, 
		       unsigned int Index, unsigned int Size,double FCT)
  : fpos(Pos), fsig(Sig) ,fcov(3,3), famp(Amp), findex(Index), fsize(Size),findexInTrack(-1),fct(FCT)
{}

unsigned int 
PndTpcCluster::get2DSize() { //return the 2-dimensional size of the cluster
  std::set<unsigned int> ids;
  for(int k=0; k<digis.size(); k++) {
    unsigned int id = (digis[k]).padId();
    ids.insert(id);
  }
  return ids.size();
}

TVector3
PndTpcCluster::calcAxis() const {
  TVectorD eigenValues(3);
  TMatrixD eigenVec=fcov.EigenVectors(eigenValues);

  // eigenvalues are sorted according to their value
  // in descendign order -> first one is largest
  TVectorD a=TMatrixDColumn(eigenVec,0);
  TVector3 Axis(a[0],a[1],a[2]);
  Axis.SetMag(1);
  return Axis;
}




PndTpcCluster::~PndTpcCluster(){}

std::ostream& operator<< (std::ostream& s, const PndTpcCluster& me){
  s << "PndTpcCluster\n"
    << "pos=("<<me.fpos.X()<<","<<me.fpos.Y()<<","<<me.fpos.Z()<<")\n"
    << "amp=" << me.famp << "\n"
    << "size=" << me.fsize <<"\n"
    << "index="<< me.findex;
  return s;
} 

ClassImp(PndTpcCluster)


