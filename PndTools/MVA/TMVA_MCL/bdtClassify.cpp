#include "PndMultiClassBdtClassify.h"

// Print the results map.
void printResult(std::map<std::string, float> const& res)
{
  std::cout << "\n================================== \n";
  for( std::map<std::string,float>::const_iterator ii=res.begin();
       ii != res.end(); ++ii)
  {
    std::cout << (*ii).first << " => " << (*ii).second << '\n';
  }
  std::cout << "======================================= \n";
}

int main(int argc, char** argv)
{
  if(argc < 2)
  {
    std::cerr << "<USAGE>: " << argv[0]
	      << " <inputFile> \n";
    exit(10);
  }
  
  std::string inFile = argv[1];

  // Labels.
  std::vector<std::string> labels;

  // Variables.
  std::vector<std::string> variables;

  // Class names
  labels.push_back("electron");
  labels.push_back("pion");
  //labels.push_back("kaon");
  //labels.push_back("muon");
  //labels.push_back("proton");
  
  // Variable names 
  variables.push_back("emc");
  variables.push_back("lat");
  variables.push_back("z20");
  variables.push_back("z53");
  
  std::vector<float> event(variables.size(), 1.2);
  std::vector<float> event1(variables.size(), 0.3);
  std::map<std::string, float> result;

  PndMultiClassBdtClassify cls(inFile, labels, variables);
  cls.Initialize();
  cls.GetMvaValues(event, result);
  printResult(result);
  cls.GetMvaValues(event1, result);
  printResult(result);
  return 0;
}
