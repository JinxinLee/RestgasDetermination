#ifndef TPCABSCLUSTERFINDER_H
#define TPCABSCLUSTERFINDER_H

#include<vector>

class PndTpcDigi;

class PndTpcAbsClusterFinder{
 public:
  PndTpcAbsClusterFinder(){_trcl=false;_saveRaw=false;}
  virtual ~PndTpcAbsClusterFinder(){}
  virtual void process(std::vector<PndTpcDigi*>&)=0;
  virtual void reset()=0;
  void setTrivialClustering(bool flag=true){_trcl=flag;}
  virtual void checkConsistency()=0;
  
  void saveRaw(bool b=true){_saveRaw=b;}
 protected:
  bool _saveRaw;
  bool _trcl;
};

#endif
