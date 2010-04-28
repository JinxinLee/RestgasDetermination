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
#include "EvtGenBase/EvtParticle.hh"
#include "EvtGenBase/EvtRandom.hh"
#include "EvtGenBase/EvtRandomEngine.hh"
#include "EvtGenBase/EvtReport.hh"
#include <string>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
using std::endl;
using std::ofstream;
using std::cout;

#include "TRandom3.h"

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

//define class for generating random nubers
class EvtRootRandomEngine:public EvtRandomEngine{
public:
  EvtRootRandomEngine(int s=0) {seed=s;}
  double random();
  int seed;
};

double EvtRootRandomEngine::random(){
  static TRandom3 randengine(seed);
  return randengine.Rndm();
}

int main(int argc, char* argv[]){

  EvtStdHep evtstdhep;
  EvtParticle *parent;

  if (argc<3) {
    cout << "\nUSAGE: simpleEvtGen <particle> <dec-file> <# events> <pbar-mom/cms-energy> <rand seed>\n" << endl;
    cout << "  <particle> = particle type to decay, e.g. 'eta_c', 'pbarpSystem' etc."<<endl;               //argv[1]
    cout << "  <dec-file> = EvtGen decay file (.DEC) to use; see directory 'test' for examples"<<endl;     //argv[2]
    cout << "  <# events> = number of events to produce; default value = 10"<<endl;                    //argv[3]
    cout << "  <pbar-mom> = (>0) momentum of the pbar beam; (<0) negativ cms energy; default value = mass of <particle>"<<endl;  //argv[4]
    cout << "               mandatory, when <particle> = pbarpSystem"<<endl;
    cout << "  <rand seed> = random seed for TRandom3. Value < 0 = use default random gen.; default = -1\n"<<endl;                    //argv[3]
    cout << "Output is stored in file 'output.evt'.\n\n"<<endl;
    return 0;
  }

  //Initialize the generator - read in the decay table and particle properties
  int seed=-1;
  if (argc>5) seed=atoi(argv[5]);
  
  EvtRandomEngine* myRandomEngine=0;
  if (seed>=0)
    myRandomEngine=new EvtRootRandomEngine(seed);
  
  EvtGen myGenerator("DECAY.DEC","evt.pdl",myRandomEngine);

  //If I wanted a user decay file, I would read it in now.
  myGenerator.readUDecay(argv[2]);

  static EvtId PART=EvtPDL::getId(std::string(argv[1]));

  int number=10;
  if (argc>=4) number=atoi(argv[3]);

  if (std::string(argv[1])=="pbarpSystem" && argc<5)
  {
    cout <<"\n******  FATAL ERROR: <particle> is 'pbarpSystem'; MUST give pbar momentum or cms energy!\n\n"<<endl;
    return 0;
  }
  //else if (std::string(argv[1])!="pbarpSystem" && argc>=5)
  // {
  //  cout <<"\n****** WARNING: overriding given momentum, setting cms energy to mass of "<<argv[1]<<".\n"<<endl;
  // }

  double val=-3.0969;
  double P = 0.0;
  double E = 0.0;
  double mp=0.93827;

  if (argc>=5) 
    val=atof(argv[4]);
  else
    val=-EvtPDL::getMass(PART);
  
  // val is the momentum of the pbar beam
  if (val>0){  
    P = val;
    E = mp+sqrt(P*P+mp*mp);
  }
  else  //val is -E_cm
  {
    val=-val;
    E = val*val/(2*mp);
    P = sqrt(E*E-val*val);
  }
  
  cout <<"\n\n############# Generating with following conditions:\n\n";
  cout <<"particle       : '"<<argv[1]<<"'"<<endl;
  cout <<"decay file     : "<<argv[2]<<endl;
  cout <<"incident 4-mom : ("<<E<<", 0, 0, "<<P<<"), m = "<<sqrt(E*E-P*P)<<endl;
  cout <<"# events       : "<<number<<"\n\n######################\n\n"<<endl;


  // Open the output file  in the format requested by PandaROOT
  ofstream out;
  out.open("output.evt");
 
  // Loop to create nEvents, starting from an Upsilon(4S)
  int i;
  for(i=0;i<number;i++){
    // Set up the parent particle

    EvtVector4R pInit(E,  0.0000, -0.0000,  P);
    parent=EvtParticleFactory::particleFactory(PART,pInit);
    parent->setDiagonalSpinDensity();  

    // Generate the event
    myGenerator.generateDecay(parent);

    // Write out the results
    evtstdhep.init();
    parent->makeStdHep(evtstdhep);

    //print out some status info
    if (i<10) report(INFO,"EvtGen") << "event Number\t"<< i << evtstdhep << endl;
    if (!((i+1)%100))  report(INFO,"EvtGen") << "event Number\t"<<i+1<<endl;
    
    // Write the output file
    out << i << "\t" << evtstdhep.getNPart();
    out <<evtstdhep<<endl;
    
    parent->deleteTree();  
  }	
  out.close();
  return 1;
}
