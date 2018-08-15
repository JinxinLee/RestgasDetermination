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

#ifndef PLAN_HH
#define PLAN_HH

#include <iostream>
#include <vector>

#include "point.hh"
#include "line.hh"

using namespace std;

class Plan{

public:
    Plan();
    ~Plan();

    Plan(vector<Point>);
    Plan(vector<Line>);
    void SetPlan(vector<Point>);
    void SetPlan(vector<Line>);
    void AddPoints(vector<Point>);
    void AddLines(vector<Line>);
    int GetNumberPoints();
    int GetNumberLines();
    vector<Point> GetPoints();
    vector<Line> GetLinesL();
    vector<Point> GetLinesP();
    vector<Point> GetPlanP();
    vector<Line> GetPlanL();
    Point GetPerp();
    Point GetCenter();
    Angles GetAngles();
    
    void Precision();
    
    bool CheckPoint(Point);
    
private:
    int number_points;
    int number_lines;
    vector<Point> points;
    vector<Line> lines;
    vector<Point> planP;
    vector<Line> planL;
    vector<Angles> angles;
    vector<Point> perps;
    Point perp;
    Point center;
    Angles angles_perp;
    int precision;
    
    bool CheckCoplanarity();
    void compute_perps();
    void SetCoplanarity();
};

#endif
