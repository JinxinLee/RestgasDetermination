/* ***************************************
 * Clustering algorithms example program *
 * Author: M.Babai@rug.nl                *
 * Version:                              *
 * LICENSE:                              *
 * ***************************************
 */
#include <iostream>
#include <cstdlib>

#include "PndMvaDataSet.h"
#include "PndMvaCluster.h"

typedef std::vector< std::pair<std::string, std::vector<float>*> > RawPoints;

// *************  DEBUG ONLY **********
void printCentroids(const ClDataSample& dat)
{
  std::cout << "==========================" << std::endl;
  for(size_t i = 0; i < dat.size(); i++)
  {
    std::vector<float>* example = dat[i];
    std::cerr<< " (" << " ";
    for(size_t j = 0; j < example->size(); j++){
      std::cerr << example->at(j) << "  ";
    }
    std::cerr<< ")" << std::endl;
  }
  std::cout << "==========================" << std::endl;
}

// *************  DEBUG ONLY **********
int main(int argc, char** argv)
{
  if(argc < 3){
    std::cout << "<ERROR> Usage: exampleCluster <DataInputFile> <numCentroids>"
	      << std::endl;
    return 1;
  }
  std::cout << "<INFO> MVA Clustering." << std::endl;

  std::string InFile(argv[1]);
  int numCentrrs = atoi(argv[2]);

  std::cout << "<INFO> Using input data from file " << InFile
	    << std::endl <<"\t number of clusters = " << numCentrrs
	    << "." << std::endl;

  std::vector<std::string>clas;
  clas.push_back("Elect"); clas.push_back("Pion");
  //clas.push_back("Kaon"); clas.push_back("Muon");
  //clas.push_back("Gamma"); clas.push_back("Proton");

  std::vector<std::string>vars;
  vars.push_back("p"); //vars.push_back("emc");
  //vars.push_back("stt"); vars.push_back("thetaC");

  PndMvaDataSet data(InFile, clas, vars);
  const RawPoints& samples = data.GetData();

  // Prepair clustering input
  // Class loop
  for(size_t cl = 0; cl < clas.size(); cl++)
  {
    ClDataSample clusteringInput;
    std::string clsName = clas[cl];

    // Example loop
    for(size_t i = 0; i < samples.size(); i++){
      if(samples[i].first == clsName){
	clusteringInput.push_back(samples[i].second);
      }
    }// We have seen all available examples.
    std::cout << "Number of examples for " << clsName 
	      << " = " <<  clusteringInput.size()
	      << std::endl;

    PndMvaCluster clust (clusteringInput, numCentrrs);
    ClDataSample& protoA = clust.Cluster();

    clust.SetNumberOfClusters(numCentrrs);
    ClDataSample& protoB = clust.Cluster();
    
    //clust.printStructs();
    printCentroids(protoA);
    printCentroids(protoB);

    // Clean-up
    protoA.clear();
    protoB.clear();
    clusteringInput.clear();
  }
  return 0;
}
