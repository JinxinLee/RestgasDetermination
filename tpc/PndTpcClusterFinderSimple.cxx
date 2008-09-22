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
  _amp(0.), _cogT(-1.), _padplane(p), _timeslice(t), _id(id)
{
}

PndTpcPrelimCluster::~PndTpcPrelimCluster()
{
}

void PndTpcPrelimCluster::addHit(const PndTpcDigi& digi,bool noXclust) {
  _digis.push_back(digi);
  //  cog();
  _cogT=0.;
  _amp=0;
  for(unsigned int i=0;i<_digis.size();++i) {
	_cogT+=_digis.at(i).amp()*_digis.at(i).t();
	_amp+=_digis.at(i).amp();
  }
  _cogT*=1./_amp;

  _possiblePads.insert(digi.padId());	
  PndTpcPad* pad = _padplane->GetPad(digi.padId());

    for(unsigned int ineigh=0;ineigh<pad->nNeighbours();++ineigh){
    PndTpcPad* neigh = _padplane->GetPad( pad->getNeighbour(ineigh) );

    bool useThisNeighbor=true;
    if(noXclust){
      if(fabs(pad->x()-neigh->x())>0.01) useThisNeighbor=false;//gt 0.1mm
    }
    if(useThisNeighbor) _possiblePads.insert(pad->getNeighbour(ineigh));	
  }

}

bool PndTpcPrelimCluster::isInTimeWindow(const PndTpcDigi* const digi){
  if( fabs(digi->t() - _cogT) <= _timeslice ) {
	return true;
  }
  else {
	return false;
  }
}

bool PndTpcPrelimCluster::isInCluster(const PndTpcDigi* const digi) {
  int padID = digi->padId();

  if(_possiblePads.count(padID) == 0) {
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

  for(unsigned int i=0;i<_digis.size();++i){
	nPad.insert(_digis.at(i).padId());

	TVector3 tempPos;
	PndTpcDigiMapper::getInstance()->map(&(_digis.at(i)),tempPos);
	double tempD = tempPos.x() * 1000.;
	int tempI = (int)tempD;
	nPadX.insert(tempI);
	tempD = tempPos.y() * 1000.;
	tempI = (int)tempD;
	nPadY.insert(tempI);
  }

  PndTpcCluster* c = new PndTpcCluster(_pos,_err,_amp,_id,_digis.size());
  c->nPad(nPad.size());
  c->nPadX(nPadX.size());
  c->nPadY(nPadY.size());

  if(saveRaw){//defined in PndTpcAbsClusterFinder.h and default false
    for(unsigned int i=0;i<_digis.size();++i){
      c->addDigi(_digis.at(i));
    }
  }

  return c;

}

void PndTpcPrelimCluster::cog(){
  _pos.SetXYZ(0,0,0);
  _amp=0;
  _cogT=0.;
  _err.SetXYZ(0,0,0);
  McIdCollection mcid;
  unsigned int ndigis=_digis.size();
  for(unsigned int id=0;id<ndigis;++id){
	PndTpcDigi adigi=_digis[id];
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
	_err+=a*a*thissig;
	
	_cogT+=a*adigi.t();
	_pos+=a*thispos;
	_amp+=a;
  }
  _pos*=1./_amp;

#ifdef TESTCHAMBER
  for(unsigned int id=0;id<ndigis;++id){
	PndTpcDigi adigi=_digis[id];
	TVector3 thispos;
	PndTpcDigiMapper::getInstance()->map(&adigi,thispos);
	double _m,_s;
	Pedestals::getPedestal(adigi.padId(),_m,_s);
	double sigmaAi = _s;
	TVector3 temp(pow(thispos.X()-_pos.X(),2.),
				  pow(thispos.Y()-_pos.Y(),2.),
				  pow(thispos.Z()-_pos.Z(),2.));
	temp *= sigmaAi*sigmaAi;
	_err += temp;
  }
#endif

  _cogT*=1./_amp;
  _err.SetX(sqrt(_err.X())/_amp);
  _err.SetY(sqrt(_err.Y())/_amp);
  _err.SetZ(sqrt(_err.Z())/_amp);
  
  //  PndTpcCluster* cl=new PndTpcCluster(_pos,_err,(unsigned int)_amp,id,ndigis);
  _dominant_mcid = mcid.DominantID();
  
}


PndTpcClusterFinderSimple::PndTpcClusterFinderSimple(PndTpcPadPlane* p,
						     std::vector<PndTpcCluster*>* ob,
						     unsigned int timeslice)
  : _padplane(p), _output_buffer(ob), _dt(timeslice), noXclust(false)
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

  prelimClusters.push_back(new PndTpcPrelimCluster(_padplane,_dt,prelimClusterCounter++));
  prelimClusters.at(0)->addHit(*(digis.at(ndigi-1)),noXclust);
  for(unsigned int _idigi=0;_idigi<ndigi-1;++_idigi) {
    int idigi = ndigi - 2 - _idigi;
    std::vector<unsigned int> selClusters;
    for(unsigned int iclust=0;iclust<prelimClusters.size();++iclust) {
      if(prelimClusters.at(iclust)->isInCluster(digis.at(idigi)) ) {
	selClusters.push_back(iclust);
      }
    }
    unsigned int nselclust = selClusters.size();
    
    if(nselclust == 0){
      PndTpcPrelimCluster* _c = new PndTpcPrelimCluster(_padplane,_dt,prelimClusterCounter++);
      _c->addHit(*(digis.at(idigi)),noXclust);
      prelimClusters.push_back(_c);
    }
    else {
      if(nselclust == 1) {
	prelimClusters.at(selClusters.at(0))->addHit(*(digis.at(idigi)),noXclust);
      }
      else{
	double _a = (double)digis.at(idigi)->amp();
	PndTpcDigi _d = *(digis.at(idigi));
	_d.amp(_a/nselclust);
	for(int i=0;i<nselclust;++i) {
	  prelimClusters.at(selClusters.at(i))->addHit(_d,noXclust);
	}
      }
    }
  }
  for(unsigned int i=0;i<prelimClusters.size();++i){
    _output_buffer->push_back(prelimClusters.at(i)->convPndTpcCluster(_saveRaw));
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
