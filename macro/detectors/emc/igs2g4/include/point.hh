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

#ifndef POINT_HH
#define POINT_HH

#include <iostream>
#include <vector>

using namespace std;

class Point{

public:
    Point();
    ~Point();
    
    Point(double, double, double);
    void SetPoint(double, double, double);
    void SetPoint(vector<double>);
    void SetX(const double);
    void SetY(const double);
    void SetZ(const double);
    vector<double> GetPoint();
    double GetX();
    double GetY();
    double GetZ();
    
    void Precision();
    int GetPrecision();

    Point& operator=(const Point&);
    friend Point operator-(Point,Point);
    friend Point operator+(Point,Point);
    friend Point operator*(Point,double);
    friend Point operator/(Point,double);
    friend Point operator*(Point,int);
    friend Point operator/(Point,int);
    friend Point operator-=(Point,Point);
    friend Point operator+=(Point,Point);
    friend Point operator*=(Point,double);
    friend Point operator/=(Point,double);
    friend Point operator*=(Point,int);
    friend Point operator/=(Point,int);
    friend bool operator==(Point,Point);
    friend bool operator!=(Point,Point);

private:
    double x, y, z;
    int precision;
};

#endif
