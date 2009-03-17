#ifndef TCHOUGHPATTERNRECO_H
#define TCHOUGHPATTERNRECO_H

#include"TCabsPatternReco.h"
#include"TCcluster.h"
#include<vector>
#include"TCevent.h"

class TChoughPatternReco : public TCabsPatternReco {

 public:
  void process(TCevent &event, std::vector<TCcluster>&);
 private:
  



};

#endif
