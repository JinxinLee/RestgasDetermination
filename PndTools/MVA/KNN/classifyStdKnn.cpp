/********************
 * Author: M. Babai *
 * M.Babai@rug.nl   *
 ********************
 */

/*
 * Example program. This code shows how to use the classify
 * procedure. This classifier is implemented based on the standard KNN
 * algorithm.
 */

#include <sstream>

#include "PndStdKnnClassify.h"

#include "TRandom3.h"
#include "TStopwatch.h"


void printResult(std::map<std::string,float>& res)
{
  std::map<std::string,float>::iterator ii;
  std::cout << "\n\t================================== \n";
  for( ii=res.begin(); ii != res.end(); ++ii)
  {
    std::cout <<"\t" << (*ii).first 
	      << "\t=> " << (*ii).second
	      <<'\n';
  }
  std::cout << "\n\t================================== \n";
}

/* *********************************************
 * Testing routine, can be deleted afterwards. *
 * *********************************************
 */

int main(int argc, char** argv)
{
  if(argc < 3)
  {
    std::cerr <<"\t<ERROR>" 
	      << argv[0] <<" <inputFile> <numOfneigh>"
	      << std::endl;
    return 1;
  }
  
  std::string InPutFileName = argv[1];
  std::string NumNeistr = argv[2];
  std::istringstream buff(NumNeistr);

  int NumNei = 0;
  buff >> NumNei;

  TRandom3 myran(4125373);
  std::vector<std::string> labels;
  std::vector<std::string> vars;
  
  // Classes (container to hold the class names)
  labels.push_back("electron");
  labels.push_back("pion");
  
  //labels.push_back("kaon");
  //labels.push_back("muon");
  //labels.push_back("proton");

  // Variables (names)
  vars.push_back("emc");
  vars.push_back("lat");
  vars.push_back("z20");
  vars.push_back("z53");

  //vars.push_back("mvd");  vars.push_back("stt");
  //vars.push_back("tof"); vars.push_back("tpc");
  
  //Create the classifier object and specify the weight file
  PndStdKnnClassify cls (InPutFileName, labels, vars);
  
  cls.Initialize();

  cls.SetKNN(NumNei);
    
  std::vector<float> evt, evt1, evt2;
  
  evt.clear();
  for(size_t j = 0; j < vars.size(); ++j)
  {
    evt.push_back(myran.Gaus(1,10));
    evt1.push_back(myran.Uniform(-1,1));
    evt2.push_back(myran.Uniform(30,50));
  }
  
  // Map to store the results
  std::map<std::string,float> res;
  
  TStopwatch ti;
  ti.Start();
  
  cls.GetMvaValues(evt, res);
  printResult(res);
  
  // cls.Classify(evt1, res);
  //printResult(res);
  
  //cls.Classify(evt2, res);
  //printResult(res);
  
  ti.Stop();
  double rtime = ti.RealTime();
  double ctime = ti.CpuTime();
  
  std::cout << "timer 1: Classifier timing results:\n"
	    << "RealTime = " << rtime << " seconds, CpuTime = "
	    << ctime
	    <<" Seconds\n\n";
  
  return 0;
}
