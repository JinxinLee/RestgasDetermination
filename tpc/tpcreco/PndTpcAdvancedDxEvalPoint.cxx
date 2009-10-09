#include "PndTpcAdvancedDxEvalPoint.h"
#include "GFAbsRecoHit.h"
#include "GFAbsTrackRep.h"
#include "GFTrack.h"
#include "PndTpcCluster.h"
#include <assert.h>
#include <iostream>

#include "TVector3.h"

using namespace std;

PndTpcAdvancedDxEvalPoint::PndTpcAdvancedDxEvalPoint( GFTrack *track, PndTpcCluster *cl):
_track(track), _cl(cl), _IndexInTrack(cl->GetIndexInTrack()), _EnergyLoss(cl->amp()),
_Pos(new TVector3(cl->pos()))
{

}

PndTpcAdvancedDxEvalPoint::PndTpcAdvancedDxEvalPoint( GFTrack *track, TVector3 *pos,unsigned int IndexInTrack, double dE): 
_track(track),_cl(NULL), _IndexInTrack(IndexInTrack), _EnergyLoss(dE), _Pos(pos)
{
}

PndTpcAdvancedDxEvalPoint::~PndTpcAdvancedDxEvalPoint()
{
	if(	_Pos )	{
		delete _Pos;
	}
}

double PndTpcAdvancedDxEvalPoint::CalculateDX(PndTpcDxEvalPoint *p)
{
	assert(p);
	GFAbsRecoHit* FirstHit=_track->getHit( GetIndexInTrack() );
	GFAbsRecoHit* SecondHit=_track->getHit( p->GetIndexInTrack() );
	assert(FirstHit); assert(SecondHit);
	GFAbsTrackRep* rep=NULL;
	rep=_track->getCardinalRep()->clone();
	assert(rep);
	double distance=0.;
	rep->extrapolate(FirstHit->getDetPlane(rep));
	
	// i assume that it makes no difference if i get the distance
	// from hit1 to hit2 or from hit 2 to hit 1
	distance=rep->extrapolate(SecondHit->getDetPlane(rep));

	delete rep;
	return distance;
}

