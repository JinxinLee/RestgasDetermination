#ifndef LCARDMAPPING_H
#define LCARDMAPPING_H

#include<cassert>
#include<string>
#include<sstream>
#include<iostream>

class LcardChannel_t{
 public:
  unsigned int ch,chip,pin;
  LcardChannel_t(unsigned int _ch,unsigned int _chip,unsigned int _pin){
    chip=_chip;ch=_ch;pin=_pin;
  }
  std::string print(){
    std::ostringstream ostr;
    ostr << ch << " " << chip << " " << pin;
    return ostr.str();
  }
};

class LcardMapping{
 private:
  LcardChannel_t* c[256];

 public:
  void print(){
    for(int i=0;i<256;++i)std::cout << c[i]->print() << std::endl;
  }
  void read(std::string);
  bool map(unsigned int chip,unsigned int ch,unsigned int& pin){
    for(int i=0;i<256;++i){
      if(chip==c[i]->chip && ch==c[i]->ch) {
	pin=c[i]->pin;
	return true;
      }
    }

    pin=0xFFFFFFFF;
    return false;

  }
};

#endif
