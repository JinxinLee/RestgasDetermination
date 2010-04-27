/* ***************************************
 * Clustering algorithms                 *
 * Author: M.Babai@rug.nl                *
 * Version:                              *
 * LICENSE:                              *
 * ***************************************
 */

#define MAXRND_CL_VALUE 2.0

#include "PndMvaCluster.h"

PndMvaCluster::PndMvaCluster()
  : m_dim(0), m_num_Cluster(0)
{
  m_classNames.clear();
}

PndMvaCluster::PndMvaCluster(const std::vector<std::string>& classNames,
			     unsigned int dimension, unsigned int nCluster)
  : m_classNames(classNames), m_dim(dimension), m_num_Cluster(nCluster)
{}

PndMvaCluster::~PndMvaCluster()
{
  m_classNames.clear();
  m_currClassIdx.clear();
  m_PointToCluster.clear();
  for(size_t i = 0; i < m_ClustersToPoints.size(); i++){
    delete m_ClustersToPoints[i];
  }
  m_ClustersToPoints.clear();
}

ClDataSample& PndMvaCluster::K_Means(const ClDataSample& InPutData)
{

  
  ClDataSample* out = new ClDataSample();
  
  // Number of Classes.
  assert(m_classNames.size() != 0);
  
  // Input data is not empty
  assert(InPutData.size() != 0);
  
  // Number of clusters.
  assert(m_num_Cluster != 0);
  
  // Dimension of the vectors
  assert(m_dim != 0);
  
  // Class loop
  for(size_t cls = 0; cls < m_classNames.size(); cls++){
    std::string currClassName = m_classNames[cls];
    
    // Init random Centroids for the current class.
    ClDataSample& currCenters = InitCentroids(currClassName);

    // Find event indices for the current class.
    for(size_t ev = 0; ev < InPutData.size(); ev++){
      if(InPutData[ev].first == currClassName){
	m_currClassIdx.push_back(ev);
      }
    }

    // Init number of events, cluster id's    
    m_PointToCluster = std::vector <unsigned int>(m_currClassIdx.size(), 0);
    
    // Make initial partitioning
    InitialPartition();
    
    std::cerr << currCenters.size() << std::endl;
    // Perform Clustering
  }//END for(cls = 0; ....)
  return (*out);
}

ClDataSample& PndMvaCluster::InitCentroids(const std::string& clsName)
{
  ClDataSample* centers = new ClDataSample();
  TRandom3 rnd(time(NULL));

  // Centroids loop
  for(unsigned int ctr = 0; ctr < m_num_Cluster; ctr++){
    // Init container
    std::vector<float>* currCtr = new std::vector<float>();
    // Dimension loop
    for(unsigned int dim = 0; dim < m_num_Cluster; dim++){
      float rndVal = static_cast<float>(rnd.Uniform(MAXRND_CL_VALUE));
      currCtr->push_back(rndVal);
    }
    centers->push_back(std::make_pair(clsName, currCtr));
  }
  std::cerr << "<INFO> Initialized "<< centers->size() 
	    << " centroids for " << clsName << std::endl;
  return (*centers);
}

void PndMvaCluster::InitialPartition()
{}
