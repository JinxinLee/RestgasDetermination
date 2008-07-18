//
//  Sample test program for running EvtGen
//  
//  Created 1999-12-27 Ryd/Lange   
//
#include "EvtGenBase/EvtPatches.hh"
#include "EvtGenBase/EvtPatches.hh"
#include <iostream>
#include "EvtGenBase/EvtParticleFactory.hh"
#include "EvtGenBase/EvtStdHep.hh"
#include "EvtGen/EvtGen.hh"
#include "EvtGenBase/EvtParticle.hh"
#include "EvtGenBase/EvtRandom.hh"
#include "EvtGenBase/EvtReport.hh"
#include <string>
using std::endl;

//Define random number fcn used by Jetset
extern "C" {
  extern float rlu_();
  extern float begran_(int *);
}

float rlu_(){
  return EvtRandom::Flat();
}
float begran_(int *){
  return EvtRandom::Flat();
}


int main(int argc, char* argv[]){

  EvtStdHep evtstdhep;
  EvtParticle *parent;

//Initialize the generator - read in the decay table and particle properties
  EvtGen myGenerator("../DECAY.DEC","../evt.pdl");
//If I wanted a user decay file, I would read it in now.
//  myGenerator.readUDecay(``../user.dec'');

  static EvtId UPS4=EvtPDL::getId(std::string("Upsilon(4S)"));

  int nEvents=5;

// Loop to create nEvents, starting from an Upsilon(4S)
  int i;
  for(i=0;i<nEvents;i++){
// Set up the parent particle
    EvtVector4R pInit(EvtPDL::getMass(UPS4),0.0,0.0,0.0);
    parent=EvtParticleFactory::particleFactory(UPS4,pInit);
    parent->setVectorSpinDensity();      

// Generate the event
    myGenerator.generateDecay(parent);

// Write out the results
    evtstdhep.init();
    parent->makeStdHep(evtstdhep);
    report(INFO,"EvtGen") << evtstdhep << endl;
    parent->deleteTree();  
  }	
  return 1;
}
