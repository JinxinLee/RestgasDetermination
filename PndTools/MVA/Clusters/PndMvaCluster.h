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
//#include <map>
#include <set>

// ROOT
#include "TRandom3.h"

typedef std::vector< std::pair<std::string, std::vector<float>*> > ClDataSample;

class PndMvaCluster
{
  // -------------- public members ---------
 public:
  PndMvaCluster();
  PndMvaCluster(const std::vector<std::string>& classNames,
		unsigned int dimension, unsigned int nCluster);
  
  virtual ~PndMvaCluster();
  
  ClDataSample& K_Means(const ClDataSample& DataPoints);

  //------- Getters
  const std::vector<std::string>& GetClassNames() const
  { return (* new std::vector<std::string>(m_classNames)); };

  int GetDimension() const
  { return m_dim; };
  
  inline int GetetNumberOfClusters() const
  { return m_num_Cluster; };
  
  //------- Setters
  inline void SetClassNames(const std::vector<std::string>& classNames)
  {m_classNames = classNames;};

  inline void SetDimension(int val)
  {m_dim = val;};

  inline void SetNumberOfCltrs(int num)
  {m_num_Cluster = num;};
  
  // -------------- protected members --------
  //protected:
  
  // -------------- private members   ---------
 private:
  // To avoid mistakes.
  PndMvaCluster(const PndMvaCluster& oth);
  PndMvaCluster& operator=(const PndMvaCluster& oth)const;
  
  // Functions & Procedures
  ClDataSample& InitCentroids(const std::string& clsName);
  void InitialPartition();

  // Variables
  std::vector <std::string> m_classNames;
  unsigned int m_dim;
  unsigned int m_num_Cluster;
  std::vector <unsigned int> m_currClassIdx;
  std::vector <unsigned int> m_PointToCluster;
  std::vector <std::set<unsigned int>* > m_ClustersToPoints;
};
#endif
