//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Functor for comparison of pointers to PndTpcClusters
//	   that uses track information.
//	   uses slow extrapolation stuff	
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Rumpel Stielzchen TUM            (original author)
//	   Ra Punzel 		 TUM            (original author)
//
//-----------------------------------------------------------

#ifndef TPCCLUSTERTRACK_HH
#define TPCCLUSTERTRACK_HH

class PndTpcCluster;
class Track;
#include <functional>

class PndTpcClusterTrack: public std::binary_function<PndTpcCluster*,PndTpcCluster*,bool>  {
public:
	// Constructors/Destructors ---------
	PndTpcClusterTrack(Track *track):_track(track){;}
	~PndTpcClusterTrack(){;}
	
	// Operators
	bool operator()(PndTpcCluster* s1, PndTpcCluster* s2) const;
private:
	Track *_track;

};

#endif
