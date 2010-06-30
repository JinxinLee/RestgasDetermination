/*
 * PndLmdDigiStrips.cxx
 *
 *  Created on: Aug 10, 2009
 *      Author: huagen
 */
#include "PndLmdDigiStrips.h"

PndLmdDigiStrips :: PndLmdDigiStrips()
{//fIndex = 0;
 fdetID = 0;
// fdetName = 0;
 fFE = 0;
 fChannel = 0;
 fCharge = 0;
}

PndLmdDigiStrips :: PndLmdDigiStrips(Int_t index, Int_t detID,
		                  TString detName, Int_t fe, Int_t channel, Double_t charge)
{   fIndex.push_back(index);
    fdetID = detID;
    fdetName = detName;
    fFE = fe;
    fChannel = channel;
    fCharge = charge;

	}

ClassImp(PndLmdDigiStrips);
