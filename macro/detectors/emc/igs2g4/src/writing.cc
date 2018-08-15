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

#include "include/writing.hh"

using namespace std;

Writing::Writing()
{
}

Writing::~Writing()
{
}

void Writing::writing_g4()
{
    fstream f;
    string path, buffer;
    
    system("if [ -f temp.txt ] ; then rm -f temp.txt ; fi");
    system("pwd > temp.txt");
    f.open("temp.txt", ios::in);
    f>>path;
    f.close();
    f.clear();
    system("if [ -f temp.txt ] ; then rm -f temp.txt ; fi");
    
    path += "/data/module1.dat";
    buffer = "if [ -f "+path+" ] ; then rm -f "+path+" ; fi";
    system(buffer.c_str());
    f.open(path.c_str(), ios ::out);
    f.width(8);
    f.setf(ios::internal);
    f<<"module";
    f.width(8);
    f.setf(ios::internal);
    f<<"row";
    f.width(8);
    f.setf(ios::internal);
    f<<"crystal";
    f.width(12);
    f.setf(ios::internal);
    f<<"theta";
    f.width(12);
    f.setf(ios::internal);
    f<<"phi";
    f.width(12);
    f.setf(ios::internal);
    f<<"tau";
    f.width(12);
    f.setf(ios::internal);
    f<<"posX";
    f.width(12);
    f.setf(ios::internal);
    f<<"posY";
    f.width(12);
    f.setf(ios::internal);
    f<<"posZ";
    f.width(12);
    f.setf(ios::internal);
    f<<"pDz";
    f.width(12);
    f.setf(ios::internal);
    f<<"pTheta";
    f.width(12);
    f.setf(ios::internal);
    f<<"pPhi";
    f.width(12);
    f.setf(ios::internal);
    f<<"pDy1";
    f.width(12);
    f.setf(ios::internal);
    f<<"pDx1";
    f.width(12);
    f.setf(ios::internal);
    f<<"pDx2";
    f.width(12);
    f.setf(ios::internal);
    f<<"pAlp1";
    f.width(12);
    f.setf(ios::internal);
    f<<"pDy2";
    f.width(12);
    f.setf(ios::internal);
    f<<"pDx3";
    f.width(12);
    f.setf(ios::internal);
    f<<"pDx4";
    f.width(12);
    f.setf(ios::internal);
    f<<"pAlp2";
    f<<endl;
    f.width(8);
    f.setf(ios::internal);
    f<<"#";
    f.width(8);
    f.setf(ios::internal);
    f<<"#";
    f.width(8);
    f.setf(ios::internal);
    f<<"#";
    f.width(12);
    f.setf(ios::internal);
    f<<"[deg]";
    f.width(12);
    f.setf(ios::internal);
    f<<"[deg]";
    f.width(12);
    f.setf(ios::internal);
    f<<"[deg]";
    f.width(12);
    f.setf(ios::internal);
    f<<"[mm]";
    f.width(12);
    f.setf(ios::internal);
    f<<"[mm]";
    f.width(12);
    f.setf(ios::internal);
    f<<"[mm]";
    f.width(12);
    f.setf(ios::internal);
    f<<"[mm]";
    f.width(12);
    f.setf(ios::internal);
    f<<"[deg]";
    f.width(12);
    f.setf(ios::internal);
    f<<"[deg]";
    f.width(12);
    f.setf(ios::internal);
    f<<"[mm]";
    f.width(12);
    f.setf(ios::internal);
    f<<"[mm]";
    f.width(12);
    f.setf(ios::internal);
    f<<"[mm]";
    f.width(12);
    f.setf(ios::internal);
    f<<"[deg]";
    f.width(12);
    f.setf(ios::internal);
    f<<"[mm]";
    f.width(12);
    f.setf(ios::internal);
    f<<"[mm]";
    f.width(12);
    f.setf(ios::internal);
    f<<"[mm]";
    f.width(12);
    f.setf(ios::internal);
    f<<"[deg]";
    f<<endl<<endl;
    for(int i=0; i<Writing::g4data.size(); i++) {
	f.width(8);
	f.setf(ios::right);
	f<<Writing::g4data[i].module;
	f.width(8);
	f.setf(ios::right);
	f<<Writing::g4data[i].row;
	f.width(8);
	f.setf(ios::right);
	f<<Writing::g4data[i].crystal;
	f.width(12);
	f.precision(6);
	f.setf(ios::right);
	f<<Writing::g4data[i].theta;
	f.width(12);
	f.precision(6);
	f.setf(ios::right);
	f<<Writing::g4data[i].phi;
	f.width(12);
	f.precision(6);
	f.setf(ios::right);
	f<<Writing::g4data[i].tau;
	f.width(12);
	f.precision(6);
	f.setf(ios::right);
	f<<Writing::g4data[i].posX;
	f.width(12);
	f.precision(6);
	f.setf(ios::right);
	f<<Writing::g4data[i].posY;
	f.width(12);
	f.precision(6);
	f.setf(ios::right);
	f<<Writing::g4data[i].posZ;
	f.width(12);
	f.precision(6);
	f.setf(ios::right);
	f<<Writing::g4data[i].pDz;
	f.width(12);
	f.precision(6);
	f.setf(ios::right);
	f<<Writing::g4data[i].pTheta;
	f.width(12);
	f.precision(6);
	f.setf(ios::right);
	f<<Writing::g4data[i].pPhi;
	f.width(12);
	f.precision(6);
	f.setf(ios::right);
	f<<Writing::g4data[i].pDy1;
	f.width(12);
	f.precision(6);
	f.setf(ios::right);
	f<<Writing::g4data[i].pDx1;
	f.width(12);
	f.precision(6);
	f.setf(ios::right);
	f<<Writing::g4data[i].pDx2;
	f.width(12);
	f.precision(6);
	f.setf(ios::right);
	f<<Writing::g4data[i].pAlp1;
	f.width(12);
	f.precision(6);
	f.setf(ios::right);
	f<<Writing::g4data[i].pDy2;
	f.width(12);
	f.precision(6);
	f.setf(ios::right);
	f<<Writing::g4data[i].pDx3;
	f.width(12);
	f.precision(6);
	f.setf(ios::right);
	f<<Writing::g4data[i].pDx4;
	f.width(12);
	f.precision(6);
	f.setf(ios::right);
	f<<Writing::g4data[i].pAlp2;
	f<<endl;
    };
    f.flush();
    f.close();
    f.clear();
    
    return;
}

void Writing::manager()
{
    Writing::writing_g4();
    
    return;
}
