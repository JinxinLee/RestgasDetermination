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

#include "include/algebra.hh"
#include "include/line.hh"
#include "include/point.hh"
#include "include/math.hh"

using namespace std;

Algebra::Algebra()
{
}

Algebra::~Algebra()
{
}

double Algebra::scalar_product(Line L1, Line L2)
{
    double sp;
    vector<Point> l1 = L1.GetLine();
    vector<Point> l2 = L2.GetLine();
    Math math;
    
    sp = (l1[1].GetX()-l1[0].GetX())*(l2[1].GetX()-l2[0].GetX())+
	    	  (l1[1].GetY()-l1[0].GetY())*(l2[1].GetY()-l2[0].GetY())+
	    	  (l1[1].GetZ()-l1[0].GetZ())*(l2[1].GetZ()-l2[0].GetZ());
//    sp = sqrt(math.Abs((l1[1].GetX()-l1[0].GetX())*(l2[1].GetX()-l2[0].GetX())+
//	    	       (l1[1].GetY()-l1[0].GetY())*(l2[1].GetY()-l2[0].GetY())+
//	    	       (l1[1].GetZ()-l1[0].GetZ())*(l2[1].GetZ()-l2[0].GetZ())));
//    sp *= math.Sign((l1[1].GetX()-l1[0].GetX())*(l2[1].GetX()-l2[0].GetX())+
//	    	    (l1[1].GetY()-l1[0].GetY())*(l2[1].GetY()-l2[0].GetY())+
//	    	    (l1[1].GetZ()-l1[0].GetZ())*(l2[1].GetZ()-l2[0].GetZ()));
    
    return sp;
}

Point Algebra::cross_product(Line L1, Line L2)
{
    Point cp;
    double x, y, z;
    vector<Point> l1 = L1.GetLine();
    vector<Point> l2 = L2.GetLine();
    
    x = (l1[1].GetY()-l1[0].GetY())*(l2[1].GetZ()-l2[0].GetZ())-
        (l2[1].GetY()-l2[0].GetY())*(l1[1].GetZ()-l1[0].GetZ());
    y = (l2[1].GetX()-l2[0].GetX())*(l1[1].GetZ()-l1[0].GetZ())-
        (l1[1].GetX()-l1[0].GetX())*(l2[1].GetZ()-l2[0].GetZ());
    z = (l1[1].GetX()-l1[0].GetX())*(l2[1].GetY()-l2[0].GetY())-
        (l2[1].GetX()-l2[0].GetX())*(l1[1].GetY()-l1[0].GetY());
    
    cp.SetPoint(x,y,z);
    
    return cp;
}

double Algebra::Magnitude(Line L1)
{
    double mag;
    vector<Point> l1 = L1.GetLine();
    
    mag = sqrt(pow(l1[1].GetX()-l1[0].GetX(),2)+
	       pow(l1[1].GetY()-l1[0].GetY(),2)+
	       pow(l1[1].GetZ()-l1[0].GetZ(),2));
    
    return mag;
}

double Algebra::det(double a[3][3])
{
    double det = 0.0;
    
    det += a[0][0]*a[1][1]*a[2][2];
    det += a[1][0]*a[2][1]*a[0][2];
    det += a[0][1]*a[1][2]*a[2][0];
    det -= a[2][0]*a[1][1]*a[0][2];
    det -= a[0][0]*a[2][1]*a[1][2];
    det -= a[2][2]*a[1][0]*a[0][1];
    
    return det;
}
