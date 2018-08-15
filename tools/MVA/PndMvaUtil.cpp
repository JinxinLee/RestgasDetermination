/* ********************************************
 * MVA Utility functions and data definitions *
 * Author: M.Babai@rug.nl                     *
 * LICENSE:                                   *
 * Version:                                   *
 * License:                                   *
 * *******************************************
 */

#include "PndMvaUtil.h"

/**
 * Computes the Euclidean distance between two given vectors of event
 * features.
 */
float ComputeDist(std::vector<float> const &EvtData, 
		  std::vector<float> const &Example)
{
  assert( Example.size() == EvtData.size() );
  
  float dist = 0.0;
  
  for(size_t i = 0; i< Example.size(); ++i)
  {
    dist += ( (EvtData[i] - Example[i]) * (EvtData[i] - Example[i]) );
  }
  
  return sqrt(dist);
}

int str2int (std::string const& str)
{
  std::stringstream ss(str);
  int n;
  ss >> n;
  return n;
}

unsigned int str2Uint (std::string const& str)
{
  std::stringstream ss(str);
  unsigned int n;
  ss >> n;
  return n;
}

std::string int2str (int n)
{
  std::stringstream ss;
  ss << n;
  return ss.str();
}
