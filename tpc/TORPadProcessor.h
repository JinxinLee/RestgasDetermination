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
  padprocessor(unsigned int id);
  ~padprocessor();

  // Operators
 
  // Accessors -----------------------
  unsigned int id() const {return _myid;}
  bool isCenter() const {return _iscenter;}
  double amp() const {if(_mydata==0) return 0;
                            else return _mydata->amp();}
  PndTpcDigi* mydata() const {return _mydata;}
  unsigned int ndata() const {return _data.size();}
  unsigned int domNeighbour() const {if(_dominant_neighb!=-1)return _neighbours[_dominant_neighb]->id();
  else return 0;}
  // Modifiers -----------------------
  void setData(PndTpcDigi* mydata); // resets data buffer!
  void addNeighbour(padprocessor* pp);
  //void SetOutBuffer();
  void addState(ppstate* state, std::string name);

  void put(PndTpcDigi* data); // transport data into this pad
  PndTpcDigi* get(unsigned int i) {return _data[i];}

  // Operations ----------------------
  std::string heartbeat(); // heartbeat of finite state machine 
                                  // returns state at end of heartbeat
  void setState(std::string name);
  std::string getCurrentState() const {return _astate;}
  void reset();
  void setClusterBuffer(std::vector<std::vector<PndTpcDigi*>*>* clb)
  {_cluster_buffer=clb;}

  friend class ppstate_initial;
  friend class ppstate_compare;
  friend class ppstate_send;
  friend class ppstate_wait;
  friend class ppstate_output;

private:

  // Private Data Members ------------    
  unsigned int _myid;
  PndTpcDigi* _mydata;
  std::vector<PndTpcDigi*> _data;   // _data[0]=mydata
  std::vector<padprocessor*> _neighbours;
  bool _iscenter;
  int _dominant_neighb;

  std::map<std::string,ppstate*> _states;
  ppstate* _active_state;
  std::string _astate;


  std::vector<std::vector<PndTpcDigi*>*>* _cluster_buffer;

  // Private Methods -----------------

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
