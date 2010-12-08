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
#include "FairHit.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op
#include <vector>
#include "TVector3.h"
#include "McIdCollection.h"
#include "TMatrixD.h"
#include "PndTpcDigiMapper.h"

// Collaborating Class Declarations --
class GFTrack;

class PndTpcCluster : public FairHit {

  friend class PndTpcLaserCorrectionTask;  

public:

  

  // Constructors/Destructors ---------
  PndTpcCluster();
  PndTpcCluster(const PndTpcCluster&);
  PndTpcCluster(const TVector3& Pos, double Amp, 
		unsigned int Index, unsigned int Size=1);
  PndTpcCluster(const TVector3& Pos, const TVector3& Sig, double Amp, 
		unsigned int Index, unsigned int Size=1);
  
  virtual ~PndTpcCluster();

  friend std::ostream& operator<< (std::ostream& s, const PndTpcCluster& me);
  // Accessors -----------------------
  const TVector3& pos() const {return fpos;}
  const TVector3& sig() const {return fsig;}
  const TMatrixD& cov() const {return fcov;}
  TVector3 axis() const {return calcAxis();}
  double amp() const {return famp;}
  unsigned int size() const {return fsize;}
  unsigned int nPad() const {return fnPad;}
  unsigned int nPadX() const {return fnPadX;}
  unsigned int nPadY() const {return fnPadY;}
  unsigned int index() const {return findex;}
  unsigned int get2DSize();
  const McIdCollection& mcId() const {return fmcid;}
  unsigned int nMcIds() const {return fmcid.nIDs();}
  double maxMcWeight() const {return fmcid.MaxRelWeight();}

  // Modifiers -----------------------
  void SetMcId(const McIdCollection& m){fmcid=m;}
  void SetIndex(unsigned int id){findex=id;}
  void SetCov(const TMatrixD& Cov){fcov=Cov;fhasaxis=false;}
  
  void SetIndexInTrack(int indexInTrack ) {findexInTrack=indexInTrack; }//for spatial sorting
  int GetIndexInTrack() const {return findexInTrack; }	//for spatial sorting
  
  void nPad(unsigned int i) {fnPad=i;}
  void nPadX(unsigned int i) {fnPadX=i;}
  void nPadY(unsigned int i) {fnPadY=i;}


  // Operations ----------------------
 
  
  unsigned int nDigi() const {
    return digis.size();
  }
  void addDigi(const PndTpcDigi& d){
    digis.push_back(d);
    AddLink(FairLink("PndTpcDigi", d.index()));
  }
  const PndTpcDigi& getDigi(int i) const{
    assert (i<digis.size());
    return digis.at(i);
  }
  
  
  
  TVector3 calcAxis() const; // calculate major axis from cluster shape

private:

  // Private Data Members ------------
  TVector3 fpos;
  TVector3 fsig; // errors
  TMatrixD fcov; // 2nd moment of cluster shape
  TVector3 faxis; // main axis of the cluster EXPERIMENTAL
  bool fhasaxis;
  double famp;
  unsigned int fsize;
  unsigned int fnPad;
  unsigned int fnPadX;
  unsigned int fnPadY;
  unsigned int findex;
  McIdCollection fmcid;
  
  int findexInTrack;	//index in track for spatial sorting

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
