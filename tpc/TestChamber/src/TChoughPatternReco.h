#ifndef TCHOUGHPATTERNRECO_H
#define TCHOUGHPATTERNRECO_H

#include"TCabsPatternReco.h"
#include"TCcluster.h"
#include<vector>

class TChoughPatternReco : public TCabsPatternReco {

 public:
  void process(std::vector<TCcluster>&);
 private:




};

#endif
