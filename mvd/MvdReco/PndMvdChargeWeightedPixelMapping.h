#ifndef PNDMVDCHARGEWEIGHTEDPIXELMAPPING_H
#define PNDMVDCHARGEWEIGHTEDPIXELMAPPING_H

#include "PndSdsChargeWeightedPixelMapping.h"
#include "PndSdsPixelDigiPar.h"
#include "PndSdsTotDigiPar.h"

//! PndMvdChargeWeightedPixelMapping.h
//! @author T.Stockmanns <t.stockmanns@fz-juelich.de>
//!
//! \brief PndSdsChargedWeightedPixelMapping: Gets a vector of DigiHits and calculates the cluster center weighted with the charge

class PndMvdChargeWeightedPixelMapping : public PndSdsChargeWeightedPixelMapping
{
	public :

	PndMvdChargeWeightedPixelMapping(Int_t verbose = 0);
	PndMvdChargeWeightedPixelMapping(PndGeoHandling* geo, Int_t verbose = 0);
	PndMvdChargeWeightedPixelMapping(PndGeoHandling* geo, PndSdsPixelDigiPar* digiPar, PndSdsTotDigiPar* totPar);
  PndMvdChargeWeightedPixelMapping(const PndMvdChargeWeightedPixelMapping& o) :
	  fDigiPar(o.fDigiPar),
	  fTotDigiPar(o.fTotDigiPar)
  {};
  PndMvdChargeWeightedPixelMapping& operator=(const PndMvdChargeWeightedPixelMapping& o)
  {
	  fDigiPar=o.fDigiPar;
	  fTotDigiPar=o.fTotDigiPar;
    return *this;
  };

	void SetChargeConverter();
	PndSdsPixelDigiPar* fDigiPar;
	PndSdsTotDigiPar* fTotDigiPar;
};

#endif
