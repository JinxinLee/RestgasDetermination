#include "dim.h"

dim::dim()
{
}

dim::~dim()
{
}

dim::dim(int seg,double min,double max,string st,unsigned int dno)
{
 nSEG = seg;
 fMIN = min;
 fMAX = max;
 sNAME = st;
 fDNO = dno;
}

