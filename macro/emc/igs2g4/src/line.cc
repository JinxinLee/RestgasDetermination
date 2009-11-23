/********************************************************************
* Description:
* Author: George-Catalin Serbanut <George.Serbanut@exp2.physik.uni-giessen.de>
*    
* Copyright (c) 2005 George-Catalin Serbanut  All rights reserved.
*
********************************************************************/
/*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <iostream>
#include <vector>
#include <math.h>

#include "include/line.hh"
#include "include/angles.hh"
#include "include/math.hh"

using namespace std;

Line::Line()
{
}

Line::~Line()
{
}

Line::Line(Point P1, Point P2)
{
    Line::number_points = 2;
    Line::points.push_back(P1);
    Line::points.push_back(P2);
    Line::SetColiniarity();
}

Line::Line(vector<Point> vp)
{
    switch (vp.size()) {
	case 0:
	    cout<<"Warning: Not enogh number of points!"<<endl;
	    cout<<"Action: Two points set into the origin of the axes!"<<endl;
	    Line::number_points = 2;
	    Line::points.push_back(Point(0.0,0.0,0.0));
	    Line::points.push_back(Point(0.0,0.0,0.0));
	    break;
	case 1:
	    cout<<"Warning: Not enough number of points!"<<endl;
	    cout<<"Action: Second point taken in the origin of the axes!"<<endl;
	    Line::number_points = 2;
	    Line::points.push_back(vp[0]);
	    Line::points.push_back(Point(0.0,0.0,0.0));
	    break;
	default:
	    Line::number_points = vp.size();
	    for(int i=0; i<vp.size(); i++) {
		points.push_back(vp[i]);
	    };
    };
    Line::SetColiniarity();
}

void Line::SetPoints(Point P1, Point P2)
{
    Line::number_points = 2;
    Line::points.push_back(P1);
    Line::points.push_back(P2);
    Line::SetColiniarity();
    
    return;
}

void Line::SetPoints(vector<Point> vp)
{
    switch (vp.size()) {
	case 0:
	    cout<<"Warning: Not enogh number of points!"<<endl;
	    cout<<"Action: Two points set into the origin of the axes!"<<endl;
	    Line::number_points = 2;
	    Line::points.push_back(Point(0.0,0.0,0.0));
	    Line::points.push_back(Point(0.0,0.0,0.0));
	    break;
	case 1:
	    cout<<"Warning: Not enough number of points!"<<endl;
	    cout<<"Action: Second point taken in the origin of the axes!"<<endl;
	    Line::number_points = 2;
	    Line::points.push_back(vp[0]);
	    Line::points.push_back(Point(0.0,0.0,0.0));
	    break;
	default:
	    Line::number_points = vp.size();
	    for(int i=0; i<vp.size(); i++) {
		points.push_back(vp[i]);
	    };
    };
    Line::SetColiniarity();
    
    return;
}

int Line::GetNumberPoints()
{
    return Line::number_points;
}

vector<Point> Line::GetPoints()
{
    return Line::points;
}

vector<Point> Line::GetLine()
{
    return Line::coliniar_points;
}

bool Line::CheckColiniarity()
{
    bool coliniarity = true;
    Angles angles;
    
    Line::Precision();
    if(Line::number_points>2) {
	for(int i=0; i<Line::number_points-1; i++) {
	    Point* point = new Point(Line::points[i+1]-Line::points[i]);
	    Line::vangles.push_back(Angles(point->GetPoint()));
	    delete point;
	};
	for(int i=0; i<Line::vangles.size()-1; i++)
	for(int j=i+1; j<Line::vangles.size(); j++)
	    if(vangles[i]!=vangles[j]) coliniarity = false;
    };
    
    return coliniarity;
}

void Line::SetColiniarity()
{
    Point central_point;
    double chi, psi, omega;
    
    if(Line::CheckColiniarity()) {
	for(int i=0; i<Line::points.size(); i++) 
	    coliniar_points.push_back(Line::points[i]);
    } else {
	for(int i=0; i<Line::points.size(); i++) 
	    central_point += points[i]/static_cast<int>(points.size());
	chi = 0.0;
	psi = 0.0;
	omega = 0.0;
	for(int i=0; i<Line::vangles.size(); i++) {
	    chi += vangles[i].GetChi()/static_cast<double>(vangles.size());
	    psi += vangles[i].GetPsi()/static_cast<double>(vangles.size());
	    omega += vangles[i].GetOmega()/static_cast<double>(vangles.size());
	};
	Line::coliniar_points.push_back(Point(central_point.GetX()-100.0*tan(psi),
					      central_point.GetY()-100.0*tan(psi)*tan(omega),
					      central_point.GetZ()-100.0));
	Line::coliniar_points.push_back(Point(central_point.GetX()+100.0*tan(psi),
					      central_point.GetY()+100.0*tan(psi)*tan(omega),
					      central_point.GetZ()+100.0));
    };
    
    return;
}

void Line::Precision()
{
    Math math;
    
    Line::precision = math.precision;
}
