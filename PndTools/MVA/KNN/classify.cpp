#include "PndKnnClassify.h"

int main(int argc, char** argv)
{
  std::vector<std::string> clas;
  std::vector<std::string> nam;
  // Classes
  clas.push_back("El"); clas.push_back("Pi"); clas.push_back("Ka");
  clas.push_back("gam"); clas.push_back("mu");
  
  // Variables
  nam.push_back("ep");  nam.push_back("tof"); nam.push_back("mvd");
  nam.push_back("p");  nam.push_back("f"); nam.push_back("d");
  nam.push_back("a");  nam.push_back("b"); nam.push_back("c");
  
  PndKnnClassify cls ("Test.root",clas,nam);
  
  //std::vector<float> evt(9,5.0);
  std::vector<float> evt;
  evt.push_back(5.0);
  evt.push_back(5.0);
  evt.push_back(5.0);
  evt.push_back(15.0);
  evt.push_back(15.0);
  evt.push_back(15.0);
  evt.push_back(15.0);
  evt.push_back(15.0);
  evt.push_back(5.0);
  
  std::map<std::string, float> res;
  
  cls.Classify(evt,150000,res);
  return 0;
}

