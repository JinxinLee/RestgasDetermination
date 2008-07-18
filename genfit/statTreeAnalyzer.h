#ifndef FITTER_STATTREEANALYZER_H
#define FITTER_STATTREEANALYZER_H

#include "statTreeAnalyzer_auto.h"

#include<string>


class statTreeAnalyzer : public statTreeAnalyzer_auto {
 private:
  std::string outfileName;
  void rangeSetHelper(double, double&, double&);

 public:
  statTreeAnalyzer(std::string treeName_source, std::string fileName_source, std::string fileName_target);
  virtual ~statTreeAnalyzer();
  virtual void Loop();
};



#endif // FITTER_STATTREEANALYZER_H
