/* ***************************************
 * Clustering algorithms example program *
 * Author: M.Babai@rug.nl                *
 * Version:                              *
 * LICENSE:                              *
 * ***************************************
 */
#include <iostream>
#include "PndMvaCluster.h"

//int main(int argc, char** argv)
int main()
{
  std::cout << "<INFO> MVA Clustering." << std::endl;
  PndMvaCluster cls;
  cls.SetNumberOfCltrs(2);
  ClDataSample out;
  ClDataSample& proto = cls.K_Means(out);

  // Cleaning
  out.clear();
  proto.clear();
  return 0;
}
