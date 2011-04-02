//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class padprocessor
//      see padprocessor.hh for details
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
#include "TORPadProcessor.h"

// C/C++ Headers ----------------------
#include <iostream>
#include "assert.h"

// Collaborating Class Headers --------
#include "TORPPState.h"
#include "TORPPState_Initial.h"
#include "TORPPState_Compare.h"
#include "TORPPState_Send.h"
#include "TORPPState_Wait.h"
#include "TORPPState_Output.h"
#include "TORPPState_End.h"
#include "PndTpcDigi.h"

// Class Member definitions -----------


padprocessor::padprocessor(unsigned int Id)
  : fiscenter(true), fmasked(false), factive_state(0), fdominant_neighb(-1), 
    fmyid(Id), fcluster_buffer(0), fmydata(NULL)
{
  ppstate* init=new ppstate_initial(this);
  addState(init,"initial");
  ppstate* comp=new ppstate_compare(this);
  addState(comp,"compare");
  ppstate* send=new ppstate_send(this);
  addState(send,"send");
  ppstate* wait=new ppstate_wait(this);
  addState(wait,"wait");
  ppstate* out=new ppstate_output(this);
  addState(out,"output");
  ppstate* end=new ppstate_end(this);
  addState(end,"end");
  setState("initial");
}


padprocessor::~padprocessor()
{
  std::map<std::string,ppstate*>::iterator istate=fstates.begin();
  while(istate!=fstates.end()){
    delete istate->second;
    ++istate;
  }
  fstates.clear();
  std::cout<<" %%%%%%%%%%%%destructing padprocessor! %%%%%%%%%%%%" <<std::endl;
}

void
padprocessor::addNeighbour(padprocessor* pp)
{
  // TODO: check for double adding!
  fneighbours.push_back(pp);
}


void
padprocessor::setData(PndTpcDigi* data) // resets data buffer NOT anymore !
{
  //reset();
  if(fmydata==NULL) fmydata=data;
  else if(data->amp()>fmydata->amp())fmydata=data;
  put(data);
}

double
padprocessor::amp() const {
  if(fmasked){
    // return largest neighbour
    unsigned int n=fneighbours.size();
    double largest=0;
    for(unsigned int i=0;i<n;++i){
      double a=fneighbours[i]->amp();
      if(largest<a)largest=a;
    }
    return largest;
  }
  else {
    if(fmydata==0) return 0;
    else return fmydata->amp();
  }
}

void
padprocessor::reset()
{
  fdominant_neighb=-1;
  fiscenter=true;
  if(fdata.size()!=0)std::cout<<"PProc"<<fmyid
			      <<" throwing "<<fdata.size()
			      <<" data away!"<<std::endl;
  fdata.clear();
  fmydata=NULL;
  setState("initial");
}

std::string
padprocessor::heartbeat(){
  if(factive_state!=0)
    return factive_state->heartbeat();
  else std::cerr<<"No active state defined!"<<std::endl;
  return std::string("not active");
}


void 
padprocessor::addState(ppstate* state, std::string name){
  fstates[name]=state; // existing state will be overridden!
}

void 
padprocessor::setState(std::string name){
  if(fstates[name]!=0){
    factive_state=fstates[name];
    fastate.clear();
    fastate=name;
    name.clear();
    //std::cout<<"Pad("<<fmyid<<"):: switching to state "<<name<<std::endl;
  }
  else std::cerr<<"Pad("<<fmyid<<"):: Unknown State "<<name<<"!"<<std::endl;
  

}

void 
padprocessor::put(PndTpcDigi* data) {
  fdata.push_back(data);
}
