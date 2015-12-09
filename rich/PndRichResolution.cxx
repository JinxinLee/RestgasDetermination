#include "PndRichResolution.h"
#include "PndPidCandidate.h"

//___________________________________________________________
PndRichResolution::~PndRichResolution() 
{
}

// -----   Default constructor   -------------------------------------------
PndRichResolution::PndRichResolution()
{
}

//______________________________________________________
Double_t PndRichResolution::Sigma(PndPidCandidate* pidcand)
{
   return 0.0003;
}

//______________________________________________________
Double_t PndRichResolution::Shift(PndPidCandidate* pidcand)
{
   return 0.00135;
}

// -------------------------------------------------------------------------

