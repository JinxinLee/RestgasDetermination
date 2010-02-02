#ifndef PNDSDSCHARGEWEIGHTEDPIXELMAPPING_H
#define PNDSDSCHARGEWEIGHTEDPIXELMAPPING_H

#include "PndSdsPixelBackMapping.h"
#include "TGeoMatrix.h"
#include "TVector3.h"
//! PndSdsChargeWeightedPixelMapping.h
//! @author T.Stockmanns <t.stockmanns@fz-juelich.de>
//!
//! \brief PndSdsChargedWeightedPixelMapping: Gets a vektor of DigiHits and calculates the cluster center weighted with the charge
//! @param [0] = Number of columns in FE
//! @param [1] = Number of rows in FE
//! @param [2] = Pixel dimension in x
//! @param [3] = Pixel dimension in y
//! \sa PndSdsPixelBackMapping.h
//TODO replace params with parameter database values
class PndSdsChargeWeightedPixelMapping : public PndSdsPixelBackMapping
{
	public :
			PndSdsChargeWeightedPixelMapping(std::vector<PndSdsDigiPixel> pixelArray, std::vector<Double_t> params):PndSdsPixelBackMapping(pixelArray, params){};
			PndSdsHit GetCluster(); ///< Main function of class to calculate the PndSdsHit out of the given PndSdsDigis
	private :
			TGeoHMatrix GetTransformation(std::string detName);
			TVector3 GetSensorDimensions(std::string detName);

};

#endif


