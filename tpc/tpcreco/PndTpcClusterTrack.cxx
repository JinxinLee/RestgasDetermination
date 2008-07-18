//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcClusterTrack
//      see PndTpcClusterTrack.h for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Miss Piggy    TUM            (original author)
//
//
//-----------------------------------------------------------


// This Class' Header ------------------
#include "PndTpcClusterTrack.h"
#include "PndTpcCluster.h"
#include "AbsRecoHit.h"
#include "AbsTrackRep.h"
#include "Track.h"
#include "DetPlane.h"

#include <iostream>
using namespace std;

bool 
PndTpcClusterTrack::operator()(PndTpcCluster* s1, PndTpcCluster* s2) const
{
	//AbsRecoHit* firstHit=_track->getHit(0);
	TVector3 o(0,0,0);
	TVector3 u(1,0,0);
	TVector3 v(0,1,0);
	DetPlane detPlane(o,u,v);
	
	AbsRecoHit* s1Hit=_track->getHit(s1->GetIndexInTrack());
	AbsRecoHit* s2Hit=_track->getHit(s2->GetIndexInTrack());
		
	AbsTrackRep* rep=_track->getCardinalRep()->clone();
	rep->extrapolate(detPlane);
	double lengthS1=rep->extrapolate(s1Hit->getDetPlane(rep));
	
	rep=_track->getCardinalRep()->clone();	//when i do not do it twice results will change 
	rep->extrapolate(detPlane);			//a bit
	double lengthS2=rep->extrapolate(s2Hit->getDetPlane(rep));
	/*
	cout << "Cluster Track sorting \"old\" lengthS1:" << lengthS1 
		<< " lengthS2: " << lengthS2 << endl;
	*/
		/*
	rep=_track->getCardinalRep()->clone();	
	rep->extrapolate(detPlane);	
	
	TVector3 s1Pos=s1->pos();
	DetPlane s1Plane(s1Pos, u, v);
	lengthS1=rep->extrapolate(s1Plane);	
	
	rep=_track->getCardinalRep()->clone();	
	rep->extrapolate(detPlane);	
	
	TVector3 s2Pos=s2->pos();
	DetPlane s2Plane(s2Pos, u, v);
	lengthS2=rep->extrapolate(s2Plane);		
	
	cout << "Cluster Track sorting \"NEW\" lengthS1:" << lengthS1 
	<< " lengthS2: " << lengthS2 << endl;
	*/
	return (lengthS1<=lengthS2);
}
