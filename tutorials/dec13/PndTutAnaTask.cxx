// ************************************************************************
//
// Template for an Analysis Task, 
// 
// for the December 2013 CM Tutorial 
//
// For further info see also
//
// http://panda-wiki.gsi.de/cgi-bin/viewauth/Computing/PandaRootRhoTutorial
// http://panda-wiki.gsi.de/cgi-bin/view/Computing/PandaRootAnalysisJuly13
//
// K.Goetzen 11/2013
//
// ************************************************************************


// The header file
#include "PndTutAnaTask.h"

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
#include "Rho4CFitter.h"
#include "RhoKinVtxFitter.h"
#include "RhoKinFitter.h"
#include "RhoVtxPoca.h"
		
		
using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
PndTutAnaTask::PndTutAnaTask(double pbarmom) :
  FairTask("Panda Tutorial Analysis Task") 
{ 
	double mp=0.938272;
	fIni.SetXYZT(0,0,pbarmom, sqrt(pbarmom*pbarmom+mp*mp)+mp);
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndTutAnaTask::~PndTutAnaTask() { }
// -------------------------------------------------------------------------


// -----   Public method Init   --------------------------------------------
InitStatus PndTutAnaTask::Init() 
{		
	// *** initialize PndAnalysis object
	fAnalysis = new PndAnalysis();
	
	// *** reset the event counter
	fEvtCount = 0;

	// *******
	// ******* PREPARE/CREATE THE STUFF YOU NEED
	// *******
	
	return kSUCCESS;
}

// -------------------------------------------------------------------------
	
void PndTutAnaTask::SetParContainers() 
{
  // *** Get run and runtime database
  FairRun* run = FairRun::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");
}

// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void PndTutAnaTask::Exec(Option_t*)
{
	// *** some variables
	int i=0,j=0, k=0, l=0;
	
	// *** necessary to read the next event
	fAnalysis->GetEvent();
	
	// *** print event counter
	if (!(++fEvtCount%100)) cout << "evt "<<fEvtCount<<endl;
	
	// *******
	// ******* PUT ANALYSIS CODE HERE
	// *******
}


void PndTutAnaTask::Finish()
{
	
	// *******
	// ******* STORE YOUR HISTOS AND TUPLES
	// *******

}

ClassImp(PndTutAnaTask)
