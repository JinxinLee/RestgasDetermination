//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcInhFieldDrifter
//     
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//     	Felix Boehmer           TUM         (original author)
//	
//-----------------------------------------------------------


// This Class' Header ------------------
#include "PndTpcInhFieldDrifter.h"

// Collaborating Class Headers --------

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <assert.h>
#include <sstream>
#include "PndTpcEFieldCyl.h"
#include "PndTpcGas.h"
#include "FairParAsciiFileIo.h"
#include "FairRunAna.h"
#include "FairParSet.h"
#include "FairRuntimeDb.h"
#include "PndTpcDigiPar.h"
#include "PndTpcRungeKutta.h"


// Class Member definitions -----------

PndTpcInhFieldDrifter::
PndTpcInhFieldDrifter(PndTpcEFieldCyl* eField,
		      PndMultiField* bField,
		      const char* outFile,
		      const char* paramFile,
		      const int rBins, const int zBins, 
		      const int split)
  :
	foutFile(outFile),
	fparamFile(paramFile),
	frBins(rBins),
        fzBins(zBins),
	fsplit(split)
{
  if(fsplit >= 0 && split >= frBins) {   //in the case split was manually set...
    std::cout << "PndTpcInhFieldDrifter::PndTpcInhFieldDrifter : "
	      <<"invalid row number requested! ABORTING"<<std::endl;
    throw 1;
  }
  
  //init parameters
  initParams();
  
  double vDrift = ((PndTpcGas*)fpar->getGas())->VDrift();
  double eNomZ = (eField->nominal()).Z();
  ffriction = 1e-5 * eNomZ / vDrift;  // friction term, see Rolandi Blum    
    
  frunKut = new PndTpcRungeKutta(1e-16, 1e-6, 1e-2, 1e-3,
			      fsc, eField,bField, ffriction);
  
}


PndTpcInhFieldDrifter::
PndTpcInhFieldDrifter(const char* eFieldFile,
		      const char* bFieldFile,
		      const char* outFile,
		      const char* paramFile,
		      const int rBins, const int zBins, 
		      const int split)
  :
	foutFile(outFile),
	fparamFile(paramFile),
	frBins(rBins),
        fzBins(zBins),
	fsplit(split)
{
  if(fsplit >= 0 && split >= frBins) {   //in the case split was manually set...
    std::cout << "PndTpcInhFieldDrifter::PndTpcInhFieldDrifter : "
	      <<"invalid row number requested! ABORTING"<<std::endl;
    throw 1;
  }
  
  //init parameters
  initParams();
  
  double vDrift = ((PndTpcGas*)fpar->getGas())->VDrift();
  
  PndTpcEFieldCyl* efield = new PndTpcEFieldCyl(eFieldFile);
  double eNomZ = (efield->nominal()).Z();
  ffriction = 1e-5 * eNomZ / vDrift;  // friction term, see Rolandi Blum    
    
  frunKut = new PndTpcRungeKutta(1e-16, 1e-6, 1e-2, 1e-3,
			      fsc, eFieldFile,bFieldFile, ffriction);
  delete efield;
}



PndTpcInhFieldDrifter::~PndTpcInhFieldDrifter()
{
  delete frunKut;
  int size = fvelocity_control->size();
  for (int i=0; i<size; i++)
    delete fvelocity_control->at(i);
  delete fvelocity_control;
}


void 
PndTpcInhFieldDrifter::run()
{
  // Calculate the Binning
  frBinWidth = (double)(ftpcMaxR - ftpcMinR) / frBins;
  fzBinWidth = (double)(ftpcMaxZ - ftpcMinZ) / fzBins;

  // Init the fields
  
  //TODO: dangerous!!! change to pointers inside 
  for (int nr = 0; nr < frBins; nr++)
  {
    fdevX.push_back(std::vector<double>(fzBins, 0));
    fdevY.push_back(std::vector<double>(fzBins, 0));
    ftime.push_back(std::vector<double>(fzBins, 0));
    fstepCount.push_back(std::vector<int>(fzBins, 0));
    fpathLength.push_back(std::vector<double>(fzBins, 0));
    
  }

  fvelocity_control = new std::vector<std::vector<double>*>;
  fvelocity_control->push_back(new std::vector<double>(4, 0)); //first position

  std::cout << "\nCalculating the Deviation Map with Runge-Kutta "<<std::endl;
  int errorcount = 0;
  
  
  //Calculate the Deviation-Map with Runge Kutta
  for (int nr = 0; nr < frBins; nr++)
  {
    if(fsplit != -1) {  //we want to split: only calculate one row in r
      if(nr != fsplit)
	continue;
    }
    for (int nz = 0; nz < fzBins; nz++)
    {
      double rPos, zPos, dx, dy, dz;

      rPos = (double)(ftpcMinR + (nr+0.5)*frBinWidth)/100.; // [m]
      zPos = (double)(ftpcMinZ + (nz+0.5)*fzBinWidth)/100.; 

      // Create the speed/position-vector for RungeKutta
      double x[6]; // x[0-2] = speed, x[3-5] = pos. (SI)

      x[3] = rPos;  // == x coordinate. symmetry!
      x[4] = 0.;    // == y
      x[5] = zPos;
      x[0] = 0.;
      x[1] = 0.;
      x[2] = -0.0027314 * 1e7;

     
      double xCopy[6]; //store the starting position
      for (int n=0; n<=5; n++)
        xCopy[n] = x[n];
      
      frunKut->setTimeStep(1e-16);
      double time_temp;

      for (int i=0; ; i++)  // get serious
      {
        if (x[5]>(ftpcMaxZ)/100. || x[5]<(ftpcMinZ/100.))	   
	  break;   	// finished when we reach the "Pads"
	
	double xTemp[3]; //needed for the drift path monitoring
        for (int n=0; n<3; n++)
          xTemp[n] = x[n+3];

        for (int j=0; ; j++)	//one step of RungeKutta
	{
	  time_temp = frunKut->getTimeStep();//store BEFORE step 
	  if (frunKut->getTimeStep() == 0)
	    frunKut->setTimeStep(1e-16);
	  if (frunKut->stepForwards(x))  //try until successful
	    break;
	}
	ftime.at(nr).at(nz) += time_temp;;
	fstepCount.at(nr).at(nz) ++;
	
        dx = x[3] - xTemp[0];
	dy = x[4] - xTemp[1]; //coordinate changes of the last RK step
        dz = x[5] - xTemp[2];
        fpathLength.at(nr).at(nz) += sqrt(dx*dx + dy*dy + dz*dz);
	
	//build the velocity-control array (in just one bin)
	if (nr == (int) std::floor((double)frBins/2) 
	    && nz == (int) std::floor((double)fzBins/2))
	{
	  if ((fstepCount.at(nr).at(nz))%100 == 0) 
	  {
	    std::vector<double>* temp_vec = new std::vector<double>(4);
	    for(int ii = 0; i <= 2; ii++)
	      temp_vec->at(ii) = x[ii];    //fill vector with components of V
	    temp_vec->at(3) = frunKut->getTimeStep();
	    fvelocity_control->push_back(temp_vec);
	
	  }
	}  
      }
      

      if(x[3]==xCopy[3] && x[4]==xCopy[4])
        errorcount++;

      // Now fill the 2 deviation fields with the values
      fdevX.at(nr).at(nz) = (x[3]-xCopy[3]) * 100;    //[cm]
      fdevY.at(nr).at(nz) = (x[4]-xCopy[4]) * 100;
    }
  std::cout<<"* "<<std::endl;
  } 	// Finish Loop over all bins

  std::cout << "\n\nAll done! \nThere have been " << errorcount 
       << " unprocessed bins..." << std::endl;

  writeToFile(foutFile);
}

void
PndTpcInhFieldDrifter::writeToFile(const char* out_file)
{
  std::cout << "\n\n**** Writing Deviation Data to File " << out_file 
	    << " ****" << std::endl;

  std::string filename(out_file);

  if(fsplit >= 0) {
    std::string s;
    std::stringstream stream;
    stream << fsplit;
    stream >> s;
    filename += s;
  }
  
  std::ofstream outfile(filename.c_str(), std::fstream::out);

  if(fsplit == 0 || fsplit == -1) {
    outfile<<std::setprecision(6);
    outfile<<frBins<<"   "<<fzBins<<"   "<<ftpcMinR<<"   "<<ftpcMaxR
	   <<"   "<<ftpcMinZ<<"   "<<ftpcMaxZ<<std::endl;
  }

  for (int nr=0; nr<frBins; nr++)
  {
    if (fsplit>=0) {
      if (nr != fsplit)
	continue;
    }
      
    for (int nz=0; nz<fzBins; nz++) 
    {
      outfile<<std::setprecision(8)<<std::setw(8);
      outfile<<fdevX.at(nr).at(nz)<<"   "
	     <<fdevY.at(nr).at(nz)<<"   "<<ftime.at(nr).at(nz)*1e9<<"   " 
	     <<fstepCount.at(nr).at(nz)<<"   "<<fpathLength.at(nr).at(nz)*100
	     <<std::endl;
    }
  }

  if(fsplit == frBins-1 || fsplit == -1) {
  outfile<<std::endl;
  outfile<<"\n\n#Format: First Line - Bins in r, Bins in z, minR, maxR, minZ, maxZ" 
	 <<"\n#Each Following Line: x-deviation [cm], y-deviation, total Runge-Kutta-Time [ns],"
         <<"\n#number of Runge-Kutta steps, real total RK-drift path [cm]" 
 	 <<"\n#This File was created by class PndTpcInhFieldDrifter"
	 <<std::endl;
  }

  outfile.close();

//   std::ofstream outfile2("DEVMAP_VELOCITY_FILE.dat", std::fstream::out);
//   outfile<<std::setprecision(8);

//   long int length = fvelocity_control->size();

//   for (int a=0; a<length; a++)
//   {
//     outfile2<<std::setprecision(8)<<std::setw(8);
//     outfile2<<"\n"<<fvelocity_control->at(a)->at(0)*1e-7<<"   "
//             <<fvelocity_control->at(a)->at(1)*1e-7<<"   "
//             <<fvelocity_control->at(a)->at(2)*1e-7<<"   "
//             <<fvelocity_control->at(a)->at(3)*1e9;
//   }
//   outfile2<<std::endl;
//   outfile2<<"#Format: v_x, v_y, v_z, corresponding Runge-Kutta-time"<<std::endl;
//   outfile2.close();
}
    


void
PndTpcInhFieldDrifter::initParams() {
  
  FairParAsciiFileIo* input = new FairParAsciiFileIo();
  
  //ugly test for valid param file
  std::ifstream paramf(fparamFile, std::fstream::in);
  if(!paramf.good()) {
    std::cout<<"PndTpcInhFieldDrifter::initParams() "
	     <<"- parameter file IO error - ABORTING"<<std::endl;
    throw 1;
  }
    
  input->open(fparamFile, "in");
  
  //create dummy run manager and load database
  FairRunAna* fRun = new FairRunAna();   
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();  
  rtdb->setFirstInput(input);
  fpar = (PndTpcDigiPar*) rtdb->getContainer("PndTpcDigiPar");
  
  fpar->FairParSet::init();
  
  //read in parameters
  ftpcMinR = fpar->getRMin();
  ftpcMaxR = fpar->getRMax();
  ftpcMinZ = fpar->getZGem();
  ftpcMaxZ = fpar->getZMax();

  std::cout<<"\n\nPndTpcInhFieldDrifter::initParams()\n"
	   <<"---------- Used Parameters ---------------------------"<<std::endl;
  std::cout<<"MinR: "<<ftpcMinR<<",   MaxR: "<<ftpcMaxR<<std::endl;
  std::cout<<"MinZ: "<<ftpcMinZ<<",   MaxZ: "<<ftpcMaxZ<<std::endl;std::cout<<std::endl;
  std::cout<<"Bins in R: "<<frBins<<",   Bins in Z: "<<fzBins<<std::endl;
  
  std::cout<<"------------------------------------------------------"<<std::endl;
  std::cout<<std::endl;  
  fsc = -1e-6*299792458*299792458/0.510998902;

  
}
