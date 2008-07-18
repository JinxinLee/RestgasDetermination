#include "PndTpcSimpleDxEvalPoint.h"
#include "PndTpcCluster.h"
#include "TVector3.h"
#include <assert.h>


PndTpcSimpleDxEvalPoint::PndTpcSimpleDxEvalPoint( PndTpcCluster *cl)
{
	_EnergyLoss=cl->amp();
	_Pos=new TVector3(cl->pos());
	//_Pos=cl;
	_IndexInTrack=cl->GetIndexInTrack();
}

PndTpcSimpleDxEvalPoint::PndTpcSimpleDxEvalPoint(TVector3 *pos, unsigned int IndexInTrack, double dE ): _IndexInTrack(IndexInTrack), _EnergyLoss(dE), _Pos(pos)
{
}

PndTpcSimpleDxEvalPoint::~PndTpcSimpleDxEvalPoint()	{
	if(	_Pos )	{
		delete _Pos;
	}
}

double PndTpcSimpleDxEvalPoint::CalculateDX(PndTpcDxEvalPoint *p)
{
	TVector3 *TheOtherPoint=p->GetPosition();
	assert(TheOtherPoint);
	TVector3 VectorBetweenPoints=(*_Pos)-(*TheOtherPoint);
	double distance=VectorBetweenPoints.Mag();	
	return distance;
}
