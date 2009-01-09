//root script
//reads a file with a charge distribution. Steered by a given _Rate this 
//charge is normalized to one drift step, then displaced and copied.
//For monitoring purposes the first 120 drift-steps are written to files
//in steps of 10.
//Author: Felix Boehmer


{
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <cstdio>

  //parameters 

  int _EventsToSim = 4e6;
  double _Rate = 2e-2;
  double _IonDriftVel = 1.766e-6;
  int _EventsSimInInput = 10000;

  int drift_control = 0;    //keep track of the number of drift-steps

  double minR;
  double segR;
  int nSegR;
  double minZ;
  double segZ;
  int nSegZ; 

  //---------------------------------------------------------------------------

  std::string _Input;

  std::cout<<"\n\nPlease enter the name of the output file of createSpaceCharge.C"
           <<std::endl;
  getline(cin, _Input);
  std::string _copy = _Input;
  
  std::string _Output = "Drifted_";
  _Output.append(_Input);
  std::string::size_type loc = _Input.find_last_of(".");
  
  std::string _SingleBinOut = "singleBin_";
  _SingleBinOut.append(_Input);
  
  std::string _MonitorOut = _copy.substr(0, loc);

  std::ofstream singleoutfile(_SingleBinOut.c_str(), std::fstream::out);

  std::ifstream infile(_Input.c_str(), std::fstream::in);
  if (!infile.good())
  {
  std::cerr<<"Input-file not existing!"<<std::endl;
  return(0);
  }

  //read geometry from infile
  infile>>minR>>segR>>nSegR>>minZ>>segZ>>nSegZ;

  //# of  events needed to drift through one segment
  double evtsSeg = segZ/_IonDriftVel*_Rate;

  //read charge density
  double chargeDensOld[nSegR][nSegZ];
  for (int nr=0; nr<nSegR; nr++)
    for (int nz=0; nz<nSegZ; nz++)
    {
      infile>>chargeDensOld[nr][nz];

  //normalize the charge to the amount expected for the number of 
  //events needed for one drift-step
      chargeDensOld[nr][nz] = chargeDensOld[nr][nz]/_EventsSimInInput*evtsSeg;
    }
  infile.close();
  
  double chargeDens[nSegR][nSegZ];
  for (int nr=0; nr<nSegR; nr++)		//Initialize to zero
    for (int nz=0; nz<nSegZ; nz++)
      chargeDens[nr][nz] =0;

 //the drift
 //all charges are displaced one segment in z direction

  for (int i=0; i < _EventsToSim/evtsSeg; i++)
  {
   
    for (int nr=0; nr<nSegR; nr++)
      for (int nz=nSegZ-1; nz>0; nz--)
        chargeDens[nr][nz] = chargeDens[nr][nz-1];
	
    //the segments in front of the gems are emptied
    for (int nr=0; nr<nSegR; nr++)
      chargeDens[nr][0] = 0;

    //create new ions
    for (int nz=0; nz<nSegZ; nz++)
      for (int nr=0; nr<nSegR; nr++)
      {
	chargeDens[nr][nz] += chargeDensOld[nr][nz];
      }
   
  //write temporary file to monitor SpaceCharge evolution
  
    if (i%10 == 0 && i <= 120)
    {   
      std::string buffer = _MonitorOut;
      buffer.append("_");
      char tmp;
      std::sprintf(&tmp, "%d", i);
      buffer.append(&tmp);
      buffer.append("steps.dat");   //this is a bit elbowy, but it works
      std::ofstream outfile(buffer.c_str(), std::fstream::out);

      outfile<<std::setprecision(6);     
      outfile<<minR<<" "<<segR<<" "<<nSegR<<" "
	     <<minZ<<" "<<segZ<<" "<<nSegZ;
      
      for (int nr=0; nr<nSegR; nr++)
	for (int nz=0; nz<nSegZ; nz++)
      outfile<<"\n"<<chargeDens[nr][nz];

      outfile<<std::endl;
    }
  
    //keep track of single bin contents for monitoring purposes  
    //set the bin here:
    int zBin = 40;
    int rBin = 5;
    
    singleoutfile<<std::setprecision(8);
    singleoutfile<<"\n"<<chargeDens[rBin][zBin];
    if (i==(_EventsToSim/evtsSeg)-1)   //not working yet
    {
      singleoutfile<<"\n\n#observed Bin: zBin = "<<zBin<<"; rBin = "<<rBin
	           <<std::endl;
      singleoutfile<<i;
    }
    
    drift_control++;
  }

  //write down final SpaceCharge
  
  std::ofstream outfile(_Output.c_str(), std::fstream::out);
  outfile<<std::setprecision(6);
  outfile<<minR<<" "<<segR<<" "<<nSegR<<" "
	 <<minZ<<" "<<segZ<<" "<<nSegZ;
  //z runs first
  for (int nr=0; nr<nSegR; nr++)
    for (int nz=0; nz<nSegZ; nz++)
      outfile<<"\n"<<chargeDens[nr][nz];
  
  outfile<<std::endl;
  outfile<<"this file was made with simple_Drift.C and the following settings"
         <<std::endl;
  outfile<<"Input "<<_Input
	 <<";\nEventsSimInInput "
	 <<_EventsSimInInput
	 <<";\nEventsToSim "<<_EventsToSim
	 <<";\nRate "<<_Rate
	 <<";\nIonDriftVel "<<_IonDriftVel << endl;
  outfile.close();
  singleoutfile.close();
    

  cout << "\n\n**********   All done.  **********"<<endl;
  cout << "\nYou can now plot the final output file " << _Output<<endl; 
  cout << "using plotChargeDens.C." << endl;
  cout << "Multiple output files in the format '"<<_MonitorOut.append("_XXsteps'")<<endl;
  cout << "have been created to visualize the drift"<<endl;
  cout << "Charge-Evolution for a single Bin has been written to"<<endl;
  cout << _SingleBinOut << endl;
  cout << "\n\n" << drift_control << " drift-steps have been processed." << endl;
 
}


// ### update on May 21st, 2007: 
// macro now handles strings internally, only input file name has to be supplied
