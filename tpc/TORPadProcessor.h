//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Processing unit for one pad
//
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef PADPROCESSOR_HH
#define PADPROCESSOR_HH

// Base Class Headers ----------------


// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op
#include <vector>
#include <map>
#include <string>
#include "PndTpcDigi.h"

// Collaborating Class Declarations --
class ppstate;


class padprocessor {
public:

  // Constructors/Destructors ---------
  padprocessor(unsigned int Id);
  ~padprocessor();

  // Operators
 
  // Accessors -----------------------
  unsigned int id() const {return fmyid;}
  bool isCenter() const {return fiscenter;}
  double amp() const {if(fmydata==0) return 0;
                            else return fmydata->amp();}
  PndTpcDigi* mydata() const {return fmydata;}
  unsigned int ndata() const {return fdata.size();}
  unsigned int domNeighbour() const {if(fdominant_neighb!=-1)return fneighbours[fdominant_neighb]->id();
  else return 0;}
  // Modifiers -----------------------
  void setData(PndTpcDigi* mydata); // resets data buffer!
  void addNeighbour(padprocessor* pp);
  //void SetOutBuffer();
  void addState(ppstate* state, std::string name);
  ppstate* getState(std::string name){return fstates[name];}
  void put(PndTpcDigi* data); // transport data into this pad
  PndTpcDigi* get(unsigned int i) {return fdata[i];}

  // Operations ----------------------
  std::string heartbeat(); // heartbeat of finite state machine 
                                  // returns state at end of heartbeat
  void setState(std::string name);
  std::string getCurrentState() const {return fastate;}
  void reset();
  void setClusterBuffer(std::vector<std::vector<PndTpcDigi*>*>* clb)
  {fcluster_buffer=clb;}

  friend class ppstate_initial;
  friend class ppstate_compare;
  friend class ppstate_send;
  friend class ppstate_wait;
  friend class ppstate_output;

private:

  // Private Data Members ------------    
  unsigned int fmyid;
  PndTpcDigi* fmydata;
  std::vector<PndTpcDigi*> fdata;   // fdata[0]=mydata
  std::vector<padprocessor*> fneighbours;
  bool fiscenter;
  int fdominant_neighb;

  std::map<std::string,ppstate*> fstates;
  ppstate* factive_state;
  std::string fastate;


  std::vector<std::vector<PndTpcDigi*>*>* fcluster_buffer;

  // Private Methods -----------------

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
