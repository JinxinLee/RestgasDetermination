//-------------------------------------------------------------------------
// Author:      Mustafa Schmidt (Mustafa.A.Schmidt@physik.uni-giessen.de)
// Changes:     
// Date:        30.11.2015
// Description: Particle Identification
//-------------------------------------------------------------------------


#include "PndDiscPID.h"


#include <iostream>




ClassImp(PndDiscPID)



PndDiscPID::PndDiscPID() : TObject(),
loglikepion(0.),
loglikekaon(0.),
loglikeproton(0.),
pion(0.),
kaon(0.),
proton(0.)
{
}

PndDiscPID::~PndDiscPID()
{
}
