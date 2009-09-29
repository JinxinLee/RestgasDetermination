#ifndef TPCABSCLUSTERFINDER_H
#define TPCABSCLUSTERFINDER_H

#include<vector>

class PndTpcDigi;

class PndTpcAbsClusterFinder{
 public:
  PndTpcAbsClusterFinder(){ftrcl=false;fsaveRaw=false;}
  virtual ~PndTpcAbsClusterFinder(){}
  virtual void process(std::vector<PndTpcDigi*>&)=0;
  virtual void reset()=0;
  void setTrivialClustering(bool flag=true){ftrcl=flag;}
  virtual void checkConsistency()=0;
  
  void saveRaw(bool b=true){fsaveRaw=b;}
 protected:
  bool fsaveRaw;
  bool ftrcl;
};

#endif
