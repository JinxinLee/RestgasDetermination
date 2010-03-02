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
#include "EvtGenBase/EvtRandomEngine.hh"

#include <string>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>

#include "TFile.h"
#include "TTree.h"
#include "TStopwatch.h"
#include "TRandom3.h"

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

using std::endl;
using std::ofstream;
using std::cout;


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


int main(int argc, char* argv[])
{

  TStopwatch timer;
  timer.Start();
  
  EvtStdHep evtstdhep;
  EvtParticle *parent;
  

  if (argc<3) {
    cout << "\nUSAGE: simpleEvtGenRO <particle> <dec-file> <# events> <pbar-mom/cms-energy> <rand seed>\n" << endl;
    cout << "  <particle> = particle type to decay, e.g. 'eta_c', 'pbarpSystem' etc."<<endl;               //argv[1]
    cout << "  <dec-file> = EvtGen decay file (.DEC) to use; see directory 'test' for examples"<<endl;     //argv[2]
    cout << "  <# events> = number of events to produce; default value = 10"<<endl;                    //argv[3]
    cout << "  <pbar-mom> = (>0) momentum of the pbar beam; (<0) negativ cms energy; default value = mass of <particle>"<<endl;  //argv[4]
    cout << "               mandatory, when <particle> = pbarpSystem"<<endl;
    cout << "  <rand seed> = random seed for TRandom3. Value < 0 = use default random gen.; default = -1\n"<<endl;                    //argv[3]
    cout << "Output is stored in file 'evtOutput.root'.\n\n"<<endl;
    return 0;
  }
  
  int seed=-1;
  if (argc>5) seed=atoi(argv[5]);
  EvtRandomEngine* myRandomEngine=0;
  if (seed>=0)
    myRandomEngine=new EvtRootRandomEngine(seed);

  //Initialize the generator - read in the decay table and particle properties
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
  //ofstream out;
  //out.open("output.evt");
   
  
  
//  TNtuple ntp("ntp","ntp","ev:N:Id:M1:M2:DF:DL:px:py:pz:E:t:x:y:z");
   
  Int_t ev=0, nLine = 0, pdgID = 0, nDecay = 0, nM1 = -1, nM2 = -1, nDF = -1, nDL = -1;
  Double_t fPx = 0., fPy = 0., fPz = 0., fE = 0.;
  Double_t fVx = 0., fVy = 0., fVz = 0., fT = 0.;
  Double_t fTht = 0., fM=0., fP=0.;
  int ntracks;
  
  TTree *ntp=new TTree("ntp","ntp");
  
  ntp->Branch("ev",&ev,"ev/I");
  ntp->Branch("N",&nLine,"N/I");
  ntp->Branch("Id",&pdgID,"ev/I");
  ntp->Branch("M1",&nM1,"ev/I");
  ntp->Branch("M2",&nM2,"ev/I");
  ntp->Branch("DF",&nDF,"ev/I");
  ntp->Branch("DL",&nDL,"ev/I");
  ntp->Branch("px",&fPx,"px/D");
  ntp->Branch("py",&fPy,"py/D");
  ntp->Branch("pz",&fPz,"pz/D");
  ntp->Branch("E",&fE,"E/D");
  ntp->Branch("t",&fT,"t/D");
  ntp->Branch("x",&fVx,"x/D");
  ntp->Branch("y",&fVy,"y/D");
  ntp->Branch("z",&fVz,"z/D");
  ntp->Branch("m",&fM,"m/D");
  ntp->Branch("p",&fP,"p/D");
  ntp->Branch("tht",&fTht,"tht/D");
  
  // Loop to create nEvents, starting from an Upsilon(4S)
  int i,j;
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
    //cout << i << "\t" << evtstdhep.getNPart();
    //cout <<evtstdhep<<endl;
    ntracks=evtstdhep.getNPart();
    
    for (j=0;j<ntracks;j++)
    {
		ev=i;
		nLine=j;
		pdgID=evtstdhep.getStdHepID(j);
		nDF = evtstdhep.getFirstDaughter(j);
		nDL = evtstdhep.getLastDaughter(j);      
		nDecay = nDL>0?(nDL-nDF)+1:1;
		nM1 = evtstdhep.getFirstMother(j);
		nM2 = evtstdhep.getLastMother(j);
		EvtVector4R p4=evtstdhep.getP4(j);
		EvtVector4R x4=evtstdhep.getX4(j);
		fE =p4.get(0);
		fPx=p4.get(1);
		fPy=p4.get(2);
		fPz=p4.get(3);
		fT =x4.get(0);
		fVx=x4.get(1);
		fVy=x4.get(2);
		fVz=x4.get(3);
		fP=sqrt(p4.get(1)*p4.get(1)+p4.get(2)*p4.get(2)+p4.get(3)*p4.get(3));
		fM=sqrt(p4.get(0)*p4.get(0) - (p4.get(1)*p4.get(1)+p4.get(2)*p4.get(2)+p4.get(3)*p4.get(3)));
		fTht=atan2(sqrt(p4.get(1)*p4.get(1)+p4.get(2)*p4.get(2)),p4.get(3));
            
	 	ntp->Fill();
    }
    
    parent->deleteTree();  
  }	
  
  TFile *f=new TFile("evtOutput.root","RECREATE");
  ntp->Write();
  f->Close();
  
  //out.close();
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  
  return 1;
}
