//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Inplementation of the crosstalk applied for CT correction
//      method "Vote" and "TimeLength"
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Maxence Vandenbroucke    TUM            (original author)
//
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndTpcCTapplyTask.h"

// C/C++ Headers ----------------------
#include <vector>
#include <map>
#include <algorithm>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "PndTpcCluster.h"
#include "TVector3.h"

#include <cmath>
 
using std::fabs;

// Class Member definitions -----------

ClassImp(PndTpcCTapplyTask)

  //  int PndTpcHouaskfh::TREE_DEPTH;

PndTpcCTapplyTask::PndTpcCTapplyTask()
: FairTask("PndTpc Crosstalk supp. application"),
  _persistence(kFALSE)
{
  _clusterBranchName = "PndTpcCluster";
  maxVote=1.0; tMin=0; zMax=8.0;
}

PndTpcCTapplyTask::~PndTpcCTapplyTask()
{
}


InitStatus
PndTpcCTapplyTask::Init()
{
//Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();
  if(ioman==0)
    {
      Error("PndTpcCTapplyTask::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  // Get input collection
  _clusterArray=(TClonesArray*) ioman->GetObject(_clusterBranchName);
  
  if(_clusterArray==0)
    {
      Error("PndTpcCTapplyTask::Init","Cluster-array not found!");
      return kERROR;
    }
   
  // create and register output array
  _clusterOutArray = new TClonesArray("PndTpcCluster");
  _clusterOutArray_unchanged = new TClonesArray("PndTpcCluster");
  
  ioman->Register("PndTpcCluster_cut","PndTpc",
		  _clusterOutArray,_persistence);
 
   _ca=new std::vector<PndTpcCluster*>;

  return kSUCCESS;
}

//EXEC
void PndTpcCTapplyTask::Exec(Option_t* opt)
{
  std::cout << "PndTpcCTapplyTask::Exec" << std::endl;

 // Reset output Arrays
  if(_clusterOutArray==0) Fatal("PndTpcCTapplyTask::Exec)",
				"No ClusterOutArray");
  _clusterOutArray->Delete();
  
  _ca->clear();

  // copy into vector
  unsigned int n=_clusterArray->GetEntries();
  for(int i=0;i<n;++i)
    { // loop over clusters
      PndTpcCluster* cl=(PndTpcCluster*)_clusterArray->At(i);
      
      if (cl->pos().Z() < zMax && //z Cut
	  cl->Fct()<maxVote)   //vote cut
	{
	  //CT corr with time
	  double tmin =9999.0,tmax=-1.0;
	  for (int u= 0; u<cl->nDigi();u++)
	    {
	      double t = cl->getDigi(u).t();
	      //  std :: cout << "t "<<t<< std::endl;
	      if (t>tmax) tmax=t;
	      if (t<tmin) tmin=t;
	    }
	  
	  if(tmax-tmin>tMin)//time length cut
	    _ca->push_back(cl);
	}
    }

  for(int i=0;i<_ca->size();++i)
    {
      PndTpcCluster* clmax=new((*_clusterOutArray)[_clusterOutArray->GetEntries()]) PndTpcCluster(*(*_ca)[i]);//saving
      //     delete (*_ca)[i];
    }
 
  return;
}
