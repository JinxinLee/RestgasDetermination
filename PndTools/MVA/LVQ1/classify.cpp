/* ***************************************
 * Author: M.Babai@rug.nl                *
 * ***************************************
 */
/*
 * Example program. This code shows how to use the classify
 * procedure. This classifier is implemented based on the KNN
 * algorithm. An implementation of kd-tree is used to improve the
 * recognition performance.
 */
#include "PndLVQClassify.h"

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

//int main(int argc, char** argv)
int main()
{
  TRandom3 myran(129);
  std::vector<std::string> clas;
  std::vector<std::string> nam;
  
  // Classes
  clas.push_back("El"); clas.push_back("Pi"); clas.push_back("Ka");
  clas.push_back("gam"); clas.push_back("mu");
  
  // Variables
  nam.push_back("ep"); nam.push_back("tof"); nam.push_back("mvd");
  nam.push_back("p");  nam.push_back("f");   nam.push_back("d");
  nam.push_back("a");  nam.push_back("b");   nam.push_back("c");
  nam.push_back("z");  nam.push_back("zz");  nam.push_back("zzz");

  TStopwatch timer;
  timer.Start();

  PndLVQClassify cls ("Train10Proto.root",clas,nam);
  std::cout << "Done, the classifier" << std::endl;

  timer.Stop();
  double rtime = timer.RealTime();
  double ctime = timer.CpuTime();
  std::cout<< "RealTime = " << rtime << " seconds, CpuTime = " 
           << ctime <<" Seconds" << std::endl;

  //Create example vectors to be classified.
  std::vector<float> evt,evt1,evt2;

  for(unsigned int j = 0; j < nam.size(); j++){
    evt.push_back(myran.Uniform(-1,1));
    evt2.push_back(myran.Uniform(3,5));
    evt1.push_back(myran.Uniform(-10,10));
  }
  
  std::map<std::string,float> res;
  
  TStopwatch timer1;
  timer1.Start();
  for(int i=0; i< 10000; i++){
    cls.Classify(evt,res);
    //printResult(res);
    
    cls.Classify(evt1,res);
    //printResult(res);
    
    cls.Classify(evt2,res);
    //printResult(res);
  }
  timer1.Stop();
  rtime = timer1.RealTime();
  ctime = timer1.CpuTime();
  std::cout << "Classifier timing results:"<<std::endl;
  std::cout<< "RealTime = " << rtime << " seconds, CpuTime = " 
           << ctime <<" Seconds" << std::endl;
  return 0;
}
