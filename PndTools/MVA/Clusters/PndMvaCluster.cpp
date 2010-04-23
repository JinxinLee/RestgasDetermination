#include "PndMvaCluster.h"

PndMvaCluster::PndMvaCluster()
{
  m_numCds = 0;
  m_classNames.clear();
  m_varNames.clear();
}

PndMvaCluster::PndMvaCluster(const std::vector<std::string>& classNames,
			     const std::vector<std::string>& varNames,
			     int ctrs)
  : m_classNames(classNames), m_varNames(varNames), m_numCds(ctrs)
{}

PndMvaCluster::~PndMvaCluster()
{}

DataSample& PndMvaCluster::K_Means(const DataSample& InPutData)const
{
  DataSample* out = new DataSample(InPutData);
  return (*out);
}
