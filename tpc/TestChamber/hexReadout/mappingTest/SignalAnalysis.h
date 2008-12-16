#ifndef SIGNAL_H
#define SIGNAL_H
#include<vector>
#include"TROOT.h"
struct pulse_t{
  double time,amp;
  int startTime;
  int ch;
  std::vector<int> samples;
};

double pedestal(UInt_t*,int);
std::vector<pulse_t*> psa(UInt_t*,int);


#endif
