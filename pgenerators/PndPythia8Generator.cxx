#include <math.h>
#include "TROOT.h"
#include "Pythia.h"
#include "FairPrimaryGenerator.h"
//#include "FairGenerator.h"

#include "PndPythia8Generator.h"

using namespace Pythia8;

// -----   Default constructor   -------------------------------------------
PndPythia8Generator::PndPythia8Generator() 
{
  fPar_it = 0;
  fMom = 15.;
  fUseRandom1 = kFALSE; 
  fUseRandom3 = kTRUE;
}
// -------------------------------------------------------------------------

// -----   Default constructor   -------------------------------------------
Bool_t PndPythia8Generator::Init() 
{
  if (fUseRandom1) fRandomEngine = new PyTr1Rng();
  if (fUseRandom3) fRandomEngine = new PyTr3Rng();
  
  fPythia.setRndmEnginePtr(fRandomEngine);
  
  // Set Parameters
  for(Int_t jj=0;jj<fPar_it;jj++){
    fPythia.readString(fParams[jj]);
    cout<<"fPythia.readString(\""<<fParams[jj]<<"\")"<<endl;
  }
  cout<<"Beam Momentum "<<fMom<<endl;

  // fPythia.init(-2212, 2212, 0., 0., fMom, 0., 0., 0.);
  // Settings according to the obsolete init function
  fPythia.settings.mode("Beams:idA",  -2212);
  fPythia.settings.mode("Beams:idB",   2212);
  fPythia.settings.mode("Beams:frameType",  3);
  fPythia.settings.parm("Beams:pxA",    0.);
  fPythia.settings.parm("Beams:pyA",    0.);
  fPythia.settings.parm("Beams:pzA",    fMom);
  fPythia.settings.parm("Beams:pxB",    0.);
  fPythia.settings.parm("Beams:pyB",    0.);
  fPythia.settings.parm("Beams:pzB",    0.);
  fPythia.init();

  return kTRUE;
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndPythia8Generator::~PndPythia8Generator() 
{
}
// -------------------------------------------------------------------------

// -----   Passing the event   ---------------------------------------------
Bool_t PndPythia8Generator::ReadEvent(FairPrimaryGenerator* cpg)
{
  Int_t npart = 0;
  while(npart == 0)
    {
      fPythia.next();
      for(int i=0; i<fPythia.event.size(); i++)
	{
	  if(fPythia.event[i].isFinal())
	    {
	      npart++;
	    };
	};
    };
  
  for(Int_t ii=0; ii<fPythia.event.size(); ii++)
    if(fPythia.event[ii].isFinal())
      {
	cpg->AddTrack((Int_t)fPythia.event[ii].id(),
		      (Double_t)fPythia.event[ii].px(),(Double_t)fPythia.event[ii].py(),(Double_t)fPythia.event[ii].pz(), 
		      0.,0.,0.);
      };
  
  return kTRUE;
}
// -------------------------------------------------------------------------
void PndPythia8Generator::SetParameters(char* par)
{
  fParams[fPar_it] = par;
  fPar_it++;
}

// -------------------------------------------------------------------------
void PndPythia8Generator::Print(){
  fPythia.settings.listAll();
}
// -------------------------------------------------------------------------

ClassImp(PndPythia8Generator)
