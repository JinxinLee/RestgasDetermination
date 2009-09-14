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
#include "EvtGen.hh"
//#include "EvtGen/EvtGen.hh"
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
  int nEvents;
  EvtStdHep evtstdhep;
  EvtParticle *parent;
  double m=0.938271;
  double p=2;
  std::cout <<"Please insert momentum:\n";
  std::cin>>p;
  std::cout <<"Please insert nEvents:\n";
  std::cin>>nEvents;
  
  //double boostm =sqrt((sqrt(p*p+m*m)+m)*(sqrt(p*p+m*m)+m)-p*p);
  //double boostp =p/boostm;
  //std::cout << boostm<<"\n";
  //std::cout << boostp<<"\n";
  //std::cin;
//Initialize the generator - read in the decay table and particle properties
  EvtGen myGenerator("./DECAY.DEC","./evt.pdl");
//If I wanted a user decay file, I would read it in now.
  myGenerator.readUDecay("./test/PPBARTEST.DEC");
// Open the output file  in the format requested by PandaROOT
  ofstream out;

  out.open("output.evt");

  static EvtId PPBAR=EvtPDL::getId(std::string("ppbar"));
 // 
  EvtPDL::reSetMass(PPBAR,m);
// Loop to create nEvents, starting from an Upsilon(4S)
  int i;

static EvtId PROTON=EvtPDL::getId(std::string("p+"));

EvtVector4R pBar(sqrt(p*p+EvtPDL::getMass(PROTON)*EvtPDL::getMass(PROTON)),0.0,0.0,p);
EvtVector4R pr(EvtPDL::getMass(PROTON),0.0,0.0,0.0);
EvtVector4R pInit;

pInit = pr + pBar;

  for(i=0;i<nEvents;i++){
// Set up the parent particle
   // EvtVector4R pInit(boostm,0.0,0.0,boostp);
   
    parent=EvtParticleFactory::particleFactory(PPBAR,pInit);
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
