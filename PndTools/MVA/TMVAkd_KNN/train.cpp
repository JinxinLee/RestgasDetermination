/*
 * Example program. This code shows how to use the train procedure, in
 * order to generate weight file to be used by the KNN based
 * classifier.
 */
#include "PndKnnTrain.h"
#include "TRandom3.h"

//int main(int argc, char** argv){
int main(){
  int totEvtNum = 100000;
  
  std::vector<std::string> clas;
  std::vector<std::string> nam;
  clas.push_back("Elec"); clas.push_back("Pion"); 
  clas.push_back("Kaon"); clas.push_back("Gamma"); 
  clas.push_back("Muon"); clas.push_back("Prot");

  nam.push_back("emc"); nam.push_back("tof"); nam.push_back("mvd");
  nam.push_back("p");   nam.push_back("stt"); nam.push_back("tpc");


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
    bla.Train(evt,"Elec");
  }
  //Pi events
  for(int i = 0; i < totEvtNum; i++){
    evt.clear();
    for(unsigned int j = 0; j < nam.size(); j++){
      evt.push_back(myran.Uniform(0,1));
    }
    bla.Train(evt,"Pion");
  }
  //Ka events
  for(int i = 0; i < totEvtNum; i++){
    evt.clear();
    for(unsigned int j = 0; j < nam.size(); j++){
      evt.push_back(myran.Uniform(1,2));
    }
    bla.Train(evt,"Kaon");
  }
  //Gam events
  for(int i = 0; i < totEvtNum; i++){ 
    evt.clear();
    for(unsigned int j = 0; j < nam.size(); j++){
      evt.push_back(myran.Uniform(2,3));
    }
    bla.Train(evt,"Gamma");
  }
  //mu events
  for(int i = 0; i < totEvtNum; i++){
    evt.clear();
    for(unsigned int j = 0; j < nam.size(); j++){
      evt.push_back(myran.Uniform(3,4));
    }
    bla.Train(evt,"Muon");
  }

  // proton events
  for(int i = 0; i < totEvtNum; i++){
    evt.clear();
    for(unsigned int j = 0; j < nam.size(); j++){
      evt.push_back(myran.Gaus(30,40));
    }
    bla.Train(evt,"Prot");
  }
  std::cout << "Done generating events.\n"
	    <<"Output is saved in " << ot
	    << std::endl;
  return 0;
}
