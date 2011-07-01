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


PndTpcPrelimCluster::PndTpcPrelimCluster(PndTpcPadPlane* p, double t, int id, double G, double C) :
  famp(0.), fcogT(-1.), fpadplane(p), ftimeslice(t), fid(id), fG(G), fC(C)
{
}


PndTpcPrelimCluster::~PndTpcPrelimCluster()
{
}


void PndTpcPrelimCluster::addHit(PndTpcDigi* digi, bool noXclust, double share) {
  fdigis.push_back(digi);
  fdigiShares[digi] = share;

  fcogT=0.;
  famp=0;
  for(unsigned int i=0;i<fdigis.size();++i) {
	  fcogT+=fdigis[i]->amp()*fdigiShares[fdigis[i]] * fdigis[i]->t();
	  famp+=fdigis[i]->amp()*fdigiShares[fdigis[i]];
  }
  fcogT*=1./famp;

  fpossiblePads.insert(digi->padId());	
  PndTpcPad* pad = fpadplane->GetPad(digi->padId());

  for(unsigned int ineigh=0; ineigh<pad->nNeighbours(); ++ineigh){
    PndTpcPad* neigh = fpadplane->GetPad( pad->getNeighbour(ineigh) );

    bool useThisNeighbor=true;
    if(noXclust){
      if(fabs(pad->x()-neigh->x())>0.01) useThisNeighbor=false;//gt 0.1mm
    }
    if(useThisNeighbor) fpossiblePads.insert(pad->getNeighbour(ineigh));	
  }

}


bool PndTpcPrelimCluster::isInTimeWindow(const PndTpcDigi* const digi){
  if( fabs(digi->t() - fcogT) <= ftimeslice ) 
    return true;
  return false;
}


bool PndTpcPrelimCluster::isInCluster(const PndTpcDigi* const digi) {
  if(!isInTimeWindow(digi))
    return false;
  int padID = digi->padId();
  if(fpossiblePads.count(padID) == 0) 
    return false;
  return true;
}


PndTpcCluster* PndTpcPrelimCluster::convPndTpcCluster(bool saveRaw) {
  cog(); // also error is calculated here
  PndTpcCluster* c = new PndTpcCluster(fpos,ferr,famp,fid,fdigis.size());
  c->SetMcId(fmcidCol);
  
  if(saveRaw){//defined in PndTpcAbsClusterFinder.h and default false
    for(unsigned int i=0;i<fdigis.size();++i){
      c->addDigi(fdigis[i], fdigiShares[fdigis[i]]);
    }
  }
  
  // simply choose first pad to define sector
  unsigned int sid=PndTpcDigiMapper::getInstance()->getPad(fdigis[0]->padId())->sectorId();
  c->SetSector(sid);
  
  return c;
}


void PndTpcPrelimCluster::cog(){
  bool DEBUG=false;

  fpos.SetXYZ(0,0,0);
  famp=0;
  McIdCollection mcid;
  unsigned int ndigis=fdigis.size();

  McId dummyID(1,1);
  McIdCollection dummyColl;
  dummyColl.AddID(dummyID);

  // loop over digis to calculate cog
  for(unsigned int id=0;id<ndigis;++id){
	  PndTpcDigi* adigi=fdigis[id];
	  double a=(double)adigi->amp()*fdigiShares[adigi];
	  mcid.AddIDCollection(adigi->mcId(),a);
	  TVector3 thispos;
	  PndTpcDigiMapper::getInstance()->map(adigi,thispos);
	  fpos+=a*thispos;
	  famp+=a;
  }
  fpos*=1./famp;

  //this block is to define the z jitter
  TVector3 zDiff1,zDiff2;
  PndTpcDigi zDiffDigi1(1,1,1,dummyColl),zDiffDigi2(1,2,1,dummyColl);
  PndTpcDigiMapper::getInstance()->map(&zDiffDigi1,zDiff1);
  PndTpcDigiMapper::getInstance()->map(&zDiffDigi2,zDiff2);
  double zDiff = zDiff2.z() - zDiff1.z();
  //end of z jitter

  // calculate errors: ------------------------------------------
	double dx, dy;
	PndTpcDigiMapper::getInstance()->padsize(fdigis[0]->padId(),dx,dy);

  ferr=(0.,0.,0.);
     
  for(unsigned int id=0;id<ndigis;++id){
    PndTpcDigi* adigi=fdigis[id];
    double a=(double)adigi->amp()*fdigiShares[adigi];
    TVector3 thispos;
    PndTpcDigiMapper::getInstance()->map(adigi,thispos);
    TVector3 df=thispos-fpos;
    double sigmaX_sq = a*df.X()*df.X();
    double sigmaY_sq = a*df.Y()*df.Y();
    double sigmaZ_sq = a*df.Z()*df.Z();
    
    TVector3 thissig(sigmaX_sq,sigmaY_sq,sigmaZ_sq);
    ferr+=thissig;  
  } // end second loop over digis

  if(ferr.X()<1E-5) ferr.SetX(sqrt(dx*dx/12));
  else ferr.SetX(sqrt(ferr.X()/famp)*fC/famp);

  if(ferr.Y()<1E-5) ferr.SetY(sqrt(dy*dy/12));
  else ferr.SetY(sqrt(ferr.Y()/famp)*fC/famp);

  if(ferr.Z()<1E-5) ferr.SetZ(sqrt(zDiff*zDiff/12));
  else ferr.SetZ(sqrt(ferr.Z()/famp)*fC/famp);
  
  if(DEBUG) ferr.Print();

  fmcidCol = mcid;
}



PndTpcClusterFinderSimple::PndTpcClusterFinderSimple(PndTpcPadPlane* p,
						     std::vector<PndTpcCluster*>* ob,
						     unsigned int timeslice, double G, double C)
  : fpadplane(p), foutput_buffer(ob), fdt(timeslice), noXclust(false),
    splitDigis(0), fG(G), fC(C), maxClusterSlice(3000)
{
// construct sector map
  std::vector<unsigned int> ids=fpadplane->GetSectorIds();
  unsigned int nsec=ids.size();
  for(unsigned int is=0;is<nsec;++is){
    unsigned int Sectorid=ids[is];
    fsectormap[Sectorid]=new std::vector<PndTpcDigi*>();
  }
  std::cout<<"PndTpcClusterFinderSimple: "
	   <<fsectormap.size()<<" Sectors instantiated."<<std::endl;
  
}


PndTpcClusterFinderSimple::~PndTpcClusterFinderSimple(){
  std::map<unsigned int,std::vector<PndTpcDigi*>*>::iterator secIt=fsectormap.begin();
  while(secIt!=fsectormap.end()){
    delete secIt->second;
    ++secIt;
  }
  fsectormap.clear();
}


void 
PndTpcClusterFinderSimple::process(std::vector<PndTpcDigi*>& alldigis)
{
  unsigned int nalldigi = alldigis.size();
  if(nalldigi<=3) return;
  
  std::sort(alldigis.begin(),alldigis.end(),PndTpcDigiAge());

  //std::cerr<<"ndigis: "<<ndigi<<std::endl;

  // sectorize on pad plane 
  for(int idi=0;idi<nalldigi;++idi){//loop over
    unsigned int sectorId=fpadplane->GetPad(alldigis[idi]->padId())->sectorId();
    fsectormap[sectorId]->push_back(alldigis[idi]);
  }
  
  // now process each sectorprocessor independently
  std::map<unsigned int,std::vector<PndTpcDigi*>* >::iterator secIt=fsectormap.begin();
  while(secIt!=fsectormap.end()){ // loop over sectors
    std::vector<PndTpcDigi*>* digis=secIt->second;
    unsigned int ndigi=digis->size();
    
    //std::cout << "Sector " << secIt->first << " with " <<ndigi<< " digits"<<std::endl; 

    if(ndigi<=3){
      ++secIt;
      continue;
    }
  // sectorize in z
  unsigned int nSlices = ndigi/maxClusterSlice + 1;
  unsigned int clusterSlice = ndigi/nSlices + 1;
  
  double startTime, stopTime, time;
  unsigned int firstDigi, lastDigi;
  
  for (unsigned int iSlice=0; iSlice<nSlices; ++iSlice){
    // find passive time window
    firstDigi = iSlice*clusterSlice;
    lastDigi = (iSlice+1)*clusterSlice - 1;
    if (lastDigi > ndigi-2) lastDigi = ndigi-1;

    startTime = (*digis)[firstDigi]->t();
    stopTime = (*digis)[lastDigi]->t();

    //std::cerr<<" active volume from "<<firstDigi<<" to "<<lastDigi<<std::endl;
    
    while(firstDigi>0){
      time = (*digis)[--firstDigi]->t();
      if (TMath::Abs(startTime-time) > 2*fdt) break;
    }
    while(lastDigi<ndigi-1){
      time = (*digis)[++lastDigi]->t();
      if (TMath::Abs(time-stopTime) > 2*fdt) break;
    }

    //std::cerr<<"  passive volume from "<<firstDigi<<" to "<<lastDigi<<std::endl;

    std::vector<PndTpcDigi*> digisInSlice;
    unsigned int ndigisInSlice = lastDigi-firstDigi;
    digisInSlice.reserve(ndigisInSlice);

    for (unsigned int i=firstDigi; i<lastDigi; ++i){
      digisInSlice.push_back((*digis)[i]);
    }
    
    
    std::stable_sort(digisInSlice.begin(),digisInSlice.end(),PndTpcDigiAmplitude());

    std::vector<PndTpcPrelimCluster*> prelimClusters;

    int prelimClusterCounter=0;

    prelimClusters.push_back(new PndTpcPrelimCluster(fpadplane,fdt,prelimClusterCounter++, fG,fC));
    prelimClusters[0]->addHit(digisInSlice.back(),noXclust);

    for(unsigned int idigi = ndigisInSlice-2; ; --idigi) { // loop over digis from back to front, last digi was already processed
      std::vector<unsigned int> selClusters; // contains indices of clusters that the digi might belong to
      for(unsigned int iclust=0;iclust<prelimClusters.size();++iclust) { // loop over prelimClusters
        if(prelimClusters[iclust]->isInCluster(digisInSlice[idigi]) ) {
          selClusters.push_back(iclust);
        }
      } // end loop over prelimClusters
      unsigned int nselclust = selClusters.size();

      if(nselclust == 0){ // digi cannot be added to existing cluster -> create a new cluster
        PndTpcPrelimCluster* fc = new PndTpcPrelimCluster(fpadplane,fdt,prelimClusterCounter++, fG,fC);
        fc->addHit(digisInSlice[idigi],noXclust);
        prelimClusters.push_back(fc);
      }
      else if(nselclust == 1) { // digi can only belong to one cluster -> add to this cluster
        prelimClusters[selClusters[0]]->addHit(digisInSlice[idigi],noXclust);
      }
      else { // digi can belong to more than one cluster -> assign digi to these clusters with corresponding share value
        PndTpcDigi* fd = digisInSlice[idigi];
        double share = 1./(double)nselclust;
        for(unsigned int i=0; i<nselclust; ++i) {
          prelimClusters[selClusters[i]]->addHit(fd,noXclust, share);
        }
        splitDigis += nselclust-1;
      }

      if(idigi==0) break;
    } // end loop over digis

    // convert prelimClusters to PndTpcClusters
    for(unsigned int i=0;i<prelimClusters.size();++i){
      double t = prelimClusters[i]->getDigi(0)->t();
      if(t>=startTime && t<stopTime)
        foutput_buffer->push_back(prelimClusters[i]->convPndTpcCluster(fsaveRaw));
      delete prelimClusters[i];
    }

  } // end loop over time-slices
  secIt->second->clear(); // clean up
   ++secIt;
  } // end loop over sectors

}


void 
PndTpcClusterFinderSimple::reset()
{ 
  splitDigis = 0;
  
}


void
PndTpcClusterFinderSimple::checkConsistency()
{
  std::cout << "PndTpcClusterFinderSimple::checkConsistency() empty implementation" << std::endl;
}
