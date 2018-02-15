/* ***************************************
 * PDEFoam  Classifier                   *
 * Author: M.Babai@rug.nl                *
 * LICENSE:                              *
 * Version: 0.1 beta1.                   *
 * License:                              *
 * ***************************************
 */
#pragma once
#ifndef PND_PDE_FOAMCLASSIFY_H
#define PND_PDE_FOAMCLASSIFY_H

// Local includes
#include "PndGpidClassifier.h"

//ROOT AND PANDA
#include "TMVA/PDEFoam.h"
#include "TMVA/Event.h"

class PndPDEFoamClassify//: public PndGpidClassifier
{
 public:
  PndPDEFoamClassify(const std::string& inputFile,
		     const std::vector<std::string>& classNames, 
		     const std::vector<std::string>& varNames);
  virtual ~PndPDEFoamClassify();
  
  void GetMvaValues(std::vector<float> eventData, 
		    std::map<std::string, float>& result);
  
  const std::string& Classify(std::vector<float> EvtData)const;

 protected:
  void ReadInputFile();

 private:
  //Functions
  PndPDEFoamClassify(const PndPDEFoamClassify& other);
  PndPDEFoamClassify& operator=(const PndPDEFoamClassify& other);

  // Variables.
  // Comment:
  /*
   * Mabe wee need to inherit from classifiers in order to avoid
   * double implementation. For the time being, lets use this.
   */
  std::string m_inputFile;
  std::vector<TMVA::PDEFoam*> m_foams;
  std::vector<PndMvaVariable> m_vars;
  std::vector<PndMvaClass> m_classes;
};
#endif// end of interface definition
