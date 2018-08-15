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

#ifndef ANGLES_HH
#define ANGLES_HH

#include <iostream>
#include <vector>

#include "point.hh"

using namespace std;

class Angles : public Point{

public:
    Angles();
    ~Angles();

    Angles(double, double, double);
    Angles(vector<double>);
    void SetVector(double, double, double);
    void SetVector(vector<double>);
    vector<double> GetAngles();
    double GetTheta();
    double GetPhi();
    double GetChi();
    double GetPsi();
    double GetOmega();
    
    void Precision();

    Angles& operator=(const Angles&);
    friend Angles operator-(Angles, Angles);
    friend bool operator==(Angles,Angles);
    friend bool operator!=(Angles,Angles);

    void SetVTheta(double);
    void SetVPhi(double);
    void SetVChi(double);
    void SetVPsi(double);
    void SetVOmega(double);

private:
    double theta, phi, chi, psi, omega;
    int precision;
    void compute();

    void SetTheta();
    void SetPhi();
    void SetChi();
    void SetPsi();
    void SetOmega();
};

#endif
