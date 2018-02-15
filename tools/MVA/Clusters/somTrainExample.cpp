#include <iostream>

// Local includes
#include "PndMvaSOMTrainer.h"
#include "PndMvaDataSet.h"
#include "PndMvaUtil.h"
#include "PndSomNode.h"
#include "TFile.h"
#include "TNtuple.h"

// If we want to write the map to the disk (just for testing)
#define WRITE_SOM_MAP_TO_DISK 1

int main(int argc, char** argv)
{
  if(argc < 4)
  {
    std::cerr << "\t<ERROR> Usage:\n"
              <<'\t' << argv[0]
	      << " <Numer of model vectors>"
	      << " <Numer of Sweeps>"
	      << " <InputTrainEventFeatureFile>"
              << std::endl;
    return 1;
  }
  std::cout << "<INFO> Starting the SOM training with "
            << argv[1] << " model vectors. Using the data from "
            << argv[3] << '\n';

  // Number of centroids.
  int numNodes = str2int(argv[1]);
  int numSwp   = str2int(argv[2]);

  // Input File name.
  std::string InFile (argv[3]);

  std::cout << "<INFO> Using input data from file " << InFile
	    <<"\n\t number of map nodes = " << numNodes
	    << ".\n";

  // Construct the class name vector  
  std::vector<std::string> label;
  label.push_back("electron");
  label.push_back("pion");
  //label.push_back("muon");

  // Construct the variable name vector  
  std::vector<std::string> vars;
  
  // vars.push_back("p");
  vars.push_back("emc");
  vars.push_back("lat");
  vars.push_back("z20");
  vars.push_back("z53");
  vars.push_back("E9E25");
  
  // Read the input points.
  PndMvaDataSet data(InFile, label, vars, TRAIN);

  // Init structures and read.
  data.Initialize();
  
  DataPoints const& samples = data.GetData();

  // Square map
  PndMvaSomTrainer som(&samples, numNodes, numNodes, numSwp,
                       SOM_RAND_FROM_DATA, RECTANGULAR);
  // som.SetNodeInitScheme(RAND_FROM_DATA);
  // som.SetNumEpochs(numSwp);
  som.InitMap();
  som.TrainBatch();
  som.Calibrate();

#if (PRINT_PND_SOM_NODE_DEBUG > 10)
  std::vector<PndSomNode*> const& map = som.GetTheMap();
  
  for(size_t i = 0; i < map.size(); i++) {
    PndSomNode* sn = map[i];
    sn->PrintNode();
  }
  std::cout << " Total number of nodes = " << som.GetNumNodes()
            << " And length is "<< map.size()
            <<'\n';
#endif// PRINT_PND_SOMNODE_DEBUG

#if ( WRITE_SOM_MAP_TO_DISK > 0 )
  std::string OutFile = "TestMapOutput.root";
  std::cout << "<INFO> Writing the map to " << OutFile << '\n';
  std::vector<TNtuple*> ClassTrees;

  for(size_t l = 0; l < label.size(); ++l)
  {
    std::string const& treeName    = label[l];
    std::string branches = vars[0];

    for(size_t br = 1; br < (vars.size() - 1); ++br)
    {
      branches = branches + ":" + vars[br];
    }
    branches = branches + ":" + vars[(vars.size() - 1)];

    TNtuple* t = new TNtuple(treeName.c_str(), "DescRTree", branches.c_str());
    std::vector<PndSomNode*> const& TheMap = som.GetTheMap();
    
    for(size_t nd = 0; nd < TheMap.size(); ++nd)
    {
      PndSomNode const* crNode = TheMap[nd];
      std::string const& NdLabel = crNode->GetLabel();
      
      if( NdLabel == treeName)
      {
        std::vector<float> const& w = crNode->GetWeight();
        t->Fill(w[0], w[1], w[2], w[3], w[4]);
      }
    }
    ClassTrees.push_back(t);
  }

  // Open File
  TFile out(OutFile.c_str(), "RECREATE", "SOM output File", 9);

  // Write trees
  for(size_t t = 0; t < ClassTrees.size(); ++t)
  {
    (ClassTrees[t])->Write();
  }
  out.Close();

  // Clean before quit
  for(size_t t = 0; t < ClassTrees.size(); ++t)
  {
    delete ClassTrees[t];
    ClassTrees[t] = 0;
  }
  ClassTrees.clear();
#endif// WRITE_SOM_MAP_TO_DISK

  return 0;
}
