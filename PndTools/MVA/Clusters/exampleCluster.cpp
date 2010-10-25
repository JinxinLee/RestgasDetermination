/* ***************************************
 * Clustering algorithms example program *
 * Author: M.Babai@rug.nl                *
 * Version:                              *
 * LICENSE:                              *
 * ***************************************
 */

#include <iostream>
#include <cstdlib>

#ifdef _OPENMP
#include <omp.h>
#endif

#include "PndMvaDataSet.h"
#include "PndMvaCluster.h"

typedef std::vector< std::pair<std::string, std::vector<float>*> > RawPoints;

// *************  DEBUG ONLY **********
void printCentroids(const ClDataSample& dat)
{
  std::cout << "==========================" << '\n';
  for(size_t i = 0; i < dat.size(); i++)
  {
    std::vector<float>* example = dat[i];
    std::cout<< " (" << " ";
    for(size_t j = 0; j < example->size(); j++)
    {
      std::cout << example->at(j) << "  ";
    }
    std::cout << ")\n";
  }
  std::cout << "==========================\n";
}
// *************  DEBUG ONLY **********

/**
 * Example program. Shows, how to use the current clustering
 * implementation.
 */
int main(int argc, char** argv)
{
  if(argc < 3)
  {
    std::cerr << "<ERROR> Usage: exampleCluster <DataInputFile> <numCentroids>"
	      << std::endl;
    return 1;
  }
  std::cout << "<INFO> MVA Clustering.\n";
  
  // Input File name.
  std::string InFile(argv[1]);

  // Number of centroids.
  int numCentrrs = atoi(argv[2]);
  
  std::cout << "<INFO> Using input data from file " << InFile
	    << '\n' <<"\t number of clusters = " << numCentrrs
	    << ".\n";

  // Construct the class name vector  
  std::vector<std::string>clas;
  clas.push_back("electron");
  clas.push_back("pion");
  // clas.push_back("kaon");
  //clas.push_back("muon");
  //clas.push_back("proton");
  
  // Construct the variable name vector  
  std::vector<std::string>vars;
  
  //vars.push_back("p");
  vars.push_back("emc");
  vars.push_back("lat");
  vars.push_back("z20");
  vars.push_back("z53");
  
  //vars.push_back("stt"); vars.push_back("thetaC");
  //vars.push_back("mvd"); vars.push_back("tof");
  
  // Read the input points.
  PndMvaDataSet data(InFile, clas, vars);

  // Get available examples
  const RawPoints& samples = data.GetData();

  std::vector< ClDataSample* > ProtoVector ( clas.size() );
  
  // Prepair clustering input
  // Class loop
  int cl = 0;
  int numClasses = clas.size();

#ifdef _OPENMP
#pragma omp parallel for schedule(dynamic)
#endif
  for(cl = 0; cl < numClasses; cl++)
  {
    ClDataSample clusteringInput;
    std::string clsName = clas[cl];
    
    // Example loop
    for(size_t i = 0; i < samples.size(); i++)
    {
      if(samples[i].first == clsName)
      {
	clusteringInput.push_back(samples[i].second);
      }
    }// We have seen all available examples.

    std::cout << "Number of examples for " << clsName 
	      << " = " <<  clusteringInput.size()
	      << '\n';
    
    PndMvaCluster clust (clusteringInput, numCentrrs);
    ClDataSample* protoA = clust.Cluster();

    printCentroids(*protoA);
    
    ProtoVector[cl] = protoA;
    
    clusteringInput.clear();
    
  }// End of class loop
  
  std::cout << "++++++++++++++++++++++++++++++++++++++\n"
	    << "Printing the copied values:\n"
	    << "++++++++++++++++++++++++++++++++++++++\n";
  
  for(size_t l = 0; l < ProtoVector.size(); l++)
  {
    ClDataSample* protoA = ProtoVector[l];
    printCentroids(*protoA);
  }

  // Cleaning
  for(size_t l = 0; l < ProtoVector.size(); l++)
  {
    delete ProtoVector[l];
  }
  
  ProtoVector.clear();
  return 0;
}
