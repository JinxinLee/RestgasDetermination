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

#include "include/trigonometry.hh"
#include "include/line.hh"
#include "include/point.hh"
#include "include/algebra.hh"

using namespace std;

Trigonometry::Trigonometry()
{
}

Trigonometry::~Trigonometry()
{
}

double Trigonometry::Angle(Line L1, Line L2)
{
    double angle;
    Algebra alg;
    
    angle = acos(alg.scalar_product(L1,L2)/(alg.Magnitude(L1)*alg.Magnitude(L2)));
    
    return angle;
}
