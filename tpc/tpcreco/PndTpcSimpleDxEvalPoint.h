#ifndef PNDTPCSIMPLEDXEVALPOINT_H
#define PNDTPCSIMPLEDXEVALPOINT_H
#include "PndTpcDxEvalPoint.h"

class Track;
class PndTpcCluster;
class TVector3;

class PndTpcSimpleDxEvalPoint:public PndTpcDxEvalPoint
{
public:
	PndTpcSimpleDxEvalPoint( TVector3 *pos, unsigned int IndexInTrack, double dE );
	PndTpcSimpleDxEvalPoint( PndTpcCluster *cl);
	~PndTpcSimpleDxEvalPoint();
	double CalculateDX(PndTpcDxEvalPoint *p);
	unsigned int GetIndexInTrack() const { return _IndexInTrack; }
	TVector3 *GetPosition() const { return _Pos;	}
	double GetDE() const { return _EnergyLoss; }
private:
	double _EnergyLoss;
	unsigned int _IndexInTrack;
	TVector3 *_Pos;
};

#endif


