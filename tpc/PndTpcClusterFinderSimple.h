#ifndef TPCCLUSTERFINDERSIMPLE_HH
#define TPCCLUSTERFINDERSIMPLE_HH

#include <vector>
#include <set>
#include "PndTpcAbsClusterFinder.h"
#include "McIdCollection.h"
#include "TVector3.h"

class PndTpcPadPlane;
class PndTpcDigi;
class PndTpcCluster;


class PndTpcPrelimCluster{
 private:
  void cog();

  TVector3 fpos;
  TVector3 ferr;
  double famp;
  double fcogT;
  int fid;

  std::vector<PndTpcDigi*> fdigis;
  std::set<unsigned int> fpossiblePads;


  PndTpcPadPlane* fpadplane;
  double ftimeslice;
  McId fdominant_mcid;

  bool isInTimeWindow(const PndTpcDigi* const);

 public:
  PndTpcPrelimCluster(PndTpcPadPlane*, double, int);
  virtual ~PndTpcPrelimCluster();

  void addHit(PndTpcDigi*, bool noXclust=false);

  bool isInCluster(const PndTpcDigi* const);

  PndTpcCluster* convPndTpcCluster(bool saveRaw);

};

class PndTpcClusterFinderSimple : public PndTpcAbsClusterFinder{
public:

  // Constructors/Destructors ---------
  PndTpcClusterFinderSimple(PndTpcPadPlane*,
						 std::vector<PndTpcCluster*>*,
						 unsigned int);
  ~PndTpcClusterFinderSimple();

  virtual void process(std::vector<PndTpcDigi*>&);
  virtual void reset();
  virtual void checkConsistency();

  void setNoXclust(bool b=true){noXclust=b;}
  unsigned int NsplitDigis(){return splitDigis;}

private:
  PndTpcPadPlane* fpadplane;
  std::vector<PndTpcCluster*>* foutput_buffer;
  bool noXclust;
  unsigned int fdt; // time slice in units of sample time
  unsigned int splitDigis;


};


#endif


