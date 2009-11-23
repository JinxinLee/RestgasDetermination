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

#include "include/manager.hh"
#include "include/data_igs.hh"
#include "include/data_g4.hh"

using namespace std;

Manager::Manager()
{
}

Manager::~Manager()
{
}

void Manager::copy_read_convert()
{
    DataIGS data;
    
    for(int i=0; i<Manager::read.igsdata.size(); i++) {
	data.crystal = Manager::read.igsdata[i].crystal;
	data.row = Manager::read.igsdata[i].row;
	data.module = Manager::read.igsdata[i].module;
	for(int j=0; j<8; j++) {
	    data.x[j] = Manager::read.igsdata[i].x[j];
	    data.y[j] = Manager::read.igsdata[i].y[j];
	    data.z[j] = Manager::read.igsdata[i].z[j];
	};
	Manager::convert.igsdata.push_back(data);
    };
    
    return;
}

void Manager::copy_convert_write()
{
    DataG4 data;
    
    for(int i=0; i<Manager::convert.g4data.size(); i++) {
	data.crystal = Manager::convert.g4data[i].crystal;
	data.row = Manager::convert.g4data[i].row;
	data.module = Manager::convert.g4data[i].module;
	data.theta = Manager::convert.g4data[i].theta;
	data.phi = Manager::convert.g4data[i].phi;
	data.tau = Manager::convert.g4data[i].tau;
	data.posX = Manager::convert.g4data[i].posX;
	data.posY = Manager::convert.g4data[i].posY;
	data.posZ = Manager::convert.g4data[i].posZ;
	data.pDz = Manager::convert.g4data[i].pDz;
	data.pTheta = Manager::convert.g4data[i].pTheta;
	data.pPhi = Manager::convert.g4data[i].pPhi;
	data.pDy1 = Manager::convert.g4data[i].pDy1;
	data.pDx1 = Manager::convert.g4data[i].pDx1;
	data.pDx2 = Manager::convert.g4data[i].pDx2;
	data.pAlp1 = Manager::convert.g4data[i].pAlp1;
	data.pDy2 = Manager::convert.g4data[i].pDy2;
	data.pDx3 = Manager::convert.g4data[i].pDx3;
	data.pDx4 = Manager::convert.g4data[i].pDx4;
	data.pAlp2 = Manager::convert.g4data[i].pAlp2;
	Manager::write.g4data.push_back(data);
    };
    
    return;
}

void Manager::manager()
{
    system("clear");
    cout<<"Reading the data... ";
    cout.flush();
    Manager::read.manager();
    cout<<"[successfully done]"<<endl<<endl;
    Manager::copy_read_convert();
    cout<<"Converting the data... "<<endl;
    Manager::convert.manager();
    cout<<"Overall process... [successfully done]"<<endl<<endl;
    Manager::copy_convert_write();
    cout<<"Writing the output... ";
    cout.flush();
    Manager::write.manager();
    cout<<"[successfully done]"<<endl<<endl;
    cout<<"Time spent for this program: ";
    
    return;
}
