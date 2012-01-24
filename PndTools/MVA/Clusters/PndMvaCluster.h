/* ***************************************
 * Clustering algorithms                 *
 * Author: M.Babai@rug.nl                *
 * Version:                              *
 * LICENSE:                              *
 * ***************************************
 */
#ifndef PND_MVA_CLUSTER_H
#define PND_MVA_CLUSTER_H

#include <iostream>
#include <cassert>
#include <vector>
#include <set>
#include <limits>

// Local includes
#include "PndMvaUtil.h"

#define PNDMVA_CLUSTER_DEBUG 0

//! Data structure of the space points and the cluster centers
typedef std::vector < std::vector<float>* > ClDataSample;

//! Clustering types
typedef enum
  { KMEANS_HARD = 0,
    KMEANS_SOFT = 1// Not Implemented yet.
  } ClusteringType;

//---------------- Class definition --------------
class PndMvaCluster
{
  //--------------------------------------------
  // -------------- public members -------------
 public:
  /**
   * Constructor.
   *@param InputData Input Data points.
   *@param nCluster  Number of clusters to be created.
   */
  explicit PndMvaCluster( ClDataSample const& InputData, unsigned int nCluster);
  
  /**
   * Destructor.
   */
  virtual ~PndMvaCluster();
  
  /**
   * Compute Cluster centers for the current input data.
   *@param ClType Clustering algorithm.
   *@return Vector containing the cluster centroids.
   */
  ClDataSample* Cluster( ClusteringType const ClType = KMEANS_HARD);
  
  //------- Getters
  /**
   * Get the number of cluster centeroids.
   *@return Number of cluster centeroids.
   */
  inline unsigned int GetNumberOfClusters() const;
  
  /**
   * Get the dimension of the input data points.
   *@return Dimension of the data points.
   */
  inline unsigned int GetClusterDimension() const;
  
  //------- Setters
  /**
   * Set the number of centroids.
   *@param val Number of centeroids.
   */
  inline void SetNumberOfClusters(unsigned int val);
  
  //__________________ DEBUG FUNCTIONS ______________
#if (PNDMVA_CLUSTER_DEBUG > 0)
  /**
   * Print the vectors and centroids and their relation.
   */
  void printStructs();
#endif
  //--------------------------------------------
  //protected:
  // -------------- private members ------------
 private:
  //! To avoid mistakes.
  PndMvaCluster(const PndMvaCluster& other);
  PndMvaCluster& operator=(const PndMvaCluster& other);
  
  // Functions & Procedures
  /// Performs the actual hard K-Means clustering.
  ClDataSample* K_Means();

  /// Initialize the centroids before clustering.
  void InitCentroids();

  /// Partitions the data points among the current cluster centroids.
  void InitialPartition();

  /// Compute (modify) the coordinates of centroids. 
  void ComputeCentroids();

  /// Clear the currently used data structures.
  void ClearStructures();

  /// Init empty Centroid to the furthest point.
  void ReInitEmptyCenter(unsigned int centerIdx);

  // Variables
  unsigned int m_num_Cluster;/// number of cluster centers.
  ClDataSample m_PointSet;/// input data points.
  unsigned int m_dimension;/// Data points dimension.

  /// Container to hold the centroid.
  ClDataSample m_Centroids;

  ///Connection of each point to a centroid.
  std::vector <unsigned int> m_PointsToClusters;

  /// Responsibility list of each centroid.
  std::vector< std::set<unsigned int>* > m_ClustersToPoints;
};

//__________________ Inlines ____________

inline unsigned int PndMvaCluster::GetNumberOfClusters() const
{return m_num_Cluster; };

inline unsigned int PndMvaCluster::GetClusterDimension() const
{ return m_dimension; };

inline void PndMvaCluster::SetNumberOfClusters(unsigned int val)
{ m_num_Cluster = val; };

#endif// End interface
