/* ***************************************
 * KNN based classifier.                 *
 * Author: M.Babai@rug.nl                *
 * Edited: E.A.Dijck@student.rug.nl      *
 * Version:                              *
 * license:                              *
 * ***************************************
 */
/*
 * Based on the straight KNN algorithm
 * Using a linear search data structure.
 */
//#pragma once
#ifndef PND_STD_KNN_CLASSIFY_H
#define PND_STD_KNN_CLASSIFY_H

// Local includes
#include "PndMvaClassifier.h"

/*
 * KNN based classification alg. implementation.
 */
class PndStdKnnClassify : public PndMvaClassifier 
{
 public:
  /**
   * @param inputFile: The name of the file that holds the weights
   * @param classNames: The names of classes to which an event might be
   * assigned to.
   * @param varNames: Variable names from which the feature vector is
   * built.
   */
  explicit PndStdKnnClassify(std::string const& inputFile,
			     std::vector<std::string> const& classNames, 
			     std::vector<std::string> const& varNames);

  //! Destructor  
  virtual ~PndStdKnnClassify();

  /**
   * Classification function.
   * @param EvtData: Feature vector of the current event which we want
   * to classify.
   * @param result: Holds the normalized results of classification
   * for every class of events.
   */
  void GetMvaValues(std::vector<float> eventData,
		    std::map<std::string,float>& result);
  /**
   * Given a feature vector describing the pattern. Classifies the pattern.
   *@param EvtData Input vector describing the pattern.
   *@return The name of the class to which the current pattern is assigned.
   */
  std::string* Classify(std::vector<float> EvtData);

  /**
   * @param Neighbours: Number of Neighbours.
   */
  inline void SetKNN( unsigned int const nNeighbours );

  /**
   * Get number of neighbors.
   */
  inline unsigned int GetKnn() const;

  /**
   * Initialize classifier.
   */
  void Initialize();
  
 private:
  //! To avoid mistakes.
  PndStdKnnClassify(const PndStdKnnClassify& other);
  PndStdKnnClassify& operator=(const PndStdKnnClassify& other);
  
  //! Number of Neighbours.
  unsigned int m_Knn;
  
  //! Container to hold the distances to current pattern.
  std::vector<PndMvaDistObj> m_distances;
};//End interface.
// ____________________________ Inlines ________________
inline unsigned int PndStdKnnClassify::GetKnn() const
{
  return m_Knn;
};

inline void PndStdKnnClassify::SetKNN( unsigned int const nNeighbours )
{
  m_Knn = nNeighbours;
};
#endif
