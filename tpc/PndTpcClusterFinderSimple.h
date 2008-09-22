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

  TVector3 _pos;
  TVector3 _err;
  double _amp;
  double _cogT;
  int _id;

  std::vector<PndTpcDigi> _digis;
  std::set<unsigned int> _possiblePads;


  PndTpcPadPlane* _padplane;
  double _timeslice;
  McId _dominant_mcid;

  bool isInTimeWindow(const PndTpcDigi* const);

 public:
  PndTpcPrelimCluster(PndTpcPadPlane*, double, int);
  virtual ~PndTpcPrelimCluster();

  void addHit(const PndTpcDigi&,bool noXclust=false);

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

private:
  PndTpcPadPlane* _padplane;
  std::vector<PndTpcCluster*>* _output_buffer;
  bool noXclust;
  unsigned int _dt; // time slice in units of sample time



};


#endif


