/*
 * Example program. This code shows how to use the classify
 * procedure. This classifier is implemented based on the KNN
 * algorithm. An implementation of kd-tree is used to improve the
 * recognition performance.
 */
#include <sstream>

#include "PndKnnClassify.h"
#include "TRandom3.h"
#include "TStopwatch.h"


void printResult(std::map<std::string,float>& res){
  std::cout << "\n\t================================== \n";
  for( std::map<std::string,float>::iterator ii=res.begin(); 
       ii != res.end(); ++ii){
    std::cout <<"\t" << (*ii).first 
	      << "\t=> " << (*ii).second << std::endl;
  }
  std::cout << "\n\t================================== \n";
}

/* *********************************************
 * Testing routine, can be deleted afterwards. *
 * *********************************************
 */

int main(int argc, char** argv)
{
  if(argc < 3){
    std::cerr <<"\t<ERROR>" 
	      <<"./classify <inputFile> <numOfneigh>"
	      <<std::endl;
      return 1;
  }
  
  std::string InPutFileName = argv[1];
  std::string NumNeistr = argv[2];
  std::istringstream buff(NumNeistr);
  int NumNei = 0;
  buff >> NumNei;

  TRandom3 myran(4125373);
  std::vector<std::string> clas;
  std::vector<std::string> nam;
  
  // Classes (container to hold the class names)
  clas.push_back("Elec"); clas.push_back("Pion"); 
  //clas.push_back("Kaon"); clas.push_back("Gamma"); 
  //clas.push_back("Muon"); clas.push_back("Prot");
  
  // Variables (names)
  nam.push_back("emc"); nam.push_back("tof"); nam.push_back("mvd");
  nam.push_back("p");   nam.push_back("stt"); nam.push_back("tpc");


  // TStopwatch timer;
  //timer.Start();

  //Create the classifier object and specify the weight file
  PndKnnClassify cls (InPutFileName.c_str(), clas, nam);
  cls.SetEvtParam(0.8,1.0);
  cls.Init();
  std::cout << ".......... Init is done." << std::endl;

  //timer.Stop();
  double rtime = 0.0;// = timer.RealTime();
  double ctime = 0.0;// = timer.CpuTime();
  std::cout<< "RealTime = " << rtime << " seconds, CpuTime = " 
           << ctime <<" Seconds" << std::endl;

  std::vector<float> evt,evt1,evt2;
  
  evt.clear();
  for(unsigned int j = 0; j < nam.size(); j++){
    evt.push_back(myran.Gaus(1,20));
    evt1.push_back(myran.Uniform(-10,10));
    evt2.push_back(myran.Uniform(30,50));
  }
  
  // Map to store the results
  std::map<std::string,float> res;
  
  TStopwatch ti;
  ti.Start();
  
  cls.Classify(evt, NumNei, res);
  printResult(res);
  
  ti.Stop();
  rtime = ti.RealTime();
  ctime = ti.CpuTime();
  std::cout << "timer 1: Classifier timing results:"<< std::endl;
  std::cout<< "RealTime = " << rtime << " seconds, CpuTime = " 
           << ctime <<" Seconds\n" << std::endl;
  return 0;
}
