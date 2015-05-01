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
// Module: EvtGen/EvtDecayNew.cc
//
// Description:
//
// Modification history:
//
// SS   April 11, 2014  1st implementation from part of EvtChic1toJpsiGam from Jan Zhong
//
//------------------------------------------------------------------------
#include "EvtGenBase/EvtPatches.hh"

#include "EvtGenModels/EvtDecayNew.hh"
#include "EvtGenBase/EvtDecayBase.hh"
#include "EvtGenBase/EvtParticle.hh"
#include "EvtGenBase/EvtRadCorr.hh"
#include "EvtGenBase/EvtRandom.hh"
#include "EvtGenBase/EvtPDL.hh"
#include "EvtGenBase/EvtReport.hh"
using std::endl;

EvtDecayNew::EvtDecayNew() : EvtDecayBase(), _prob(0.), _weight(0.)
{
}

void EvtDecayNew::makeDecay(EvtParticle* p, bool recursive)
{
  (void)recursive; // to remove "unused" warnings 
  int ntimes=10000;
  double dummy;
  
  do
    {
      decay(p);
      ntimes--;
      dummy=getProbMax(_prob)*EvtRandom::Flat();
      p->setDecayProb(_prob/getProbMax(_prob));
    }while(ntimes&&(_prob<dummy));
  
  if (ntimes==0)
    {
      report(EVT_DEBUG,"EvtGen") << "Tried accept/reject:10000" <<endl;
    }
  
  if (getPHOTOS() || EvtRadCorr::alwaysRadCorr()) 
    {
      EvtRadCorr::doRadCorr(p);
    }		    
}




