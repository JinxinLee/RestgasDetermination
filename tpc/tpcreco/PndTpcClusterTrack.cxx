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
#include "GFAbsRecoHit.h"
#include "GFAbsTrackRep.h"
#include "GFTrack.h"
#include "GFDetPlane.h"

#include <iostream>
using namespace std;

bool 
PndTpcClusterTrack::operator()(PndTpcCluster* s1, PndTpcCluster* s2) const
{
	//GFAbsRecoHit* firstHit=_track->getHit(0);
	TVector3 o(0,0,0);
	TVector3 u(1,0,0);
	TVector3 v(0,1,0);
	GFDetPlane detPlane(o,u,v);
	
	GFAbsRecoHit* s1Hit=_track->getHit(s1->GetIndexInTrack());
	GFAbsRecoHit* s2Hit=_track->getHit(s2->GetIndexInTrack());
		
	GFAbsTrackRep* rep=_track->getCardinalRep()->clone();
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
	GFDetPlane s1Plane(s1Pos, u, v);
	lengthS1=rep->extrapolate(s1Plane);	
	
	rep=_track->getCardinalRep()->clone();	
	rep->extrapolate(detPlane);	
	
	TVector3 s2Pos=s2->pos();
	GFDetPlane s2Plane(s2Pos, u, v);
	lengthS2=rep->extrapolate(s2Plane);		
	
	cout << "Cluster Track sorting \"NEW\" lengthS1:" << lengthS1 
	<< " lengthS2: " << lengthS2 << endl;
	*/
	return (lengthS1<=lengthS2);
}
