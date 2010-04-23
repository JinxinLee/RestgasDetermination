#ifndef PNDMVACLUSTER_H
#define PNDMVACLUSTER_H

#include <vector>
#include <string>

#include "PndMvaDataSet.h"


typedef std::vector< std::pair<std::string, std::vector<float>*> > DataSample;

class PndMvaCluster
{
  // -------------- public members ---------
 public:
  PndMvaCluster();
  PndMvaCluster(const std::vector<std::string>& classNames,
		const std::vector<std::string>& varNames,
		int ctrs);
  
  virtual ~PndMvaCluster();
  
  DataSample& K_Means(const DataSample& DataPoints) const;

  //------- Getters
  const std::vector<std::string>& GetClassNames() const
  { return (* new std::vector<std::string>(m_classNames)); };

  const std::vector<std::string>& GetVarNames() const
  { return (* new std::vector<std::string>(m_varNames)); };
  
  inline int GetetNumberOfCltrs() const
  { return m_numCds; };
  
  //------- Setters
  inline void SetClassNames(const std::vector<std::string>& classNames)
  {m_classNames = classNames;};

  inline void SetVarNames(const std::vector<std::string>& varNames)
  {m_varNames = varNames;};

  inline void SetNumberOfCltrs(int num)
  {m_numCds = num;};
  
  // -------------- protected members --------
  //protected:
  
  // -------------- private members   ---------
 private:
  // To avoid mistakes.
  PndMvaCluster(const PndMvaCluster& oth);
  PndMvaCluster& operator=(const PndMvaCluster& oth);
  
  std::vector<std::string> m_classNames;
  std::vector<std::string> m_varNames;
  int m_numCds;
};
#endif
