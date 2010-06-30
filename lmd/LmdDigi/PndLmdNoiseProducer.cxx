// -------------------------------------------------------------------------
// -----                PndLmdNoiseProducer source file                -----
// -----                  Created 04/02/10  by M.Michel                -----
// -----              Based on PndMvdNoiseProducer by R.Kliemt         -----
// -------------------------------------------------------------------------

#include "PndLmdNoiseProducer.h"

// -----   Default constructor   -------------------------------------------
PndLmdNoiseProducer::PndLmdNoiseProducer()
{
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndLmdNoiseProducer::~PndLmdNoiseProducer()
{
}

void PndLmdNoiseProducer::SetBranchNames(TString inBranchNameStrip, TString inBranchNamePixel, TString folderName)
{
  fBranchNameStrip = inBranchNameStrip;
  fBranchNamePixel = inBranchNamePixel;
  fFolderName = folderName;
}

void PndLmdNoiseProducer::SetBranchNames()
{
  fBranchNameStrip = "LMDStripDigis";
  fBranchNamePixel = "LMDPixelDigis";
  fFolderName = "PndLmd";
}

void PndLmdNoiseProducer::SetMCPointType()
{
	}


ClassImp(PndLmdNoiseProducer)
