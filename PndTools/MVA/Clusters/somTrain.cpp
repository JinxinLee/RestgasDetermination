#include <iostream>

// Local includes
#include "PndMvaSOMTrainer.h"
#include "PndMvaDataSet.h"
#include "PndMvaUtil.h"
#include "PndSomNode.h"

int main(int argc, char** argv)
{
  if(argc < 4)
  {
    std::cerr << "\t<ERROR> Usage:\n"
              <<'\t' << argv[0]
	      << " <Numer of model vectors>"
	      << " <Numer of Sweeps>"
	      <<" <InputTrainEventFeatureFile>"
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
  std::string InFile(argv[3]);

  std::cout << "<INFO> Using input data from file " << InFile
	    <<"\n\t number of map nodes = " << numNodes
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
  
  // Read the input points.
  PndMvaDataSet data(InFile, label, vars, TRAIN);

  // Init structures and read.
  data.Initialize();
  
  DataPoints const& samples = data.GetData();

  // Square map
  PndMvaSomTrainer som(&samples, numNodes, numNodes, numSwp, RAND_FROM_DATA, RECTANGULAR);
  
  /*
  som.SetNodeInitScheme(RAND_FROM_DATA);
  som.SetNumEpochs(numSwp);
  */
  
  std::cout << "numSwp = " << numSwp
            << std::endl;
  som.InitMap();
  som.TrainBatch();
  
#if (PRINT_SOMNODE_DEBUG > 0)
  std::vector<PndSomNode*> const& map = som.GetTheMap();
  
  for(size_t i = 0; i < map.size(); i++) {
    PndSomNode* sn = map[i];
    sn->PrintNode();
  }
  
  std::cout << " Total number of nodes = " << som.GetNumNodes()
            << " And length is "<< map.size()
            <<'\n';
#endif

  // Clean before quit
  return 0;
}
