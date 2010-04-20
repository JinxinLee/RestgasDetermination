/* ********************************************
 * MVA Utility functions and data definitions *
 * Author: M.Babai@rug.nl                     *
 * LICENSE:                                   *
 * Version: 0.1 beta1.                        *
 * License:                                   *
 * *******************************************
 */

#include "PndMvaUtil.h"

/**
 * Computes the Euclidean distance between two given vectors of
 * event features.
 */
float ComputeDist(const std::vector<float> &EvtData, 
		  const std::vector<float> &Example)
{
  assert(Example.size() == EvtData.size());
  
  float dist = 0.0;
  
  for(unsigned int i = 0; i< Example.size(); i++)
  {
    dist += (EvtData[i] - Example[i]) * (EvtData[i] - Example[i]);
  }
  return sqrt(dist);
}
