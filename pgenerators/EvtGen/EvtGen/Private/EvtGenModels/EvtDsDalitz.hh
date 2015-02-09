//--------------------------------------------------------------------------
//
// Environment:
//      This software is part of the EvtGen package developed jointly
//      for the BaBar and CLEO collaborations.  If you use all or part
//      of it, please give an appropriate acknowledgement.
//
// Copyright Information: See EvtGen/COPYRIGHT
//      Copyright (C) 1998      Caltech, UCSB
//
// 
//
//
// Modification history:
//
//    DJL/RYD     August 11, 1998         Module created
//
//
// ** Partly used as DS_DALITZ in PandaRoot        July, 2013       Lu Cao
//------------------------------------------------------------------------


#ifndef EVTDSDALITZ_HH
#define EVTDSDALITZ_HH

#include "EvtGenBase/EvtDecayAmp.hh"
#include "EvtGenBase/EvtFlatte.hh"
#include <vector>

class EvtParticle;

class EvtDsDalitz:public  EvtDecayAmp  {

public:

  EvtDsDalitz();
  virtual ~EvtDsDalitz();

  std::string getName();
  EvtDecayBase* clone();

  void init();
  void initProbMax();

  void decay(EvtParticle *p); 

private:

  int _d1,_d2,_d3,_flag;

    vector<EvtFlatteParam> _kkpi_params;
  
};

#endif
