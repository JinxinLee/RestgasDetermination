/*
 *  
 *  S.Vanniarajan  V.Suyam.Jothi@kvi.nl
 *
 *  This is the Mutivariate Event classification tool
 *  designed for PANDAROOT Analysis package for 
 *  PANDA Experiments.
 *       TMVA(Toolkit for MultiVariate Analysis) is a 
 *  two class classifier(signal and background). 
 *  available with ROOT package. mainly used for 
 *  Event Classification in High Energy Physics Experiments.
 *         
 *       This tool here is Designed from TMVA, for Multiclass 
 * Classification purpose. 
 *
 *                  S.Vanniarajan  01-08-08
 * Modified:
 * M. Babai
 */

#include "PndTrack.h"

PndTrack::PndTrack():TObject(), fTpcDeDx(0.0)
		     fMvdDeDx(0.0), fP(0.0),
		     fEmcE(0.0),  fTof(0.0),
		     fSpeed(0.0), fLen(0.0)
{}

PndTrack::~PndTrack()
{
 destroy();
}

void PndTrack::destroy()
{}

ClassImp(PndTrack);
