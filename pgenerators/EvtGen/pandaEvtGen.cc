//
//  Sample test program for running EvtGen
//  
//  Created 17/10/2006 by Stefano Spataro
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
#include <fstream>
using std::endl;
using std::ofstream;

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
  EvtGen myGenerator("./DECAY.DEC","./evt.pdl");
//If I wanted a user decay file, I would read it in now.
  myGenerator.readUDecay("./test/ETACTOGG.DEC");
// Open the output file  in the format requested by PandaROOT
  ofstream out;
  out.open("output.evt");

  static EvtId ETAC=EvtPDL::getId(std::string("eta_c"));
  
  int nEvents = 10;

// Loop to create nEvents, starting from an Upsilon(4S)
  int i;
  for(i=0;i<nEvents;i++){
// Set up the parent particle
    EvtVector4R pInit(EvtPDL::getMass(ETAC),0.0,0.0,0.0);
   
    parent=EvtParticleFactory::particleFactory(ETAC,pInit);
    parent->setDiagonalSpinDensity();      

// Generate the event
    myGenerator.generateDecay(parent);

// Write out the results
    evtstdhep.init();
    parent->makeStdHep(evtstdhep);
    report(INFO,"EvtGen") << "event Number\t" << i << evtstdhep << endl;
// Write the output file
    out << i << "\t" << evtstdhep.getNPart();
    out <<evtstdhep<<endl;
    
    parent->deleteTree();  
  }	
  out.close();
  return 1;
}
