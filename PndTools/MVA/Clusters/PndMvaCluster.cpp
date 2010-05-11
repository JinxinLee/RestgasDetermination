/* ***************************************
 * Clustering algorithms                 *
 * Author: M.Babai@rug.nl                *
 * Version:                              *
 * LICENSE:                              *
 * ***************************************
 */
#define DEBUG 0

#include "PndMvaCluster.h"

PndMvaCluster::PndMvaCluster(const ClDataSample& InputData, unsigned int nCluster)
  : m_num_Cluster(nCluster), 
    m_PointSet(InputData),
    m_dimension( InputData.size() == 0 ? 0 : (m_PointSet[0])->size() ),
    m_PointsToClusters( std::vector<unsigned int>(m_PointSet.size(), 0) )
{}

PndMvaCluster::~PndMvaCluster()
{
  for(size_t ctr = 0; ctr < m_Centroids.size(); ctr++)
  {
    delete m_Centroids[ctr];
  }
  m_Centroids.clear();

  m_PointsToClusters.clear();

  for(size_t i = 0; i < m_ClustersToPoints.size(); i++)
  {
    delete m_ClustersToPoints[i];
  }
  m_ClustersToPoints.clear();
}

ClDataSample& PndMvaCluster::Cluster(ClusteringType ClType)
{
  ClDataSample* out = new ClDataSample();
  switch(ClType)
  {
  case KMEANS_HARD:
    std::cout << "Hard clustering" << std::endl;
    break;
  case KMEANS_SOFT:
    std::cout << "Soft clustering" << std::endl;
    break;
  default:
    std::cout << "No clustering" << std::endl;
    break;
  }
  return (*out);
}

ClDataSample& PndMvaCluster::K_Means()
{
  bool some_point_is_moving = true;
  bool move;
  unsigned int num_iter = 0;


  // Number of clusters.
  assert(m_num_Cluster != 0);
  InitCentroids();
  InitialPartition();
  //====== Perform clustering =======
  while (some_point_is_moving){
#if DEBUG
    std::cout << "<-I-> Num Iterations " << num_iter 
	      << std::endl;
#endif
    some_point_is_moving = false;
    ComputeCentroids();
    float minDist, currDist = 0.0;
    unsigned int to_cluster = 0;
    //Loop through the points
    for(size_t pt = 0; pt < m_PointSet.size(); pt++){
      minDist = ComputeDist( *(m_PointSet[pt]), *(m_Centroids[m_PointsToClusters[pt]]));
      move = false;
      // Centroids loop
      for(size_t ctr = 0; ctr < m_Centroids.size(); ctr++){
	currDist = ComputeDist( *(m_PointSet[pt]), *(m_Centroids[ctr]));
	if( currDist < minDist ){
	  minDist = currDist;
	  move = true;
	  to_cluster = ctr;
	  // Remove from current cluster
	  (m_ClustersToPoints[m_PointsToClusters[pt]])->erase(pt);
	  some_point_is_moving = true;
	}
      }
      // move towards a closer centroid
      if (move){
	m_PointsToClusters[pt] = to_cluster;
	(m_ClustersToPoints[to_cluster])->insert(pt);
      }
    }
    num_iter++;
  }//while (some_point_is_moving)
  //===========
  // Copy centroid to the output structure
  ClDataSample* Cl_Out = new ClDataSample();
  for(size_t ctr = 0; ctr < m_Centroids.size(); ctr++)
  {
    std::vector<float>* ct = new std::vector<float>( *(m_Centroids[ctr]) );
    Cl_Out->push_back(ct);
  }
  return (*Cl_Out);
}

void PndMvaCluster::ComputeCentroids()
{
  // Centeroids loop
  for(size_t ct = 0; ct < m_Centroids.size(); ct++)
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
      for(size_t idx = 0; idx < curPt->size(); idx++)
      {
	curCt->at(idx) += curPt->at(idx);
      }
    }
    // If no points in cluster It will Go to inf. Correct this.
    // FIXME FIXME. For now we do nothing but maybe better to do
    //'singleton' Create a new cluster consisting of the one point
    //furthest from its centroid.
    if(ClusterPoints->size() != 0){
      for(size_t dim = 0; dim < curCt->size(); dim++)
      {
	curCt->at(dim) /= ClusterPoints->size();
      }
    }
  }
}

void PndMvaCluster::InitCentroids()
{
  // Init Centroids container;
  for(unsigned int i = 0; i < m_num_Cluster; i++)
  {
    std::vector<float>* ctr = new std::vector<float>();
    for(unsigned int j = 0; j < m_dimension; j++)
    {
      ctr->push_back(0.0);
    }
    m_Centroids.push_back(ctr);
  }
  
  for(unsigned int cl = 0; cl < m_num_Cluster; cl++)
  {
    std::set<unsigned int>* ptSet = new std::set<unsigned int>();
    m_ClustersToPoints.push_back(ptSet);
  }
}

void PndMvaCluster::InitialPartition()
{
  assert(!(m_PointSet.size() < m_num_Cluster));
  // Data points loop
  for(size_t pt = 0; pt < m_PointSet.size(); pt++)
  {
    unsigned int cluster_idx = pt % m_num_Cluster;
    // Add point to cluster and cluster to point
    m_PointsToClusters[pt] = cluster_idx;
    (m_ClustersToPoints[cluster_idx])->insert(pt);
  }
}

void PndMvaCluster::printStructs()
{
  std::cerr << "Printing input points." << std::endl;
  for(size_t pt = 0; pt < m_PointSet.size(); pt++){
    std::vector<float>* currPt = m_PointSet[pt];
    for(size_t i = 0; i < currPt->size(); i++){
      std::cerr << currPt->at(i) << " ";
    }
    std::cerr << std::endl;
  }
  
  std::cerr << "Printing centroids." << std::endl;
  for(size_t ctr = 0; ctr < m_Centroids.size(); ctr++){
    std::vector<float>* currCtr = m_Centroids[ctr];
    for(size_t i = 0; i < currCtr->size(); i++){
      std::cerr << currCtr->at(i) << " ";
    }
    std::cerr << std::endl;
  }

  std::cerr << "Printing point to centroid." << std::endl;
  for(size_t j = 0; j < m_PointsToClusters.size(); j++){
    std::cerr << j << " -> " << m_PointsToClusters[j] << std::endl;
  }

  std::cerr << "Printing centroid to point." << std::endl;
  std::set<unsigned int>::const_iterator iter;
  for(size_t k = 0; k < m_ClustersToPoints.size(); k++){
    std::cerr << " CTR" << k << " -> ";
    std::set<unsigned int>* curSet = m_ClustersToPoints[k];
    for( iter = curSet->begin(); iter != curSet->end(); iter++){
      std::cerr << *iter << ' ';
    }
    std::cerr << std::endl;
  }
}
