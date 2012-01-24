/* ************************************
 *  Author: M. Babai (M.Babai@rug.nl) *
 *                                    *
 *  KNN based pid classifier          *
 *                                    *
 * Modified:                          *
 *                                    *
 * ************************************/

#ifndef PND_KNN_CLASSIFY_H
#define PND_KNN_CLASSIFY_H

// LOCAL includes
#include "PndMvaClassifier.h"

// TMVA
#include "TMVA/NodekNN.h"
#include "TMVA/ModulekNN.h"

//____________________________________________
/// !< Type definition of the neighbour list.
// typedef std::list < std::pair<const TMVA::kNN::Node<TMVA::kNN::Event>*, float> > ResList;
//____________________________________________

/**
 * KNN based classification alg. implementation.
 */
class PndKnnClassify: public PndMvaClassifier
{
 public:
  /**
   * Constructor.
   * @param inputFile: File that holds the weights
   * @param classNames: Class names.
   * @param varNames: Variable names from which the feature vector is
   * built.
   */
  explicit PndKnnClassify(std::string const& inputFile,
			  std::vector<std::string> const& classNames, 
			  std::vector<std::string> const& varNames);
  //! Destructor
  virtual ~PndKnnClassify();
  
  /**
   * Classification function.
   * @param eventData: Feature vector of the current event.
   * @param result: Holds the normalized results of classification
   */
  void GetMvaValues(std::vector<float> eventData, 
		    std::map<std::string, float>& result);
  /**
   * Given a feature vector describing the pattern. Classifies the pattern.
   *@param EvtData Input vector describing the pattern.
   *@return The name of the class to which the current pattern is assigned.
   */
  std::string* Classify(std::vector<float> EvtData);

  /**
   * Set the scalefactor and the event weight for KNN classifier.
   * @param scFact  Scale factor.
   * @param weight  Events weight.
   */
  inline void SetEvtParam(float const scFact, double const weight);

  //! Set the number of neighbours.
  inline void SetKnn(size_t const N);
  
  //! Get the number of neighbours.
  inline size_t GetKnn();

  /**
   * Initialize the needed internal and external data structures.
   */
  virtual void Initialize();

  /// DEBUG Produces a lot of output.
  void print(){m_module->Print();}
  /// DEBUG
  
  // ==================  Private ===============
 private:
  // To avoid mistakes. ;)
  PndKnnClassify(PndKnnClassify const& other);
  PndKnnClassify& operator=(PndKnnClassify const& other);

  /**
   * Initialize the KNN classifier.
   */
  void InitKNN();
  
  //!< Type definition for the neighbours list.
  typedef std::list < std::pair<const TMVA::kNN::Node<TMVA::kNN::Event>*, float> > ResList;
  
  size_t m_knn;//!< Number of required neighbours.
  float m_ScaleFact;//!< Scalefactor Default =  0.8
  double m_weight;//!< Event weight
  TMVA::kNN::ModulekNN* m_module;// TMVA Knn module.

  /**
   * Mapping between class names and their indices in the class vector.
   */
  std::map<std::string, size_t> m_classIndices;
};//End of classifier interface definition

//___________________ Inline implementation. __________________________________
inline void PndKnnClassify::SetEvtParam(float const scFact, double const weight)
{
  m_ScaleFact = scFact;
  m_weight = weight;
};

inline void PndKnnClassify::SetKnn(size_t const N)
{
  m_knn = N;
};

inline size_t PndKnnClassify::GetKnn()
{ return m_knn; };
#endif
