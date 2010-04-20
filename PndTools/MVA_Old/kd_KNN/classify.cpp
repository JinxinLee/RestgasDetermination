/*
 * Example program. This code shows how to use the classify
 * procedure. This classifier is implemented based on the KNN
 * algorithm. An implementation of kd-tree is used to improve the
 * recognition performance.
 */
#include "PndKnnClassify.h"

#include "TRandom3.h"
#include "TStopwatch.h"


void printResult(std::map<std::string,float>& res){
  std::cout << "\n================================== \n";
  for( std::map<std::string,float>::iterator ii=res.begin(); 
       ii != res.end(); ++ii){
    std::cout << (*ii).first << " => " << (*ii).second << std::endl;
  }
  std::cout << "======================================= \n";
}

/* *********************************************
 * Testing routine, can be deleted afterwards. *
 * *********************************************
 */

int main(int argc, char** argv)
{
  TRandom3 myran(129);
  std::vector<std::string> clas;
  std::vector<std::string> nam;
  
  // Classes (container to hold the class names)
  clas.push_back("El"); clas.push_back("Pi"); clas.push_back("Ka");
  clas.push_back("gam"); clas.push_back("mu");
  
  // Variables (names)
  nam.push_back("ep"); nam.push_back("tof"); nam.push_back("mvd");
  nam.push_back("p");  nam.push_back("f");   nam.push_back("d");
  //nam.push_back("a");  nam.push_back("b");   nam.push_back("c");
  //nam.push_back("z");  nam.push_back("zz");  nam.push_back("zzz");

  TStopwatch timer;
  timer.Start();

  //Create the classifier object and specify the weight file
  PndKnnClassify cls ("Test.root",clas,nam);
  std::cout << "Done, creating the trees" << std::endl;

  timer.Stop();
  double rtime = timer.RealTime();
  double ctime = timer.CpuTime();
  std::cout<< "RealTime = " << rtime << " seconds, CpuTime = " 
           << ctime <<" Seconds" << std::endl;

  std::vector<float> evt,evt1,evt2;
  
  evt.clear();
  for(unsigned int j = 0; j < nam.size(); j++){
    evt.push_back(myran.Uniform(-1,1));
    evt1.push_back(myran.Uniform(-10,10));
    evt2.push_back(myran.Uniform(30,50));
  }
  
  std::map<std::string,float> res;
  
  TStopwatch timer1;
  timer1.Start();
  /*
  cls.Classify(evt,1500,res);
  printResult(res);
  */
  /*
    for(int a = 0; a < 1000; a++){
    cls.Classify(evt,500,res);
    printResult(res);
    }
  */
  cls.Classify(evt1,500,res);
  printResult(res);
  cls.Classify(evt2,500,res);
  printResult(res);
  
  
  timer1.Stop();
  rtime = timer1.RealTime();
  ctime = timer1.CpuTime();
  std::cout << "Classifier timing results:"<<std::endl;
  std::cout<< "RealTime = " << rtime << " seconds, CpuTime = " 
           << ctime <<" Seconds" << std::endl;
  return 0;
}
