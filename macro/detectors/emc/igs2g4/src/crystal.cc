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

#include "include/crystal.hh"
#include "include/point.hh"
#include "include/line.hh"
#include "include/plan.hh"
#include "include/angles.hh"
#include "include/math.hh"
#include "include/algebra.hh"

using namespace std;

Crystal::Crystal()
{
}

Crystal::~Crystal()
{
}

void Crystal::SetCrystal(vector<Point> p)
{
    if(!Crystal::crystal_in.empty()) Crystal::crystal_in.clear();
    if(!Crystal::crystal_out.empty()) Crystal::crystal_out.clear();
    if(!Crystal::crystal.empty()) Crystal::crystal.clear();
    for(int i=0; i<p.size(); i++)
	Crystal::crystal_in.push_back(p[i]);
    Crystal::set_crystal();
}

void Crystal::Precision()
{
    Math math;
    
    Crystal::precision = math.precision;
}

void Crystal::set_crystal()
{
    Crystal::set_order();
    Crystal::set_crystal_out();
}

void Crystal::set_order()
{
    vector<Point> pts;
    Algebra alg;
    Point temp;
    Math math;
    
    for(int i=0; i<Crystal::crystal_in.size(); i++) 
	pts.push_back(Crystal::crystal_in[i]);

    for(int i=0; i<pts.size()-1; i++) 
    for(int j=i+1; j<pts.size(); j++) 
    if(math.Precision(alg.Magnitude(Line(Point(0.0,0.0,0.0),pts[i]))-
		      alg.Magnitude(Line(Point(0.0,0.0,0.0),pts[j])),
		      math.precision)>0.0) {
	temp = pts[j];
	pts[j] = pts[i];
	pts[i] = temp;
    };
    
    for(int i=0; i<4; i++) {
    int j1=i+4;
    for(int j2=j1+1; j2<8; j2++)
    if(math.Precision(alg.Magnitude(Line(pts[i],pts[j1]))-alg.Magnitude(Line(pts[i],pts[j2])),math.precision)>0.0) {
	temp = pts[j1];
	pts[j1] = pts[j2];
	pts[j2] = temp;
    };
    };
// >>>> for the barrel >>>>
//    for(int i=0; i<pts.size(); i++) Crystal::crystal.push_back(pts[i]);
// >>>> for the forward endcap >>>>
/*    pts.clear();
    pts.push_back(Crystal::crystal_in[0]);
    pts.push_back(Crystal::crystal_in[1]);
    pts.push_back(Crystal::crystal_in[2]);
    pts.push_back(Crystal::crystal_in[3]);
    pts.push_back(Crystal::crystal_in[7]);
    pts.push_back(Crystal::crystal_in[6]);
    pts.push_back(Crystal::crystal_in[5]);
    pts.push_back(Crystal::crystal_in[4]);
*/    for(int i=0; i<pts.size(); i++) Crystal::crystal.push_back(Crystal::crystal_in[i]);
}

void Crystal::set_crystal_out()
{
    Algebra alg;
    vector<Point> plan1P;
    vector<Point> plan2P;
    Point perp1, perp2, perp;
    Point center1, center2;
    Point temp;
    vector<Angles> al;
    Angles ap;
    Math math;
    double a[3][3], atemp[3][3];
    double col1[3], col2[3];
    double x1, y1, z1;
    double x2, y2, z2;
    double psi, omega, psil, omegal;
    
    for(int i=0; i<4; i++) { plan1P.push_back(Crystal::crystal[i]);
//	cout<<i+1<<": "<<Crystal::crystal[i].GetX()<<"; "<<Crystal::crystal[i].GetY()<<"; "<<Crystal::crystal[i].GetZ()<<endl;;
    };
    for(int i=4; i<8; i++) { plan2P.push_back(Crystal::crystal[i]);
//	cout<<i+1<<": "<<Crystal::crystal[i].GetX()<<"; "<<Crystal::crystal[i].GetY()<<"; "<<Crystal::crystal[i].GetZ()<<endl;;
    };
    
//    vector<int> vi;
//    if(!vi.empty()) vi.clear();
//    for(int i=0; i<8; i++)
//    for(int j=0; j<8; j++)
//    if(math.Precision(Crystal::crystal_in[i].GetX()-Crystal::crystal[j].GetX(),2)==0.0)
//    if(math.Precision(Crystal::crystal_in[i].GetY()-Crystal::crystal[j].GetY(),2)==0.0)
//    if(math.Precision(Crystal::crystal_in[i].GetZ()-Crystal::crystal[j].GetZ(),2)==0.0)
//	vi.push_back(j);
//    cout<<"IGES order  : ";
//    for(int i=0; i<8; i++) cout<<i+1<<" ";
//    cout<<endl;
//    cout<<"IGS2G4 order: ";
//    for(int i=0; i<8; i++) cout<<vi[i]+1<<" ";
//    cout<<endl;
//    vi.clear();
    
    Plan plan1 = Plan(plan1P);
    Plan plan2 = Plan(plan2P);
    
    center1 = plan1.GetCenter();
    center2 = plan2.GetCenter();
//cout<<"center 1: "<<center1.GetX()<<"; "<<center1.GetY()<<"; "<<center1.GetZ()<<endl;
//cout<<"center 2: "<<center2.GetX()<<"; "<<center2.GetY()<<"; "<<center2.GetZ()<<endl;
    perp1 = plan1.GetPerp();
    perp2 = plan2.GetPerp();
    perp = perp1 + perp2;
//    perp = perp/2.0;
    Crystal::perp = perp;
    
    for(int i=0; i<4; i++) {
	temp = plan2P[i] - plan1P[i];
	al.push_back(Angles(temp.GetPoint()));
//cout<<al.size()-1<<": "<<endl;
//cout<<"temp: "<<temp.GetX()<<"; "<<temp.GetY()<<"; "<<temp.GetZ()<<endl;
cout<<"angs: "<<al[al.size()-1].GetChi()<<"; "<<al[al.size()-1].GetPsi()<<"; "<<al[al.size()-1].GetOmega()<<endl;
cout<<tan(al[al.size()-1].GetChi())<<"; "<<tan(al[al.size()-1].GetPsi())<<"; "<<tan(al[al.size()-1].GetOmega())<<endl;
cout<<tan(al[al.size()-1].GetChi())*tan(al[al.size()-1].GetPsi())*tan(al[al.size()-1].GetOmega())<<endl;
    };
    
    ap = Angles(perp.GetPoint());
//cout<<perp.GetX()<<"; "<<perp.GetY()<<"; "<<perp.GetZ()<<endl;
//cout<<ap.GetChi()<<"; "<<ap.GetPsi()<<"; "<<ap.GetOmega()<<endl;
//cout<<tan(ap.GetChi())<<"; "<<tan(ap.GetPsi())<<"; "<<tan(ap.GetOmega())<<endl;
//cout<<tan(ap.GetChi())*tan(ap.GetPsi())*tan(ap.GetOmega())<<endl;
    
    perp1 = center1 + perp;
    perp2 = center2 + perp;

    for(int i=0; i<4; i++) {
	psi = ap.GetPsi();
	omega = ap.GetOmega();
	psil = al[i].GetPsi();
	omegal = al[i].GetOmega();
	a[0][0] = tan(ap.GetPsi());
	a[0][1] = tan(ap.GetPsi())*tan(ap.GetOmega());
	a[0][2] = 1.0;
	a[1][0] = 1.0;
	a[1][1] = 0.0;
	a[1][2] = (-1.0)*tan(al[i].GetPsi());
	a[2][0] = (-1.0)*tan(al[i].GetOmega());
	a[2][1] = 1.0;
	a[2][2] = 0.0;
	col1[0] = tan(ap.GetPsi())*center1.GetX()
		+ tan(ap.GetPsi())*tan(ap.GetOmega())*center1.GetY()
		+ center1.GetZ();
	col1[1] = plan1P[i].GetX() 
		- tan(al[i].GetPsi())*plan1P[i].GetZ();
	col1[2] = plan1P[i].GetY()
		- tan(al[i].GetOmega())*plan1P[i].GetX();
	col2[0] = tan(ap.GetPsi())*center2.GetX()
		+ tan(ap.GetPsi())*tan(ap.GetOmega())*center2.GetY()
		+ center2.GetZ();
	col2[1] = plan2P[i].GetX() 
		- tan(al[i].GetPsi())*plan2P[i].GetZ();
	col2[2] = plan2P[i].GetY()
		- tan(al[i].GetOmega())*plan2P[i].GetX();

//cout<<ap.GetPsi()*180.0/acos(-1.0)<<";  "<<ap.GetOmega()*180.0/acos(-1.0)<<endl;
//cout<<al[i].GetPsi()*180.0/acos(-1.0)<<";  "<<al[i].GetOmega()*180.0/acos(-1.0)<<endl;

if(math.Precision(omega-acos(-1.0)/2.0, math.precision)!=0.0) {};
	for(int il1=0; il1<3; il1++) {
	for(int il2=0; il2<3; il2++) {
	    atemp[il1][il2] = a[il1][il2];
//	    cout<<a[il1][il2]<<"; ";
	};
//	cout<<endl;
	};
	for(int il3=0; il3<3; il3++) {
	    atemp[il3][0] = col1[il3];
//	    cout<<col1[il3]<<"; ";
	};
//	cout<<endl;
	x1 = alg.det(atemp)/alg.det(a);
//cout<<alg.det(atemp)<<" / "<<alg.det(a)<<" = "<<x1<<endl;
	for(int il1=0; il1<3; il1++)
	for(int il2=0; il2<3; il2++)
	    atemp[il1][il2] = a[il1][il2];
	for(int il3=0; il3<3; il3++)
	    atemp[il3][1] = col1[il3];
	y1 = alg.det(atemp)/alg.det(a);
//cout<<alg.det(atemp)<<" / "<<alg.det(a)<<" = "<<y1<<endl;
	for(int il1=0; il1<3; il1++)
	for(int il2=0; il2<3; il2++)
	    atemp[il1][il2] = a[il1][il2];
	for(int il3=0; il3<3; il3++)
	    atemp[il3][2] = col1[il3];
	z1 = alg.det(atemp)/alg.det(a);
//cout<<alg.det(atemp)<<" / "<<alg.det(a)<<" = "<<z1<<endl;
	plan1P[i] = Point(x1,y1,z1);

	for(int il1=0; il1<3; il1++)
	for(int il2=0; il2<3; il2++)
	    atemp[il1][il2] = a[il1][il2];
	for(int il1=0; il1<3; il1++)
	    atemp[il1][0] = col2[il1];
	x2 = alg.det(atemp)/alg.det(a);
	for(int il1=0; il1<3; il1++)
	for(int il2=0; il2<3; il2++)
	    atemp[il1][il2] = a[il1][il2];
	for(int il1=0; il1<3; il1++)
	    atemp[il1][1] = col2[il1];
	y2 = alg.det(atemp)/alg.det(a);
	for(int il1=0; il1<3; il1++)
	for(int il2=0; il2<3; il2++)
	    atemp[il1][il2] = a[il1][il2];
	for(int il1=0; il1<3; il1++)
	    atemp[il1][2] = col2[il1];
	z2 = alg.det(atemp)/alg.det(a);
	plan2P[i] = Point(x2,y2,z2);
    };
    
    for(int i=0; i<4; i++) Crystal::crystal_out.push_back(plan1P[i]);
    for(int i=0; i<4; i++) Crystal::crystal_out.push_back(plan2P[i]);
}

vector<Point> Crystal::GetCrystalIn()
{
    return Crystal::crystal_in;
}

vector<Point> Crystal::GetCrystalOut()
{
    return Crystal::crystal_out;
}

Point Crystal::GetPerp()
{
    return Crystal::perp;
}
