/* ***************************************
 * Clustering algorithms                 *
 * Author: M.Babai@rug.nl                *
 * Version:                              *
 * LICENSE:                              *
 * ***************************************
 */
#pragma once
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
typedef std::vector< std::pair<std::string, std::vector<float>*> > DataPoints;

//! Clustering types
typedef enum ClusteringType{
  KMEANS_HARD = 0,
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
  explicit PndMvaCluster( DataPoints const& InputData, size_t nCluster);

  /**
   * Constructor.
   *@param InputData Input Data points.
   *@param nCluster  Number of clusters to be created.
   *@param prune  Prune the created cluster.
   *@param forceLabels Force to label the undecided and unknown points.
   */
  explicit PndMvaCluster( DataPoints const& InputData, size_t nCluster,
                          bool const prune, bool const forceLabels);

  /**
   * Destructor.
   */
  virtual ~PndMvaCluster();
  
  /**
   * Compute Cluster centers for the current input data.
   *@param ClType Clustering algorithm.
   *@return Vector containing the cluster centroids.
   */
  virtual DataPoints* Cluster( ClusteringType const ClType = KMEANS_HARD);

  /**
   * Compute Cluster centers for the current input data. The label of
   * the majority of their members determines the label of the center.
   *@param ClType Clustering algorithm (default is KMEANS_HARD).
   *@param labels The labes of the currently available classes.
   *@return Vector containing the cluster centroids.
   */
  virtual DataPoints* ClusterAndLabel( ClusteringType const ClType,
                                       std::vector<std::string> const& labels);

  //------- Getters
  /**
   * Get the number of cluster centeroids.
   *@return Number of cluster centeroids.
   */
  inline size_t GetNumberOfClusters() const;
  
  /**
   * Get the dimension of the input data points.
   *@return Dimension of the data points.
   */
  inline size_t GetClusterDimension() const;
  
  //------- Setters
  /**
   * Set the number of centroids.
   *@param val Number of centeroids.
   */
  inline void SetNumberOfClusters(size_t val);

  /**
   * Set if the created cluster list needs to be pruned. Remove the
   * unknown and undecided labels.
   *@param prune If prune after clustering.
   */
  inline void Setprune (bool const prune = false);
  
  /**
   * The centroids that do not have a label (unknown, undecided) will
   * get the label of the nearest data point from the input data set.
   *@param forceLabel Force the labeling even if the determined label
   *is unknown or undecided.
   */
  inline void SetForceToLabel (bool const forceLabel = false);
  
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
  DataPoints* K_Means();

  /// Initialize the centroids before clustering.
  void InitCentroids();

  /// Partitions the data points among the current cluster centroids.
  void InitialPartition();

  /// Compute (modify) the coordinates of centroids. 
  void ComputeCentroids();

  /// Clear the currently used data structures.
  void ClearStructures();

  /// Init empty Centroid to the furthest point.
  void ReInitEmptyCenter(size_t centerIdx);

  // Variables
  size_t m_num_Cluster;/// number of cluster centers.
  DataPoints m_PointSet;/// input data points.
  size_t m_dimension;/// Data points dimension.

  /// Container to hold the centroid.
  DataPoints m_Centroids;

  ///Connection of each point to a centroid.
  std::vector <size_t> m_PointsToClusters;

  /// Responsibility list of each centroid.
  std::vector< std::set<size_t>* > m_ClustersToPoints;
  
  bool m_prune;//! If prune the current cluster.
  bool m_forceToLabel;// Force to label the current mean
};

//__________________ Inlines ____________

inline size_t PndMvaCluster::GetNumberOfClusters() const
{
  return m_num_Cluster;
};

inline size_t PndMvaCluster::GetClusterDimension() const
{
  return m_dimension;
};

inline void PndMvaCluster::SetNumberOfClusters(size_t val)
{
  m_num_Cluster = val;
};

inline void PndMvaCluster::Setprune (bool const prune)
{
  m_prune = prune;
};
inline void PndMvaCluster::SetForceToLabel (bool const label)
{
  m_forceToLabel = label;
};
#endif// End interface
