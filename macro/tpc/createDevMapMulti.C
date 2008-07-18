//ROOT script to create the deviation Map using Runge Kutta 
//Argument rBin forces PndTpcInhFieldDrifter to calculate only one row in r
//In this way the macro can effectively be run on the cluster
//Output will be rBin ascii files which need to be combined to one 
//generic Deviation Map file.

//Author: Felix Boehmer


#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include "tpc/PndTpcInhFieldDrifter.h"


void createDevMapMulti(int rBin) {

//define variables
int rBins = 15;   
int zBins = 30;
int _rBin = rBin;

std::string efield_file = "Efield_21-05-08.dat";  //DOLFIN output

// Use PndTpcEFieldCyl for building the B-Field (custom input) ------------

std::string bfield_file = "andrea_b_field.dat"; //for bfield as efield
// ------------------------------------------------------------------------

std::string out_file = "DevMap_corrected_zeroB_30-05-08_try3.dat";


// PndMultiField part -----------------------------------------------------

// PndMultiField* bField= new PndMultiField();

// PndTransMap* map= new PndTransMap("TransMap", "R");
// PndDipoleMap* map1= new PndDipoleMap("DipoleMap", "R");
// PndSolenoidMap* map2= new PndSolenoidMap("SolenoidMap", "R");
// bField->AddField(map);
// bField->AddField(map1);
// bField->AddField(map2);
// bField->Init();

//if(bField != 0)
//  std::cout<<"\nsuccessfully initialized the B-Field";

// ------------------------------------------------------------------------



// start the simulation
PndTpcInhFieldDrifter* drifter = new PndTpcInhFieldDrifter(efield_file.c_str(),
							   bfield_file.c_str(),
							   out_file,
							   rBins, zBins,_rBin);
std::cout<<"\nStarting the Drifter..."<<std::endl;

//Set Field Mode
drifter->turnOffEDist(false);
drifter->turnOffBField(true);

drifter->run();
delete drifter;
//delete bField;

}
