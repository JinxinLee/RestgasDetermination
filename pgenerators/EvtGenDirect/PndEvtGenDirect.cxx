// -------------------------------------------------------------------------
// -----                PndEvtGenDirect source file                  -----
// -----                                                          -----
// -------------------------------------------------------------------------


#include <iostream>
#include "TClonesArray.h"
#include "TFile.h"
#include "TLorentzVector.h"
#include "TTree.h"
#include "TVector3.h"
#include "TParticle.h"
#include "PndEvtGenDirect.h"
#include "FairPrimaryGenerator.h"
#include "TRandom.h"

#include "EvtGenBase/EvtPatches.hh"
#include "EvtGenBase/EvtPatches.hh"
#include "EvtGenBase/EvtParticleFactory.hh"
#include "EvtGenBase/EvtStdHep.hh"
#include "EvtGenBase/EvtParticle.hh"
#include "EvtGenBase/EvtRandom.hh"
#include "EvtGenBase/EvtRandomEngine.hh"
#include "EvtGenBase/EvtReport.hh"
#include <string>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

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

// -----   Default constructor   ------------------------------------------
PndEvtGenDirect::PndEvtGenDirect() {

}
// ------------------------------------------------------------------------

// -----   Standard constructor   -----------------------------------------
PndEvtGenDirect::PndEvtGenDirect(TString particle,TString decfile,Double_t Mom, Long_t Seed) {
    cout << "<I> PndEvtGenDirect"<<endl;
    cout << "<I> Particle: "<<particle<<endl;
    cout << "<I> decfile: "<<decfile<<endl;
    cout << "<I> pbar-Momentum: "<<Mom<<endl;
    cout << "<I> Rnd Seed: "<<Seed<<endl;

  //Initialize the generator - read in the decay table and particle properties
  
  EvtRandomEngine* myRandomEngine=0;
  if (Seed>=0)
    myRandomEngine=new EvtRootRandomEngine(Seed);

  myGenerator=new EvtGen("DECAY.DEC","evt.pdl",myRandomEngine);

  //If I wanted a user decay file, I would read it in now.
  if(decfile!="") myGenerator->readUDecay(decfile.Data());

  PART=EvtPDL::getId(std::string(particle.Data()));

  if (particle=="pbarpSystem" && Mom==0)
  {
    cout <<"\n******  FATAL ERROR: <particle> is 'pbarpSystem'; MUST give pbar momentum or cms energy!\n\n"<<endl;
  }

  double val=-3.0969;
  P = 0.0;
  E = 0.0;
  double mp=0.93827;

  if (Mom>0) 
    val=Mom;
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
  
  cout <<"\n############# Generating with following conditions:\n\n";
  cout <<"incident 4-mom : ("<<E<<", 0, 0, "<<P<<"), m = "<<sqrt(E*E-P*P)<<endl;
  cout <<"\n######################\n\n"<<endl;


}
// ------------------------------------------------------------------------

// -----   Destructor   ---------------------------------------------------
PndEvtGenDirect::~PndEvtGenDirect() {

}
// ------------------------------------------------------------------------

// -----   Public method ReadEvent   --------------------------------------
Bool_t PndEvtGenDirect::ReadEvent(FairPrimaryGenerator* primGen) {
static Int_t evtnr=0;
  // Loop to create nEvents, starting from an Upsilon(4S)
    // Set up the parent particle
  EvtParticle *parent;

    EvtVector4R pInit(E,  0.0000, -0.0000,  P);
    parent=EvtParticleFactory::particleFactory(PART,pInit);
    parent->setDiagonalSpinDensity();  


    // Generate the event
    myGenerator->generateDecay(parent);
    // Write out the results
    evtstdhep.init();
    parent->makeStdHep(evtstdhep);

	Bool_t plotflag;
	plotflag=false;
    //print out some status info
    if (evtnr<10 || ((evtnr+1)%100)==0){
	cout << "PndEvtGenDirect::ReadEvent "<<evtnr <<" "<<E<<" "<<P << endl;
	parent->printParticle();
	report(INFO,"EvtGen") << "event Number\t"<< evtnr << evtstdhep << endl;
	cout << evtnr << "\t" << evtstdhep.getNPart();
	cout <<evtstdhep<<endl;
	cout <<"==== now compare ==="<<endl;
	plotflag=true;
    }
    
    // Write the output

  	Int_t  npart;
	Double_t fX, fY, fZ, fT;
  	double Px,Py,Pz, fE; // ,Pm[1000],Wh[1000];
  	int Id;
  	npart=evtstdhep.getNPart();
	EvtVector4R vxyz,pxyz;
	
	for(Int_t i=0; i<npart; i++){
		Int_t nFD, nLD;
		// add track
		nFD=evtstdhep.getFirstDaughter(i);
		nLD=evtstdhep.getLastDaughter(i);
		if(nFD==-1 && nLD==-1){
			Id=evtstdhep.getStdHepID(i);
			vxyz=evtstdhep.getX4(i);
			pxyz=evtstdhep.getP4(i);
			fT=vxyz.get(0);
			fX=vxyz.get(1);
			fY=vxyz.get(2);
			fZ=vxyz.get(3);
			fE=pxyz.get(0);
			Px=pxyz.get(1);
			Py=pxyz.get(2);
			Pz=pxyz.get(3);
			if(plotflag) printf("- I -: new particle at: %f, %f, %f (%f)-> %f %f %f (%f) ID %d ##Daughters %d %d\n", fX, fY, fZ, fT,Px, Py, Pz, fE, Id, nFD, nLD);
			primGen->AddTrack(Id, Px, Py, Pz, fX, fY, fZ);
		}
	}
	if(plotflag) cout <<"==== compare end ==="<<endl;

    parent->deleteTree();  

	evtnr++;

  return kTRUE;

}
// ------------------------------------------------------------------------

ClassImp(PndEvtGenDirect)

