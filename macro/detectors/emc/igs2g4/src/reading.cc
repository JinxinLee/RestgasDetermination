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
#include <string>
#include <fstream>
#include <vector>

#include "include/reading.hh"
#include "include/data_igs.hh"

using namespace std;

Reading::Reading()
{
}

Reading::~Reading()
{
}

void Reading::read_igs()
{
    fstream f;
    string path, line, module;
    vector<string> igsline;
    DataIGS data;
    string::size_type pos_label, pos_pct, pos_start, pos_end;
    
    system("if [ -f tmp.txt ] ; then rm -f tmp.txt ; fi ");
    system("pwd > tmp.txt");
    f.open("tmp.txt", ios::in);
    f>>path;
    f.close();
    f.clear();
    system("if [ -f tmp.txt ] ; then rm -f tmp.txt ; fi ");
    
    path += "/data/emcbar.igs";
    f.open(path.c_str(), ios::in);
    while(getline(f,line,'\n')) {
	igsline.push_back(line);
    };
    f.close();
    f.clear();
    
    data.crystal = 0;
    data.row = 1;
    data.module = 3;
    for(int i=0; i<igsline.size(); i++){
	pos_label = igsline[i].find("406,", 0);
	if((pos_label != string::npos)&&(pos_label == 0)) {
//	    pos_start = igsline[i].find("HTranslation", 0);
//	    pos_start += 12;
//	    pos_end = igsline[i].find(",",pos_start);
//	    pos_end--;
//	    data.crystal = atoi(igsline[i].substr(pos_start, pos_end-pos_start).c_str());
//	    pos_start = igsline[i].find("row", 0);
//	    pos_start += 4;
//	    pos_end = igsline[i].find(",", pos_start);
//	    data.row = atoi(igsline[i].substr(pos_start, pos_end-pos_start).c_str());
	    data.crystal++;
//	    cout<<igsline[i]<<endl;
	    int k = 0;
	    int l = 0;
	    while(l<7) {
		pos_pct = igsline[i+k+1].find("110,", 0);
		if(pos_pct != string::npos) {
		    pos_start = igsline[i+k+1].find(",", 0);
		    pos_start++;
		    pos_end = igsline[i+k+1].find(",", pos_start);
		    data.x[l] = (double)atof(igsline[i+k+1].substr(pos_start, pos_end-pos_start).c_str());
		    pos_start = pos_end+1;
		    pos_end = igsline[i+k+1].find(",", pos_start);
		    data.y[l] = (double)atof(igsline[i+k+1].substr(pos_start, pos_end-pos_start).c_str());
		    pos_start = pos_end+1;
		    pos_end = igsline[i+k+1].find(",", pos_start);
		    data.z[l] = (double)atof(igsline[i+k+1].substr(pos_start, pos_end-pos_start).c_str());
		    l++;
		};
		k++;
	    };
	    k--;
	    pos_start = pos_end+1;
	    pos_end = igsline[i+k+1].find(",", pos_start);
	    if(pos_end == string::npos) {
		k++;
		pos_start = 0;
		pos_end = igsline[i+k+1].find(",", pos_start);
	    };
	    data.x[l] = (double)atof(igsline[i+k+1].substr(pos_start, pos_end-pos_start).c_str());
	    pos_start = pos_end+1;
	    pos_end = igsline[i+k+1].find(",", pos_start);
	    if(pos_end == string::npos) {
		k++;
		pos_start = 0;
		pos_end = igsline[i+k+1].find(",", pos_start);
	    };
	    data.y[l] = (double)atof(igsline[i+k+1].substr(pos_start, pos_end-pos_start).c_str());
	    pos_start = pos_end+1;
	    pos_end = igsline[i+k+1].find(",", pos_start);
	    if(pos_end == string::npos) {
		k++;
		pos_start = 0;
		pos_end = igsline[i+k+1].find(",", pos_start);
	    };
	    data.z[l] = (double)atof(igsline[i+k+1].substr(pos_start, pos_end-pos_start).c_str());
//	    if((data.crystal>=1)&&(data.crystal<=10)) {
//		if(Reading::igsdata.size()>0)
//		if(data.row<Reading::igsdata[Reading::igsdata.size()-1].row) 
//		    data.module = 2;
		Reading::igsdata.push_back(data);
//	    };
	};
    };
    
//    cout<<Reading::igsdata.size()<<endl;
    
    return;
}

void Reading::manager()
{
    Reading::read_igs();
    
    return;
}
