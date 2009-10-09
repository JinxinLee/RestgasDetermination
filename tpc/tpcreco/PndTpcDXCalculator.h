#ifndef PNDTPCDXCALCULATOR
#define PNDTPCDXCALCULATOR

#include <vector>
#include <ostream>

class TVector3;
class PndTpcDxEvalPoint;
class PndTpcCluster;
class GFTrack;

class PndTpcDXCalculator
{
public:
	//use this constructor to calculate distances between clusters along a track
	PndTpcDXCalculator(GFTrack *track);
	
	//use this constructor to just take distances between clusters as they are
	//this should give reasonable results if diffusion is turned off
	PndTpcDXCalculator();
	
	~PndTpcDXCalculator();
	void SetNextPoint(const PndTpcCluster &cl);
	
	//indexing is needed when sorting along a track
	void SetNextPoint(double x, double y, double z, double de, unsigned int i=0);
	
	// *---|---*-----|-----*		=> 3 points set => 1 de/dx point
	void GetCenteredDEDX(double &de, double &dx);
	// *|------*|----------*|	=> 3 points set => 2 de/dx point
	void GetDEDX(double &de, double &dx);
	
	friend std::ostream& operator<< (std::ostream& s, const PndTpcDXCalculator& me);
private:
	std::vector<PndTpcDxEvalPoint*> _points;
	GFTrack *_track;
	bool DistanceAlongTrack;
	
	void RemoveUnusedPoint();
};

#endif
