//Root Script to create and plot the deviation Map using Runge Kutta 
//Author: Felix Boehmer

{
#include <fstream>
#include <vector>
#include <string>

 
 //define variables
int rBins = 3;   //default values
int zBins = 5;
std::string efield_file = "EField_smallsteps_GEANT3_ALICE_L5_1MeV_cuts_withPIPE_MVD.dat";
std::string bfield_file = "andrea_b_field.dat"; //for bfield as efield
std::string out_file = "DevMap_TEST.dat";
std::string param_file = "tpc/tpc.par";

// std::cout<<"\n\nEnter name of E-Field-file (usually DOLFIN output): ";
// getline(std::cin,efield_file);
// std::cout<<std::endl;
// std::ifstream infile(efield_file.c_str(), std::fstream::in);
// if (!infile.good())
// {
//   std::cerr<<"Not Existing"<<std::endl;
//   return(0);
// }
// std::cout<<"Enter the desired number of bins in r-direction: ";
// std::cin>>rBins;
// std::cout<<std::endl;
// std::cout<<"Enter the desired number of bins in z-direction: ";
// std::cin>>zBins;
// std::cout<<std::endl;


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

PndTpcInhFieldDrifter* drifter = new PndTpcInhFieldDrifter(efield_file.c_str(),
							   bfield_file.c_str(),
							   out_file.c_str(),
							   param_file.c_str(),
							   rBins, zBins);
std::cout<<"\nStarting the Drifter..."<<std::endl;
drifter->run();
delete drifter;
//delete bField;

}
