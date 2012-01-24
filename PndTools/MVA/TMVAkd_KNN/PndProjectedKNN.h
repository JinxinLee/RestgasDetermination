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

class PndProjectedKNN: public PndMvaClassifier
{
 public:
  //! Constructor
  explicit PndProjectedKNN(std::string const& inputFile,
			   std::vector<std::string> const& classNames,
			   std::vector<std::string> const& varNames,
			   std::vector< std::vector<std::string> > const& varCombinations);
  //! Destructor
  virtual ~PndProjectedKNN();

  //! Get pdf's
  void GetMvaValues(std::vector<float> eventData, 
		    std::map<std::string, float>& result);

  //! Classify Current event.
  std::string* Classify(std::vector<float> EvtData);

  //! Set params
  inline void SetEvtParam(float const scFact, double const weight);
  
  //! Set number of neighbors
  inline void SetKnn(unsigned int const val);

  //! Init classifiers
  void InitKNN();

 private:
  //! to avoid mistakes.
  PndProjectedKNN( PndProjectedKNN const& other);
  PndProjectedKNN& operator= ( PndProjectedKNN const& other);
  
  // Free allocated memory.
  void destroy();

  std::vector<std::vector<std::string> > m_varCombinations;
  
  // First = classifier, Second = parameter indices
  std::vector <std::pair<PndKnnClassify*, std::vector<int>*> > m_classifiers;
  
  float  m_ScaleFact;  //! Scale factor.
  double m_weight;    //! Weight.
  unsigned int m_knn;//! Number of neigbours.
};
//End, class interface

inline void PndProjectedKNN::SetEvtParam(float const scFact, double const weight)
{
  m_ScaleFact = scFact;
  m_weight = weight;
};

inline void PndProjectedKNN::SetKnn(unsigned int const val)
{
  m_knn = val;
};
#endif
