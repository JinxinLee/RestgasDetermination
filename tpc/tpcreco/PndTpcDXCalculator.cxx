#include "PndTpcDXCalculator.h"

#include "TVector3.h"
#include <iostream>
#include <assert.h>

#include "tpcreco/PndTpcSimpleDxEvalPoint.h"
#include "tpcreco/PndTpcAdvancedDxEvalPoint.h"
using namespace std;


PndTpcDXCalculator::PndTpcDXCalculator(GFTrack *track):_track(track), DistanceAlongTrack(true)
{

}

PndTpcDXCalculator::PndTpcDXCalculator(): _track(NULL), DistanceAlongTrack(false)
{

}


PndTpcDXCalculator::~PndTpcDXCalculator()
{
	vector<PndTpcDxEvalPoint*>::iterator it;
	vector<PndTpcDxEvalPoint*>::iterator end=_points.end();
	int i=0;
	for(it=_points.begin();it!=end; ++it)
	{
		if(*it)	{
			delete (*it);
			*it=NULL;
		}
		i++;
	}
}

void PndTpcDXCalculator::SetNextPoint(double x, double y, double z, double de, unsigned int i)
{
	TVector3 *p=new TVector3(x,y,z);	
	
	if(DistanceAlongTrack)	{
		_points.push_back(new PndTpcAdvancedDxEvalPoint(_track,p,i,de));
	}
	else	{
		_points.push_back(new PndTpcSimpleDxEvalPoint(p,i,de));
	}
}

void PndTpcDXCalculator::GetDEDX(double &de, double &dx)
{
	if( _points.size()>1 )	{
		try	{
			assert(_points[_points.size()-1]);
			assert(_points[_points.size()-2]);
			double distance=(_points[_points.size()-1])->CalculateDX((_points[_points.size()-2]));
			dx=distance;
			de=(_points[_points.size()-1])->GetDE();
			RemoveUnusedPoint();
		}
		catch(...)	{
			cout << "Caught a stupid exception!" << endl;
			de=0.;
			dx=0.;
		}
	}
	else	{
		de=0.;
		dx=0.;
	}
}

void PndTpcDXCalculator::GetCenteredDEDX(double &de, double &dx)
{
	if( _points.size() > 2 )	{
		Double_t distanceToMidPoint=(_points[_points.size()-3])->CalculateDX((_points[_points.size()-2]));
		Double_t distanceFromMidPoint=(_points[_points.size()-2])->CalculateDX((_points[_points.size()-1]));
		dx=distanceToMidPoint/2.+distanceFromMidPoint/2.;
		de=(_points[_points.size()-2])->GetDE();
		//cout << "NEW PndTpcDXCalculator centered de: " << de << " dx " << dx << endl;
		RemoveUnusedPoint();
	}
	else		{
		de=0.;
		dx=0.;
	}
}

void PndTpcDXCalculator::RemoveUnusedPoint()
{
	if(_points.size()>0)	{
		if(_points[0])	{
			delete _points[0];
			_points[0]=NULL;
			_points.erase(_points.begin());
		}
	}
}

std::ostream& operator<< (std::ostream& s, const PndTpcDXCalculator& me){	
    	for(int i=0; i<me._points.size(); i++)	{
		s << "dedx=("<< (me._points[i])->GetDE() <<","
									<<((me._points[i])->GetPosition() )->X() <<","
									<<((me._points[i])->GetPosition() )->Y() <<","
									<<((me._points[i])->GetPosition() )->Z() <<")  "
									<< "cluster index: " 
									<< (me._points[i])->GetIndexInTrack() << "\t";
									
    }
    return s;
} 
