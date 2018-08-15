/* ***************************************
 * MultiClass MLP Classifier functions   *
 * Author: M.Babai@rug.nl                *
 * Version:                              *
 * LICENSE:                              *
 * ***************************************
 */
#include "PndMultiClassBdtClassify.h"


PndMultiClassBdtClassify::PndMultiClassBdtClassify(std::string const& inputFile,
						   std::vector<std::string> const& classNames, 
						   std::vector<std::string> const& varNames)
  : PndMvaClassifier(inputFile, classNames, varNames),
    m_reader(0),
    m_weightsFile(inputFile),
    m_readerOptions("!Color:!Silent")
{}

PndMultiClassBdtClassify::~PndMultiClassBdtClassify()
{
  if(m_reader)
  {
    delete m_reader;
    m_reader = 0;
  }
}

void PndMultiClassBdtClassify::GetMvaValues( std::vector<float> EvtData,
					     std::map<std::string, float>& result )
{
  m_EventContainer = EvtData;
  std::vector<float> const& outputs = m_reader->EvaluateMulticlass( "BDT method" );
  
  std::vector<PndMvaClass> const& label = m_dataSets.GetClasses();

  for(size_t cls = 0; cls < outputs.size(); ++cls)
  {
    result[ label[cls].Name ] = outputs[cls];
  }
}

std::string* PndMultiClassBdtClassify::Classify( std::vector<float> EvtData )
{
  // The map to store the results.
  std::map<std::string, float> result;

  // Get the MVA values.
  GetMvaValues(EvtData, result);
  
  // Fetch the labels.
  std::vector<PndMvaClass> const& labels = m_dataSets.GetClasses();
  
  float winner = std::numeric_limits<float>::min();
  std::string winLabel;

  // Loop labels
  for(size_t cls = 0; cls < labels.size(); ++cls)
  {
    if (winner < result[ labels[cls].Name ])
    {
      winLabel = labels[cls].Name;
      winner   = result[ labels[cls].Name ];
    }
  }
  return (new std::string(winLabel));
}

void PndMultiClassBdtClassify::Initialize()
{
  PndMvaClassifier::SetAppType(TMVACLS);
  PndMvaClassifier::Initialize();
  
  m_reader = new TMVA::Reader(m_readerOptions.c_str());
  std::vector<PndMvaVariable> const& variables = m_dataSets.GetVars();
  m_EventContainer = std::vector<float>(variables.size(), 0.00);
  
  for(size_t i = 0; i < variables.size(); ++i)
  {
    m_reader->AddVariable( (variables[i].Name).c_str(), &m_EventContainer[i] );
  }

  m_reader->BookMVA("BDT method", m_weightsFile.c_str());
}
void PndMultiClassBdtClassify::SetOptions()
{
}
