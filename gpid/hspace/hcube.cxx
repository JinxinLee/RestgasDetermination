#include "hbox.h"

hbox::hbox()
{
}

hbox::~hbox()
{
}

void hbox::setCoord( const vector<int> coordinate)
{
   coord = coordinate;
}

vector<int> hbox::getCoord( )
{
   return coord;
}



bool hbox::operator==(const hbox& a )
{
 if(dim.coord == a.dim.getCoord()) return 1;
 else
  return 0;
}
