//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Testerclass for tpcreco package
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

#ifndef TPCRECOTESTER_HH
#define TPCRECOTESTER_HH

// Base Class Headers ----------------
#include "TObject.h"

// Collaborating Class Headers -------
#include <vector>
#include <list>
#include "TVector2.h"

// Collaborating Class Declarations --
class PndTpcCluster;
class PndTpcRiemannHit;

class PndTpcRecoTester : public TObject {
public:

  // Constructors/Destructors ---------
  PndTpcRecoTester(){}
  virtual ~PndTpcRecoTester(){}

  // Operations ----------------------
  void testConfMap();
  void testConfMapFit();
  int testConfFinder();
  void testConfTrackMerge(int nevents,bool split=false, bool merge=true);
  void testConfFinderStat();
  void testBasicConfFit();
  void testRotated();
  void testStdDis(double* pars=NULL);
  void testRiemannMap();
  //void testRiemannTrack();  // not implemented
  void testRiemannFinder(unsigned int ntrk=1);
  void testRiemannFit(unsigned int ntrk=100, double r=100);
  void testRiemannSZFit(double sigma);
  void testRiemannInit();
  void testRiemannSort(double dip, double sigma, int draw=-1);
  void testRiemannSortBatch(int ntrk, double sigma);

private:
  void PlotRiemannHits(std::list<PndTpcRiemannHit*>& rhits);
  void maketoytracks(int n, std::vector<PndTpcCluster*>& cll, bool vertex=false,double sigma=0.0);
  void maketoyevent(int n,std::vector<double>& rv,
		    std::vector<TVector2>& dirv, 
		    std::vector<double>& dipv,
		    std::vector<TVector2>& startv);
  void maketoyhits(int ntrk,std::vector<double>& rv,
		   std::vector<TVector2>& dirv, 
		   std::vector<double>& dipv,
		   std::vector<TVector2>& startv,
		   std::vector<PndTpcCluster*>& cll,
		   double sigma=0.0);

  // Private Data Members ------------
 
  // Private Methods -----------------

public:
  ClassDef(PndTpcRecoTester,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
