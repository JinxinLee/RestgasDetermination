/* ***************************************
 * Author: M.Babai@rug.nl                *
 * ***************************************
 */
/*
 * Example program. This code shows how to use the train procedure, in
 * order to generate weight file to be used by the KNN based
 * classifier.
 */
#include <sstream>

#include "PndKnnTrain.h"
#include "TRandom3.h"

int str2int (const std::string &str)
{
  std::stringstream ss(str);
  int n;
  ss >> n;
  return n;
}

std::string int2str (int n)
{
  std::stringstream ss;
  ss << n;
  return ss.str();
}


int main(int argc, char** argv)
{
  if(argc < 3){
    std::cerr << "\t<ERROR> Usage\n"
	      <<"\t./train <InFile> <OutFile>"
	      << std::endl;
    return 1;
  }

  std::string inpt   = argv[1];
  std::string oupt   = argv[2];
  
  std::vector<std::string> clas;
  std::vector<std::string> nam;
  // Class Names  
  clas.push_back("electron"); clas.push_back("pion"); 
  //clas.push_back("kaon");
  //clas.push_back("muon");
  //clas.push_back("proton");
  
  // Parameter Names
  //nam.push_back("p");
  nam.push_back("emc");
  nam.push_back("lat");
  nam.push_back("z20");
  nam.push_back("z53");
  
  //nam.push_back("tof"); nam.push_back("mvd");
  //nam.push_back("stt"); nam.push_back("thetaC");
  
  const char* file = inpt.c_str();
  
  PndKnnTrain kNNtr (file, clas, nam, true);
  NormType tt = VARX;
  kNNtr.NormalizeData(tt);
  kNNtr.SetOutPutFile(oupt);
  kNNtr.Train();

  return 0;
}
