/*
 * Example program. This code shows how to use the classify
 * procedure. This classifier is implemented based on the KNN
 * algorithm. An implementation of kd-tree is used to improve the
 * recognition performance.
 */
// C++
#include <sstream>

// PANDA ROOT
#include "PndProjectedKNN.h"

// ROOT
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
	      <<"./classify <inputFileWeight> <numOfneigh>"
	      <<std::endl;
      return 1;
  }
  
  std::string InPutFileName = argv[1];
  std::string NumNeistr = argv[2];
  std::istringstream buff(NumNeistr);
  int NumNei = 0;
  buff >> NumNei;

  TRandom3 myran(712535);
  std::vector<std::string> clas;
  std::vector<std::string> stru;
  std::vector<std::vector<std::string> > names;
  std::vector<std::string> nam1;// = new std::vector<std::string>();
  std::vector<std::string> nam2;// = new std::vector<std::string>();
  std::vector<std::string> nam3;// = new std::vector<std::string>();
 
  // Classes (container to hold the class names)
  clas.push_back("Elect"); clas.push_back("Pion"); 
  clas.push_back("Kaon"); //clas.push_back("Gamma"); 
  clas.push_back("Muon"); clas.push_back("Proton");
  
  // Event structure
  stru.push_back("p"); stru.push_back("stt");   
  stru.push_back("emc"); stru.push_back("tof");
  stru.push_back("mvd");

  // Variables (combinations)  
  nam1.push_back("p");   nam1.push_back("stt");
  nam2.push_back("emc"); nam2.push_back("tof"); 
  nam2.push_back("mvd");
  nam3.push_back("tof"); nam3.push_back("mvd");

  names.push_back(nam1); 
  names.push_back(nam2);
  names.push_back(nam3);
  //Create the classifier object and specify the weight file

  //PndKnnClassify cls (InPutFileName.c_str(), clas, nam1);
  PndProjectedKNN cls (InPutFileName, clas, stru, names);
  cls.SetKnn(NumNei);
  cls.SetEvtParam(0.8,1.0);
  cls.InitKNN();

  std::cout << ".......... Init is done." << std::endl;

  std::vector<float> evt, evt1, evt2;
  
  for(unsigned int j = 0; j < nam1.size(); j++){
    evt.push_back(myran.Gaus(1,2));
    evt1.push_back(myran.Uniform(-1,1));
    evt2.push_back(myran.Uniform(3,5));
  }
  
  // Map to store the results
  std::map<std::string,float> res;
  
  TStopwatch ti;
  ti.Start();
 
  cls.GetMvaValues(evt, res);
  printResult(res);

  cls.GetMvaValues(evt1, res);
  printResult(res);
  
  cls.GetMvaValues(evt2, res);
  printResult(res);
  
  ti.Stop();
  double rtime = ti.RealTime();
  double ctime = ti.CpuTime();
  std::cout << "timer 1: Classifier timing results:"<< std::endl;
  std::cout<< "RealTime = " << rtime << " seconds, CpuTime = " 
           << ctime <<" Seconds\n" << std::endl;
  return 0;
}
