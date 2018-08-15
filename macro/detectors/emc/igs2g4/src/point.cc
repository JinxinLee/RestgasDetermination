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

#include "include/point.hh"
#include "include/math.hh"

using namespace std;

Point::Point()
{
    Point::x = 0.0;
    Point::y = 0.0;
    Point::z = 0.0;
    Point::Precision();
}

Point::~Point()
{
}

Point::Point(double xl, double yl, double zl)
{
    Point::x = xl;
    Point::y = yl;
    Point::z = zl;
    Point::Precision();
}

void Point::SetPoint(double xl, double yl, double zl)
{
    Point::x = xl;
    Point::y = yl;
    Point::z = zl;
    Point::Precision();
    
    return;
}

void Point::SetPoint(vector<double> vp)
{
    Point::Precision();
    if(vp.size()<3) {
	cout<<"Warning: Wrong number of dimensions!"<<endl;
	cout<<"Action: Not enough dimensions inserted! The rest will be taken zero"<<endl;
    };
    if(vp.size()>3) {
	cout<<"Worning: Wrong number of dimensions!"<<endl;
	cout<<"Action: Only the first 3 dimensions will be consider!"<<endl;
    };
    switch(vp.size()) {
	case 0:
	    Point::x = 0.0;
	    Point::y = 0.0;
	    Point::z = 0.0;
	    break;
	case 1:
	    Point::x = vp[0];
	    Point::y = 0.0;
	    Point::z = 0.0;
	    break;
	case 2:
	    Point::x = vp[0];
	    Point::y = vp[1];
	    Point::z = 0.0;
	    break;
	default:
	    Point::x = vp[0];
	    Point::y = vp[1];
	    Point::z = vp[2];
    };
    
    return;
}

void Point::SetX(const double xl)
{
    Point::Precision();
    Point::x = xl;
    
    return;
}

void Point::SetY(const double yl)
{
    Point::Precision();
    Point::y = yl;
    
    return;
}

void Point::SetZ(const double zl)
{
    Point::Precision();
    Point::z = zl;
    
    return;
}

vector<double> Point::GetPoint()
{
    vector<double> lpoint;
    
    lpoint.push_back(Point::x);
    lpoint.push_back(Point::y);
    lpoint.push_back(Point::z);
    
    return lpoint;
}

double Point::GetX()
{
    return Point::x;
}

double Point::GetY()
{
    return Point::y;
}

double Point::GetZ()
{
    return Point::z;
}

void Point::Precision()
{
    Math math;
    
    Point::precision = math.precision;
}

int Point::GetPrecision()
{
    return Point::precision;
}

Point& Point::operator=(const Point& point)
{
    Point::x = point.x;
    Point::y = point.y;
    Point::z = point.z;
    
    return *this;
}

Point operator-(Point P1, Point P2)
{
    Point temp;
    temp.SetX(P1.GetX() - P2.GetX());
    temp.SetY(P1.GetY() - P2.GetY());
    temp.SetZ(P1.GetZ() - P2.GetZ());
    
    return temp;
}

Point operator+(Point P1, Point P2)
{
    Point temp;
    temp.SetX(P1.GetX() + P2.GetX());
    temp.SetY(P1.GetY() + P2.GetY());
    temp.SetZ(P1.GetZ() + P2.GetZ());
    
    return temp;
}

Point operator*(Point p, double d)
{
    Point temp;
    temp.SetX(p.GetX() * d);
    temp.SetY(p.GetY() * d);
    temp.SetZ(p.GetZ() * d);
    
    return temp;
}

Point operator/(Point p, double d)
{
    Point temp;
    Math math;
    
    if(math.Precision(d,temp.precision)!=0.0) {
	temp.SetX(p.GetX() / d);
	temp.SetY(p.GetY() / d);
	temp.SetZ(p.GetZ() / d);
    } else {
	cout<<"Error: Division by zero! [point division]"<<endl;
	cout<<"Action: Abord the program"<<endl;
	exit(1);
    };
    
    return temp;
}

Point operator*(Point p, int i)
{
    return operator*(p,static_cast<double>(i));
}

Point operator/(Point p, int i)
{
    return operator/(p,static_cast<double>(i));
}

Point operator-=(Point P1, Point P2)
{
    Point temp;

    temp.SetX(P1.GetX()-P2.GetX());
    temp.SetY(P1.GetY()-P2.GetY());
    temp.SetZ(P1.GetZ()-P2.GetZ());
    
    return temp;
}

Point operator+=(Point P1, Point P2)
{
    Point temp;

    temp.SetX(P1.GetX()+P2.GetX());
    temp.SetY(P1.GetY()+P2.GetY());
    temp.SetZ(P1.GetZ()+P2.GetZ());
    
    return temp;
}

Point operator*=(Point p, double d)
{
    Point temp;

    temp.SetX(p.GetX()*d);
    temp.SetY(p.GetY()*d);
    temp.SetZ(p.GetZ()*d);
    
    return temp;
}

Point operator/=(Point p, double d)
{
    Point temp;
    Math math;
    
    if(math.Precision(d,temp.precision)!=0.0) {
	temp.SetX(p.GetX()/d);
	temp.SetY(p.GetY()/d);
	temp.SetZ(p.GetZ()/d);
    } else {
	cout<<"Error: Division by zero! [point division]"<<endl;
	cout<<"Action: Abord the program"<<endl;
	exit(1);
    };
    
    return temp;
}

Point operator*=(Point p, int i)
{
    return operator*=(p,static_cast<double>(i));
}

Point operator/=(Point p, int i)
{
    return operator/=(p,static_cast<double>(i));
}

bool operator==(Point P1, Point P2)
{
    bool identity = false;
    Math math;
    Point point = P2 - P1;
    
    if(math.Precision(point.GetX(),point.GetPrecision())==0.0)
    if(math.Precision(point.GetY(),point.GetPrecision())==0.0)
    if(math.Precision(point.GetZ(),point.GetPrecision())==0.0)
	identity = true;
    
    return identity;
}

bool operator!=(Point P1, Point P2)
{
    bool identity = false;

    if(!(P1==P2)) identity = true;
    
    return identity;
}
