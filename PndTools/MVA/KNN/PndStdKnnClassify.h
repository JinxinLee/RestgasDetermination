/* ***************************************
 * KNN based classifier.                 *
 * Author: M.Babai@rug.nl                *
 * Edited: E.A.Dijck@student.rug.nl      *
 * Version: 0.1 beta1                    *
 * license:                              *
 * ***************************************
 */
/*
 * Based on the straight KNN algorithm
 * Using a linear search data structure.
 */
#ifndef PND_STD_KNN_CLASSIFY_H
#define PND_STD_KNN_CLASSIFY_H

// Local includes
#include "PndGpidClassifier.h"

/*
 * KNN based classification alg. implementation.
 */
class PndStdKnnClassify : public PndGpidClassifier 
{
 public:
  /*
   * @param inputFile: The name of the file that holds the weights
   * @param classNames: The names of classes to which an event might be
   * assigned to.
   * @param varNames: Variable names from which the feature vector is
   * built.
   */
  PndStdKnnClassify(const std::string& inputFile,
		    const std::vector<std::string>& classNames, 
		    const std::vector<std::string>& varNames);

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
  const std::string& Classify(std::vector<float> EvtData);

  /**
   * @param Neighbours: Number of Neighbours.
   */
  inline  void SetKNN(const unsigned int nNeighbours)
  { m_Knn = nNeighbours; };

 private:
  PndStdKnnClassify(const PndStdKnnClassify& other);
  PndStdKnnClassify& operator=(const PndStdKnnClassify& other);

  //! Number of Neighbours.
  unsigned int m_Knn;
  
  //! Container to hold the distances to current pattern.
  std::vector<PndMvaDistObj> m_distances;
};
#endif //End of PndStdKnnClassify definitions.
