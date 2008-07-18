#include "Coordinates.h"

std::ostream& operator<< (std::ostream& s, const Coordinates &me)
{
	if(me.size()==0)	{
		s << "Coordinates invalid!";
	}
	s << "(" << me[0];
	for(unsigned int j=1; j<me.size(); j++)
	{
		s << "," << me[j];
	}
	s << ")";
	return s;
}

Coordinates::Coordinates(double x, double y)
{
	m_Coordinates.push_back(x);
	m_Coordinates.push_back(y);
}
