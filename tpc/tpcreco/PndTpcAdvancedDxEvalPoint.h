#ifndef PNDTPCADVANCEDDXEVALPOINT_H
#define PNDTPCADVANCEDDXEVALPOINT_H
#include "PndTpcDxEvalPoint.h"

class Track;
class PndTpcCluster;
class TVector3;

class PndTpcAdvancedDxEvalPoint:public PndTpcDxEvalPoint
{
public:
	PndTpcAdvancedDxEvalPoint( Track *track, PndTpcCluster *cl);
	PndTpcAdvancedDxEvalPoint( Track *track, TVector3 *pos, unsigned int IndexInTrack, double dE);
	~PndTpcAdvancedDxEvalPoint();
	double CalculateDX(PndTpcDxEvalPoint *p);
	unsigned int GetIndexInTrack() const { return _IndexInTrack; }
	double GetDE() const { return _EnergyLoss; }
	TVector3 *GetPosition() const { return _Pos;	}
private:
	Track *_track;
	PndTpcCluster *_cl;
	unsigned int _IndexInTrack;
	double _EnergyLoss;
	TVector3 *_Pos;
};

#endif

