//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      can be used to draw the sequence of clusters in a track
//	   with the position of the clusters	
// 	   the idea behind is to investigate the influence of diffusion on dx
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Viola Michael    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef PNDTPCDEDXDIAGNOSTICSTASK_HH
#define PNDTPCDEDXDIAGNOSTICSTASK_HH

// Base Class Headers ----------------
#include "FairTask.h"

class GFRecoHitFactory;
class PndTpcDEDXDiagnosticsTask : public FairTask
{
public:
	PndTpcDEDXDiagnosticsTask();	//do not run the task if you don' t want to save it
	virtual ~PndTpcDEDXDiagnosticsTask()	{}
	virtual InitStatus Init();
	virtual void Exec(Option_t* opt);
private:
	Bool_t _persistence;
	
	TString _trackBranchName;
  	TString _clusterBranchName;
	
	TClonesArray* _clusterArray;
  	TClonesArray* _trackArray;
	
	//Output Array
  	TClonesArray* _dEdxDiagnostics;
public:
	ClassDef(PndTpcDEDXDiagnosticsTask,1)
};

#endif
