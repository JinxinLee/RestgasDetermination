#include "PndEmcPSAFPGALinFitter.h"

PndEmcPSAFPGALinFitter::PndEmcPSAFPGALinFitter()
{
  reset();
}

PndEmcPSAFPGALinFitter::~PndEmcPSAFPGALinFitter()
{
}
void PndEmcPSAFPGALinFitter::reset()
{
  Np=0;
  x=0.0; xx=0.0;
  xy=0.0; y=0.0;
}

void PndEmcPSAFPGALinFitter::putPoint(double ix,double iy)
{
  x += ix;
  xy += ix*iy;
  xx += ix*ix;
  y += iy;
  Np++;
}

void PndEmcPSAFPGALinFitter::fit()
{
  if (Np >= 2) {
    k = (Np*xy-y*x)/(Np*xx-x*x);
    a = (y-k*x)/Np;
  } else {
    k=0; a=0;
  }
}

double PndEmcPSAFPGALinFitter::offset()
{
  return a;
}

double PndEmcPSAFPGALinFitter::slope()
{
  return k;
}

double PndEmcPSAFPGALinFitter::average()
{
  if (Np > 0)
    return y/Np;
  return 0.0;
}
