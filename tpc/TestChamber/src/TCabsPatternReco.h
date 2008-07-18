#ifndef TCABSPATTERNRECO_H
#define TCABSPATTERNRECO_H

#include<vector>
class TCcluster;

class TCabsPatternReco{


 public:
  virtual ~TCabsPatternReco(){}
  virtual void process(std::vector<TCcluster>&)=0;

};


#endif
