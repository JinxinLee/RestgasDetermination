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
  : _pos(0,0,0), _sig(0.1,0.1,0.01),_cov(3,3),_amp(0), _size(0), _nPad(0), _nPadX(0), _nPadY(0), _index(0),_indexInTrack(-1)
{}

PndTpcCluster::PndTpcCluster(const PndTpcCluster& clust) 
  : _pos(clust._pos), _sig(clust._sig),_cov(clust._cov), _axis(clust._axis),_amp(clust._amp),
    _hasaxis(clust._hasaxis), _nPad(clust._nPad), _nPadX(clust._nPadX),_nPadY(clust._nPadY),
    _index(clust._index), _size(clust._size),_indexInTrack(clust._indexInTrack),
    _mcid(clust._mcid)
{ 
  for(unsigned int i=0; i<(clust.digis).size(); ++i) 
    digis.push_back(PndTpcDigi((clust.digis)[i]));
}

PndTpcCluster::PndTpcCluster(const TVector3& pos, double amp, 
		       unsigned int index, unsigned int size)
  : _pos(pos), _sig(0.1,0.1,0.01),_cov(3,3), _amp(amp), _index(index), _size(size),_indexInTrack(-1)
{}

PndTpcCluster::PndTpcCluster(const TVector3& pos, const TVector3& sig, double amp, 
		       unsigned int index, unsigned int size)
  : _pos(pos), _sig(sig) ,_cov(3,3), _amp(amp), _index(index), _size(size),_indexInTrack(-1)
{}


TVector3
PndTpcCluster::calcAxis() const {
  TVectorD eigenValues(3);
  TMatrixD eigenVec=_cov.EigenVectors(eigenValues);

  // eigenvalues are sorted according to their value
  // in descendign order -> first one is largest
  TVectorD a=TMatrixDColumn(eigenVec,0);
  TVector3 axis(a[0],a[1],a[2]);
  axis.SetMag(1);
  return axis;
}




PndTpcCluster::~PndTpcCluster(){}

std::ostream& operator<< (std::ostream& s, const PndTpcCluster& me){
  s << "PndTpcCluster\n"
    << "pos=("<<me._pos.X()<<","<<me._pos.Y()<<","<<me._pos.Z()<<")\n"
    << "amp=" << me._amp << "\n"
    << "size=" << me._size <<"\n"
    << "index="<< me._index;
  return s;
} 

ClassImp(PndTpcCluster)


