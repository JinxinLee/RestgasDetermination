//#pragma once
#ifndef PndKnnClassify_H
#define PndKnnClassify_H

// LOCAL includes
#include "PndGpidClassifier.h"

// TMVA
#include "TMVA/NodekNN.h"
#include "TMVA/ModulekNN.h"

/**
 * KNN based classification alg. implementation.
 */
class PndKnnClassify: public PndGpidClassifier
{
 public:
  /**
   * Constructor.
   * @param inputFile: File that holds the weights
   * @param classNames: Class names.
   * @param varNames: Variable names from which the feature vector is
   * built.
   */
  PndKnnClassify(const std::string& inputFile,
		 const std::vector<std::string>& classNames, 
		 const std::vector<std::string>& varNames);
  //! Destructor
  virtual ~PndKnnClassify();
  
  /**
   * Classification function.
   * @param eventData: Feature vector of the current event.
   * @param result: Holds the normalized results of classification
   */
  void GetMvaValues(std::vector<float> eventData, 
		    std::map<std::string, float>& result);

  const std::string& Classify(std::vector<float> EvtData)const;

  /**
   * Set the scalefactor and the event weight for KNN classifier.
   * @param scFact  Scale factor.
   * @param weight  Events weight.
   */
  inline void SetEvtParam(const float scFact, const double weight)
  {m_ScaleFact = scFact; m_weight = weight; };
  
  inline void SetKnn(const int N){m_knn = N;};

  /**
   * Initialize the KNN classifier.
   */
  void InitKNN();
  
  /// DEBUG Produces a lot of output.
  void print(){m_module->Print();}
  /// DEBUG
  
  // ==================  Private ===============
 private:
  //PndKnnClassify();
  PndKnnClassify(const PndKnnClassify& other);
  inline PndKnnClassify& operator=(const PndKnnClassify& other);

  typedef std::list < std::pair<const TMVA::kNN::Node<TMVA::kNN::Event>*, float> > ResList;

  int m_knn;
  float m_ScaleFact;//!< Scalefactor Default =  0.8
  double m_weight;//!< Event weight
  TMVA::kNN::ModulekNN* m_module;

  /**
   * Mapping between class names and their indices in the class vector.
   */
  std::map<std::string, size_t> m_classIndices;
};
#endif// End of classifier interface definition
