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
  if(argc < 2){
    std::cerr << "\t<ERROR> Usage\n"
	      <<"\t./train <InFile>"
	      << std::endl;
    return 1;
  }
  /*
    int totEvtNum = 0;
    std::string numstr = argv[1];
    std::istringstream buff(numstr);
    buff >> totEvtNum;
  */
  std::string ot   = argv[1];
  
  std::vector<std::string> clas;
  std::vector<std::string> nam;
  
  // Class names
  /*
  clas.push_back("Elect"); clas.push_back("Pion"); 
  clas.push_back("Kaon"); clas.push_back("Gamma"); 
  clas.push_back("Muon"); //clas.push_back("Prot");
  */
  
  clas.push_back("electron"); clas.push_back("pion"); 
  clas.push_back("kaon"); clas.push_back("muon");
  clas.push_back("proton");
  
  // Parameter names
  nam.push_back("emc"); nam.push_back("tof"); nam.push_back("mvd");
  nam.push_back("p");   nam.push_back("stt"); nam.push_back("thetaC");

  const char* file = ot.c_str();
  
  PndKnnTrain kNNtr (file, clas, nam, true);
  NormType tt = MINMAX;
  kNNtr.NormalizeData(tt);
  kNNtr.SetOutPutFile("NormalizedDataOut.root");
  kNNtr.Train();

  return 0;
}
