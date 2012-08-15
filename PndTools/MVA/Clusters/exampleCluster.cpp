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

#define CLUSTER_MULTI_LABEL  1

typedef std::vector< std::pair<std::string, std::vector<float>*> > RawPoints;

// *************  DEBUG ONLY **********
#if (DEBUG_CLUSTERS_PRINT > 0)
void printCentroids(DataPoints const& dat)
{
  std::cout << "==========================\n";
  for(size_t i = 0; i < dat.size(); i++)
  {
    std::string label = dat[i].first;
    std::vector<float>* example = dat[i].second;
    std::cout<< " label = "<< label << " (" << " ";
    for(size_t j = 0; j < example->size(); j++)
    {
      std::cout << example->at(j) << "  ";
    }
    std::cout << ")\n";
  }
  std::cout << "==========================\n";
}
#endif
// *************  DEBUG ONLY **********

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

  // vars.push_back("p");
  vars.push_back("emc");
  vars.push_back("lat");
  vars.push_back("z20");
  vars.push_back("z53");
  vars.push_back("E9E25");
  // vars.push_back("E1");
  // vars.push_back("E9");
  // vars.push_back("E25");
  // vars.push_back("E1E9");
  // vars.push_back("stt"); vars.push_back("thetaC");
  // vars.push_back("mvd"); vars.push_back("tof");  

  // Read the input points.
  PndMvaDataSet data(InFile, label, vars, TRAIN);
  
  // Init structures and read.
  data.Initialize();
  
  // Get available examples
  RawPoints const& samples = data.GetData();

  // Per label clustering. In case one provides the labeld data
  // seprated.
#if (CLUSTER_MULTI_LABEL == 0)
  std::vector< DataPoints* > ProtoVector ( label.size() );
  
  // Prepair clustering input
  // Class loop
  int cl = 0;
  int numClasses = label.size();

#ifdef _OPENMP
#pragma omp parallel for private(cl) schedule(dynamic)
#endif
  for(cl = 0; cl < numClasses; cl++)
  {
    DataPoints clusteringInput;
    std::string clsName = label[cl];
    
    // Example loop
    for(size_t i = 0; i < samples.size(); i++)
    {
      if(samples[i].first == clsName)
      {
	clusteringInput.push_back(samples[i]);
      }
    }// We have seen all available examples.

    std::cout << "Number of examples for " << clsName 
	      << " = " <<  clusteringInput.size()
	      << '\n';
    
    PndMvaCluster clust (clusteringInput, numCentrrs);
    //DataPoints* protoA = clust.Cluster();
    DataPoints* protoA = clust.ClusterAndLabel(KMEANS_HARD, label);

#if (DEBUG_CLUSTERS_PRINT > 0)
    printCentroids(*protoA);
#endif
    
    ProtoVector[cl] = protoA;
    
    clusteringInput.clear();
    //delete protoA;
  }// End of class loop
#endif

  // Semi unsupervised clustering. One can do the labeling afterward.
#if (CLUSTER_MULTI_LABEL == 1 )
  PndMvaCluster clst (samples, numCentrrs);
  DataPoints* protoB = clst.ClusterAndLabel(KMEANS_HARD, label);

#if (DEBUG_CLUSTERS_PRINT > 0)
  // Print Cluster nodes.
  printCentroids(*protoB);
#endif

  delete protoB;
#endif

#if ( (DEBUG_CLUSTERS_PRINT > 1) && (CLUSTER_MULTI_LABEL == 0) )
  std::cout << "++++++++++++++++++++++++++++++++++++++\n"
	    << "Printing the copied values:\n"
	    << "++++++++++++++++++++++++++++++++++++++\n";
  
  for(size_t l = 0; l < ProtoVector.size(); l++)
  {
    DataPoints* protoA = ProtoVector[l];
    printCentroids(*protoA);
  }
#endif

#if (CLUSTER_MULTI_LABEL == 0 )
  //_______________ Cleaning ___________________//
  for(size_t l = 0; l < ProtoVector.size(); l++)
  {
    delete ProtoVector[l];
  }
  ProtoVector.clear();
#endif

  return 0;
}
