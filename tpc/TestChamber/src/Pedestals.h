#ifndef PEDESTALS_H
#define PEDESTALS_H
#include <string>
#include <map>

class Pedestals{
 public:
  Pedestals(std::string pedFileName);
  static void getPedestals(std::map<int,double>& m,std::map<int,double>& s);
  static void getPedestal(int,double&,double&);
 private:
  static Pedestals *theInstance;
  static void readPedestals(std::string pedFileName);
  static std::map<int,double> _mean;
  static std::map<int,double> _sigma;
};
#endif
 
 
