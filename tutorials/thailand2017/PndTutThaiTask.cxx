// ************************************************************************
//
// psi(2S) -> J/psi (-> mu+ mu-) pi+ pi- Analysis Example Task
// 
// for the Rho Tutorial, see
// http://panda-wiki.gsi.de/cgi-bin/viewauth/Computing/PandaRootRhoTutorial
//
// K.Goetzen 7/2013
// ************************************************************************


// The header file
#include "PndTutThaiTask.h"

// C++ headers
#include <string>
#include <iostream>

// FAIR headers
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"

// ROOT headers
#include "TClonesArray.h"
#include "TVector3.h"
#include "TH1F.h"
#include "TH2F.h"

// RHO headers
#include "RhoCandidate.h"
#include "RhoHistogram/RhoTuple.h"
#include "RhoFactory.h"
#include "RhoMassParticleSelector.h"

// Analysis headers
#include "PndAnalysis.h"
#include "Pnd4CFitter.h"
#include "PndKinVtxFitter.h"
#include "PndKinFitter.h"
#include "PndVtxPoca.h"
		
		
using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
PndTutThaiTask::PndTutThaiTask() :
  FairTask("Panda Tutorial Analysis Task") { 
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndTutThaiTask::~PndTutThaiTask() { }
// -------------------------------------------------------------------------


// ----- Method to select true PID candidates
int PndTutThaiTask::SelectTruePid(PndAnalysis *ana, RhoCandList &l)
{
	int removed = 0;
	
	for (int ii=l.GetLength()-1;ii>=0;--ii)
	{
		if ( !(ana->McTruthMatch(l[ii])) )
		{
			l.Remove(l[ii]);
			removed++;
		}
	}
	
	return removed;
}
// -------------------------------------------------------------------------


// -----   Public method Init   --------------------------------------------
InitStatus PndTutThaiTask::Init() 
{		
	// initialize analysis object
	fAnalysis = new PndAnalysis();
	
	// reset the event counter
	fEvtCount = 0;

	// Mass selector for the jpsi cands
	fJpsiMassSel=new RhoMassParticleSelector("jpsi",3.096,1.0);
	
	//  #### EXERCISE: create the histograms
	hjpsim_all = new TH1F("hjpsim_all","J/#psi mass (all)",200,0,4.5);
	hpsim_all  = new TH1F("hpsim_all","#psi(2S) mass (all)",200,0,5);
	// ...
	
	// *** the lorentz vector of the initial psi(2S)
	fIni.SetXYZT(0, 0, 6.231552, 7.240065);
	
	return kSUCCESS;
}

// -------------------------------------------------------------------------
	
void PndTutThaiTask::SetParContainers() 
{
  // Get run and runtime database
  FairRun* run = FairRun::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");
}

// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void PndTutThaiTask::Exec(Option_t*)
{
	// *** some variables
	int j=0;
	
	// necessary to read the next event
	fAnalysis->GetEventInTask();
	
	if (!(++fEvtCount%100)) cout << "evt "<<fEvtCount<<endl;


	// #### EXERCISE
	// (1) combinatorics with desired PID algorithms and tightness, and PDG set for truth match
	// (2) select rought mass window for resonances
	// (3) do vertex fitting for resonances to charged particles
	// (4) fill final histograms, with and without truth match	
	
}


void PndTutThaiTask::Finish()
{	
	// #### EXERCISE : write all histograms
	hjpsim_all->Write();
	hpsim_all->Write();
	// ...
		
}

ClassImp(PndTutThaiTask)
