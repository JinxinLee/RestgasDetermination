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
#include <algorithm>
// Collaborating Class Headers --------
#include "PndTpcPadPlane.h"
#include "PndTpcSectorProcessor.h"
#include "PndTpcCluster.h"
#include "PndTpcDigiAge.h"
#include "PndTpcDigiMapper.h"
#include "McId.h"
#include "McIdCollection.h"
#include "TVector3.h"

#include "FairMultiLinkedData.h"
#include "PndDetectorList.h"

// Class Member definitions -----------

PndTpcClusterFinder::PndTpcClusterFinder(PndTpcPadPlane* p,
					 std::vector<PndTpcCluster*>* ob,
					 unsigned int timeslice, 
					 int mode, int sectorid,
					 bool datamode)
  : fpadplane(p), foutput_buffer(ob), fdt(timeslice), fmode(mode),
    fDataMode(datamode)
{
  // construct sector processors
  std::vector<unsigned int> ids=fpadplane->GetSectorIds();
  if(sectorid>=0){
    unsigned int myid=(unsigned int)sectorid;
    if(std::count(ids.begin(),ids.end(),myid)==0){
      std::cout<<"SectorId "<<myid<<" not found in padplane! Aborting!"
	       <<std::endl;
      throw *p;
    }
    else {
      ids.clear();
      ids.push_back(myid);
    }
  } // only select one sector
  unsigned int nsec=ids.size();
  for(unsigned int is=0;is<nsec;++is){
    unsigned int Sectorid=ids[is];
    fsproc[Sectorid]=new PndTpcSectorProcessor(fsaveRaw, fDataMode);
    fsproc[Sectorid]->Init(fpadplane,Sectorid,ob);
    fsectormap[Sectorid]=new std::vector<PndTpcDigi*>();
  }
  std::cout<<"PndTpcClusterFinder: "
	   <<fsproc.size()<<" SectorProcessors instantiated."<<std::endl;
}

PndTpcClusterFinder::~PndTpcClusterFinder(){
  std::map<unsigned int,PndTpcSectorProcessor*>::iterator secIt=fsproc.begin();
  while(secIt!=fsproc.end()){
    delete secIt->second;
    ++secIt;
  }
  fsproc.clear();
}



void 
PndTpcClusterFinder::process(std::vector<PndTpcDigi*>& digis)
{
  // time binning:
  sort(digis.begin(),digis.end(),PndTpcDigiAge());
  unsigned int ndigis=digis.size();
  if(ndigis==0)return;

  if(ftrcl){// trivial clustering;
    for(unsigned int i=0;i<ndigis;++i){
      TVector3 pos;
      PndTpcDigiMapper::getInstance()->map(digis[i],pos);
      McIdCollection id=digis[i]->mcId();
      PndTpcCluster* cl=new PndTpcCluster(pos,digis[i]->amp(),0);
      if(fsaveRaw){
	cl->addDigi(*(digis[i]));
      }
      cl->SetMcId(id);
      //set link to the track
      if(!fDataMode)
	cl->SetLink(FairLink("MCTrack", id.DominantID().mctrackID()));
      foutput_buffer->push_back(cl);
    }
    return;
  }
  // ---------------------- MODE 0 - global time bins ------------------
  if(fmode==0){
    double oldt=digis[0]->t();
    unsigned int i=0;
    while(i<ndigis){
      if(digis[i]->t()-fdt>oldt){
	// trigger processing
	//std::cout<<"Beginning cluster finding cycle at t="<<oldt<<std::endl;
	std::map<unsigned int,PndTpcSectorProcessor*>::iterator secIt=fsproc.begin();
	while(secIt!=fsproc.end()){
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
    std::map<unsigned int,PndTpcSectorProcessor*>::iterator secIt=fsproc.begin();
    while(secIt!=fsproc.end()){
      secIt->second->process();
      secIt->second->reset();
      ++secIt;
    }
  }
  // ---------------------- MODE 1 - individual time bins ------------------
  else if(fmode==1){
    // make the time binning in each sector separately:
    // build sectormap
    for(int idi=0;idi<ndigis;++idi){//loop over
      unsigned int sectorId=fpadplane->GetPad(digis[idi]->padId())->sectorId();
      fsectormap[sectorId]->push_back(digis[idi]);
    }
    std::cout<<"DEBUG: built sector map"<<std::endl;
    
  // now process each sectorprocessor independently
  std::map<unsigned int,std::vector<PndTpcDigi*>* >::iterator secIt=fsectormap.begin();
  while(secIt!=fsectormap.end()){ // loop over sectors
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
      if(padmap[adigi->padId()] || adigi->t()-fdt>oldt){
	//std::cout<<"digi_t="<<adigi->t()<<"   old_t="<<oldt<<std::endl;
    	  // process
    	  fsproc[secIt->first]->process();
	  // reset
    	  fsproc[secIt->first]->reset();
	  padmap.clear();
	  // set next time window at next! digi
    	  if(i<ndinsec-1)oldt=adigi->t();
      }
      padmap[adigi->padId()]=true;
      fsproc[secIt->first]->putDigi(adigi);
      ++i;
    }// end loop over digis in this sector
    // process remaining digis
    // process
    fsproc[secIt->first]->process();
    // reset
    fsproc[secIt->first]->reset();
    secIt->second->clear();
    ++secIt;
  }// end loop over sectors
  
  }
}


void 
PndTpcClusterFinder::reset()
{
  std::map<unsigned int,PndTpcSectorProcessor*>::iterator secIt=fsproc.begin();
  while(secIt!=fsproc.end()){
    secIt->second->reset();
    ++secIt;
  }
}


void
PndTpcClusterFinder::putDigi(PndTpcDigi* digi)
{
  PndTpcPad* pad=fpadplane->GetPad(digi->padId());
  if(pad==NULL){
    std::cerr<<"Unkown pad ID " << digi->padId() << ". Throwing."<<std::endl;
    throw superior_unknown_padId(digi->padId());
  }
  unsigned int sectorId=pad->sectorId();
  //std::cout<<"putting digi("<<digi->padId()<<") in sector "<<sectorId<<std::endl;
  fsproc[sectorId]->putDigi(digi);
}


void
PndTpcClusterFinder::checkConsistency()
{
  assert(fpadplane!=NULL);
  std::vector<PndTpcPad*> buffer=fpadplane->GetPads();
  unsigned int npads=buffer.size();
  std::cout<<"PndTpcClusterFinder::checkConsistency: "
	   <<npads<<" pads in PadList"<<std::endl;
  unsigned int npproc=0;
  std::map<unsigned int, PndTpcSectorProcessor*>::iterator secit=fsproc.begin();
  std::map<unsigned int, PndTpcSectorProcessor*>::iterator secend=fsproc.end();
  while(secit!=secend){// loop over sectors
    PndTpcSectorProcessor* secproc=secit->second;
    npproc+=secproc->getNPads();
    ++secit;
  }
  std::cout<<"PndTpcClusterFinder::checkConsistency: "
	   <<npads<<" pad processors available"<<std::endl;
  assert(npads=npproc);
  for(int ip=0;ip<npads;++ip){
    PndTpcPad* mypad=buffer[ip];
    if(mypad==NULL){
      std::cout<<"Pad ID#"<<ip<<" not used."<<std::endl;
      continue;
    }

    unsigned int sid=mypad->sectorId();
    unsigned int pid=mypad->id();
    const padprocessor* pp=fsproc[sid]->getPP(pid);
    
    if(pp==NULL){
      std::cout<<"Padprocessor#"<<pid<<" not instantiated!"<<std::endl;
    }
  }
  //std::cout<<fsproc[72]->getId()<<std::endl;
}
