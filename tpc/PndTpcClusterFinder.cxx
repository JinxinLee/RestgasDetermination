//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcClusterFinder
//      see PndTpcClusterFinder.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndTpcClusterFinder.h"

// C/C++ Headers ----------------------
#include <iostream>
#include "assert.h"

// Collaborating Class Headers --------
#include "PndTpcPadPlane.h"
#include "PndTpcSectorProcessor.h"
#include "PndTpcCluster.h"
#include "PndTpcDigiAge.h"
#include "PndTpcDigiMapper.h"
#include "McId.h"
#include "McIdCollection.h"
#include "TVector3.h"

// Class Member definitions -----------

PndTpcClusterFinder::PndTpcClusterFinder(PndTpcPadPlane* p,
				   std::vector<PndTpcCluster*>* ob,
				   unsigned int timeslice, int mode)
  : _padplane(p), _output_buffer(ob), _dt(timeslice), _mode(mode)
{
  // construct sector processors
  std::vector<unsigned int> ids=_padplane->GetSectorIds();
  unsigned int nsec=ids.size();
  for(unsigned int is=0;is<nsec;++is){
    unsigned int sectorid=ids[is];
    _sproc[sectorid]=new PndTpcSectorProcessor(_saveRaw);
    _sproc[sectorid]->Init(_padplane,sectorid,ob);
    _sectormap[sectorid]=new std::vector<PndTpcDigi*>();
  }
  std::cout<<"PndTpcClusterFinder: "
	   <<_sproc.size()<<" SectorProcessors instantiated."<<std::endl;
}

PndTpcClusterFinder::~PndTpcClusterFinder(){
  std::map<unsigned int,PndTpcSectorProcessor*>::iterator secIt=_sproc.begin();
  while(secIt!=_sproc.end()){
    delete secIt->second;
    ++secIt;
  }
  _sproc.clear();
}



void 
PndTpcClusterFinder::process(std::vector<PndTpcDigi*>& digis)
{
  // time binning:
  sort(digis.begin(),digis.end(),PndTpcDigiAge());
  unsigned int ndigis=digis.size();
  if(ndigis==0)return;

  if(_trcl){// trivial clustering;
    for(unsigned int i=0;i<ndigis;++i){
      TVector3 pos;
      PndTpcDigiMapper::getInstance()->map(digis[i],pos);
      McIdCollection id=digis[i]->mcId();
      PndTpcCluster* cl=new PndTpcCluster(pos,digis[i]->amp(),0);
      if(_saveRaw){
	cl->addDigi(*(digis[i]));
      }
      cl->SetMcId(id);
      _output_buffer->push_back(cl);
    }
    return;
  }
  // ---------------------- MODE 0 - global time bins ------------------
  if(_mode==0){
    double oldt=digis[0]->t();
    unsigned int i=0;
    while(i<ndigis){
      if(digis[i]->t()-_dt>oldt){
	// trigger processing
	//std::cout<<"Beginning cluster finding cycle at t="<<oldt<<std::endl;
	std::map<unsigned int,PndTpcSectorProcessor*>::iterator secIt=_sproc.begin();
	while(secIt!=_sproc.end()){
	  secIt->second->process();
	  secIt->second->reset();
	  ++secIt;
	}
	oldt=digis[i]->t();
      }
      putDigi(digis[i]);
      ++i;
    }// end loop over digis
    // process remaining
    std::map<unsigned int,PndTpcSectorProcessor*>::iterator secIt=_sproc.begin();
    while(secIt!=_sproc.end()){
      secIt->second->process();
      secIt->second->reset();
      ++secIt;
    }
  }
  // ---------------------- MODE 1 - individual time bins ------------------
  else if(_mode==1){
    // make the time binning in each sector separately:
  // build sectormap
  for(int idi=0;idi<ndigis;++idi){//loop over
    unsigned int sectorId=_padplane->GetPad(digis[idi]->padId())->sectorId();
    _sectormap[sectorId]->push_back(digis[idi]);
  }

  // now process each sectorprocessor independently
  std::map<unsigned int,std::vector<PndTpcDigi*>* >::iterator secIt=_sectormap.begin();
  while(secIt!=_sectormap.end()){ // loop over sectors
    std::vector<PndTpcDigi*>* digiList=secIt->second;
    unsigned int ndinsec=digiList->size();
    if(ndinsec==0){ 
    	++secIt;
    	continue;
    }
    unsigned int i=0;
    double oldt=(*digiList)[0]->t(); 
    while(i<ndinsec){ // loop over digis in this sector
      // keep track of hit pads
      std::map<unsigned int, bool> padmap;
      PndTpcDigi* adigi=(*digiList)[i];
      // check if pad was hit already or timeslice filled 
      // if so process sector before going on
      if(padmap[adigi->padId()] || adigi->t()-_dt>oldt){
	//std::cout<<"digi_t="<<adigi->t()<<"   old_t="<<oldt<<std::endl;
    	  // process
    	  _sproc[secIt->first]->process();
    	  // reset
    	  _sproc[secIt->first]->reset();
    	  padmap.clear();
    	  // set next time window at next! digi
    	  if(i<ndinsec-1)oldt=adigi->t();
      }
      padmap[adigi->padId()]=true;
      _sproc[secIt->first]->putDigi(adigi);
      ++i;
    }// end loop over digis in this sector
    // process remaining digis
    // process
    _sproc[secIt->first]->process();
    // reset
    _sproc[secIt->first]->reset();
    secIt->second->clear();
    ++secIt;
  }// end loop over sectors
  
  }
}


void 
PndTpcClusterFinder::reset()
{
  std::map<unsigned int,PndTpcSectorProcessor*>::iterator secIt=_sproc.begin();
  while(secIt!=_sproc.end()){
    secIt->second->reset();
    ++secIt;
  }
}


void
PndTpcClusterFinder::putDigi(PndTpcDigi* digi)
{
  PndTpcPad* pad=_padplane->GetPad(digi->padId());
  unsigned int sectorId=pad->sectorId();
  //std::cout<<"putting digi("<<digi->padId()<<") in sector "<<sectorId<<std::endl;
  _sproc[sectorId]->putDigi(digi);
}


void
PndTpcClusterFinder::checkConsistency()
{
  assert(_padplane!=NULL);
  std::vector<PndTpcPad*> buffer=_padplane->GetPads();
  unsigned int npads=buffer.size();
  std::cout<<"PndTpcClusterFinder::checkConsistency: "
	   <<npads<<" pads in PadList"<<std::endl;
  unsigned int npproc=0;
  std::map<unsigned int, PndTpcSectorProcessor*>::iterator secit=_sproc.begin();
  std::map<unsigned int, PndTpcSectorProcessor*>::iterator secend=_sproc.end();
  while(secit!=secend){// loop over sectors
    PndTpcSectorProcessor* secproc=secit->second;
    npproc+=secproc->getNPads();
    ++secit;
  }
  std::cout<<"PndTpcClusterFinder::checkConsistency: "
	   <<npads<<" pad processors available"<<std::endl;
  assert(npads=npproc);
  for(int ip=0;ip<npads;++ip){
    unsigned int sid=buffer[ip]->sectorId();
    unsigned int pid=buffer[ip]->id();
    const padprocessor* pp=_sproc[sid]->getPP(pid);
    
    if(pp==NULL){
      std::cout<<"Padprocessor#"<<pid<<" not instantiated!"<<std::endl;
    }
  }
  //std::cout<<_sproc[72]->getId()<<std::endl;
}
