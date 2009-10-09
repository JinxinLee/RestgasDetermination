//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Conformal mapping track finder
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

#ifndef TPCCONFTRACKFINDER_HH
#define TPCCONFTRACKFINDER_HH

// Base Class Headers ----------------


// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op
#include <vector>
#include "TMatrixT.h"

// Collaborating Class Declarations --
class PndTpcCluster;
class GFTrackCand;
class TClonesArray;
class PndTpcConfMapRecoHit;
class GFTrack;
class TH2D;

class PndTpcConfTrackFinder {
public:

  // Constructors/Destructors ---------
  PndTpcConfTrackFinder(); 
  ~PndTpcConfTrackFinder();

  // Operations ----------------------
  void buildTracks(std::vector<PndTpcCluster*>& clusters,
		   std::vector<GFTrackCand*>& candlist);

  void buildTracks(TClonesArray* clusterArray,
		   std::vector<GFTrackCand*>& candlist);

  void configure(double xcut, double ycut, double zcut,
		 double chi2cut,
		 unsigned int minHitsForFit,
		 bool domerge,
		 double radiusMergeCut,  // fractional cut
		 double dipMergeCut,     // fractional cut
		 double distMergeCut);    

  void setConfFitResults(std::vector<TMatrixT<double> >* r,
			 std::vector<TMatrixT<double> >* c){_conffitresults=r;
			 _conffitcov=c;}

  void setMaxHitsInTrack(unsigned int m){_maxHitsInTrack=m;}
  std::vector<GFTrack*>& getTrkListRef(){return _trks;}

private:

  // Private Data Members ------------
  unsigned int _minHitsForFit;
  unsigned int _maxHitsInTrack;
  double _zcut;
  double _xcut;
  double _ycut;
  double _chi2cut;
  double _radiusMergeCut;
  double _dipMergeCut;
  double _distMergeCut;
  bool _domerge;

  std::vector<TMatrixT<double> >* _conffitresults;
  std::vector<TMatrixT<double> >* _conffitcov;
  std::vector<GFTrack*> _trks; // all tracks

  // Private Methods -----------------
  PndTpcConfTrackFinder(const PndTpcConfTrackFinder&); // prohibit copying

  bool matchHitTrack(PndTpcConfMapRecoHit* hit,
		     GFTrack* trk,
		     double& matchQuality);

  bool matchConfTrack(PndTpcConfMapRecoHit* hit,
		      GFTrack* trk,
		      double& matchQuality);
  
  bool addHit2Track(PndTpcConfMapRecoHit* hit,
		    GFTrack* trk);
  bool applyProximityCuts(PndTpcConfMapRecoHit* hit,
			  PndTpcConfMapRecoHit* hitontrk);

  double hitDist(PndTpcConfMapRecoHit* hit1,
		 PndTpcConfMapRecoHit* hit2);

  void merge(std::vector<GFTrack*>& trks);
  void mergeTracks(GFTrack* trkA,GFTrack* trkB);

};

#endif




//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
