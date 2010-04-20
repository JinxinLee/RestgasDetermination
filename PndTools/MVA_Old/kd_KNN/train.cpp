/*
 * Example program. This code shows how to use the train procedure, in
 * order to generate weight file to be used by the KNN based
 * classifier.
 */
#include "PndKnnTrain.h"
#include "TRandom3.h"

int main(int argc, char** argv){
  int totEvtNum = 10000;
  
  std::vector<std::string> clas;
  std::vector<std::string> nam;
  clas.push_back("El"); clas.push_back("Pi"); clas.push_back("Ka");
  clas.push_back("gam"); clas.push_back("mu");

  nam.push_back("ep"); nam.push_back("tof"); nam.push_back("mvd");
  nam.push_back("p");  nam.push_back("f");   nam.push_back("d");
  nam.push_back("a");  nam.push_back("b");   nam.push_back("c");
  nam.push_back("z");  nam.push_back("zz");  nam.push_back("zzz");

  std::string ot   = "Test.root";
  const char* file = ot.c_str();
  
  PndKnnTrain bla (file,clas,nam);

  std::vector<float> evt;
  
  TRandom3 myran(987629);
  //El events
  for(int i = 0; i < totEvtNum; i++){
    evt.clear();
    for(unsigned int j = 0; j < nam.size(); j++){
      evt.push_back(myran.Uniform(-1,1));
    }
    bla.Train(evt,"El");
  }
  //Pi events
  for(int i = 0; i < totEvtNum; i++){
    evt.clear();
    for(unsigned int j = 0; j < nam.size(); j++){
      evt.push_back(myran.Uniform(0,1));
    }
    bla.Train(evt,"Pi");
  }
  //Ka events
  for(int i = 0; i < totEvtNum; i++){
    evt.clear();
    for(unsigned int j = 0; j < nam.size(); j++){
      evt.push_back(myran.Uniform(1,2));
    }
    bla.Train(evt,"Ka");
  }
  //Gam events
  for(int i = 0; i < totEvtNum; i++){ 
    evt.clear();
    for(unsigned int j = 0; j < nam.size(); j++){
      evt.push_back(myran.Uniform(2,3));
    }
    bla.Train(evt,"gam");
  }
  //mu events
  for(int i = 0; i < totEvtNum; i++){
    evt.clear();
    for(unsigned int j = 0; j < nam.size(); j++){
      evt.push_back(myran.Uniform(3,4));
    }
    bla.Train(evt,"mu");
  }
  return 0;
}
