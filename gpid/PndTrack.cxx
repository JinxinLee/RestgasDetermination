#include "PndTrack.h"
ClassImp(PndTrack);

PndTrack::PndTrack()
:
TObject()
{
fTpcDeDx = 0.0;
fMvdDeDx = 0.0;
fP = 0.0;
fEmcE = 0.0;
fTof = 0.0;
fSpeed = 0.0;
fLen = 0.0;
}

PndTrack::~PndTrack()
{
 destroy();
}

void PndTrack::destroy()
{
}
