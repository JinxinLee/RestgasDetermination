/* ***************************************
 * Clustering algorithms                 *
 * Author: M.Babai@rug.nl                *
 * Version:                              *
 * LICENSE:                              *
 * ***************************************
 */
#ifndef PNDMVACLUSTER_H
#define PNDMVACLUSTER_H

#include <iostream>
#include <string>
#include <cassert>
#include <ctime>
#include <vector>
#include <set>
#include <limits>

// Local includes
#include "PndMvaUtil.h"

// ROOT
#include "TRandom3.h"

typedef std::vector<std::vector<float>*> ClDataSample;

class PndMvaCluster
{
  // -------------- public members ---------
 public:
  PndMvaCluster(const ClDataSample& InputData, unsigned int nCluster);
  
  virtual ~PndMvaCluster();
  
  ClDataSample& K_Means();
  
  //------- Getters
  inline unsigned int GetNumberOfClusters()
  { return m_num_Cluster; };
  
  inline unsigned int GetClusterDimension()
  { return m_dimension; };
  
  //------- Setters
  inline void SetNumberOfClusters(unsigned int val)
  { m_num_Cluster = val; };
  
  // DEBUG FUNCTIONS Maybe Removed.
  void printStructs();

  // -------------- protected members --------
  //protected:
  
  // -------------- private members   ---------
 private:
  // To avoid mistakes.
  PndMvaCluster(const PndMvaCluster& oth);
  PndMvaCluster& operator=(const PndMvaCluster& oth)const;
  
  // Functions & Procedures
  void InitCentroids();
  void InitialPartition();

  // Variables
  unsigned int m_num_Cluster;
  unsigned int m_dimension;
  TRandom3 m_rnd;
  ClDataSample m_PointSet;
  ClDataSample m_Centroids;
  
  std::vector<unsigned int> m_PointsToClusters;
  std::vector< std::set<unsigned int>* > m_ClustersToPoints;
};
#endif
