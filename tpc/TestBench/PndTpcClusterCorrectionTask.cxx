//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Correction of cluster positions based on fit of 
//      aquired data
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Felix Boehmer    TUM            (original author)
//
//
//-----------------------------------------------------------

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "PndTpcDigiPar.h"

#include "PndTpcClusterCorrectionTask.h"

ClassImp(PndTpcClusterCorrectionTask)


PndTpcClusterCorrectionTask::PndTpcClusterCorrectionTask()
  : FairTask("Pnd Tpc Cluster Correction"),
  _persistence(false), _parSet(false)
{
  _clusterBranchName = "PndTpcCluster_raw";
  _clusterOutName = "PndTpcCluster";
  corrFunc = new TF1("fitfunc", 
		     "[0]+[1]*x+[2]*TMath::Power(x,2)+[3]*TMath::Power(x,3)+[4]*TMath::Power(x,4)+[5]*TMath::Power(x,5)",
		     0,1);
}

PndTpcClusterCorrectionTask::~PndTpcClusterCorrectionTask()
{
  delete corrFunc;
}



InitStatus
PndTpcClusterCorrectionTask::Init() {

//Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

  if(ioman==0)
    {
      Error("PndTpcIdealTrackingTask::Init","RootManager not instantiated!");
      return kERROR;
    }

   // Get input collection
  _clusterArray=(TClonesArray*) ioman->GetObject(_clusterBranchName);

  if(_clusterArray==0)
    {
      Error("PndTpcIdealTrackingTask::Init","Cluster-array not found!");
      return kERROR;
    }

  fpadplane = fpar->getPadPlane();
  
  // create and register output array
  _outArray = new TClonesArray("PndTpcCluster"); 
  ioman->Register(_clusterOutName,"PndTpc",_outArray,_persistence);
  
  return kSUCCESS;
}

void
PndTpcClusterCorrectionTask::SetParContainers() {

  std::cout<<"PndTpcClusterFinderTask::SetParContainers"<<std::endl;
  std::cout.flush();

  // Get run and runtime database
  FairRun* run = FairRun::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  FairRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("SetParContainers", "No runtime database");

  // Get PndTpc digitisation parameter container
  fpar= (PndTpcDigiPar*) db->getContainer("PndTpcDigiPar");
  if (! fpar ) Fatal("SetParContainers", "PndTpcDigiPar not found");
}





void
PndTpcClusterCorrectionTask::Exec(Option_t* opt) {
  std::cout<<"PndTpcClusterCorrectionTask::Exec()"<<std::endl;

  // Reset output Array
  if(_outArray==0) 
    Fatal("PndTpcClusterCorrectionTask::Exec)","No OutputArray!");
  
  _outArray->Delete();
  
  if(!_parSet) {
    std::cerr<<"PndTpcClusterCorrectionTask::Exec() Correction parameters not properly set!"
	     <<"... Aborting"<<std::endl;
    return;
  }
  
  //loop over clusters
  unsigned int nCl = _clusterArray->GetEntriesFast();
  for(unsigned int ic=0; ic<nCl; ic++) {
    PndTpcCluster* cl = (PndTpcCluster*) _clusterArray->At(ic);
    PndTpcCluster* cp = new ((*_outArray)[ic]) PndTpcCluster(*cl);
    TVector3 clpos = cp->pos();
    if(!(cp->size() == 2 && cl->nPad()==2)) 
      continue;
    //loop over digigs
    std::vector<TVector3*> positions;
    std::vector<double> amps;
    unsigned int nDigi = cp->nDigi();
    assert(nDigi==2);
    for(unsigned int id=0; id<nDigi; id++) {
      double x,y;
      fpadplane->GetPadXY(cp->getDigi(id)->padId(),x,y);
      positions.push_back(new TVector3(x,y,0.)); //very inefficient, I know ...
      amps.push_back(cp->getDigi(id)->amp());
    }//end loop over digis

    //now construct cluster axis and correct
    TVector3 axis = *(positions[0]) - *(positions[1]);
    //define positive axis direction along positive x-axis
    if(axis.X()<0)
      axis*=(-1.);
    axis.SetMag(1.); //normalize
    
    //calculate eta (fraction of left amplitude and total amplitude)
    double smallAmp = amps[0];
    if(positions[1]->X() < positions[0]->X())
      smallAmp = amps[1];
    double eta = smallAmp/(amps[0]+amps[1]);
    
    double shift = corrFunc->Eval(eta);
    TVector3 newPos = clpos+(shift*axis);
    cp->SetPos(newPos);

    std::cout<<"    shifted cluster position by "<<shift<<" along cluster axis"
	     <<std::endl;
    for(unsigned int i=0; i<positions.size(); i++)
      delete positions[i];

    } //end loop over clusters
  return;
    
}

void
PndTpcClusterCorrectionTask::SetParameters(double* parArr) {
  for(unsigned int ip=0; ip<6; ip++) {  //TODO: define this nicer
    corrFunc->SetParameter(ip, parArr[ip]);  //no range checks!
  }
  _parSet=true;
}



