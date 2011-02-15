#include "PndMultiClassMlpClassify.h"


PndMultiClassMlpClassify::PndMultiClassMlpClassify(std::string const& inputFile,
						   std::vector<std::string> const& classNames, 
						   std::vector<std::string> const& varNames)
: PndMvaClassifier(inputFile, classNames, varNames)
{}

PndMultiClassMlpClassify::~PndMultiClassMlpClassify()
{}

void PndMultiClassMlpClassify::GetMvaValues( std::vector<float> EvtData,
					     std::map<std::string, float>& result )
{
  std::cout << "Not implemented yet "
	    << EvtData.size() + result.size()
	    <<'\n';
}

std::string* PndMultiClassMlpClassify::Classify( std::vector<float> EvtData )
{
  std::cout << EvtData.size();
  return (new std::string("\n\tNot_Implemented_YET\n\n"));
}
