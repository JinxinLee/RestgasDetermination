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

  //______________ Protected__________________
 protected:
  //______________ Private ____________________
 private:
  PndMultiClassMlpClassify(PndMultiClassMlpClassify const& oth);
  PndMultiClassMlpClassify& operator=(PndMultiClassMlpClassify const& oth);
};
#endif
