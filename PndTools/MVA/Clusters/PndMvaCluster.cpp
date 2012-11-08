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
PndMvaCluster::PndMvaCluster( DataPoints const& InputData, size_t nCluster)
  : m_num_Cluster(nCluster), 
    m_PointSet(InputData),
    m_dimension( InputData.size() == 0 ? 0 : ( (m_PointSet[0]).second)->size() ),
    m_PointsToClusters( std::vector<size_t>(m_PointSet.size(), 0) ),
    m_prune (false),
    m_forceToLabel (false)
{}

/**
 * Constructor.
 *@param InputData Input Data points.
 *@param nCluster  Number of clusters to be created.
 *@param prune  Prune the created cluster.
 *@param forceLabels Force to label the undecided and unknown points.
 */
PndMvaCluster::PndMvaCluster( DataPoints const& InputData, size_t nCluster,
                              bool const prune, bool const forceLabels)
  : m_num_Cluster(nCluster),
    m_PointSet(InputData),
    m_dimension( InputData.size() == 0 ? 0 : ( (m_PointSet[0]).second)->size() ),
    m_PointsToClusters( std::vector<size_t>(m_PointSet.size(), 0) ),
    m_prune (prune),
    m_forceToLabel (forceLabels)
{}

/**
 * Destructor.
 */
PndMvaCluster::~PndMvaCluster()
{
  ClearStructures();
  
  //for(size_t p = 0; p < m_PointSet.size(); ++p) {
    //delete m_PointSet[p].second;
  // }
  m_PointSet.clear();
}

/*
 * Clear the currently used data structures.
 */
void PndMvaCluster::ClearStructures()
{
  for(size_t ctr = 0; ctr < m_Centroids.size(); ctr++)
  {
    delete (m_Centroids[ctr].second);
  }
  m_Centroids.clear();
  
  for(size_t i = 0; i < m_ClustersToPoints.size(); i++)
  {
    delete m_ClustersToPoints[i];
  }
  m_ClustersToPoints.clear();
  
  m_PointsToClusters.clear();
}

/**
 * Compute Cluster centers for the current input data.
 *@param ClType Clustering algorithm.
 *@return Vector containing the cluster centroids.
 */
DataPoints* PndMvaCluster::Cluster(const ClusteringType ClType)
{
  // Zero dimension makes no sense.
  assert(m_dimension != 0);

  // More clusters than data points?
  assert( m_PointSet.size() > m_num_Cluster );

  // Zero clusters means do nothing
  assert(m_num_Cluster != 0);
  
  switch(ClType)
  {
  case KMEANS_SOFT://Returns empty centroids (not implemented yet).
    std::cout << "<INFO> Soft clustering.\n";
    std::cerr << "<ERROR> Not implemented yet.\n" << std::flush;
    return ( new DataPoints() );
    break;

  default://Hard k_means clustering
    std::cout << "<INFO> Hard K_Means clustering.\n";
    return K_Means();
    break;
  }
}

/**
 * Compute Cluster centers for the current input data. The label of
 * the majority of their members determines the label of the center.
 *@param ClType Clustering algorithm.
 *@return Vector containing the cluster centroids.
 */
DataPoints* PndMvaCluster::ClusterAndLabel( ClusteringType const ClType,
                                            std::vector<std::string> const& label)
{
  // Perform clustering
  DataPoints* dat = Cluster(ClType);

  // Map to hold counts per label.
  std::map < std::string, size_t> labelCnt;

  // Local temporary vars.
  size_t maxCnt;// Count of labels for the current centroid.
  std::string wLab;// Winning label

  // Responsibility list loop. We need to visit all of the centroids
  // and relabel them
  std::set<size_t>::iterator it;

  // Centroids list
  for(size_t ct = 0; ct < m_ClustersToPoints.size(); ++ct)
  {
    // Init counts map to zero
    for(size_t l = 0; l < label.size(); ++l)
    {
      std::string const& curLabel = label[l];
      labelCnt[curLabel] = 0;
    }
    
    // Responsibility list of the i-th centroid.
    std::set<size_t>* lst = m_ClustersToPoints[ct];
    
    // Count numbers per label for the members of the current
    // centroid.
    for(it = lst->begin(); it != lst->end(); ++it)
    {
      size_t dataIdx = *it;
      std::string const& curLabel = m_PointSet[dataIdx].first;
      labelCnt[curLabel] += 1;
    }
    // Done counting, Decide by finding maximum
    maxCnt = 0;//std::numeric_limits<size_t>::min();
    
    for(size_t lb = 0; lb < label.size(); ++lb)
    {
      if( labelCnt[label[lb]] > maxCnt )
      {
        maxCnt = labelCnt[label[lb]];
        wLab = label[lb];
      }
      else if(labelCnt[label[lb]] == maxCnt) {//Equal numbers
        // maxCnt = labelCnt[label[lb]];
        if(maxCnt == 0) {// No members in the list
          wLab = "Empty_MemberList";
        }
        else {// Equal number, we can not decide.
          std::ostringstream cnt;
          cnt << maxCnt;
          wLab = "UNDECIDED_" + cnt.str();
        }
      }
    }// Found winning label. Assign
    (*dat)[ct].first = wLab;
  }
  //_________________________ Prune if required. ______________
  if(m_prune)
  {
    std::cout << "\t<-I-> Pruning the centroids.\n";
    for( size_t j = 0; j < dat->size(); ++j)
    {
      std::string const& curLabel = (*dat)[j].first;
      if( ( curLabel == "Empty_MemberList") || 
          ( curLabel.find("UNDECIDED_") != std::string::npos)
        )
      {

        std::cout << "\t<-I-> Dropping centroid number "
                  << j << '\n';

        (*dat)[j].first = "DROP_FROM_CENTROIDS";
      }
    }// All elements to be droped are marked.
    std::vector< std::pair<std::string, std::vector<float>*> >::iterator drp = dat->begin();
    while( drp != dat->end() )
    {
      std::string const& drpLab = (*drp).first;
      // If we need to drop current centroid.
      if( drpLab == "DROP_FROM_CENTROIDS")
      {
        delete (*drp).second;// Delete vector
        dat->erase(drp);// Delete element (std::pair)
        drp = dat->begin();// Go to begin (note size is changed!)
      }
      else
      {
        drp++;
      }
    }
    std::cout << "\t<-I-> Total number of dropped elements = "
              << (m_num_Cluster - dat->size())
              << '\n';
  }// End if(m_prune)

  //________________________ Force labeling if required. ________
  if(m_forceToLabel)
  {
    float minDist, distance;
    std::string WinLabel;
    std::cout << "\t<-I-> Forcing to label the centroids based "
              << "on the data points with the shortest distance.\n";

    // Centroids loop
    for( size_t j = 0; j < dat->size(); ++j)
    {
      std::string const& curLabel = (*dat)[j].first;
      if( ( curLabel == "Empty_MemberList") || 
          ( curLabel.find("UNDECIDED_") != std::string::npos)
        )
      {
        std::cout << "\t<-I-> Relabeling centroid number "
                  << j << '\n';

        minDist = std::numeric_limits<float>::max();
        std::vector<float> const* curCenter = (*dat)[j].second;

        // Loop through input data and find the element with the shortest
        // distance to the current centroid.
        for(size_t k = 0; k < m_PointSet.size(); k++)
        {
          std::vector<float> const* curDataPoint = m_PointSet[k].second;
          distance = ComputeDist((*curCenter), (*curDataPoint));
          if( distance < minDist)
          {
            minDist = distance;
            WinLabel = m_PointSet[k].first;
          }
        }// Met all examples. Label is determined
        (*dat)[j].first = WinLabel;
      }// END of if (label)
    }// All elements are visited and relabeld.
  }// END if(m_forceToLabel)
  
  // Return the computed centroids.
  return (dat);
}

/*
 * Performs the actual hard K-Means clustering.
 */
DataPoints* PndMvaCluster::K_Means()
{
  bool some_point_is_moving = true;
  bool move;
  size_t num_iter = 0;
  
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

    // Loop through the data points
    for(size_t pt = 0; pt < m_PointSet.size(); pt++)
    {
      // Distance of the current point to its centroid.
      minDist = ComputeDist( *(m_PointSet[pt].second), *(m_Centroids[m_PointsToClusters[pt]].second));
      move = false;
    
      // Centroids loop
      for(size_t ctr = 0; ctr < m_num_Cluster; ctr++)
      {
        // Distance to all other centroids.
	currDist = ComputeDist( *(m_PointSet[pt].second), *(m_Centroids[ctr].second));

	// Select the the centroid with the shortest distance
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
      if(move)
      {
	m_PointsToClusters[pt] = to_cluster;
	(m_ClustersToPoints[to_cluster])->insert(pt);
      }
    }
    num_iter++;

    // ================ Check empty clusters =======
    
    // For now we do nothing but maybe better to do 'singleton' Create
    // a new cluster consisting of the one point furthest from its
    // centroid.
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
    //std::cout << "Iter num " << num_iter << " Tot Sum " << distSum
    //<< std::endl;
  }//END OF while (some_point_is_moving)
  
  std::cout << "\t<-I-> Num Iterations " << num_iter 
	    << '\n';  
  //======================================
  // Copy centroid to the output structure
  DataPoints* Cl_Out = new DataPoints();
  
  for(size_t ctr = 0; ctr < m_num_Cluster; ctr++)
  {
    // Create copy vector. Give up the ownership; we want to clean up
    // at the end.
    std::vector<float>* ct = new std::vector<float>( *(m_Centroids[ctr].second) );
    Cl_Out->push_back( std::make_pair(m_Centroids[ctr].first, ct) );
  }

  //////____________ DEBUG CODE _______________________
#if (PNDMVA_CLUSTER_DEBUG > 0)
  std::set<size_t>::iterator it;
  size_t pion, elec;
  pion = 0; elec = 0;
  for(size_t i =0 ; i < m_ClustersToPoints.size(); i++) {
    std::set<size_t>* lst = m_ClustersToPoints[i];
    for(it = lst->begin(); it != lst->end(); ++it) {
      size_t dataIdx = *it;
      std::string const& lb = m_PointSet[dataIdx].first;
      if(lb  == "electron")
        elec++;
      else
        pion++;
    }
    std::cout << "index = " << i << " Repons list length = " 
              << (m_ClustersToPoints[i])->size() 
              << " #electrons = " << elec <<" #pion = " << pion
              << std::endl;
    pion = elec = 0;
  }
#endif// End of debug code

  // Return the output structure
  return (Cl_Out);
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
    std::vector<float>* curCt = m_Centroids[ct].second;
    
    // Points in the current cluster
    std::set<size_t>* ClusterPoints = m_ClustersToPoints[ct];
    
    // Loop through the points and update the cluster center
    for(std::set<size_t>::const_iterator iter = ClusterPoints->begin();
	iter != ClusterPoints->end(); ++iter)
    {
      size_t pt = *iter;
      std::vector<float>* curPt = m_PointSet[pt].second;
      
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
  for(size_t i = 0; i < m_num_Cluster; i++)
  {
    std::vector<float>* ctr = new std::vector<float>(m_dimension, 0.0);
    m_Centroids.push_back( std::make_pair("NO_LABEL", ctr) );
  }
  
  for(size_t cl = 0; cl < m_num_Cluster; cl++)
  {
    std::set<size_t>* ptSet = new std::set<size_t>();
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
    size_t cluster_idx = (pt % m_num_Cluster);
    // Add point to cluster and cluster to point
    m_PointsToClusters[pt] = cluster_idx;
    (m_ClustersToPoints[cluster_idx])->insert(pt);
  }
}

/* 
 * Init empty Centroid to the furthest point of its center.
 *@param centerIdx Index of the centroid with zero responsibility.
 */
void PndMvaCluster::ReInitEmptyCenter(size_t centerIdx)
{
  float maxDist = std::numeric_limits<float>::min();
  float currDist = 0.0;
  size_t point_Idx = 0;

  // Find the point with the largest dist to its centeroid.
  for(size_t i = 0; i < m_PointsToClusters.size(); i++)
  {
    currDist = ComputeDist(*(m_PointSet[i].second), *(m_Centroids[m_PointsToClusters[i]].second));
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
    (m_Centroids[centerIdx].second)->at(dim) = (m_PointSet[point_Idx].second)->at(dim);
  }
  
  // Add point to cluster and cluster to point & init cluster
  m_ClustersToPoints[centerIdx]->insert(point_Idx);
  m_PointsToClusters[point_Idx] = centerIdx;
}

//__________________ DEBUG FUNCTIONS ______________
#if (PNDMVA_CLUSTER_DEBUG > 0)
/**
 * Print the vectors and centroids and their relation.
 */
void PndMvaCluster::printStructs()
{
  std::cerr << "Printing input points." << std::endl;
  for(size_t pt = 0; pt < m_PointSet.size(); pt++)
  {
    std::vector<float>* currPt = m_PointSet[pt].second;
    for(size_t i = 0; i < currPt->size(); i++)
    {
      std::cerr << currPt->at(i) << " ";
    }
    std::cerr << std::endl;
  }
  
  std::cerr << "Printing centroids." << std::endl;
  for(size_t ctr = 0; ctr < m_Centroids.size(); ctr++)
  {
    std::vector<float>* currCtr = m_Centroids[ctr].second;
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
  std::set<size_t>::const_iterator iter;
  for(size_t k = 0; k < m_ClustersToPoints.size(); k++)
  {
    std::cerr << " CTR" << k << " -> ";
    std::set<size_t>* curSet = m_ClustersToPoints[k];
    for( iter = curSet->begin(); iter != curSet->end(); ++iter)
    {
      std::cerr << *iter << ' ';
    }
    std::cerr << std::endl;
  }
}
#endif
