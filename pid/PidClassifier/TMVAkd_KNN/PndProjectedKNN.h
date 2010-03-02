/* ************************************
 *  Author: M. Babai (M.Babai@rug.nl) *
 *                                    *
 *  pid classifier                    *
 *                                    *
 * Modified:                          *
 *                                    *
 * ************************************/
#ifndef PNDPROJECTEDKNN_H
#define PNDPROJECTEDKNN_H

// Local Headers
#include "PndKnnClassify.h"

class PndProjectedKNN: public PndGpidClassifier
{
 public:
  //! Constructor
  PndProjectedKNN(const std::string& inputFile,
		  const std::vector<std::string>& classNames,
		  const std::vector<std::string>& varNames,
		  const std::vector< std::vector<std::string> >& varCombinations);
  //! Destructor
  virtual ~PndProjectedKNN();

  //! Classify
  void GetMvaValues(std::vector<float> eventData,
		    std::map<std::string, float>& result);

  const std::string& Classify(std::vector<float> EvtData)const;

  //! Set params
  inline void SetEvtParam(const float scFact, const double weight)
  {m_ScaleFact = scFact; m_weight = weight; };
  
  //! Set number of neighbors
  inline void SetKnn(const unsigned int N)
  {m_knn = N;};

  //! Init classifiers
  void InitKNN();

 private:
  PndProjectedKNN(const PndProjectedKNN& other);
  inline PndProjectedKNN& operator= (const PndProjectedKNN& other);
  
  void destroy();

  std::vector<std::vector<std::string> > m_varCombinations;
  
  // First = classifier, Second = parameter indices
  std::vector <std::pair<PndKnnClassify*, std::vector<int>*> > m_classifiers;
  
  float  m_ScaleFact;
  double m_weight;
  unsigned int m_knn;
};
#endif//End, class interface
