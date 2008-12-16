#include<cstdio>
#include<ctime>
#include<vector>
#include<fstream>
#include<sstream>

#include"LcardMapping.h"


void LcardMapping::read(std::string filename){
  std::cout << "reading file " << filename << std::endl;
  std::ifstream input(filename.c_str());
  char line[200];
  std::vector<LcardChannel_t> channels;
  int lines=0;
  while(input.getline(line,199)) {
    std::istringstream istr(line);
    std::string s;
    istr >> s;
    if(s.substr(0,1)=="#" || s.substr(0,1)=="") continue;
    unsigned int i1,i2,i3,i4,i5,i6,i7;
    std::istringstream istr2(s.c_str());
    istr2 >> i1;
    istr >> i2;
    istr >> i3;
    istr >> i4;
    istr >> i5;
    istr >> i6;
    istr >> i7;


    //channel,chip,pin
    LcardChannel_t chan(i3,i2,i6);
    channels.push_back(chan);
  }

  int counter=0;
  for(int i=0;i<channels.size();++i){
    if(channels.at(i).pin!=0){
      c[counter++] = new LcardChannel_t(channels.at(i));
    }
  }
  assert(counter==256);

  input.close();
}

