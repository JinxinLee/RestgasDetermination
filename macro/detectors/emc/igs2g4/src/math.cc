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
#include <vector>

#include "include/math.hh"
#include "include/point.hh"
#include "include/angles.hh"

using namespace std;

int Math::precision=12;

Math::Math()
{
}

Math::~Math()
{
}

double Math::Abs(double d)
{
    double ld = d;
    
    if(d<0.0) ld = (-1.0)*d;

    return ld;
}

double Math::Sign(double d)
{
    double ld = 1.0;
    
    if(d<0.0) ld = -1.0;
    
    return ld;
}

float Math::Sign(float f)
{
    float lf = 1.0;
    
    if(f<0.0) lf = -1.0;
    
    return lf;
}

int Math::Sign(int i)
{
    int li = 1;
    
    if(i<0) li = -1;
    
    return li;
}

double Math::Precision(double d, int i)
{
    double ld = d;
    long int lid;
    
    for(int e=0; e<i; e++) ld *= 10.0;
    lid = static_cast<long int>(ld);
    ld = (1.0)*static_cast<double>(lid);
    for(int e=0; e<i; e++) ld /= 10.0;
    
    return ld;
}

/*vector<PointD> Math::get_coplanarity(vector<PointD> P, vector<PointD> line)
{
    PointD lp, lpc;
    vector<PointD> lvp;
    int prec = Math::precision;
    int vas;
    double chi_p, psi_p, omega_p; //perp
    double chi_l, psi_l, omega_l; //(C1,C2)
    vector<PointD> matrix;
    PointD column, column_free;
    double det, deti[3];
    
    if(P.size()>=3) {
	if(line.size()==2) {
	    if(!Math::coplanarity_check(P)) {
		vas = Math::vangles.size();
		chi_p = 0.0;
		psi_p = 0.0;
		omega_p = 0.0;
		for(int i=0; i<vas; i++) {
		    chi_p += Math::vangles[i].chi;
		    psi_p += Math::vangles[i].psi;
		    omega_p += Math::vangles[i].omega;
		};
		chi_p /= static_cast<double>(vas);
		psi_p /= static_cast<double>(vas);
		omega_p /= static_cast<double>(vas);
		if(!Math::vangles.empty()) Math::vangles.clear();
		Math::vangles = compute_angles(line);
		chi_l = vangles[0].chi;
		psi_l = vangles[0].psi;
		omega_l = vangles[0].omega;
cout<<tan(chi_p)<<";"<<tan(psi_p)<<";"<<tan(omega_p)<<endl;
cout<<tan(chi_l)<<";"<<tan(psi_l)<<";"<<tan(omega_l)<<endl;
		//computing the point
		if(!matrix.empty()) matrix.clear();
		matrix.push_back(PointD(tan(omega_p)*tan(chi_p),tan(chi_p),1.0));
		matrix.push_back(PointD(0.0,1.0,(-1.0)*tan(chi_l)));
		matrix.push_back(PointD((-1.0)*tan(psi_l),0.0,1.0));
		det = Math::determinant(matrix);
		lpc = PointD(0.0,0.0,0.0);
		for(int i=0; i<P.size(); i++) {
		    lpc.x += P[i].x;
		    lpc.y += P[i].y;
		    lpc.z += P[i].z;
		};
		lpc.x /= P.size();
		lpc.y /= P.size();
		lpc.z /= P.size();
		for(int i=0; i<P.size(); i++) {
		    column_free.x = lpc.x*tan(omega_p)*tan(chi_p)+lpc.y*tan(chi_p)+lpc.z;
		    column_free.y = P[i].y-P[i].z*tan(omega_l);
		    column_free.z = P[i].z-P[i].x*tan(psi_l);
		    column.x = matrix[0].x;
		    matrix[0].x = column_free.x;
		    column.y = matrix[1].x;
		    matrix[1].x = column_free.y;
		    column.z = matrix[2].x;
		    matrix[2].x = column_free.z;
		    deti[0] = Math::determinant(matrix);
		    matrix[0].x = column.x;
		    matrix[1].x = column.y;
		    matrix[2].x = column.z;
		    column.x = matrix[0].y;
		    matrix[0].y = column_free.x;
		    column.y = matrix[1].y;
		    matrix[1].y = column_free.y;
		    column.z = matrix[2].y;
		    matrix[2].y = column_free.z;
		    deti[1] = Math::determinant(matrix);
		    matrix[0].y = column.x;
		    matrix[1].y = column.y;
		    matrix[2].y = column.z;
		    column.x = matrix[0].z;
		    matrix[0].z = column_free.x;
		    column.y = matrix[1].z;
		    matrix[1].z = column_free.y;
		    column.z = matrix[2].z;
		    matrix[2].z = column_free.z;
		    deti[2] = Math::determinant(matrix);
		    lp.x = deti[0]/det;
		    lp.y = deti[1]/det;
		    lp.z = deti[2]/det;
		    lvp.push_back(lp);
cout<<"Point "<<i+1<<": "<<P[i].x<<";"<<P[i].y<<";"<<P[i].z<<endl;
cout<<det<<";"<<deti[0]<<";"<<deti[1]<<";"<<deti[2]<<endl;
cout<<"Point "<<i+1<<": "<<lp.x<<";"<<lp.y<<";"<<lp.z<<endl;
		};
	    };
	} else {
	    cout<<"Error: Too many points in the definition of the line"<<endl;
	    cout<<"Action: Abord the program!"<<endl;
	    exit(1);
	};
    } else {
	cout<<"Error: Not enough points for defining the plan!"<<endl;
	cout<<"Action: Abord the program!"<<endl;
	exit(1);
    };
    
    return lvp;
}

bool Math::coplanarity_check(vector<PointD> P)
{
    bool coplanarity = true;
    double perp_x, perp_y, perp_z, x[3], y[3], z[3], chi, psi, omega;
    bool chi_b, psi_b, omega_b;
    int prec = Math::precision;
    PointD perp;
    vector<PointD> vperp;
    
    if(!Math::vangles.empty()) Math::vangles.clear();
    if(P.size()>=3) {
	for(int cont1=0; cont1<P.size()-2; cont1++)
	for(int cont2=cont1+1; cont2<P.size()-1; cont2++)
	for(int cont3=cont2+1; cont3<P.size(); cont3++)
	{
	    //first point
	    x[0] = Math::Precision(P[cont1].x,prec);
	    y[0] = Math::Precision(P[cont1].y,prec);
	    z[0] = Math::Precision(P[cont1].x,prec);
	    //second point
	    x[1] = Math::Precision(P[cont2].x,prec);
	    y[1] = Math::Precision(P[cont2].y,prec);
	    z[1] = Math::Precision(P[cont2].z,prec);
	    //third point
	    x[2] = Math::Precision(P[cont3].x,prec);
	    y[2] = Math::Precision(P[cont3].y,prec);
	    z[2] = Math::Precision(P[cont3].z,prec);
	    //computing the perpendicular
	    perp_x = (y[1]-y[0])*(z[2]-z[0])-(y[2]-y[0])*(z[1]-z[0]);
	    perp_y = (x[2]-x[0])*(z[1]-z[0])-(x[1]-x[0])*(z[2]-z[0]);
	    perp_z = (x[1]-x[0])*(y[2]-y[0])-(x[2]-x[0])*(y[1]-y[0]);
	    //computing the angles
	    if(perp_z<0.0) {
		perp_x *= -1.0;
		perp_y *= -1.0;
		perp_z *= -1.0;
	    };
	    perp.x = perp_x;
	    perp.y = perp_y;
	    perp.z = perp_z;
	    vperp.clear();
	    vperp.push_back(perp);
	    Math::vangles = Math::add_elements(Math::compute_angles(vperp),Math::vangles);
	};
	if(Math::vangles.size()>1){
	    chi_b = false;
	    psi_b = false;
	    omega_b = false;
	    for(int cont1=0; cont1<Math::vangles.size()-1; cont1++)
	    for(int cont2=cont1+1; cont2<Math::vangles.size(); cont2++)
	    {
		if(Math::Precision(Math::vangles[cont1].chi-Math::vangles[cont2].chi,prec)!=0.0) chi_b = true;
		if(Math::Precision(Math::vangles[cont1].psi-Math::vangles[cont2].psi,prec)!=0.0) psi_b = true;
		if(Math::Precision(Math::vangles[cont1].omega-Math::vangles[cont2].omega,prec)!=0.0) omega_b = true;
	    };
	    if(chi_b||psi_b||omega_b) coplanarity = false;
	} else {
	    cout<<"Warning: Three points always define one plan!"<<endl;
	    return true;
	};
    } else {
	cout<<"Error: Not enough points for checking the coplanarity!"<<endl;
	cout<<"Action: Coplanarity test aborded and the flag set to failed!"<<endl;
	return false;
    };
    
    return coplanarity;
}

vector<Angles> Math::compute_angles(vector<PointD> P)
{
    vector<Angles> vangs;
    Angles angs;
    double perp_x, perp_y, perp_z, x[3], y[3], z[3];
    
    if(P.size()<1) {
	cout<<"Warning: No point to define the angles!"<<endl;
	cout<<"Action: Angles set to 0!"<<endl;
	angs.theta = 0.0;
	angs.phi = 0.0;
	angs.chi = 0.0;
	angs.psi = 0.0;
	angs.omega = 0.0;
    } else if(P.size()==1) {
	if((P[0].x==0.0)&&(P[0].y==0.0)&&(P[0].z==0.0)) {
	    cout<<"Warning: Point set in the origin of the axes!"<<endl;
	    cout<<"Action: Angles set to 0!"<<endl;
	    angs.theta = 0.0;
	    angs.phi = 0.0;
	    angs.chi = 0.0;
	    angs.psi = 0.0;
	    angs.omega = 0.0;
	} else {
	    angs.theta = atan2(sqrt(pow(P[0].x,2)+pow(P[0].y,2)),P[0].z);
	    angs.phi = atan2(P[0].y,P[0].x);
	    angs.chi = atan2(P[0].y,P[0].z);
	    angs.psi = atan2(P[0].z,P[0].x);
	    angs.omega = atan2(P[0].x,P[0].y);
	};
    } else if(P.size()==2) {
	angs.theta = atan2(sqrt(pow(P[1].x-P[0].x,2)+pow(P[1].y-P[0].y,2)),P[1].z-P[0].z);
	angs.phi = atan2(P[1].y-P[0].y,P[1].x-P[0].x);
	angs.chi = atan2(P[1].y-P[0].y,P[1].z-P[0].z);
	angs.psi = atan2(P[1].z-P[0].z,P[1].x-P[0].x);
	angs.omega = atan2(P[1].x-P[0].x,P[1].y-P[0].y);
    } else {
	for(int cont1=0; cont1<P.size()-2; cont1++)
	for(int cont2=cont1+1; cont2<P.size()-1; cont2++)
	for(int cont3=cont2+1; cont3<P.size(); cont3++)
	{
	    //first point
	    x[0] = P[cont1].x;
	    y[0] = P[cont1].y;
	    z[0] = P[cont1].x;
	    //second point
	    x[1] = P[cont2].x;
	    y[1] = P[cont2].y;
	    z[1] = P[cont2].z;
	    //third point
	    x[2] = P[cont3].x;
	    y[2] = P[cont3].y;
	    z[2] = P[cont3].z;
	    //computing the perpendicular
	    perp_x = (y[1]-y[0])*(z[2]-z[0])-(y[2]-y[0])*(z[1]-z[0]);
	    perp_y = (x[2]-x[0])*(z[1]-z[0])-(x[1]-x[0])*(z[2]-z[0]);
	    perp_z = (x[1]-x[0])*(y[2]-y[0])-(x[2]-x[0])*(y[1]-y[0]);
	    //computing the angles
	    angs.theta = atan2(sqrt(pow(perp_x,2)+pow(perp_y,2)),perp_z);
	    angs.phi = atan2(perp_y,perp_x);
	    angs.chi = atan2(perp_y,perp_z);
	    angs.psi = atan2(perp_z,perp_x);
	    angs.omega = atan2(perp_x,perp_y);
	};
    };
    vangs.push_back(angs);
    
    return vangs;
}

vector<Angles> Math::add_elements(vector<Angles> vangs1, vector<Angles> vangs2)
{
    vector<Angles> lvangs;
    
    for(int i=0; i<vangs1.size(); i++) lvangs.push_back(vangs1[i]);
    for(int i=0; i<vangs2.size(); i++) lvangs.push_back(vangs2[i]);
    
    return lvangs;
}

double Math::determinant(vector<PointD> Matrix)
{
    double det = 0.0;
    double a[3][3];

cout<<"Determinant section:"<<endl;    
    for(int i=0; i<3; i++) {
	a[i][0] = Matrix[i].x;
	a[i][1] = Matrix[i].y;
	a[i][2] = Matrix[i].z;
cout<<a[i][0]<<";"<<a[i][1]<<";"<<a[i][2]<<endl;
    };
    det += (+1.0)*a[0][0]*a[1][1]*a[2][2];
    det += (+1.0)*a[1][0]*a[2][1]*a[0][2];
    det += (+1.0)*a[0][1]*a[1][2]*a[2][0];
    det += (-1.0)*a[2][0]*a[1][1]*a[0][2];
    det += (-1.0)*a[2][1]*a[1][2]*a[0][0];
    det += (-1.0)*a[1][0]*a[0][1]*a[2][2];
    
    return det;
}

Angles Math::operator= (Angles angs_in)
{
    return angs_in;
}

vector<Angles> Math::operator= (vector<Angles> angs)
{
    return angs;
}

PointD Math::operator= (PointD pd)
{
    return pd;
}

vector<PointD> Math::operator= (vector<PointD> vpd)
{
    return vpd;
}
*/
