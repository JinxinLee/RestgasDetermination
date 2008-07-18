#ifndef COORDINATES_HH
#define COORDINATES_HH

#include <vector>
#include <ostream>

class Coordinates
{
public:
	Coordinates(double x)	{m_Coordinates.push_back(x);}
	Coordinates(double x, double y);
	~Coordinates(){}
	unsigned int size()	const {return m_Coordinates.size();} 
	double operator[] (unsigned int i) const { 
		return (m_Coordinates.size()>i ? m_Coordinates[i] : 0.); }
	friend std::ostream& operator<< (std::ostream& s, const Coordinates &me);
private:
	Coordinates(){}	//for ROOT's sake
	std::vector<double> m_Coordinates;
};

#endif
