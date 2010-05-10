/* ***************************************
 * Clustering algorithms                 *
 * Author: M.Babai@rug.nl                *
 * Version:                              *
 * LICENSE:                              *
 * ***************************************
 */

#define MAXRND_CL_VALUE 1

#include "PndMvaCluster.h"

PndMvaCluster::PndMvaCluster(const ClDataSample& InputData, unsigned int nCluster)
  : m_num_Cluster(nCluster), m_PointSet(InputData)
{
  assert( m_PointSet.size() != 0 );
  
  m_rnd = TRandom3 (time(NULL));
  m_dimension = (m_PointSet[0])->size();
  
  m_PointsToClusters = std::vector<unsigned int>(m_PointSet.size(), 0);
  
  for(unsigned int cl = 0; cl < m_num_Cluster; cl++){
    std::set<unsigned int>* ptSet = new std::set<unsigned int>();
    m_ClustersToPoints.push_back(ptSet);
  }
}

PndMvaCluster::~PndMvaCluster()
{
  for(size_t ctr = 0; ctr < m_Centroids.size(); ctr++){
    delete m_Centroids[ctr];
  }
  m_Centroids.clear();

  m_PointsToClusters.clear();

  for(size_t i = 0; i < m_ClustersToPoints.size(); i++){
    delete m_ClustersToPoints[i];
  }
  m_ClustersToPoints.clear();
}

ClDataSample& PndMvaCluster::K_Means()
{
  // Number of clusters.
  assert(m_num_Cluster != 0);
  InitCentroids();
  InitialPartition();

  // Copy centroid to the output
  ClDataSample* Cl_Out = new ClDataSample();
  for(size_t ctr = 0; ctr < m_Centroids.size(); ctr++){
    std::vector<float>* ct = new std::vector<float>( *(m_Centroids[ctr]) );
    Cl_Out->push_back(ct);
  }
  return (*Cl_Out);
}

void PndMvaCluster::InitCentroids()
{
  // Init Centroids container;
  for(unsigned int i = 0; i < m_num_Cluster; i++){
    std::vector<float>* ctr = new std::vector<float>();
    for(unsigned int j = 0; j < m_dimension; j++){
      ctr->push_back(static_cast<float>(m_rnd.Uniform(MAXRND_CL_VALUE)));
    }
    m_Centroids.push_back(ctr);
  }
}

void PndMvaCluster::InitialPartition()
{
  // Data points loop
  for(size_t pt = 0; pt < m_PointSet.size(); pt++){
    std::vector<float>* currPt = m_PointSet[pt];
    float minDist = std::numeric_limits<float>::max();
    unsigned int cluster_idx = 0;//closest centeroid index
  // Centroids loop
    for(size_t ctr = 0; ctr < m_Centroids.size(); ctr++){
      std::vector<float>* currCtr = m_Centroids[ctr];
      float dist = ComputeDist(*currCtr, *currPt);
      if(dist < minDist){
	minDist = dist;
	cluster_idx = ctr;
      }
    }
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
