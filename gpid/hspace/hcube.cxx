#include "hypercube.h"

hcube::hcube()
{
}

hcube::~hcube()
{
}

void hcube::setCoord( const vector<int> coordinate)
{
   coord = coordinate;
}


bool hcube::operator==(const hcube& a )
{
 if(dim != a.dim) return 0;
 for(int i = 0; i < a.dim ; i++)
  {
   if ( a.coord.at(i)  != coord.at(i)) return 0;
  }
  return 1;
}
