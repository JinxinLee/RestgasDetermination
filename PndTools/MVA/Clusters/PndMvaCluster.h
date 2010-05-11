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
#include <vector>
#include <set>
#include <limits>

// Local includes
#include "PndMvaUtil.h"

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
  PndMvaCluster(const PndMvaCluster& other);
  PndMvaCluster& operator=(const PndMvaCluster& other);
  
  // Functions & Procedures
  void InitCentroids();
  void InitialPartition();
  void ComputeCentroids();

  // Variables
  unsigned int m_num_Cluster;
  ClDataSample m_PointSet;
  unsigned int m_dimension;
  ClDataSample m_Centroids;
  
  std::vector<unsigned int> m_PointsToClusters;
  std::vector< std::set<unsigned int>* > m_ClustersToPoints;
};
#endif
