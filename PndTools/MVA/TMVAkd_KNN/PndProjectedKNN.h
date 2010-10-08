/* ************************************
 *  Author: M. Babai (M.Babai@rug.nl) *
 *                                    *
 *  pid classifier                    *
 *                                    *
 * Modified:                          *
 *                                    *
 * ************************************/
#ifndef PND_PROJECTED_KNN_H
#define PND_PROJECTED_KNN_H

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

  //! Get pdf's
  void GetMvaValues(std::vector<float> eventData, 
		    std::map<std::string, float>& result);

  //! Classify Current event.
  std::string* Classify(std::vector<float> EvtData);

  //! Set params
  inline void SetEvtParam(const float scFact, const double weight)
  { 
    m_ScaleFact = scFact;
    m_weight = weight;
  };
  
  //! Set number of neighbors
  inline void SetKnn(const unsigned int val)
  { m_knn = val; };

  //! Init classifiers
  void InitKNN();

 private:
  //! to avoid mistakes.
  PndProjectedKNN(const PndProjectedKNN& other);
  PndProjectedKNN& operator= (const PndProjectedKNN& other);
  
  // Free allocated memory.
  void destroy();

  std::vector<std::vector<std::string> > m_varCombinations;
  
  // First = classifier, Second = parameter indices
  std::vector <std::pair<PndKnnClassify*, std::vector<int>*> > m_classifiers;
  
  float  m_ScaleFact;  //! Scale factor.
  double m_weight;    //! Weight.
  unsigned int m_knn;//! Number of neigbours.
};
#endif//End, class interface
