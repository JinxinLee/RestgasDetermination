//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      a cluster in the tpc
//
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef TPCCLUSTER_HH
#define TPCCLUSTER_HH

// Base Class Headers ----------------
#include "TObject.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op
#include <vector>
#include "TVector3.h"
#include "McIdCollection.h"
#include "TMatrixD.h"
#include "PndTpcDigiMapper.h"
#include "PndTpcLaserCorrectionTask.h"

// Collaborating Class Declarations --
class Track;

class PndTpcCluster : public TObject{

  friend class PndTpcLaserCorrectionTask;  

public:

  

  // Constructors/Destructors ---------
  PndTpcCluster();
  PndTpcCluster(const PndTpcCluster&);
  PndTpcCluster(const TVector3& pos, double amp, 
			 unsigned int index, unsigned int size=1);
  PndTpcCluster(const TVector3& pos, const TVector3& sig, double amp, 
			 unsigned int index, unsigned int size=1);
  
  virtual ~PndTpcCluster();

  friend std::ostream& operator<< (std::ostream& s, const PndTpcCluster& me);
  // Accessors -----------------------
  const TVector3& pos() const {return _pos;}
  const TVector3& sig() const {return _sig;}
  const TMatrixD& cov() const {return _cov;}
  TVector3 axis() const {return calcAxis();}
  double amp() const {return _amp;}
  unsigned int size() const {return _size;}
  unsigned int nPad() const {return _nPad;}
  unsigned int nPadX() const {return _nPadX;}
  unsigned int nPadY() const {return _nPadY;}
  unsigned int index() const {return _index;}
  const McIdCollection& mcId() const {return _mcid;}
  unsigned int nMcIds() const {return _mcid.nIDs();}
  double maxMcWeight() const {return _mcid.MaxRelWeight();}

  // Modifiers -----------------------
  void SetMcId(const McIdCollection& m){_mcid=m;}
  void SetIndex(unsigned int id){_index=id;}
  void SetCov(const TMatrixD& cov){_cov=cov;_hasaxis=false;}
  
  void SetIndexInTrack(int indexInTrack ) {_indexInTrack=indexInTrack; }//for spatial sorting
  int GetIndexInTrack() const {return _indexInTrack; }	//for spatial sorting
  
  void nPad(unsigned int i) {_nPad=i;}
  void nPadX(unsigned int i) {_nPadX=i;}
  void nPadY(unsigned int i) {_nPadY=i;}


  // Operations ----------------------
 
  
  unsigned int nDigi() const {
    return digis.size();
  }
  void addDigi(const PndTpcDigi& d){
    digis.push_back(d);
  }
  PndTpcDigi getDigi(int i) const{
    assert (i<digis.size());
    return digis.at(i);
  }
  
  TVector3 calcAxis() const; // calculate major axis from cluster shape

private:

  // Private Data Members ------------
  TVector3 _pos;
  TVector3 _sig; // errors
  TMatrixD _cov; // 2nd moment of cluster shape
  TVector3 _axis; // main axis of the cluster EXPERIMENTAL
  bool _hasaxis;
  double _amp;
  unsigned int _size;
  unsigned int _nPad;
  unsigned int _nPadX;
  unsigned int _nPadY;
  unsigned int _index;
  McIdCollection _mcid;
  
  int _indexInTrack;	//index in track //for spatial sorting

  //for optional saving of raw info that went into the cluster
  std::vector<PndTpcDigi> digis;

  // Private Methods -----------------

  

public:
  ClassDef(PndTpcCluster,4)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
