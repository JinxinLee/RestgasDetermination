/* ***************************************
 * MultiClass MLP Classifier functions   *
 * Author: M.Babai@rug.nl                *
 * Version:                              *
 * LICENSE:                              *
 * ***************************************
 */
#ifndef PND_MULTI_CLASS_MLP_CLASSIFY_H
#define PND_MULTI_CLASS_MLP_CLASSIFY_H

//Local includes
#include "PndMvaClassifier.h"

// Root && TMVA
#include "TMVA/Reader.h"

class PndMultiClassMlpClassify : public PndMvaClassifier
{
  //______________ Public ____________________
 public:
  PndMultiClassMlpClassify(std::string const& inputFile,
			   std::vector<std::string> const& classNames, 
			   std::vector<std::string> const& varNames);
  
  ~PndMultiClassMlpClassify();
  
  void GetMvaValues( std::vector<float> EvtData,
		     std::map<std::string, float>& result );
  
  std::string *Classify( std::vector<float> EvtData );
  
  void Initialize();
  //______________ Protected__________________
  //protected:
  //______________ Private ____________________
 private:
  PndMultiClassMlpClassify(PndMultiClassMlpClassify const& oth);
  PndMultiClassMlpClassify& operator=(PndMultiClassMlpClassify const& oth);
  
  void SetOptions();

  //======================
  TMVA::Reader* m_reader;

  std::string m_weightsFile;
  std::string m_readerOptions;
  std::string m_methodName;
  std::vector<float> m_EventContainer;
};
#endif
