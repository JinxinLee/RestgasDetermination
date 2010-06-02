/* ***************************************
 * Clustering algorithms                 *
 * Author: M.Babai@rug.nl                *
 * Version:                              *
 * LICENSE:                              *
 * ***************************************
 */

#include "PndMvaCluster.h"

/**
 * Constructor.
 *@param InputData Input Data points.
 *@param nCluster  Number of clusters to be created.
 */
PndMvaCluster::PndMvaCluster(const ClDataSample& InputData, unsigned int nCluster)
  : m_num_Cluster(nCluster), 
    m_PointSet(InputData),
    m_dimension( InputData.size() == 0 ? 0 : (m_PointSet[0])->size() ),
    m_PointsToClusters( std::vector<unsigned int>(m_PointSet.size(), 0) )
{}

/**
 * Destructor.
 */
PndMvaCluster::~PndMvaCluster()
{
  ClearStructures();
}

/*
 * Clear the currently used data structures.
 */
void PndMvaCluster::ClearStructures()
{
  for(size_t ctr = 0; ctr < m_Centroids.size(); ctr++)
  {
    delete m_Centroids[ctr];
  }
  m_Centroids.clear();
    
  for(size_t i = 0; i < m_ClustersToPoints.size(); i++)
  {
    delete m_ClustersToPoints[i];
  }
  m_ClustersToPoints.clear();
}

/**
 * Compute Cluster centers for the current input data.
 *@param ClType Clustering algorithm.
 *@return Vector containing the cluster centroids.
 */
ClDataSample& PndMvaCluster::Cluster(ClusteringType ClType)
{
  // Zero dimension makes no sense.
  assert(m_dimension != 0);
  // More clusters than data points?
  assert(!(m_PointSet.size() < m_num_Cluster));
  // Zero clusters means do nothing
  assert(m_num_Cluster != 0);
  
  switch(ClType)
  {
  case KMEANS_SOFT://Returns empty centroids.
    std::cerr << "<INFO> Soft clustering." << std::endl;
    std::cerr << "<ERROR> Not implemented yet." << std::endl;
    return *( new ClDataSample() );
    break;
  default://Hard k_means clustering
    std::cerr << "<INFO> Hard K_Means clustering." << std::endl;
    return K_Means();
    break;
  }
}

/*
 * Performs the actual hard K-Means clustering.
 */
ClDataSample& PndMvaCluster::K_Means()
{
  bool some_point_is_moving = true;
  bool move;
  unsigned int num_iter = 0;
  
  // Init data structures.
  InitCentroids();
  InitialPartition();
  
  //====== Perform clustering =======
  while (some_point_is_moving)
  {
    //float distSum = 0.0;
    some_point_is_moving = false;
    ComputeCentroids();

    float minDist, currDist;
    minDist = currDist = 0.0;
    unsigned int to_cluster = 0;

    //Loop through the points
    for(size_t pt = 0; pt < m_PointSet.size(); pt++)
    {
      minDist = ComputeDist( *(m_PointSet[pt]), *(m_Centroids[m_PointsToClusters[pt]]));
      move = false;
    
      // Centroids loop
      for(size_t ctr = 0; ctr < m_num_Cluster; ctr++)
      {
	currDist = ComputeDist( *(m_PointSet[pt]), *(m_Centroids[ctr]));
	
	if( (currDist < minDist) && !(m_PointsToClusters[pt] == ctr) )
	{
	  minDist = currDist;
	  move = true;
	  to_cluster = ctr;
	  
	  // Remove from current cluster
	  (m_ClustersToPoints[m_PointsToClusters[pt]])->erase(pt);
	  some_point_is_moving = true;

	  // Stronger property. Minimizing the total distance.
	  //distSum += minDist;
	}
      }
      // move towards a closer centroid
      if (move)
      {
	m_PointsToClusters[pt] = to_cluster;
	(m_ClustersToPoints[to_cluster])->insert(pt);
      }
    }
    num_iter++;

    // ================ Check empty clusters =======
    // FIXME FIXME. For now we do nothing but maybe better to do
    //'singleton' Create a new cluster consisting of the one point
    //furthest from its centroid.
    /*
      for(size_t i = 0; i < m_ClustersToPoints.size(); i++)
      {
      if( (m_ClustersToPoints[i])->size() == 0)
      {
      //ReInitEmptyCenter(i);
      std::cout << "Empty cluster index = " << i << std::endl;
      }
      }
    */
    //std::cout << "Iter num " << num_iter << " Tot Sum " << distSum << std::endl;
  }//END OF while (some_point_is_moving)
  
  std::cout << "<-I-> Num Iterations " << num_iter 
	    << std::endl;  
  
  //======================================
  // Copy centroid to the output structure
  ClDataSample* Cl_Out = new ClDataSample();
  for(size_t ctr = 0; ctr < m_num_Cluster; ctr++)
  {
    std::vector<float>* ct = new std::vector<float>( *(m_Centroids[ctr]) );
    Cl_Out->push_back(ct);
  }
  return (*Cl_Out);
}

/*
 * Compute (modify) the coordinates of centroids.
 */
void PndMvaCluster::ComputeCentroids()
{
  // Centeroids loop
  for(size_t ct = 0; ct < m_num_Cluster; ct++)
  {
    // Current cluster centroid
    std::vector<float>* curCt = m_Centroids[ct];
    
    // Points in the current cluster
    std::set<unsigned int>* ClusterPoints = m_ClustersToPoints[ct];
    
    // Loop through the points and update the cluster center
    for(std::set<unsigned int>::const_iterator iter = ClusterPoints->begin();
	iter != ClusterPoints->end(); iter++)
    {
      unsigned int pt = *iter;
      std::vector<float>* curPt = m_PointSet[pt];
      
      for(size_t idx = 0; idx < m_dimension; idx++)
      {
	curCt->at(idx) += curPt->at(idx);
      }
    }
    // If no points in cluster It will Go to inf. Correct this.
    if(ClusterPoints->size() != 0)
    {
      for(size_t dim = 0; dim < m_dimension; dim++)
      {
	curCt->at(dim) /= ClusterPoints->size();
      }
    }
  }
}

/*
 * Initialize the centroids before clustering.
 */
void PndMvaCluster::InitCentroids()
{
  // Clear previous initializations.
  ClearStructures();
  
  // Init Centroids container;
  for(unsigned int i = 0; i < m_num_Cluster; i++)
  {
    std::vector<float>* ctr = new std::vector<float>(m_dimension, 0.0);
    m_Centroids.push_back(ctr);
  }
  
  for(unsigned int cl = 0; cl < m_num_Cluster; cl++)
  {
    std::set<unsigned int>* ptSet = new std::set<unsigned int>();
    m_ClustersToPoints.push_back(ptSet);
  }
}

/*
 * Partitions the data points among the current cluster centroids.
 */
void PndMvaCluster::InitialPartition()
{
  // Data points loop
  for(size_t pt = 0; pt < m_PointSet.size(); pt++)
  {
    unsigned int cluster_idx = (pt % m_num_Cluster);
    // Add point to cluster and cluster to point
    m_PointsToClusters[pt] = cluster_idx;
    (m_ClustersToPoints[cluster_idx])->insert(pt);
  }
}

/* 
 * Init empty Centroid to the furthest point of its center.
 *@param centerIdx Index of the centroid with zero responsibility.
 */
void PndMvaCluster::ReInitEmptyCenter(unsigned int centerIdx)
{
  float maxDist = std::numeric_limits<float>::min();
  float currDist = 0.0;
  unsigned int point_Idx = 0;
  // Find the point with the largest dist to its centeroid.
  for(size_t i = 0; i < m_PointsToClusters.size(); i++)
  {
    currDist = ComputeDist(*(m_PointSet[i]), *(m_Centroids[m_PointsToClusters[i]]));
    if(currDist > maxDist)
    {
      maxDist = currDist;
      point_Idx = i;
    }
  }
  // Find the responsible cluster and delete index.
  for(size_t cl = 0; cl < m_ClustersToPoints.size(); cl++)
  {
    (m_ClustersToPoints[cl])->erase(point_Idx);
  }
  // Copy point to cluster.
  for(size_t dim = 0; dim < m_dimension; dim++)
  {
    (m_Centroids[centerIdx])->at(dim) = (m_PointSet[point_Idx])->at(dim);
  }
  // Add point to cluster and cluster to point & init cluster
  m_ClustersToPoints[centerIdx]->insert(point_Idx);
  m_PointsToClusters[point_Idx] = centerIdx;
}

// DEBUG FUNCTIONS Maybe Removed.
/**
 * Print the vectors and centroids and their relation.
 */
void PndMvaCluster::printStructs()
{
  std::cerr << "Printing input points." << std::endl;
  for(size_t pt = 0; pt < m_PointSet.size(); pt++)
  {
    std::vector<float>* currPt = m_PointSet[pt];
    for(size_t i = 0; i < currPt->size(); i++)
    {
      std::cerr << currPt->at(i) << " ";
    }
    std::cerr << std::endl;
  }
  
  std::cerr << "Printing centroids." << std::endl;
  for(size_t ctr = 0; ctr < m_Centroids.size(); ctr++)
  {
    std::vector<float>* currCtr = m_Centroids[ctr];
    for(size_t i = 0; i < currCtr->size(); i++)
    {
      std::cerr << currCtr->at(i) << " ";
    }
    std::cerr << std::endl;
  }
  
  std::cerr << "Printing point to centroid." << std::endl;
  for(size_t j = 0; j < m_PointsToClusters.size(); j++)
  {
    std::cerr << j << " -> " << m_PointsToClusters[j] << std::endl;
  }
  
  std::cerr << "Printing centroid to point." << std::endl;
  std::set<unsigned int>::const_iterator iter;
  for(size_t k = 0; k < m_ClustersToPoints.size(); k++)
  {
    std::cerr << " CTR" << k << " -> ";
    std::set<unsigned int>* curSet = m_ClustersToPoints[k];
    for( iter = curSet->begin(); iter != curSet->end(); iter++)
    {
      std::cerr << *iter << ' ';
    }
    std::cerr << std::endl;
  }
}
