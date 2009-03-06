/* ***************************************
 * Author: M.Babai@rug.nl                *
 * ***************************************
 */
/*
 * Example program. This code shows how to use the train procedure, in
 * order to generate weight file to be used by the KNN based
 * classifier.
 */
#include "PndLVQTrain.h"
#include "TRandom3.h"

int main(int argc, char** argv)
{
  std::vector<std::string> clas;
  std::vector<std::string> nam;
  //Class names
  clas.push_back("El"); clas.push_back("Pi"); clas.push_back("Ka");
  clas.push_back("gam"); clas.push_back("mu");

  //Variable names 
  nam.push_back("ep"); nam.push_back("tof"); nam.push_back("mvd");
  nam.push_back("p");  nam.push_back("f");   nam.push_back("d");
  nam.push_back("a");  nam.push_back("b");   nam.push_back("c");
  nam.push_back("z");  nam.push_back("zz");  nam.push_back("zzz");

  PndLVQTrain tr ("TestInputRand.root",clas,nam);
  //tr.Train(30,"TrainProto.root");
  tr.Train21(30,"TrainProto.root");
  return 0;
}
