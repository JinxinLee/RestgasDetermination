// this is a root script to demonstrate
// alignment and navigation through root geometries
// A geometry is loaded from a roof file
// please modify the path to the root file
// according to your needs
// single methods can be operated easily out
// and placed somewhere else
// author: Promme (Promme@web.de)

// run by calling
// root -l Modify_root_geometry.C+

#include <iostream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;


#include "TGeoManager.h"

const string filename = "/home/jasinski/bin/pandaroot/geometry/Luminosity-Detector.root";
//const string filename = "/home/jasinski/bin/pandaroot/macro/lmd/test/mom_1_5/Lumi_Params_0.root";

// load a geometry provided by the filename above
bool Load_Geometry(){
	TGeoManager::Import(filename.c_str());
	//gGeoManager->Draw("ogl"); <- keeps breaking when not done within the context of PandaROOT
	return true;
}

// actually this recursive call returns a list of paths to nodes
// in the loaded geometry
// if all_nodes == true then EVERY node is returned
void Get_List_of_Sensors(vector <string>& list_of_sensors, bool all_nodes = false, bool first_call = true){
	if (first_call){
		gGeoManager->CdTop();
		first_call = false;
	}
	//cout << gGeoManager->GetPath() << " ";
	string nodename(gGeoManager->GetCurrentNavigator()->GetCurrentNode()->GetName());
	//cout << "current node is " << nodename << endl;
	//if (nodename.compare(0, volumename_without_copynumber.size(), volumename_without_copynumber) == 0){ <<< here you can compare to a specific name in the volume to be selective to it
	int nnodes = gGeoManager->GetCurrentNode()->GetNdaughters();
	if (nnodes == 0 || all_nodes){ // a leave is reached when no daughter volumes are accessible anymore
		//cout << " found a sensor " << nav_paths[7] << endl;
		list_of_sensors.push_back(gGeoManager->GetPath());
	}
	for (int i = 0; i < nnodes; i++){
		//cout << " navigating into node " << i << endl;
		gGeoManager->CdDown(i);
		Get_List_of_Sensors(list_of_sensors, all_nodes, first_call);
		//if (nodename == nav_paths[1] || found_lmd){ <<< this is lmd search specific and not part of this spinout
		//	cout << " found the lmd node! Aborting recursive search. " << endl;
		//	gGeoManager->CdUp();
		//	break;
		//}
		gGeoManager->CdUp();
	}
	//if (nodename.compare(0, nav_paths[0].size(), nav_paths[0]) == 0){
	//	result = gGeoManager->GetPath();
	//	cout << " top volume is " << result << endl;
	//}
}

/*
// if aligned == false the original not aligned matrix will be returned
TGeoHMatrix* Get_matrix(string path, bool aligned = true){
	TGeoHMatrix* result = NULL;
	if (!gGeoManager->CheckPath(path.c_str())){
		cout << " Error in Get_matrix: path " << path << " is not valid " << endl;
		return result;
	}
	if (!aligned){ // try to obtain the original matrix, if available
		TGeoPNEntry *pne;
		//TGeoHMatrix *ide;  // ideal
		//TGeoHMatrix *mis;  // misaligned
		stringstream uname;
		uname << "foo_" <<
		pne = gGeoManager->GetAlignableEntry(uname.c_str());
		if (pne){
			TGeoPhysicalNode *node = pne->GetPhysicalNode();// new TGeoPhysicalNode(path.c_str());//
			if (node){
				result = new TGeoHMatrix(*node->GetOriginalMatrix());
				//if (aligned) result = new TGeoHMatrix(*node->GetNode(node->GetLevel())->GetMatrix());
				return result;
			} else {
				cout << " no node found in pn entry at " << path << endl;
				cout << " obtaining default matrix " << endl;
			}
		} else {
			cout << " no pn entry (alignable node) found in " << path << endl;
			cout << " obtaining default matrix " << endl;
		}
	}
	// I don't care if it was aligned or not, use the last one
	gGeoManager->cd(path.c_str());
	TGeoMatrix* matrix = gGeoManager->GetCurrentNode()->GetMatrix();
	if (!matrix)
		return NULL;
	result = new TGeoHMatrix(*(matrix));
	return result;
}*/

bool Set_matrix(string path, TGeoHMatrix* matrix, string uniquename, int uniqueID){
	if (!matrix) return false;
	if (!gGeoManager->CheckPath(path.c_str())){
		cout << " Error in PndLmdDim::Set_matrix: path " << path << " is not valid " << endl;
		return false;
	}
	TGeoPNEntry *pne;
	TGeoPhysicalNode *node;
	pne = gGeoManager->GetAlignableEntry(uniquename.c_str());
	if (!pne){
		cout << " creating alignable entry for " << path << endl;
		pne = gGeoManager->SetAlignableEntry(uniquename.c_str(), path.c_str(), uniqueID);
		if (pne){
			node = new TGeoPhysicalNode(path.c_str());
			pne->SetPhysicalNode(node);
		}
	}
	if (!pne) {
		cout << " Error: no pn entry (alignable node) at " << path << " created " << endl;
		return false;
	}
	node = pne->GetPhysicalNode();
	if (!node) {
		cout << " no node found for pn entry (alignable node) at " << path << endl;
		return false;
	}
	return node->Align(matrix);
}

int Modify_root_geometry(){
	Load_Geometry();
	vector <string> list_of_sensors;
	Get_List_of_Sensors(list_of_sensors, false);
	int uID = 0;
	for (int inode = 0; inode = list_of_sensors.size(); inode++){
		cout << list_of_sensors[inode] << endl;
		stringstream uname;

	}
  return 0;
}
