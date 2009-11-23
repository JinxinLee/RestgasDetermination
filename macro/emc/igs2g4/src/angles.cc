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
#include <math.h>

#include "include/angles.hh"
#include "include/math.hh"

using namespace std;

Angles::Angles()
{
}

Angles::~Angles()
{
}

Angles::Angles(double xl, double yl, double zl) : Point(xl,yl,zl)
{
    Angles::compute();
}

Angles::Angles(vector<double> p) : Point(p[0], p[1], p[2])
{
    Angles::compute();
}

void Angles::SetVector(double xl, double yl, double zl)
{
    Angles::SetPoint(xl,yl,zl);
    Angles::compute();
    
    return;
}

void Angles::SetVector(vector<double> vvl)
{
    Angles::SetPoint(vvl);
    Angles::compute();
    
    return;
}

void Angles::compute()
{
    Angles:Precision();
    Angles::SetTheta();
    Angles::SetPhi();
    Angles::SetChi();
    Angles::SetPsi();
    Angles::SetOmega();
    
    return;
}

void Angles::SetTheta()
{
    Angles::theta = atan2(sqrt(pow(Angles::GetX(),2)+pow(Angles::GetY(),2)),Angles::GetZ());

    return;
}

void Angles::SetPhi()
{
    Angles::phi = atan2(Angles::GetY(),Angles::GetX());
    
    return;
}

void Angles::SetChi()
{
    Angles::chi = atan2(Angles::GetZ(),Angles::GetY());
    
    return;
}

void Angles::SetPsi()
{
    Angles::psi = atan2(Angles::GetX(),Angles::GetZ());
    
    return;
}

void Angles::SetOmega()
{
    Angles::omega = Angles::phi;
    
    return;
}

void Angles::SetVTheta(double d)
{
    Angles::theta = d;
}

void Angles::SetVPhi(double d)
{
    Angles::phi = d;
}

void Angles::SetVChi(double d)
{
    Angles::chi = d;
}

void Angles::SetVPsi(double d)
{
    Angles::psi = d;
}

void Angles::SetVOmega(double d)
{
    Angles::omega = d;
}

void Angles::Precision()
{
    Math math;
    
    Angles::precision = math.precision;
}

Angles& Angles::operator=(const Angles& angles)
{
    theta = angles.theta;
    phi = angles.phi;
    chi = angles.chi;
    psi = angles.psi;
    omega = angles.omega;
    
    return *this;
}

Angles operator-(Angles A2, Angles A1)
{
    Angles temp;
    temp.SetVTheta(A2.GetTheta() - A1.GetTheta());
    temp.SetVPhi(A2.GetPhi() - A1.GetPhi());
    temp.SetVChi(A2.GetChi() - A1.GetChi());
    temp.SetVPsi(A2.GetPsi() - A1.GetPsi());
    temp.SetVOmega(A2.GetOmega() - A1.GetOmega());
    
    return temp;
}
bool operator==(Angles A1, Angles A2)
{
    bool identity = false;
    Math math;
    Angles angles;
    angles = A2 - A1;
    
    if(math.Precision(angles.GetTheta(),angles.GetPrecision())==0.0)
    if(math.Precision(angles.GetPhi(),angles.GetPrecision())==0.0)
    if(math.Precision(angles.GetChi(),angles.GetPrecision())==0.0)
    if(math.Precision(angles.GetPsi(),angles.GetPrecision())==0.0)
    if(math.Precision(angles.GetOmega(),angles.GetPrecision())==0.0)
	identity = true;
    
    return identity;
}

bool operator!=(Angles A1, Angles A2)
{
    bool identity = false;
    
    if(!(A1==A2)) identity = true;
    
    return identity;
}

vector<double> Angles::GetAngles()
{
    vector<double> temp;
    
    temp.push_back(Angles::theta);
    temp.push_back(Angles::phi);
    temp.push_back(Angles::chi);
    temp.push_back(Angles::psi);
    temp.push_back(Angles::omega);
    
    return temp;
}

double Angles::GetTheta()
{
    return Angles::theta;
}

double Angles::GetPhi()
{
    return Angles::phi;
}

double Angles::GetChi()
{
    return Angles::chi;
}

double Angles::GetPsi()
{
    return Angles::psi;
}

double Angles::GetOmega()
{
    return Angles::omega;
}
