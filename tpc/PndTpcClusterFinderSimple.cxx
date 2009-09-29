#include "PndTpcClusterFinderSimple.h"

#include <iostream>
#include <algorithm>
#include "assert.h"

#include "PndTpcPadPlane.h"
#include "PndTpcCluster.h"
#include "PndTpcDigiMapper.h"
#include "PndTpcDigiAmplitude.h"
#include "McId.h"
#include "McIdCollection.h"

#ifdef TESTCHAMBER
#include "Pedestals.h"
#endif


PndTpcPrelimCluster::PndTpcPrelimCluster(PndTpcPadPlane* p, double t, int id) :
  famp(0.), fcogT(-1.), fpadplane(p), ftimeslice(t), fid(id)
{
}

PndTpcPrelimCluster::~PndTpcPrelimCluster()
{
}

void PndTpcPrelimCluster::addHit(const PndTpcDigi& digi,bool noXclust) {
  fdigis.push_back(digi);
  //  cog();
  fcogT=0.;
  famp=0;
  for(unsigned int i=0;i<fdigis.size();++i) {
	fcogT+=fdigis.at(i).amp()*fdigis.at(i).t();
	famp+=fdigis.at(i).amp();
  }
  fcogT*=1./famp;

  fpossiblePads.insert(digi.padId());	
  PndTpcPad* pad = fpadplane->GetPad(digi.padId());

    for(unsigned int ineigh=0;ineigh<pad->nNeighbours();++ineigh){
    PndTpcPad* neigh = fpadplane->GetPad( pad->getNeighbour(ineigh) );

    bool useThisNeighbor=true;
    if(noXclust){
      if(fabs(pad->x()-neigh->x())>0.01) useThisNeighbor=false;//gt 0.1mm
    }
    if(useThisNeighbor) fpossiblePads.insert(pad->getNeighbour(ineigh));	
  }

}

bool PndTpcPrelimCluster::isInTimeWindow(const PndTpcDigi* const digi){
  if( fabs(digi->t() - fcogT) <= ftimeslice ) {
	return true;
  }
  else {
	return false;
  }
}

bool PndTpcPrelimCluster::isInCluster(const PndTpcDigi* const digi) {
  int padID = digi->padId();

  if(fpossiblePads.count(padID) == 0) {
	return false;
  }
  else {
	return isInTimeWindow(digi);
  }

}


PndTpcCluster* PndTpcPrelimCluster::convPndTpcCluster(bool saveRaw) {
  cog();

  std::set<int> nPad;
  std::set<int> nPadX;
  std::set<int> nPadY;

  for(unsigned int i=0;i<fdigis.size();++i){
	nPad.insert(fdigis.at(i).padId());

	TVector3 tempPos;
	PndTpcDigiMapper::getInstance()->map(&(fdigis.at(i)),tempPos);
	double tempD = tempPos.x() * 1000.;
	int tempI = (int)tempD;
	nPadX.insert(tempI);
	tempD = tempPos.y() * 1000.;
	tempI = (int)tempD;
	nPadY.insert(tempI);
  }

  PndTpcCluster* c = new PndTpcCluster(fpos,ferr,famp,fid,fdigis.size());
  c->nPad(nPad.size());
  c->nPadX(nPadX.size());
  c->nPadY(nPadY.size());

  if(saveRaw){//defined in PndTpcAbsClusterFinder.h and default false
    for(unsigned int i=0;i<fdigis.size();++i){
      c->addDigi(fdigis.at(i));
    }
  }

  return c;

}

void PndTpcPrelimCluster::cog(){
  fpos.SetXYZ(0,0,0);
  famp=0;
  fcogT=0.;
  ferr.SetXYZ(0,0,0);
  McIdCollection mcid;
  unsigned int ndigis=fdigis.size();
  for(unsigned int id=0;id<ndigis;++id){
	PndTpcDigi adigi=fdigis[id];
	mcid.AddIDCollection(adigi.mcId());
	double a=(double)adigi.amp();
	TVector3 thispos;
	PndTpcDigiMapper::getInstance()->map(&adigi,thispos);
	double dx;
	double dy;
	PndTpcDigiMapper::getInstance()->padsize(adigi.padId(),dx,dy);
	
	McId dummyID(1,1);
	McIdCollection dummyColl;
	dummyColl.AddID(dummyID);
	
	//this block is to define the z jitter
	TVector3 zDiff1,zDiff2;
	PndTpcDigi zDiffDigi1(1,1,1,dummyColl),zDiffDigi2(1,2,1,dummyColl);
	PndTpcDigiMapper::getInstance()->map(&zDiffDigi1,zDiff1);
	PndTpcDigiMapper::getInstance()->map(&zDiffDigi2,zDiff2);
	double zDiff = zDiff2.z() - zDiff1.z();
	//end of z jitter
	
	double Dl = PndTpcDigiMapper::getInstance()->getGas()->Dl();
	double Dt = PndTpcDigiMapper::getInstance()->getGas()->Dt();
	
	double diffSigmaL = Dl * sqrt(thispos.z());
	double diffSigmaT = Dt * sqrt(thispos.z());
	double sigmaX_sq = dx*dx/12. + diffSigmaT*diffSigmaT;
	double sigmaY_sq = dy*dy/12. + diffSigmaT*diffSigmaT;
	double sigmaZ_sq = zDiff*zDiff/12. + diffSigmaL*diffSigmaL;
	
	TVector3 thissig(sigmaX_sq,sigmaY_sq,sigmaZ_sq);
	ferr+=a*a*thissig;
	
	fcogT+=a*adigi.t();
	fpos+=a*thispos;
	famp+=a;
  }
  fpos*=1./famp;

#ifdef TESTCHAMBER
  for(unsigned int id=0;id<ndigis;++id){
	PndTpcDigi adigi=fdigis[id];
	TVector3 thispos;
	PndTpcDigiMapper::getInstance()->map(&adigi,thispos);
	double fm,fs;
	Pedestals::getPedestal(adigi.padId(),fm,fs);
	double sigmaAi = fs;
	TVector3 temp(pow(thispos.X()-fpos.X(),2.),
				  pow(thispos.Y()-fpos.Y(),2.),
				  pow(thispos.Z()-fpos.Z(),2.));
	temp *= sigmaAi*sigmaAi;
	ferr += temp;
  }
#endif

  fcogT*=1./famp;
  ferr.SetX(sqrt(ferr.X())/famp);
  ferr.SetY(sqrt(ferr.Y())/famp);
  ferr.SetZ(sqrt(ferr.Z())/famp);
  
  //  PndTpcCluster* cl=new PndTpcCluster(fpos,ferr,(unsigned int)famp,id,ndigis);
  fdominant_mcid = mcid.DominantID();
  
}


PndTpcClusterFinderSimple::PndTpcClusterFinderSimple(PndTpcPadPlane* p,
						     std::vector<PndTpcCluster*>* ob,
						     unsigned int timeslice)
  : fpadplane(p), foutput_buffer(ob), fdt(timeslice), noXclust(false)
{

}

PndTpcClusterFinderSimple::~PndTpcClusterFinderSimple(){

}



void 
PndTpcClusterFinderSimple::process(std::vector<PndTpcDigi*>& digis)
{
  sort(digis.begin(),digis.end(),PndTpcDigiAmplitude());
  unsigned int ndigi = digis.size();

  if(ndigi<=2) return;
  
  std::vector<PndTpcPrelimCluster*> prelimClusters;

  int prelimClusterCounter=0;

  prelimClusters.push_back(new PndTpcPrelimCluster(fpadplane,fdt,prelimClusterCounter++));
  prelimClusters.at(0)->addHit(*(digis.at(ndigi-1)),noXclust);
  for(unsigned int fidigi=0;fidigi<ndigi-1;++fidigi) {
    int idigi = ndigi - 2 - fidigi;
    std::vector<unsigned int> selClusters;
    for(unsigned int iclust=0;iclust<prelimClusters.size();++iclust) {
      if(prelimClusters.at(iclust)->isInCluster(digis.at(idigi)) ) {
	selClusters.push_back(iclust);
      }
    }
    unsigned int nselclust = selClusters.size();
    
    if(nselclust == 0){
      PndTpcPrelimCluster* fc = new PndTpcPrelimCluster(fpadplane,fdt,prelimClusterCounter++);
      fc->addHit(*(digis.at(idigi)),noXclust);
      prelimClusters.push_back(fc);
    }
    else {
      if(nselclust == 1) {
	prelimClusters.at(selClusters.at(0))->addHit(*(digis.at(idigi)),noXclust);
      }
      else{
	double fa = (double)digis.at(idigi)->amp();
	PndTpcDigi fd = *(digis.at(idigi));
	fd.amp(fa/nselclust);
	for(int i=0;i<nselclust;++i) {
	  prelimClusters.at(selClusters.at(i))->addHit(fd,noXclust);
	}
      }
    }
  }
  for(unsigned int i=0;i<prelimClusters.size();++i){
    foutput_buffer->push_back(prelimClusters.at(i)->convPndTpcCluster(fsaveRaw));
    delete prelimClusters.at(i);
  }
}



void 
PndTpcClusterFinderSimple::reset()
{
  
}


void
PndTpcClusterFinderSimple::checkConsistency()
{
  std::cout << "PndTpcClusterFinderSimple::checkConsistency() empty implementation" << std::endl;
}
