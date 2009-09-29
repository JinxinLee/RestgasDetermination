//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcSectorProcessor
//      see PndTpcSectorProcessor.hh for details
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
#include "PndTpcSectorProcessor.h"

// C/C++ Headers ----------------------
#include <iostream>
#include "assert.h"

// Collaborating Class Headers --------
#include "PndTpcPadPlane.h"
#include "PndTpcDigiMapper.h"
#include "TORPadProcessor.h"
#include "PndTpcCluster.h"
#include "TMatrixD.h"

// Class Member definitions -----------


PndTpcSectorProcessor::~PndTpcSectorProcessor()
{
  // clean up PadProcessors
  std::map<unsigned int,padprocessor*>::iterator ipad=fpproc.begin();
  while(ipad!=fpproc.end()){
    delete ipad->second;
    ++ipad;
  }
  fpproc.clear();
}

void
PndTpcSectorProcessor::Init(PndTpcPadPlane* p,
			 unsigned int id,
			 std::vector<PndTpcCluster*>* ob)
{
  fpadplane=p;
  fSectorId=id;
  foutput_buffer=ob;

  // Build PadProcessors
  std::map<unsigned int, PndTpcPad*>* pads=fpadplane->GetSectorList(fSectorId);
  std::map<unsigned int,PndTpcPad*>::iterator ipad=pads->begin();
  while(ipad!=pads->end()){
    fpproc[ipad->second->id()]=new padprocessor(ipad->second->id());
    fpproc[ipad->second->id()]->setClusterBuffer(&fcluster_buffer);
    ++ipad;
  }
  // Connect PadProcessor neighbours
  ipad=pads->begin();
  while(ipad!=pads->end()){
    unsigned int nneighb=ipad->second->nNeighbours();
    for(unsigned int in=0; in<nneighb; ++in){
      unsigned int neighID=ipad->second->getNeighbour(in);
      if(fpproc[neighID]!=0){
        fpproc[ipad->second->id()]->addNeighbour(fpproc[neighID]);
        //cout << "Pad " << ipad->second->id() << " connects to Pad " << neighID <<endl;
      }
      else std::cout << "Neighbour " << neighID 
		     <<" not in this Sector!!!" << std::endl;
    }
    ++ipad;
  }
}

void
PndTpcSectorProcessor::process(){
  
  //fcluster_buffer.clear();

  unsigned int ndigis=fdigi_buffer.size();
  if(ndigis==0)return;
  //if(ndigis>0)std::cout<<ndigis<<" digis in sector "<<fSectorId<<std::endl;
  // loop over digis
  for(unsigned int i=0; i<ndigis; ++i){
    // fill padprocessors
    //std::cout<<"fill data in pproc "<<(fdigi_buffer[i])->padId()<<std::endl;
    //unsigned int digisid=fpadplane->GetPad((fdigi_buffer[i])->padId())->sectorId();
    //std::cout<<"in sector digisid="<<digisid<<" should be sector"<<fSectorId<<std::endl;
    padprocessor* pp=fpproc[fdigi_buffer[i]->padId()];
    assert(pp!=NULL);
    pp->setData(fdigi_buffer[i]);
    // activate pad processor by putting it into active list
    // this way only thos pads that have been hit are processing
    factivepads[fdigi_buffer[i]->padId()]=pp;
  } // end loop over digis


  // run padProcessors
  // TODO: stop criterion, how many heartbeats????
  bool GoOn=true;
  int counter=0;
  while(GoOn && counter<40){
    //cout<<"beat! ***************"<<endl;
    ++counter;
    GoOn=false;
    std::map<unsigned int, padprocessor*>::iterator ip=factivepads.begin();
    for(;ip!=factivepads.end(); ++ip){
      std::string nextstate=ip->second->heartbeat();
      if(nextstate!="end"){
	GoOn=true;
	//std::cout<<"padproc_"<<ip->first<<" in state: "<<nextstate<<std::endl;
	//std::cout<<"ndata="<<ip->second->ndata()
	// <<"  iscenter="<<ip->second->isCenter()
	//       <<"  domneighb="<<ip->second->domNeighbour()<<std::endl;
      }
    }

    // check number of digis still here
    //unsigned int numdig=0;
    //std::map<unsigned int, padprocessor*>::iterator ppit=fpproc.begin();
    //while(ppit!=fpproc.end()){
    //  numdig+=ppit->second->ndata();
    //  ++ppit;
      //}
    

  }// end heartbeats
  if(counter>=39)std::cout<<"SectorProc"<<fSectorId
			  <<" terminated after 40beats loosing data"
			  <<std::endl;
  //std::cout<<counter<<" heartbeats done"<<std::endl;
  //add digis in clusters
  unsigned int numdig=0;
  for(unsigned int ic=0;ic<fcluster_buffer.size();++ic){
    fcluster_buffer[ic];
    numdig+= fcluster_buffer[ic]->size();
  }

  int dif=(int)ndigis-(int)numdig;
  if(numdig!=ndigis)std::cout<<"SectorProc"<<fSectorId
			     <<" lost "<<dif
			     <<" digis"<<std::endl;

  //if(fcluster_buffer.size()!=0){
  // std::cout<<"Sector "<<fSectorId<<": "
  //  	     <<numdig<<" digis used out of "
  //  	     <<ndigis<<" digis."<<std::endl;
  //}

  
  // cleanup ************
  //factivepads.clear();
  //fdigi_buffer.clear();
  
  // calculate cog and create PndTpcClusters;
  cog();
}

void
PndTpcSectorProcessor::reset()
{
 fdigi_buffer.clear();
 fcluster_buffer.clear();
 std::map<unsigned int, padprocessor*>::iterator ppit=factivepads.begin();
 while(ppit!=factivepads.end()){
   //std::cout<<"still "<<ppit->second->ndata()<<" data on padproc"<<std::endl;
   ppit->second->reset();
   ++ppit;
 }
 factivepads.clear();
}

void
PndTpcSectorProcessor::cog(){
  assert(fpadplane!=NULL);
  assert(foutput_buffer!=NULL);
  // Calculate COG
  for(unsigned int ic=0;ic<fcluster_buffer.size();++ic){
    std::vector<PndTpcDigi*>* digis=fcluster_buffer[ic];
    TVector3 pos(0,0,0);
    double amp=0;
    TVector3 sig(0,0,0);
    unsigned int id=0;
    McIdCollection mcid;
    unsigned int ndigis=digis->size();
    for(;id<ndigis;++id){
      PndTpcDigi* adigi=(*digis)[id];
      double a=(double)adigi->amp();
      mcid.AddIDCollection(adigi->mcId(),a);
      TVector3 thispos;
      PndTpcDigiMapper::getInstance()->map(adigi,thispos);
      double dx;
      double dy;
      PndTpcDigiMapper::getInstance()->padsize(adigi->padId(),dx,dy);

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

	  double driftl=thispos.z()-PndTpcDigiMapper::getInstance()->zGem();
	  //assert(driftl>=0);
	  double absdriftl=fabs(driftl);

	  double diffSigmaL = Dl * sqrt(absdriftl);
	  double diffSigmaT = Dt * sqrt(absdriftl);
	  double sigmaX_sq = dx*dx/12. + diffSigmaT*diffSigmaT;
	  double sigmaY_sq = dy*dy/12. + diffSigmaT*diffSigmaT;
	  double sigmaZ_sq = zDiff*zDiff/12. + diffSigmaL*diffSigmaL;

      TVector3 thissig(sigmaX_sq,sigmaY_sq,sigmaZ_sq);
      sig+=a*a*thissig;


      pos+=a*thispos;
      amp+=a;
    }
    pos*=1./amp;
    sig.SetX(sqrt(sig.X())/amp);
    sig.SetY(sqrt(sig.Y())/amp);
    sig.SetZ(sqrt(sig.Z())/amp);

    PndTpcCluster* cl=new PndTpcCluster(pos,sig,amp,id,ndigis);
    mcid.Renormalize();
    cl->SetMcId(mcid);

    // loop again over the digis to calculate 2nd moment
    TMatrixD cov(3,3);
    for(id=0;id<ndigis;++id){
      PndTpcDigi* adigi=(*digis)[id];
      TVector3 thispos;
      PndTpcDigiMapper::getInstance()->map(adigi,thispos);
      TMatrixD c(3,1);
      for(int i=0; i<3; ++i)c[i][0]=thispos[i]-pos[i];
      TMatrixD c_t(TMatrixD::kTransposed,c);
      TMatrixD acov(c,TMatrixD::kMult,c_t);
      cov+=acov;
      
      cl->addDigi(*adigi);
    }
    cov*=1./(double)ndigis;
    cl->SetCov(cov);

    foutput_buffer->push_back(cl);
  }
}
