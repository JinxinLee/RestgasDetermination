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

/// use of external generators
#if EVTGEN_EXTERNAL
#include "EvtGenExternal/EvtExternalGenList.hh"
#include "EvtGenBase/EvtAbsRadCorr.hh"
#include "EvtGenBase/EvtDecayBase.hh"
#endif

#include "TFile.h"
#include "TTree.h"
#include "TStopwatch.h"
#include "TRandom3.h"

#define MAX 200

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
  //ofstream out;
  //out.open("output.evt");
   
  
  
//  TNtuple ntp("ntp","ntp","ev:N:Id:M1:M2:DF:DL:px:py:pz:E:t:x:y:z");
   
  Int_t ev=0;
  Int_t nLine[MAX], pdgID[MAX], nDau[MAX], nM1[MAX], nM2[MAX], nDF[MAX], nDL[MAX];
  Double_t fPx[MAX], fPy[MAX], fPz[MAX], fE[MAX];
  Double_t fVx[MAX], fVy[MAX], fVz[MAX], fT[MAX];
  Double_t fTht[MAX], fM[MAX], fP[MAX], fPt[MAX];
  
  Int_t nTrk=0;
  
  TTree *ntp=new TTree("ntp","ntp");
  
  ntp->Branch("ev",&ev,"ev/I");
  ntp->Branch("nTrk",&nTrk,"nTrk/I");
  
  ntp->Branch("N",nLine,"N[nTrk]/I");
  ntp->Branch("Id",pdgID,"Id[nTrk]/I");
  
  ntp->Branch("M1",nM1,"M1[nTrk]/I");
  ntp->Branch("M2",nM2,"M2[nTrk]/I");
  ntp->Branch("DF",nDF,"DF[nTrk]/I");
  ntp->Branch("DL",nDL,"DL[nTrk]/I");
  ntp->Branch("nDau",nDau,"nDau[nTrk]/I");
  
  ntp->Branch("px",fPx,"px[nTrk]/D");
  ntp->Branch("py",fPy,"py[nTrk]/D");
  ntp->Branch("pz",fPz,"pz[nTrk]/D");
  ntp->Branch("E",fE,"E[nTrk]/D");
  
  ntp->Branch("t",fT,"t[nTrk]/D");
  ntp->Branch("x",fVx,"x[nTrk]/D");
  ntp->Branch("y",fVy,"y[nTrk]/D");
  ntp->Branch("z",fVz,"z[nTrk]/D");
  
  ntp->Branch("m",fM,"m[nTrk]/D");
  ntp->Branch("p",fP,"p[nTrk]/D");
  ntp->Branch("pt",fPt,"pt[nTrk]/D");
  ntp->Branch("tht",fTht,"tht[nTrk]/D");
  
  // Loop to create nEvents, starting from an Upsilon(4S)
  int i,j;
  for(i=0;i<number;i++){
    // Set up the parent particle
    
    ev=i;

    EvtVector4R pInit(E,  0.0000, -0.0000,  P);
    parent=EvtParticleFactory::particleFactory(PART,pInit);
    parent->setDiagonalSpinDensity();  

    // Generate the event
    myGenerator.generateDecay(parent);

    // Write out the results
    evtstdhep.init();
    parent->makeStdHep(evtstdhep);

    //print out some status info
    if (i<10) report(EVTGEN_EVT_INFO,"EvtGen") << "event Number\t"<< i << evtstdhep << endl;
    if (!((i+1)%100))  report(EVTGEN_EVT_INFO,"EvtGen") << "event Number\t"<<i+1<<endl;
    
    // Write the output file
    //cout << i << "\t" << evtstdhep.getNPart();
    //cout <<evtstdhep<<endl;
    nTrk=evtstdhep.getNPart();
    
    if (nTrk>=MAX)
    {
      nTrk=MAX;
      cout <<"-W- Event #"<<i<<": exeeding maximum particle number "<<MAX<<endl;
    }
    
    for (j=0;j<nTrk;j++)
    {
		nLine[j]  = j;
		pdgID[j]  = evtstdhep.getStdHepID(j);       // PDG code
		nDF[j]    = evtstdhep.getFirstDaughter(j);  // Index first daughter (-1 if stable)
		nDL[j]    = evtstdhep.getLastDaughter(j);   // Index last daughter (-1 if stable)
		nDau[j]   = nDL[j]>0?(nDL[j]-nDF[j])+1:0;            // Number of daughters 
		nM1[j]    = evtstdhep.getFirstMother(j);    // Index of first mother 
		nM2[j]    = evtstdhep.getLastMother(j);     // Index of second mother
		
		EvtVector4R p4=evtstdhep.getP4(j);
		EvtVector4R x4=evtstdhep.getX4(j);
		
		fE[j]     = p4.get(0);						// Energy
		fPx[j]    = p4.get(1);						// Px component
		fPy[j]    = p4.get(2);						// Py component
		fPz[j]    = p4.get(3);						// Pz component
		
		fT[j]     = x4.get(0);						// Time
		fVx[j]    = x4.get(1);						// Vertex x component
		fVy[j]    = x4.get(2);						// Vertex y component
		fVz[j]    = x4.get(3);						// Vertex z component
		
		double pt2  = p4.get(1)*p4.get(1)+p4.get(2)*p4.get(2);
		double p2   = pt2 + p4.get(3)*p4.get(3);
		
		fPt[j]    = sqrt(pt2);						// Transverse Momentum
		fP[j]     = sqrt(p2);						// Modulus of Momentum
		
		fM[j]     = sqrt(p4.get(0)*p4.get(0) - p2); // Invariant Mass of particle
		fTht[j]   = atan2(fPt[j],p4.get(3));        // Polar angle
    }
    
	ntp->Fill();
    
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
