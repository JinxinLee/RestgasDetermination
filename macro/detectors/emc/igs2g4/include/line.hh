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

#ifndef LINE_HH
#define LINE_HH

#include <iostream>
#include <vector>

#include "point.hh"
#include "angles.hh"

using namespace std;

class Line{

public:
    Line();
    ~Line();
    
    Line(Point, Point);
    Line(vector<Point>);
    void SetPoints(Point, Point);
    void SetPoints(vector<Point>);
    int GetNumberPoints();
    vector<Point> GetPoints();
    vector<Point> GetLine();
    
    void Precision();

private:
    int number_points;
    int precision;
    vector<Point> points;
    vector<Point> coliniar_points;
    vector<Angles> vangles;
    
    bool CheckColiniarity();
    void SetColiniarity();
};

#endif
