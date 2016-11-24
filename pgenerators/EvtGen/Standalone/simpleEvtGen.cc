//
//  Sample test program for running EvtGen
//  
//  Created 17/10/2006 by Stefano Spataro
//
#include <iostream>
#include <string>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>

#include "EvtGen/EvtGen.hh"
#include "EvtGenBase/EvtPatches.hh"
#include "EvtGenBase/EvtParticleFactory.hh"
#include "EvtGenBase/EvtStdHep.hh"
#include "EvtGenBase/EvtParticle.hh"
#include "EvtGenBase/EvtRandom.hh"
#include "EvtGenBase/EvtRandomEngine.hh"
#include "EvtGenBase/EvtReport.hh"
#include "EvtGenBase/EvtHepMCEvent.hh"

/// use of external generators
#if EVTGEN_EXTERNAL
#include "EvtGenExternal/EvtExternalGenList.hh"
#include "EvtGenBase/EvtAbsRadCorr.hh"
#include "EvtGenBase/EvtDecayBase.hh"
#endif

using std::endl;
using std::ofstream;
using std::cout;

#include "TRandom3.h"

#ifndef EVTGEN_EVT_INFO
#define EVTGEN_EVT_INFO EVT_INFO
#endif

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
    cout << "\nUSAGE: simpleEvtGen <particle> <dec-file> <# events> <pbar-mom/cms-energy> <rand seed> <A_Target>\n" << endl;
    cout << "  <particle> = particle type to decay, e.g. 'eta_c', 'pbarpSystem' etc."<<endl;               //argv[1]
    cout << "  <dec-file> = EvtGen decay file (.DEC) to use; see directory 'test' for examples"<<endl;     //argv[2]
    cout << "  <# events> = number of events to produce; default value = 10"<<endl;                    //argv[3]
    cout << "  <pbar-mom> = (>0) momentum of the pbar beam; (<0) negativ cms energy; default value = mass of <particle>"<<endl;  //argv[4]
    cout << "               mandatory, when <particle> = pbarpSystem"<<endl;
    cout << "  <rand seed> = random seed for TRandom3. Value < 0 = use default random gen.; default = -1"<<endl; //argv[5]
    cout << "  <A_Target> = target nucleus mass number; mandatory when <particle> = 'pbarASystem'\n"<<endl;      //argv[6]
    cout << "Output is stored in file 'output.evt'.\n\n"<<endl;
    return 0;
  }

  //Initialize the generator - read in the decay table and particle properties
  int seed=-1;
  if (argc>5) seed=atoi(argv[5]);
  
  EvtRandomEngine* myRandomEngine=0;
  if (seed>=0)
    myRandomEngine=new EvtRootRandomEngine(seed);
  
#if EVTGEN_EXTERNAL
   EvtExternalGenList genList;
   EvtAbsRadCorr* radCorrEngine = genList.getPhotosModel();
   std::list<EvtDecayBase*> extraModels = genList.getListOfModels();

   // Create the EvtGen generator object
   EvtGen myGenerator("DECAY.DEC","evt.pdl",myRandomEngine,
                     radCorrEngine, &extraModels);
#else
   //If you don't want to use external generators, use the following:
  EvtGen myGenerator("DECAY.DEC","evt.pdl",myRandomEngine);
#endif

  //If I wanted a user decay file, I would read it in now.
  myGenerator.readUDecay(argv[2]);

  static EvtId PART=EvtPDL::getId(std::string(argv[1]));

  int number=10;
  if (argc>=4) number=atoi(argv[3]);

  double atarget = 0.;
  if (argc>=7) atarget=atof(argv[6]);

  if (std::string(argv[1])=="pbarpSystem" && argc<5)
  {
    cout <<"\n******  FATAL EVT_ERROR: <particle> is 'pbarpSystem'; MUST give pbar momentum or cms energy!\n\n"<<endl;
    return 0;
  }
  //else if (std::string(argv[1])!="pbarpSystem" && argc>=5)
  // {
  //  cout <<"\n****** WARNING: overriding given momentum, setting cms energy to mass of "<<argv[1]<<".\n"<<endl;
  // }

  if ((std::string(argv[1])=="pbardSystem" || std::string(argv[1])=="pbarASystem") && argc<5)
  {
    cout <<"\n******  FATAL EVT_ERROR: <particle> is 'pbardSystem' or 'pbarASystem'; MUST give pbar momentum!\n\n"<<endl;
    return 0;
  }

  if ((std::string(argv[1])=="pbardSystem" || std::string(argv[1])=="pbarASystem") && atof(argv[4])<0)
  {
    cout <<"\n******  FATAL EVT_ERROR: <particle> is 'pbardSystem' or 'pbarASystem'; cms energy doesn't make sense, give the pbar momentum!\n\n"<<endl;
    return 0;
  }

  if (std::string(argv[1])=="pbarASystem" && (argc<7 || atof(argv[6])<3 || atof(argv[6])>238))
  {
    cout <<"\n******  FATAL EVT_ERROR: <particle> is 'pbarASystem'; give target mass number within 3 and 238!\n\n"<<endl;
    return 0;
  }

  double val=-3.0969;
  double P = 0.0;
  double E = 0.0;
  double mp=0.938272;
  double md=1.875613;
  double mu=0.931494;
  double mtarg;

  if (std::string(argv[1])=="pbarpSystem") mtarg = mp;
  if (std::string(argv[1])=="pbardSystem") mtarg = md;
  if (std::string(argv[1])=="pbarASystem") mtarg = mu*atarget;

  if (argc>=5) 
    val=atof(argv[4]);
  else
    val=-EvtPDL::getMass(PART);
  
  // val is the momentum of the pbar beam
  if (val>0){  
    P = val;
    E = mtarg+sqrt(P*P+mp*mp);
  }
  else  //val is -E_cm (for p target only)
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
    if (i<10) report(EVTGEN_EVT_INFO,"EvtGen") << "event Number\t"<< i  << evtstdhep << endl;/// old style, commented out in EvtStdHep.cpp, but existing in EvtStdHep.hh
    if (!((i+1)%100))  report(EVTGEN_EVT_INFO,"EvtGen") << "event Number\t"<<i+1<<endl;
    
    /// Write the output file
    out << i << "\t" << evtstdhep.getNPart();
    out << evtstdhep; /// old style, commented out in EvtStdHep.cpp, but existing in EvtStdHep.hh
    /// OR write out the verbose results
    //     EvtHepMCEvent theEvent;
    //     theEvent.constructEvent(parent);
    //     HepMC::GenEvent* genEvent = theEvent.getEvent();
    //     genEvent->print(out);
    /// OR write them in a very short form
    //    parent->printTree();
    parent->deleteTree();  
  }	
  out.close();
  return 1;
}
