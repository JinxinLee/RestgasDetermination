#ifndef MAPPING_H
#define MAPPING_H

#include<string>
#include<sstream>
#include<iostream>

class Channel_t{
 public:
  double x,y;
  unsigned int conn,pin;
  std::string name;

  Channel_t(){
    name="dreggn";
  }
  Channel_t(std::string _name,double _x,double _y,unsigned int _conn,unsigned int _pin){
    name=_name;x=_x;y=_y;conn=_conn;pin=_pin;
  }
  std::string print(){
    std::ostringstream ostr;
    ostr << name << " " << x << " " << y << " " << conn << " " << pin;
    return ostr.str();
  }
};

class Mapping{
 private:
  Channel_t* c[6][256];
 public:
  void read(std::string);
  void write(std::string);
  Channel_t* getChannel(unsigned int iConn,unsigned int iPin){
    assert(iConn<6 && iPin<256);
    return c[iConn][iPin];
  }
};

#endif
