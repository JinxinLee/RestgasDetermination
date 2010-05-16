#ifndef PNDMVDCHARGEWEIGHTEDPIXELMAPPING_H
#define PNDMVDCHARGEWEIGHTEDPIXELMAPPING_H

#include "PndSdsChargeWeightedPixelMapping.h"

//! PndMvdChargeWeightedPixelMapping.h
//! @author T.Stockmanns <t.stockmanns@fz-juelich.de>
//!
//! \brief PndSdsChargedWeightedPixelMapping: Gets a vector of DigiHits and calculates the cluster center weighted with the charge

class PndMvdChargeWeightedPixelMapping : public PndSdsChargeWeightedPixelMapping
{
	public :

	PndMvdChargeWeightedPixelMapping(Int_t verbose = 0);
	PndMvdChargeWeightedPixelMapping(PndGeoHandling* geo, Int_t verbose = 0);
};

#endif
