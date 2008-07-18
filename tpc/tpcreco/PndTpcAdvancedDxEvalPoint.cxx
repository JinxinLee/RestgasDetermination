#include "PndTpcAdvancedDxEvalPoint.h"
#include "AbsRecoHit.h"
#include "AbsTrackRep.h"
#include "Track.h"
#include "PndTpcCluster.h"

#include "TVector3.h"

PndTpcAdvancedDxEvalPoint::PndTpcAdvancedDxEvalPoint( Track *track, PndTpcCluster *cl):
_track(track), _cl(cl)
{
	_IndexInTrack=_cl->GetIndexInTrack();
	_EnergyLoss=cl->amp();
	_Pos=new TVector3(cl->pos());
	_IndexInTrack=cl->GetIndexInTrack();
}

PndTpcAdvancedDxEvalPoint::PndTpcAdvancedDxEvalPoint( Track *track, TVector3 *pos,unsigned int IndexInTrack, double dE): 
_track(track), _IndexInTrack(IndexInTrack), _EnergyLoss(dE), _Pos(NULL)
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
	AbsRecoHit* FirstHit=_track->getHit( GetIndexInTrack() );
	AbsRecoHit* SecondHit=_track->getHit( p->GetIndexInTrack() );

	AbsTrackRep* rep=_track->getCardinalRep()->clone();	// is this a memory leak?
	rep->extrapolate(FirstHit->getDetPlane(rep));
	
	// i assume that it makes no difference if i get the distance
	// from hit1 to hit2 or from hit 2 to hit 1
	double distance=rep->extrapolate(SecondHit->getDetPlane(rep));

	return distance;
}

