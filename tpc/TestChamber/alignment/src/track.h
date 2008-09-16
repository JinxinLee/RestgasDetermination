
#ifndef TRACK_H
#define TRACK_H
#include <utility>
class Track{
    double z0;
    std::pair<double,double> startpoint;
    std::pair<double,double>  tangent;

public:
    std::pair<double,double> intersect(double z);
    inline double getX0(){return startpoint.first;};
    inline double getY0(){return startpoint.second;};
    inline double getTx(){return tangent.first;};
    inline double getTy(){return tangent.second;};
	
    Track(double x0, double y0, double z0, double tx0, double ty0);
};
#endif
