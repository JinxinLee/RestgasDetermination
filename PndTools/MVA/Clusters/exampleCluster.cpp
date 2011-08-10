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

#define DEBUG_CLUSTERS_PRINT 0

typedef std::vector< std::pair<std::string, std::vector<float>*> > RawPoints;

#if DEBUG_CLUSTERS_PRINT
// *************  DEBUG ONLY **********
void printCentroids(ClDataSample const& dat)
{
  std::cout << "==========================\n";
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
#endif

/**
 * Example program. Shows, how to use the current clustering
 * implementation.
 */
int main(int argc, char** argv)
{
  if(argc < 3)
  {
    std::cerr << "<ERROR> Usage:" << argv[0]
	      <<" <DataInputFile> <numCentroids>"
	      << std::endl;
    return 1;
  }
  std::cout << "<INFO> MVA Clustering.\n";
  
  // Input File name.
  std::string InFile(argv[1]);

  // Number of centroids.
  int numCentrrs = str2int(argv[2]);
  
  std::cout << "<INFO> Using input data from file " << InFile
	    <<"\n\t number of clusters = " << numCentrrs
	    << ".\n";

  // Construct the class name vector  
  std::vector<std::string> label;
  label.push_back("electron");
  label.push_back("pion");
  
  // Construct the variable name vector  
  std::vector<std::string> vars;
  
  vars.push_back("emc");
  vars.push_back("lat");
  vars.push_back("z20");
  vars.push_back("z53");

  //vars.push_back("stt"); vars.push_back("thetaC");
  //vars.push_back("mvd"); vars.push_back("tof");  

  // Read the input points.
  PndMvaDataSet data(InFile, label, vars, TRAIN);
  
  // Init structures and read.
  data.Initialize();
  
  // Get available examples
  RawPoints const& samples = data.GetData();

  std::vector< ClDataSample* > ProtoVector ( label.size() );
  
  // Prepair clustering input
  // Class loop
  int cl = 0;
  int numClasses = label.size();

#ifdef _OPENMP
#pragma omp parallel for private(cl) schedule(dynamic)
#endif
  for(cl = 0; cl < numClasses; cl++)
  {
    ClDataSample clusteringInput;
    std::string clsName = label[cl];
    
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

#if DEBUG_CLUSTERS_PRINT
    printCentroids(*protoA);
#endif
    
    ProtoVector[cl] = protoA;
    
    clusteringInput.clear();
    
  }// End of class loop

#if DEBUG_CLUSTERS_PRINT
  std::cout << "++++++++++++++++++++++++++++++++++++++\n"
	    << "Printing the copied values:\n"
	    << "++++++++++++++++++++++++++++++++++++++\n";
  
  for(size_t l = 0; l < ProtoVector.size(); l++)
  {
    ClDataSample* protoA = ProtoVector[l];
    printCentroids(*protoA);
  }
#endif

  //_______________ Cleaning ___________________//
  for(size_t l = 0; l < ProtoVector.size(); l++)
  {
    delete ProtoVector[l];
  }
  
  ProtoVector.clear();
  return 0;
}
