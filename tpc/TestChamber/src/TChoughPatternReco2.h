#ifndef TCHOUGHPATTERNRECO_H
#define TCHOUGHPATTERNRECO_H

#include"TCabsPatternReco2.h"
#include"TCcluster.h"
#include<vector>
#include"TCevent.h"

class TChoughPatternReco2 : public TCabsPatternReco {

 public:
  void process(TCevent &event, std::vector<TCcluster>&);
 private:
  



};

#endif
