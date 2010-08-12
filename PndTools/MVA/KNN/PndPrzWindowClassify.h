/* ***************************************
 * Parzen Window based classifier.       *
 * Author: M.Babai@rug.nl                *
 * Version: 0.1 beta1                    *
 * license:                              *
 * ***************************************
 */
/*
 * Based on the straight parzen window algorithm Using a linear search
 * data structure.
 */
#ifndef PND_PRZ_WINDOW_CLASSIFY_H
#define PND_PRZ_WINDOW_CLASSIFY_H

// Local includes
#include "PndGpidClassifier.h"

/*
 * Parzen Window based classification alg. implementation.
 */
class PndPrzWindowClassify : public PndGpidClassifier 
{
 public:
  /*
   * @param inputFile: The name of the file that holds the weights
   * @param classNames: The names of classes to which an event might be
   * assigned to.
   * @param varNames: Variable names from which the feature vector is
   * built.
   */
  PndPrzWindowClassify(const std::string& inputFile,
		       const std::vector<std::string>& classNames, 
		       const std::vector<std::string>& varNames);

  //! Destructor  
  virtual ~PndPrzWindowClassify();

  /**
   * Classification function.
   * @param EvtData: Feature vector of the current event which we want
   * to classify.
   * @param result: Holds the normalized results of classification
   * for every class of events. Note: this parameter will be
   * cleaned and modified.
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
   * Set the window size. Equal size in every dimension.
   *@param wsize Window (Bin) size for all dimensions.
   */
  void setWindowSize(float wsize);

  /**
   * Set the window size. Specify the size for every dimension.
   *@param wsize Window (Bin) size for each dimensions.<dim name, size>
   */
  inline void setWindowSize(const std::map<std::string, float>& wsize);

  /**
   * Get hypercube volume
   */
  inline float GetHyperCubeVolume() const;

  /**
   * Get window sizes for all dimensions.
   */
  inline const std::map<std::string, float>& GetWindowSize() const;

 private:
  // To avoid mistakes.
  PndPrzWindowClassify(const PndPrzWindowClassify& other);
  PndPrzWindowClassify& operator=(const PndPrzWindowClassify& other);

  /**
   * A very simple implementation of a kernel function. It can be
   * considerd as a multi-dimensional histogram with constant bin size
   * in each dimension.
   *@param evtDat Vector containing event data.
   *@return 1 if the current test sample inside the window, else 0;
   */
  float histKernel(const std::vector<float>& evtDat, const std::vector<float>& trSample);

  //! Compute the hypercube volume
  float CompHyperCubeVolume();

  //! Hypercube volume
  float m_volumeN;

  //! Window size in each dimension
  std::map<std::string, float> m_Wsize;
};

// ================================================================
inline void PndPrzWindowClassify::setWindowSize(const std::map<std::string, float>& wsize)
{
  m_Wsize = std::map<std::string, float>(wsize);
  // Set hypercube volume.
  m_volumeN = CompHyperCubeVolume();
};

inline float PndPrzWindowClassify::GetHyperCubeVolume() const
{
  return m_volumeN;
};

inline const std::map<std::string, float>& PndPrzWindowClassify::GetWindowSize() const
{
  return *(new std::map<std::string, float>(m_Wsize));
};
#endif //End of PndPrzWindowClassify interface.
