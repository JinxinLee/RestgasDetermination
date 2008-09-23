//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      MPI module 
//      ClusterFinderModule
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------


// C/C++ Headers ----------------------
#include <iostream>
#include <mpi.h>

// Collaborating Class Headers --------
#include "PndTpcPadShapePool.h"
#include "PndTpcPadPlane.h"
#include "PndTpcClusterFinder.h"
#include "TString.h"


int main(int argc, char** argv){

  
  // Setup MPI context
  MPI_Init(&argc, &argv);
  int rank=-1;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  std::cout<<"MPI_rank="<<rank<<std::endl;

  // CFM Preparations

  // Setup the ClusterFinder
  int _timeslice=12; // to be read from config file
  int _mode=1;
  TString padshapes="/afs/e18/panda/SIM/sneubert/pandaroot/trunk/tpc/Hexagons0.2.dat";
  TString padplane="/afs/e18/panda/SIM/sneubert/pandaroot/trunk/tpc/pndhexplane.dat";


  PndTpcPadShapePool* _padShapes = new PndTpcPadShapePool(padshapes);
  PndTpcPadPlane* _padPlane= new PndTpcPadPlane(padplane, _padShapes);
  std::vector<PndTpcCluster*>* _cluster_buffer=new std::vector<PndTpcCluster*>;
  PndTpcClusterFinder* _finder=
    new PndTpcClusterFinder(_padPlane,
			    _cluster_buffer,
			    _timeslice, _mode,1);
  
  //_finder->checkConsistency();

};


