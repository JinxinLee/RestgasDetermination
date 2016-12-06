//-------------------------------------------------------------------------
// Author:      Mustafa Schmidt (Mustafa.A.Schmidt@physik.uni-giessen.de)
// Changes:     
// Date:        30.11.2015
// Description: Reconstruction
//-------------------------------------------------------------------------


#include "PndDiscReconResult.h"


#include <iostream>




ClassImp(PndDiscReconResult)



PndDiscReconResult::PndDiscReconResult() : TObject(),
cherenkov_angle(0.),
particle(0),
hypothesis(0),
sensor(0),
pixel(0),
time(0.)
{
}

PndDiscReconResult::~PndDiscReconResult()
{
}
