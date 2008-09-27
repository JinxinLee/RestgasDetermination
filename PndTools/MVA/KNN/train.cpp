#include "PndKnnTrain.h"


int main(int argc, char** argv)
{
  int totEvtNum = 100000;
  
  std::vector<std::string> clas;
  std::vector<std::string> nam;
  clas.push_back("El"); clas.push_back("Pi"); clas.push_back("Ka");
  clas.push_back("gam"); clas.push_back("mu");
  
  nam.push_back("ep");  nam.push_back("tof"); nam.push_back("mvd");
  nam.push_back("p");  nam.push_back("f"); nam.push_back("d");
  nam.push_back("a");  nam.push_back("b"); nam.push_back("c");
  
  std::string ot   = "Test.root";
  const char* file = ot.c_str();
  
  PndKnnTrain bla (file,clas,nam);
  
  std::vector<float> evt;
  
  for(int i=0; i < totEvtNum; i++){
  
  evt.clear();
  evt.push_back(1.0);
  evt.push_back(2.0);
  evt.push_back(3.0);
  evt.push_back(4.0);
  evt.push_back(5.0);
  evt.push_back(6.0);
  evt.push_back(7.0);
  evt.push_back(8.0);
  evt.push_back(9.0);
  bla.Train(evt,"El");
  
  evt.clear();
  evt.push_back(11.0);
  evt.push_back(12.0);
  evt.push_back(13.0);
  evt.push_back(14.0);
  evt.push_back(15.0);
  evt.push_back(16.0);
  evt.push_back(17.0);
  evt.push_back(18.0);
  evt.push_back(19.0);
  bla.Train(evt,"Pi");
  
  evt.clear();
  evt.push_back(21.0);
  evt.push_back(22.0);
  evt.push_back(23.0);
  evt.push_back(24.0);
  evt.push_back(25.0);
  evt.push_back(26.0);
  evt.push_back(27.0);
  evt.push_back(28.0);
  evt.push_back(29.0);
  bla.Train(evt,"Ka");
  
  evt.clear();
  evt.push_back(31.0);
  evt.push_back(32.0);
  evt.push_back(33.0);
  evt.push_back(34.0);
  evt.push_back(35.0);
  evt.push_back(36.0);
  evt.push_back(37.0);
  evt.push_back(38.0);
  evt.push_back(39.0);
  bla.Train(evt,"gam");
  
  evt.clear();
  evt.push_back(41.0);
  evt.push_back(42.0);
  evt.push_back(43.0);
  evt.push_back(44.0);
  evt.push_back(45.0);
  evt.push_back(46.0);
  evt.push_back(47.0);
  evt.push_back(48.0);
  evt.push_back(49.0);
  bla.Train(evt,"mu");
  }
  return 0;
}

