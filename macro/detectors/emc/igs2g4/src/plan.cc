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

#include "include/plan.hh"
#include "include/point.hh"
#include "include/line.hh"
#include "include/algebra.hh"
#include "include/math.hh"

using namespace std;

Plan::Plan()
{
}

Plan::~Plan()
{
}

Plan::Plan(vector<Point> pts)
{
    Plan::number_points = pts.size();
    for(int i=0; i<pts.size(); i++)
	Plan::points.push_back(pts[i]);
    Plan::SetCoplanarity();
}

Plan::Plan(vector<Line> lns)
{
    Plan::number_lines = lns.size();
    Plan::number_points = 0;
    for(int i=0; i<lns.size(); i++) {
	Plan::lines.push_back(lns[i]);
	Plan::AddPoints(lns[i].GetPoints());
    };
    Plan::SetCoplanarity();
}

void Plan::SetPlan(vector<Point> pts)
{
    Plan::number_points = pts.size();
    for(int i=0; i<pts.size(); i++)
	Plan::points.push_back(pts[i]);
    Plan::SetCoplanarity();
    
    return;
}

void Plan::SetPlan(vector<Line> lns)
{
    Plan::number_lines = lns.size();
    Plan::number_points = 0;
    for(int i=0; i<lns.size(); i++) {
	Plan::lines.push_back(lns[i]);
	Plan::AddPoints(lns[i].GetPoints());
    };
    Plan::SetCoplanarity();
}

void Plan::AddPoints(vector<Point> pts)
{
    Plan::number_points += pts.size();
    for(int i=0; i<pts.size(); i++) points.push_back(pts[i]);
    Plan::SetCoplanarity();
}

void Plan::AddLines(vector<Line> lns)
{
    Plan::number_lines += lns.size();
    for(int i=0; i<lns.size(); i++) {
	Plan::lines.push_back(lns[i]);
	Plan::AddPoints(lns[i].GetPoints());
    };
    Plan::SetCoplanarity();
}

int Plan::GetNumberPoints()
{
    return Plan::number_points;
}

int Plan::GetNumberLines()
{
    return Plan::number_lines;
}

vector<Point> Plan::GetPoints()
{
    return Plan::points;
}

vector<Point> Plan::GetLinesP()
{
    return Plan::points;
}

vector<Line> Plan::GetLinesL()
{
    return Plan::lines;
}

vector<Point> Plan::GetPlanP()
{
    return Plan::planP;
}

vector<Line> Plan::GetPlanL()
{
    return Plan::planL;
}

Point Plan::GetPerp()
{
    return Plan::perp;
}

Point Plan::GetCenter()
{
    return Plan::center;
}

bool Plan::CheckPoint(Point P1)
{
    bool cp = false;
    Algebra alg;
    Math math;
    
    if(math.Precision(math.Abs(alg.scalar_product(Line(center,P1),Line(Point(0.0,0.0,0.0),Plan::perp))),Plan::precision)==0.0) cp = true;
    
    return cp;
}

bool Plan::CheckCoplanarity()
{
    bool cp = false;
    
    Plan::Precision();
    switch (Plan::points.size()) {
	case 0:
	case 1:
	case 2:
	    cout<<"Error: No plan inserted for coplanarity check!"<<endl;
	    cout<<"Action: Abord the program!"<<endl;
	    exit(1);
	    break;
	case 3:
	    cp = true;
	    Plan::compute_perps();
	    break;
	default:
	    Plan::compute_perps();
	    cp = true;
	    for(int i1=0; i1<Plan::perps.size()-1; i1++)
	    for(int i2=i1+1; i2<Plan::perps.size(); i2++) 
	    if(Plan::perps[i1]!=Plan::perps[i2]) cp = false;
    };
    
    return cp;
}

void Plan::compute_perps()
{
    Algebra alg;
    Point point;
    
    for(int l1=0; l1<Plan::number_points-2; l1++)
    for(int l2=l1+1; l2<Plan::number_points-1; l2++)
    for(int l3=l2+1; l3<Plan::number_points; l3++){
	point = Point(alg.cross_product(Line(Plan::points[l1],Plan::points[l2]),
					Line(Plan::points[l2],Plan::points[l3])));
//	point = point/alg.Magnitude(Line(Point(0.0,0.0,0.0),point));
	if(point.GetZ()<0.0) 
	    point = point*(-1.0);
	Plan::perps.push_back(point);
    };
    
    return;
}

void Plan::SetCoplanarity()
{
    Point p;
    Algebra alg;
    Math math;
    int contor = 0;
    int loop = 0;
    vector<Point> vp;
    
    Plan::center = Point(0.0,0.0,0.0);
    for(int i=0; i<Plan::number_points; i++) 
	Plan::center = Plan::center + Plan::points[i]/static_cast<double>(Plan::number_points);
//cout<<"-->"<<Plan::center.GetX()<<"; "<<Plan::center.GetY()<<"; "<<Plan::center.GetY()<<endl;
    if(Plan::CheckCoplanarity()) {
	Plan::perp = Plan::perps[0];
    } else {
	for(int i=0; i<Plan::perps.size(); i++) Plan::perp = Plan::perp + Plan::perps[i];
//	for(int i=0; i<Plan::perps.size(); i++) Plan::perp = Plan::perp + Plan::perps[i]/static_cast<double>(Plan::perps.size());
    };
    Plan::angles_perp = Angles(perp.GetPoint());
}

void Plan::Precision()
{
    Math math;
    
    Plan::precision = math.precision;
}
