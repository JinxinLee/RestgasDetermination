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
#include "PndTpcInhFieldDrifter.h"
#include "PndMultiField.h"
#include "PndTransMap.h"
#include "PndDipoleMap.h"
#include "PndSolenoidMap.h"
#include "PndTpcEFieldCyl.h"


void createDevMapMulti(int rBin) {

  //define variables
  int rBins = 26;   
  int zBins = 149;
  int _rBin = rBin;

  std::string basedir = "/afs/e18/panda/SIM/fboehmer/trunk/";
  std::string targetdir = "/afs/e18/panda/DATA/fboehmer/dipl_data/SpaceCharge/07_01_2009";
  
  std::string efield_file = basedir+"tpc/EField_G3_ALICE_L5_1MeV_dCUTS_wPIPE_wMVD_march09.dat";
  
  // Use PndTpcEFieldCyl for building the B-Field (custom input) ------------
  
  std::string bfield_file = "andrea_b_field.dat"; //for bfield as efield
  // ------------------------------------------------------------------------
  
  std::string out_file = targetdir+"DevMap_Efield_March_09_Bfield_Official_highRes.dat";
  std::string param_file = basedir+"tpc/tpc.par";
  
  
  // PndMultiField part -----------------------------------------------------
  
  PndMultiField *bField= new PndMultiField();
  
  PndTransMap *map_tr= new PndTransMap("TransMap", "R");
  PndDipoleMap *map_d1= new PndDipoleMap("DipoleMap", "R");
  //  PndDipoleMap *map_d2= new PndDipoleMap("DipoleMap2", "R");
  PndSolenoidMap *map_s1= new PndSolenoidMap("SolenoidMap1", "R");
  PndSolenoidMap *map_s2= new PndSolenoidMap("SolenoidMap2", "R");
  PndSolenoidMap *map_s3= new PndSolenoidMap("SolenoidMap3", "R");
  PndSolenoidMap *map_s4= new PndSolenoidMap("SolenoidMap4", "R");
  
  bField->AddField(map_tr);
  bField->AddField(map_d1);
  //bField->AddField(map_d2);
  bField->AddField(map_s1);
  bField->AddField(map_s2);
  bField->AddField(map_s3);
  bField->AddField(map_s4);
  
  bField->Init();
  
  //create Efield
  PndTpcEFieldCyl* eField = new PndTpcEFieldCyl(efield_file.c_str());
   
  //if(bField != 0)
  //  std::cout<<"\nsuccessfully initialized the B-Field";
  
  // ------------------------------------------------------------------------
  
  
  
  // start the simulation
  // PndTpcInhFieldDrifter* drifter = new PndTpcInhFieldDrifter(efield_file.c_str(),
  // 							   bfield_file.c_str(),
// 							   out_file.c_str(),
// 							   param_file.c_str(),
// 							   rBins, zBins, _rBin);
  
  PndTpcInhFieldDrifter* drifter = new PndTpcInhFieldDrifter(eField,
							     bField,
							     out_file.c_str(),
							     param_file.c_str(),
							     rBins, zBins, _rBin);
  
  
  std::cout<<"\nStarting the Drifter..."<<std::endl;
  
  //Set Field Mode
  
  drifter->run();
  delete drifter;
  //delete bField;

}
