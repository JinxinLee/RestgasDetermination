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


padprocessor::padprocessor(unsigned int id)
  : _iscenter(true), _active_state(0), _dominant_neighb(-1), 
    _myid(id), _cluster_buffer(0), _mydata(NULL)
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
  std::map<std::string,ppstate*>::iterator istate=_states.begin();
  while(istate!=_states.end()){
    delete istate->second;
    ++istate;
  }
  _states.clear();
  std::cout<<" %%%%%%%%%%%%destructing padprocessor! %%%%%%%%%%%%" <<std::endl;
}

void
padprocessor::addNeighbour(padprocessor* pp)
{
  // TODO: check for double adding!
  _neighbours.push_back(pp);
}


void
padprocessor::setData(PndTpcDigi* data) // resets data buffer NOT anymore !
{
  //reset();
  if(_mydata==NULL) _mydata=data;
  else if(data->amp()>_mydata->amp())_mydata=data;
  put(data);
}

void
padprocessor::reset()
{
  _dominant_neighb=-1;
  _iscenter=true;
  if(_data.size()!=0)std::cout<<"PProc"<<_myid
			      <<" throwing "<<_data.size()
			      <<" data away!"<<std::endl;
  _data.clear();
  _mydata=NULL;
  setState("initial");
}

std::string
padprocessor::heartbeat(){
  if(_active_state!=0)
    return _active_state->heartbeat();
  else std::cerr<<"No active state defined!"<<std::endl;
  return std::string("not active");
}


void 
padprocessor::addState(ppstate* state, std::string name){
  _states[name]=state; // existing state will be overridden!
}

void 
padprocessor::setState(std::string name){
  if(_states[name]!=0){
    _active_state=_states[name];
    _astate=name;
    //std::cout<<"Pad("<<_myid<<"):: switching to state "<<name<<std::endl;
  }
  else std::cerr<<"Pad("<<_myid<<"):: Unknown State "<<name<<"!"<<std::endl;
  

}

void 
padprocessor::put(PndTpcDigi* data) {
  _data.push_back(data);
}
