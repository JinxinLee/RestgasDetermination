#ifndef TCCLUSTERSORT_H
#define TCCLUSTERSORT_H

#include"TCcluster.h"

class TCclusterSortUVW{
 public:
  bool operator()(TCcluster c1, TCcluster c2);
};

class TCclusterSortXYZ{
 public:
  bool operator()(TCcluster c1, TCcluster c2);
};

#endif
