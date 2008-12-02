#include<cstdio>
#include<ctime>
#include<vector>
#include<fstream>
#include<sstream>

#include"Mapping.h"


void Mapping::read(std::string filename){
  std::ifstream input(filename.c_str());
  char line[200];
  std::vector<Channel_t> channels;
  int lines=0;
  while(input.getline(line,199)) {
    std::istringstream istr(line);
    std::string s;
    istr >> s;
    if(s.substr(0,1)=="#" || s.substr(0,1)=="") continue;
    double x,y;
    unsigned int conn,pin;
    std::string name;
    name = s;
    istr >> x;
    istr >> y;
    istr >> conn;
    istr >> pin;
    Channel_t chan(name,x,y,conn,pin);
    //chan.print();
    channels.push_back(chan);
  }

  for(int i=0;i<6;++i){
    for(int j=0;j<256;++j){
      c[i][j]=new Channel_t("dreggn",-60.,-60.,i,j+PINOFFSET);
    }
  }

  for(unsigned int i=0;i<channels.size();++i){
    int iConn,iPin;
    iConn = channels.at(i).conn;
    iPin = channels.at(i).pin-PINOFFSET;
    //channels.at(i).print();
    //std::cout << c[iConn][iPin] << std::endl;
    c[iConn][iPin] = new Channel_t(channels.at(i));
    //c[iConn][iPin]->print();
  }
  input.close();
}

void Mapping::write(std::string filename){
  std::ofstream output(filename.c_str());
  std::cout << "write" << std::endl;
  for(int i=0;i<6;++i){
    for(int j=0;j<256;++j){
      //std::cout << i << " " << j << std::endl;
      //std::cout << c[i][j] << std::endl;
      if(c[i][j]!=NULL){
	if(c[i][j]->name!="dreggn"){
	  output << c[i][j]->print() << std::endl;;
	}
	//c[i][j]->print();
      }
    }
  }
  output.close();
}
