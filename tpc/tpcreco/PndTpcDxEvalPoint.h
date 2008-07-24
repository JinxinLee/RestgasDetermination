#ifndef PNDTPCDXEVALPOINT_H
#define PNDTPCDXEVALPOINT_H

class Track;
class PndTpcCluster;
class TVector3;

class PndTpcDxEvalPoint
{
public:
	virtual ~PndTpcDxEvalPoint() {};
	virtual double CalculateDX(PndTpcDxEvalPoint *p)=0;
	virtual unsigned int GetIndexInTrack() const=0;
	virtual double GetDE() const=0;
	virtual TVector3 *GetPosition() const=0; 

};

#endif
