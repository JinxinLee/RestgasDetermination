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
#include <fstream>
#include <string>
#include <vector>

#include <math.h>

#include "include/data_igs.hh"
#include "include/converting.hh"
#include "include/math.hh"
#include "include/algebra.hh"
#include "include/trigonometry.hh"
#include "include/point.hh"
#include "include/line.hh"
#include "include/plan.hh"
#include "include/crystal.hh"

using namespace std;

Convert::Convert()
{
}

Convert::~Convert()
{
}

void Convert::multiplication()
{
    vector<DataIGS> ldigs;
    DataIGS ldigs1;
    double ra = 22.5*acos(-1.0)/180.0;
    double x1, y1, z1;
    double x2, y2, z2;
    
    for(int i=0; i<16; i++) 
    for(int j=0; j<Convert::igsdata.size(); j++) {
	for(int k=0; k<8; k++) {
	    x1 = Convert::igsdata[j].x[k];
	    y1 = Convert::igsdata[j].y[k];
	    z1 = Convert::igsdata[j].z[k];
	    // Oz ccw rotation
	    x2 = x1*cos(ra*static_cast<double>(i))+y1*sin(ra*static_cast<double>(i));
	    y2 = y1*cos(ra*static_cast<double>(i))-x1*sin(ra*static_cast<double>(i));
	    z2 = z1;
	    ldigs1.x[k] = x2;
	    ldigs1.y[k] = y2;
	    ldigs1.z[k] = z2;
	};
	ldigs1.module = Convert::igsdata[j].module;
	ldigs1.row = Convert::igsdata[j].row;
	ldigs1.crystal = 10*i+Convert::igsdata[j].crystal;
	ldigs.push_back(ldigs1);
    };
    
    Convert::igsdata.clear();
    for(int i=0; i<ldigs.size(); i++) Convert::igsdata.push_back(ldigs[i]);
    
    return;
}

void Convert::convert_igs_g4()
{
    Trigonometry trig;
    Algebra alg;
    Math math;
    vector<Point> plan1, plan2;
    vector<Point> median;
    Point p;
    Crystal crys;
    fstream file;
    double deviation;

    for(int i=0; i<Convert::igsdata.size(); i++) {
	Convert::dg4.crystal = Convert::igsdata[i].crystal;
	Convert::dg4.row = Convert::igsdata[i].row;
	Convert::dg4.module = Convert::igsdata[i].module;
if((i+1)%1000==0) cout<<i+1<<" out of "<<Convert::igsdata.size()<<" crystals converted"<<endl;
if((i+1)==Convert::igsdata.size()) cout<<i+1<<" out of "<<Convert::igsdata.size()<<" crystals converted"<<endl;
//cout<<"Crystal "<<i+1<<endl;
	Convert::rotate_axes(i);
//	Convert::order();

    if(!plan1.empty()) plan1.clear();
    if(!plan2.empty()) plan2.clear();
    if(!median.empty()) median.clear();
    for(int point=0; point<8; point++) {
	p.SetX(Convert::digs.x[point]);
	p.SetY(Convert::digs.y[point]);
	p.SetZ(Convert::digs.z[point]);
	median.push_back(p);
    };
    crys.SetCrystal(median);
//deviation
    plan1.clear();
    plan2.clear();
    plan1 = crys.GetCrystalIn();
    plan2 = crys.GetCrystalOut();
    file.open("data/deviation.dat", ios::out | ios::app);
    file.width(10);
    file.setf(ios::right);
    file<<i+1;
    deviation = sqrt(pow(plan2[0].GetX()-plan1[0].GetX(),2)+
		     pow(plan2[0].GetY()-plan1[0].GetY(),2)+
		     pow(plan2[0].GetZ()-plan1[0].GetZ(),2));
    file.width(12);
    file.setf(ios::right);
    file.precision(6);
    file<<deviation;
    deviation = sqrt(pow(plan2[1].GetX()-plan1[1].GetX(),2)+
		     pow(plan2[1].GetY()-plan1[1].GetY(),2)+
		     pow(plan2[1].GetZ()-plan1[1].GetZ(),2));
    file.width(12);
    file.setf(ios::right);
    file.precision(6);
    file<<deviation;
//    deviation = sqrt(pow(plan2[2].GetX()-plan1[6].GetX(),2)+
//		     pow(plan2[2].GetY()-plan1[6].GetY(),2)+
//		     pow(plan2[2].GetZ()-plan1[6].GetZ(),2));
    deviation = sqrt(pow(plan2[2].GetX()-plan1[2].GetX(),2)+
		     pow(plan2[2].GetY()-plan1[2].GetY(),2)+
		     pow(plan2[2].GetZ()-plan1[2].GetZ(),2));
    file.width(12);
    file.setf(ios::right);
    file.precision(6);
    file<<deviation;
//    deviation = sqrt(pow(plan2[3].GetX()-plan1[7].GetX(),2)+
//		     pow(plan2[3].GetY()-plan1[7].GetY(),2)+
//		     pow(plan2[3].GetZ()-plan1[7].GetZ(),2));
    deviation = sqrt(pow(plan2[3].GetX()-plan1[3].GetX(),2)+
		     pow(plan2[3].GetY()-plan1[3].GetY(),2)+
		     pow(plan2[3].GetZ()-plan1[3].GetZ(),2));
    file.width(12);
    file.setf(ios::right);
    file.precision(6);
    file<<deviation;
    deviation = sqrt(pow(plan2[4].GetX()-plan1[4].GetX(),2)+
		     pow(plan2[4].GetY()-plan1[4].GetY(),2)+
		     pow(plan2[4].GetZ()-plan1[4].GetZ(),2));
    file.width(12);
    file.setf(ios::right);
    file.precision(6);
    file<<deviation;
    deviation = sqrt(pow(plan2[5].GetX()-plan1[5].GetX(),2)+
		     pow(plan2[5].GetY()-plan1[5].GetY(),2)+
		     pow(plan2[5].GetZ()-plan1[5].GetZ(),2));
    file.width(12);
    file.setf(ios::right);
    file.precision(6);
    file<<deviation;
//    if((i%720==9)||(i%720==19)||(i%720==439)||(i%720==449)) {
	deviation = sqrt(pow(plan2[6].GetX()-plan1[4].GetX(),2)+
	    	         pow(plan2[6].GetY()-plan1[4].GetY(),2)+
		         pow(plan2[6].GetZ()-plan1[4].GetZ(),2));
	file.width(12);
	file.setf(ios::right);
	file.precision(6);
//	file<<deviation;
	deviation = sqrt(pow(plan2[7].GetX()-plan1[5].GetX(),2)+
			 pow(plan2[7].GetY()-plan1[5].GetY(),2)+
			 pow(plan2[7].GetZ()-plan1[5].GetZ(),2));
	file.width(12);
	file.setf(ios::right);
	file.precision(6);
//	file<<deviation;
//    } else {
	deviation = sqrt(pow(plan2[6].GetX()-plan1[6].GetX(),2)+
	    	         pow(plan2[6].GetY()-plan1[6].GetY(),2)+
		         pow(plan2[6].GetZ()-plan1[6].GetZ(),2));
	file.width(12);
	file.setf(ios::right);
	file.precision(6);
	file<<deviation;
	deviation = sqrt(pow(plan2[7].GetX()-plan1[7].GetX(),2)+
			 pow(plan2[7].GetY()-plan1[7].GetY(),2)+
			 pow(plan2[7].GetZ()-plan1[7].GetZ(),2));
	file.width(12);
	file.setf(ios::right);
	file.precision(6);
	file<<deviation;
//    };
    file<<endl;
    file.close();
    file.clear();
//end deviation

//    if(igsdata[i].module==1) Convert::perp = crys.GetPerp();
//    if(igsdata[i].module!=1) Convert::perp = crys.GetPerp()*(-1.0);
    
//    plan1 = crys.GetCrystalIn();
//    plan2 = crys.GetCrystalOut();
//    median.clear();
//    median.push_back(Point(0.0,0.0,0.0));
//    median.push_back(Point(0.0,0.0,0.0));
//    median.push_back(Point(0.0,0.0,0.0));
    if(i==0)
    for(int point=0; point<8; point++) {
	cout<<"Point "<<point+1<<": "<<plan1[point].GetX()<<"; "<<plan1[point].GetY()<<"; "<<plan1[point].GetZ()<<"; "
	    <<sqrt(pow(plan1[point].GetX(),2)+pow(plan1[point].GetY(),2)+pow(plan1[point].GetZ(),2))<<endl;
	cout<<"Point "<<point+1<<": "<<plan2[point].GetX()<<"; "<<plan2[point].GetY()<<"; "<<plan2[point].GetZ()<<"; "
	    <<sqrt(pow(plan2[point].GetX(),2)+pow(plan2[point].GetY(),2)+pow(plan2[point].GetZ(),2))<<endl;
//	median[0] = median[0] + plan1[point]/8.0;
//	median[1] = median[1] + plan2[point]/8.0;
    };
//    median[2] = median[1] - median[0];
//    cout<<"Initial center position: "<<median[0].GetX()<<"; "<<median[0].GetY()<<"; "<<median[0].GetZ()<<endl;
//    cout<<"Final center position  : "<<median[1].GetX()<<"; "<<median[1].GetY()<<"; "<<median[1].GetZ()<<endl;
//    cout<<"Difference             : "<<median[2].GetX()<<"; "<<median[2].GetY()<<"; "<<median[2].GetZ()<<endl;
//    if((math.Precision(median[2].GetX()-1.0,math.precision)>0.0)||(math.Precision(median[2].GetZ()-1.0,math.precision)>0.0))
//    cout<<"Warning:               : "<<median[2].GetX()<<"; "<<median[2].GetY()<<"; "<<median[2].GetZ()<<endl;
//    for(int p1=0; p1<2; p1++) 
//    for(int p2=0; p2<4; p2++) 
//    for(int p3=0; p3<4; p3++) 
//    for(int p4=0; p4<4; p4++)
//    if(p2>p1)
//    if((p3!=p2)&&(p3!=p1)) 
//    if((p4>p3)&&(p4!=p2)&&(p4!=p1)) {
//	double angle_line = 180.0*trig.Angle(Line(plan2[p1],plan2[p2]),Line(plan2[p3],plan2[p4]))/acos(-1.0);
//	if((math.Precision(10.0-math.Abs(angle_line),6)>=0.0)||(math.Precision(math.Abs(angle_line)-170.0,6)>=0.0))
//	cout<<"Angle(("<<p1<<","<<p2<<"),("<<p3<<","<<p4<<")): "<<angle_line<<" deg"<<endl;
//    };
    for(int point=0; point<8; point++) {
	Convert::digs.x[point] = (crys.GetCrystalOut())[point].GetX();
	Convert::digs.y[point] = (crys.GetCrystalOut())[point].GetY();
	Convert::digs.z[point] = (crys.GetCrystalOut())[point].GetZ();
    };
	
	Convert::center_position();
//	if(math.Sign((crys.GetPerp()).GetZ()*(Convert::zc2-Convert::zc1))==1.0)
//	    Convert::perp = crys.GetPerp();
//	if(math.Sign((crys.GetPerp()).GetZ()*(Convert::zc2-Convert::zc1))==-1.0)
//	    Convert::perp = crys.GetPerp()*(-1.0);
	if(math.Precision(trig.Angle(Line(Point(0.0,0.0,0.0),crys.GetPerp()),
	    Line(Point(Convert::xc1,Convert::yc1,Convert::zc1),
		 Point(Convert::xc2,Convert::yc2,Convert::zc2))),math.precision)>(acos(-1.0)/2.0)) {
		    Convert::perp = crys.GetPerp()*(-1.0);
	} else {
	    Convert::perp = crys.GetPerp();
	};
	Convert::substract();
	Convert::rotate_points();
	Convert::pDz();
	Convert::pTheta();
	Convert::pPhi();
	for(int j=0; j<8; j++) Convert::pts[j] = -1;
	Convert::compute_plane("lower");
	Convert::compute_plane("upper");
	
	// just numerical imprecision
	Convert::dg4.pAlp1 = (Convert::dg4.pAlp1+Convert::dg4.pAlp2)/2.0;
	if(Convert::math.Abs(Convert::dg4.pAlp1)<(1e-5)) Convert::dg4.pAlp1 = 0.0;
	Convert::dg4.pAlp2 = Convert::dg4.pAlp1;
	if(Convert::dg4.module==2) {
	    Convert::dg4.pAlp1 *= -1.0;
	    Convert::dg4.pAlp2 *= -1.0;
	};
	
	Convert::g4data.push_back(Convert::dg4);
    };
    return;
}

// rotating the axes - no need!
void Convert::rotate_axes(int igs_contor)
{
    for(int j=0; j<8; j++) {
//        Convert::digs.x[j] = Convert::igsdata[igs_contor].z[j];
//        Convert::digs.y[j] = Convert::igsdata[igs_contor].x[j];
//        Convert::digs.z[j] = Convert::igsdata[igs_contor].y[j];
        Convert::digs.x[j] = Convert::igsdata[igs_contor].x[j];
        Convert::digs.y[j] = Convert::igsdata[igs_contor].y[j];
        Convert::digs.z[j] = Convert::igsdata[igs_contor].z[j];
    };

    return;
}

// ordering over plans: p1 is the lower one (the first 4 points) and 
// p2 is the upper one (the last 4 points)
// in this version it can be found in the convert_igs_g4 function
void Convert::order()
{
    return;
}

// computing the center of the crystal position
void Convert::center_position()
{
    Convert::xc1 = 0.0;
    Convert::yc1 = 0.0;
    Convert::zc1 = 0.0;
    Convert::xc2 = 0.0;
    Convert::yc2 = 0.0;
    Convert::zc2 = 0.0;
    for(int j=0; j<4; j++) {
        Convert::xc1 += Convert::digs.x[j];
        Convert::yc1 += Convert::digs.y[j];
        Convert::zc1 += Convert::digs.z[j];
    };
    for(int j=4; j<8; j++) {
        Convert::xc2 += Convert::digs.x[j];
        Convert::yc2 += Convert::digs.y[j];
        Convert::zc2 += Convert::digs.z[j];
    };
    Convert::xc1 = Convert::xc1/4.0;
    Convert::yc1 = Convert::yc1/4.0;
    Convert::zc1 = Convert::zc1/4.0;
    Convert::xc2 = Convert::xc2/4.0;
    Convert::yc2 = Convert::yc2/4.0;
    Convert::zc2 = Convert::zc2/4.0;
    Convert::dg4.posX = (Convert::xc1+Convert::xc2)/2.0;
    Convert::dg4.posY = (Convert::yc1+Convert::yc2)/2.0;
    Convert::dg4.posZ = (Convert::zc1+Convert::zc2)/2.0;
    Convert::xc = Convert::dg4.posX;
    Convert::yc = Convert::dg4.posY;
    Convert::zc = Convert::dg4.posZ;

    return;
}

void Convert::substract()
{
    for(int i=0; i<8; i++) {
	Convert::digs.x[i] -= Convert::dg4.posX;
	Convert::digs.y[i] -= Convert::dg4.posY;
	Convert::digs.z[i] -= Convert::dg4.posZ;
    };
    Convert::xc1 -= Convert::dg4.posX;
    Convert::xc2 -= Convert::dg4.posX;
    Convert::yc1 -= Convert::dg4.posY;
    Convert::yc2 -= Convert::dg4.posY;
    Convert::zc1 -= Convert::dg4.posZ;
    Convert::zc2 -= Convert::dg4.posZ;

    return;
}

void Convert::rotate_points()
{
    double x1, y1, z1; // before
    double x2, y2, z2; // after
    double theta, phi, tau;
    Angles angs;
    Point pt;
    Algebra alg;
    
    angs = Angles(Convert::perp.GetPoint());
    theta = angs.GetTheta();
    phi = angs.GetOmega();
    
//    cout<<"Angles: "<<angs.GetTheta()*180.0/acos(-1.0)<<"; "
//	<<angs.GetChi()*180.0/acos(-1.0)<<"; "
//	<<angs.GetPsi()*180.0/acos(-1.0)<<"; "
//	<<angs.GetOmega()*180.0/acos(-1.0)<<endl;
    
    for(int i=0; i<8; i++) {
	x1 = Convert::digs.x[i];
	y1 = Convert::digs.y[i];
	z1 = Convert::digs.z[i];
	// Oz ccw rotation
	x2 = x1*cos(phi)+y1*sin(phi);
	y2 = y1*cos(phi)-x1*sin(phi);
	z2 = z1;
	Convert::digs.x[i] = x2;
	Convert::digs.y[i] = y2;
	Convert::digs.z[i] = z2;
    };
    for(int i=0; i<8; i++) {
	x1 = Convert::digs.x[i];
	y1 = Convert::digs.y[i];
	z1 = Convert::digs.z[i];
	// Oy ccw rotation
	x2 = x1*cos(theta)-z1*sin(theta);
	y2 = y1;
	z2 = z1*cos(theta)+x1*sin(theta);
	Convert::digs.x[i] = x2;
	Convert::digs.y[i] = y2;
	Convert::digs.z[i] = z2;
    };
//pt = alg.cross_product(Line(Point(Convert::digs.x[0],Convert::digs.y[0],Convert::digs.z[0]),
//			    Point(Convert::digs.x[1],Convert::digs.y[1],Convert::digs.z[1])),
//		       Line(Point(Convert::digs.x[2],Convert::digs.y[2],Convert::digs.z[2]),
//			    Point(Convert::digs.x[1],Convert::digs.y[1],Convert::digs.z[1]))
//		      );
//cout<<"X: "<<alg.scalar_product(Line(pt,Point(0.0,0.0,0.0)),Line(Point(1.0,0.0,0.0),Point(0.0,0.0,0.0)))<<endl;
//cout<<"Y: "<<alg.scalar_product(Line(pt,Point(0.0,0.0,0.0)),Line(Point(0.0,1.0,0.0),Point(0.0,0.0,0.0)))<<endl;
    // rotating planes centers
    for(int i=0; i<2; i++) {
	if(i==0) {
	    x1 = Convert::xc1;
	    y1 = Convert::yc1;
	    z1 = Convert::zc1;
	} else {
	    x1 = Convert::xc2;
	    y1 = Convert::yc2;
	    z1 = Convert::zc2;
	};
	// Oz ccw rotation
	x2 = x1*cos(phi)+y1*sin(phi);
	y2 = y1*cos(phi)-x1*sin(phi);
	z2 = z1;
	// the new coordinates
	x1 = x2;
	y1 = y2;
	z1 = z2;
	// Oy ccw rotation
	x2 = x1*cos(theta)-z1*sin(theta);
	y2 = y1;
	z2 = z1*cos(theta)+x1*sin(theta);
	if(i==0) {
	    Convert::xc1 = x2;
	    Convert::yc1 = y2;
	    Convert::zc1 = z2;
	} else {
	    Convert::xc2 = x2;
	    Convert::yc2 = y2;
	    Convert::zc2 = z2;
	};
    };
    
    pt = Point(Convert::digs.x[0]-Convert::digs.x[1],
	       Convert::digs.y[0]-Convert::digs.y[1],
	       Convert::digs.z[0]-Convert::digs.z[1]);
    angs = Angles(pt.GetX(),pt.GetY(),pt.GetZ());
    tau = angs.GetOmega();

    for(int i=0; i<8; i++) {
	x1 = Convert::digs.x[i];
	y1 = Convert::digs.y[i];
	z1 = Convert::digs.z[i];
	// Oz ccw rotation
	x2 = x1*cos(tau)+y1*sin(tau);
	y2 = y1*cos(tau)-x1*sin(tau);
	z2 = z1;
	Convert::digs.x[i] = x2;
	Convert::digs.y[i] = y2;
	Convert::digs.z[i] = z2;
    };
    // rotating planes centers
    for(int i=0; i<2; i++) {
	if(i==0) {
	    x1 = Convert::xc1;
	    y1 = Convert::yc1;
	    z1 = Convert::zc1;
	} else {
	    x1 = Convert::xc2;
	    y1 = Convert::yc2;
	    z1 = Convert::zc2;
	};
	// Oz ccw rotation
	x2 = x1*cos(tau)+y1*sin(tau);
	y2 = y1*cos(tau)-x1*sin(tau);
	z2 = z1;
	if(i==0) {
	    Convert::xc1 = x2;
	    Convert::yc1 = y2;
	    Convert::zc1 = z2;
	} else {
	    Convert::xc2 = x2;
	    Convert::yc2 = y2;
	    Convert::zc2 = z2;
	};
    };

    if(Convert::digs.y[0]>Convert::digs.y[3]) {
	for(int i=0; i<8; i++) {
	    x1 = Convert::digs.x[i];
	    y1 = Convert::digs.y[i];
	    z1 = Convert::digs.z[i];
	    // Oz ccw rotation
	    x2 = -1.0*x1;
	    y2 = -1.0*y1;
	    z2 = z1;
	    Convert::digs.x[i] = x2;
	    Convert::digs.y[i] = y2;
	    Convert::digs.z[i] = z2;
	};
	// rotating planes centers
	for(int i=0; i<2; i++) {
	    if(i==0) {
		x1 = Convert::xc1;
		y1 = Convert::yc1;
		z1 = Convert::zc1;
	    } else {
		x1 = Convert::xc2;
		y1 = Convert::yc2;
		z1 = Convert::zc2;
	    };
	    // Oz ccw rotation
	    x2 = -1.0*x1;
	    y2 = -1.0*y1;
	    z2 = z1;
	    if(i==0) {
		Convert::xc1 = x2;
		Convert::yc1 = y2;
		Convert::zc1 = z2;
	    } else {
		Convert::xc2 = x2;
		Convert::yc2 = y2;
		Convert::zc2 = z2;
	    };
	};
	tau -= acos(-1.0);
    };

    Convert::dg4.theta = 180.0*theta/acos(-1.0);
    Convert::dg4.phi = 180.0*phi/acos(-1.0);
    Convert::dg4.tau = 180.0*tau/acos(-1.0);

//cout<<Convert::digs.y[0]<<"; "<<Convert::digs.y[1]<<"; "<<Convert::digs.y[2]<<"; "<<Convert::digs.y[3]<<endl;

    return;
}

// half-length along the z-axis
void Convert::pDz()
{
    Convert::dg4.pDz = (Convert::math.Abs( Convert::zc1)+Convert::math.Abs( Convert::zc2))/2.0;
        
    return;
}

// polar angle
void Convert::pTheta()
{
    double lxc1, lxc2, lyc1, lyc2, lzc1, lzc2;
    lxc1 = Convert::xc1;
    lxc2 = Convert::xc2;
    lyc1 = Convert::yc1;
    lyc2 = Convert::yc2;
    lzc1 = Convert::zc1;
    lzc2 = Convert::zc2;
    Convert::dg4.pTheta = atan2(sqrt(pow(lxc2-lxc1,2)+pow(lyc2-lyc1,2)),(lzc2-lzc1));
    Convert::dg4.pTheta = 180.0*Convert::dg4.pTheta/acos(-1.0);

    return;
}

// azimuthal angle
void Convert::pPhi()
{
    double lxc1, lxc2, lyc1, lyc2;
    
    lxc1 = Convert::xc1;
    lxc2 = Convert::xc2;
    lyc1 = Convert::yc1;
    lyc2 = Convert::yc2;
    Convert::dg4.pPhi = atan2((lyc2-lyc1),(lxc2-lxc1));
    Convert::dg4.pPhi = 180.0*Convert::dg4.pPhi/acos(-1.0);

    return;
}

void Convert::compute_plane(const string igs_cp)
{
    Convert::igs_plane = igs_cp;
    Convert::pDy();
    Convert::pDxs();
    Convert::pAlp();

    return;
}

// half-length along y-axis at -/+pDz and sorting the points
void Convert::pDy()
{
    double pDy = -10e+10;
    int min;
    double yc;
    string lip = Convert::igs_plane;
	
    if(lip=="lower") {
        min = 0;
        yc = Convert::yc1;
    };
    if(lip=="upper") {
        min = 4;
        yc = Convert::yc2;
    };
	
/*    for(int l1=min; l1<min+3; l1++) for(int l2=l1+1; l2<=min+3; l2++)
    if(math.Abs(yc-(Convert::digs.y[l1]+Convert::digs.y[l2])/2.0)>pDy) {
    	Convert::pts[min] = l1;
	Convert::pts[min+1] = l2;
	pDy = math.Abs(yc-(Convert::digs.y[l1]+Convert::digs.y[l2])/2.0);
    };
    Convert::pts[min+2] = -1;
    for(int j=min; j<min+4; j++) 
    if((j!=Convert::pts[min])&&(j!=Convert::pts[min+1]))
    if(Convert::pts[min+2]==-1) {
        Convert::pts[min+2] = j;
    } else {
        Convert::pts[min+3] = j;
    };
*/

    pDy = 0.0;
    for(int i=0; i<4; i++) pDy += math.Abs(yc-Convert::digs.y[i+min]);
    pDy /= 4.0;

    if(lip=="lower") {
	Convert::dg4.pDy1 = pDy;
    };
    if(lip=="upper") {
	Convert::dg4.pDy2 = pDy;
    };

    return;
}

// very sensitive here!!!
// half-length along x-axis at -/+pDy and at -/+pDz
void Convert::pDxs()
{
    int min;
    double yc, pDy, pDx1, pDx2;
    string lip = Convert::igs_plane;
    Math math;
    
    if(lip=="lower") {
        min = 0;
        yc = Convert::yc1;
	pDy = Convert::dg4.pDy1;
    };
    if(lip=="upper") {
        min = 4;
        yc = Convert::yc2;
	pDy = Convert::dg4.pDy2;
    };
    
/*    if(((yc+pDy)-(Convert::digs.y[Convert::pts[min]]+Convert::digs.y[Convert::pts[min+1]])/2.0)<(1e-5)) {
        pDx1 = Convert::math.Abs((Convert::digs.x[Convert::pts[min]]-Convert::digs.x[Convert::pts[min+1]])/2.0);
        pDx2 = Convert::math.Abs((Convert::digs.x[Convert::pts[min+2]]-Convert::digs.x[Convert::pts[min+3]])/2.0);
    } else {
        pDx1 = Convert::math.Abs((Convert::digs.x[Convert::pts[min+2]]-Convert::digs.x[Convert::pts[min+3]])/2.0);
        pDx2 = Convert::math.Abs((Convert::digs.x[Convert::pts[min]]-Convert::digs.x[Convert::pts[min+1]])/2.0);
    };
*/

    pDx1 = math.Abs(Convert::digs.x[0+min]-Convert::digs.x[1+min])/2.0;
    pDx2 = math.Abs(Convert::digs.x[2+min]-Convert::digs.x[3+min])/2.0;

    if(lip=="lower") {
	Convert::dg4.pDx1 = pDx1;
	Convert::dg4.pDx2 = pDx2;
    };
    if(lip=="upper") {
	Convert::dg4.pDx3 = pDx1;
	Convert::dg4.pDx4 = pDx2;
    };

    return;
}

// angle of the deviation of y-midpoints line from y-axis at -/+pDz
void Convert::pAlp()
{
    double a, b, c;
    int min;
    double yc, pDy, pDx1, pDx2, xc, pAlp;
    string lip = Convert::igs_plane;
    Trigonometry trig;
    
    if(lip=="lower") {
        min = 0;
        yc = Convert::yc1;
        xc = Convert::xc1;
	pDy = Convert::dg4.pDy1;
	pDx1 = Convert::dg4.pDx1;
	pDx2 = Convert::dg4.pDx2;
    };
    if(lip=="upper") {
        min = 4;
        yc = Convert::yc2;
        xc = Convert::xc2;
	pDy = Convert::dg4.pDy2;
	pDx1 = Convert::dg4.pDx3;
	pDx2 = Convert::dg4.pDx4;
    };
/*    
    a = pDy;
    if(((yc+pDy)-(Convert::digs.y[Convert::pts[min]]+Convert::digs.y[Convert::pts[min+1]])/2.0)<(1e-5)) {
        b = sqrt(pow(pDx1,2)+pow((Convert::digs.y[Convert::pts[min+0]]-Convert::digs.y[Convert::pts[min+1]])/2.0,2));
        c = sqrt(pow((xc-Convert::digs.x[Convert::pts[min+1]]),2)+pow((yc-Convert::digs.y[Convert::pts[min+1]]),2));
    } else {
        b = sqrt(pow(pDx1,2)+pow((Convert::digs.y[Convert::pts[min+2]]-Convert::digs.y[Convert::pts[min+3]])/2.0,2));
        c = sqrt(pow((xc-Convert::digs.x[Convert::pts[min+3]]),2)+pow((yc-Convert::digs.y[Convert::pts[min+3]]),2));
    };
    pAlp = acos((a*a+b*b-c*c)/(2.0*a*b));
    pAlp = 180.0*pAlp/acos(-1.0);
    pAlp = 90.0-pAlp;
*/

    pAlp = trig.Angle(Line(Point(0.0,-1.0,0.0),Point(0.0,1.0,0.0)),
		      Line(Point((Convert::digs.x[0+min]+Convert::digs.x[1+min])/2.0,yc-pDy,0.0),
		    	   Point((Convert::digs.x[2+min]+Convert::digs.x[3+min])/2.0,yc+pDy,0.0)));
    pAlp = 180.0*pAlp/acos(-1.0);

    if(lip=="lower") {
	Convert::dg4.pAlp1 = pAlp;
    };
    if(lip=="upper") {
	Convert::dg4.pAlp2 = pAlp;
    };

    return;
}

void Convert::manager()
{
//    Convert::multiplication();
    Convert::convert_igs_g4();
    
    return;
}
