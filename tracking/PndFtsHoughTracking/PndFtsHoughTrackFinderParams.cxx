#include "PndFtsHoughTrackFinderParams.h"

ClassImp(PndFtsHoughTrackFinderParams)
// -----   Default constructor   -------------------------------------------

PndFtsHoughTrackFinderParams::PndFtsHoughTrackFinderParams()
: fParabolaStepsPerThetaDeg(4.),
  fParabolaHwScan(20.),

  fParabolaNBinsPzxInv(600),
  fParabolaQDivPzxArgMax(0.015),

  fZLineParabola(342.75), // 368. seemed fine
  fZParabolaLine(605.),

  fUnused(0)
{ }

// -------------------------------------------------------------------------


// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndFtsHoughTrackFinderParams::~PndFtsHoughTrackFinderParams() { }
// -------------------------------------------------------------------------

