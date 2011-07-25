#ifndef TPCCLUSTERFINDERSIMPLE_HH
#define TPCCLUSTERFINDERSIMPLE_HH

#include <vector>
#include <map>
#include <set>
#include "PndTpcAbsClusterFinder.h"
#include "PndTpcDigiAge.h"
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
  double fG;
  double fC;

  std::vector<PndTpcDigi*> fdigis;
  std::map<const PndTpcDigi*, double> fdigiShares;
  std::set<unsigned int> fpossiblePads;

  PndTpcPadPlane* fpadplane;
  double ftimeslice;
  McIdCollection fmcidCol;

  bool isInTimeWindow(const PndTpcDigi* const);

 public:
  PndTpcPrelimCluster(PndTpcPadPlane*, double, int, double G, double C);
  virtual ~PndTpcPrelimCluster();

  void addHit(PndTpcDigi*, bool noXclust=false, double share=1.);
  double getCogT(){return fcogT;}
  PndTpcDigi* getDigi(unsigned int id){return fdigis[id];}

  bool isInCluster(const PndTpcDigi* const);

  PndTpcCluster* convPndTpcCluster(bool saveRaw);

};

class PndTpcClusterFinderSimple : public PndTpcAbsClusterFinder{
public:

  // Constructors/Destructors ---------
  PndTpcClusterFinderSimple(PndTpcPadPlane*,
						 std::vector<PndTpcCluster*>*,
						 unsigned int, double G=1, double C=1);
  ~PndTpcClusterFinderSimple();
  
  virtual void process(std::vector<PndTpcDigi*>&);
  virtual void reset();
  virtual void checkConsistency();

  void setMaxClusterSlice(unsigned int i){maxClusterSlice = i;}

  void setSectorize(bool opt=true){sectorize=opt;}
  void setNoXclust(bool b=true){noXclust=b;}
  unsigned int NsplitDigis(){return splitDigis;}
  
private:
  virtual void processSector(std::vector<PndTpcDigi*>&);
  std::map<unsigned int, std::vector<PndTpcDigi*>* > fsectormap;
  PndTpcPadPlane* fpadplane;
  std::vector<PndTpcCluster*>* foutput_buffer;

  bool sectorize;
  bool noXclust;
  unsigned int fdt; // time slice in units of sample time
  unsigned int splitDigis;
  double fG;
  double fC;

  unsigned int maxClusterSlice;

};


#endif


