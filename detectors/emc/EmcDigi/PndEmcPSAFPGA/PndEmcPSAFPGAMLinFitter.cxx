#include "PndEmcPSAFPGAMLinFitter.h"

void PndEmcPSAFPGAMLinFitter::set(int newBufferSize)
{
  Np =newBufferSize;
  mx.set(newBufferSize);
  mxx.set(newBufferSize);
  mxy.set(newBufferSize);
  my.set(newBufferSize);
  return;
}

void PndEmcPSAFPGAMLinFitter::putPoint(double ix, double iy)
{
  x = mx.put(ix);
  xy = mxy.put(ix*iy);
  xx = mxx.put(ix*ix);
  y = my.put(iy);
}

void PndEmcPSAFPGAMLinFitter::fit()
{
  k = (Np*xy - y*x) / (Np*xx - x*x);
  a = (y- k*x) / Np;
}

double PndEmcPSAFPGAMLinFitter::offset()
{
  return a;
}

double PndEmcPSAFPGAMLinFitter::slope()
{
  return k;
}

double PndEmcPSAFPGAMLinFitter::average()
{
  if (Np > 0)
    return y/Np;
  return 0.0;
}
