#ifndef PNDMVDCHARGEWEIGHTEDPIXELMAPPING_H
#define PNDMVDCHARGEWEIGHTEDPIXELMAPPING_H

#include "PndMvdPixelBackMapping.h"
#include "TGeoMatrix.h"
#include "TVector3.h"
//! PndMvdChargeWeightedPixelMapping.h
//! @author T.Stockmanns <t.stockmanns@fz-juelich.de>
//!
//! \brief PndMvdChargedWeightedPixelMapping: Gets a vektor of DigiHits and calculates the cluster center weighted with the charge
//! @param [0] = Number of columns in FE
//! @param [1] = Number of rows in FE
//! @param [2] = Pixel dimension in x
//! @param [3] = Pixel dimension in y
//! \sa PndMvdPixelBackMapping.h
//TODO replace params with parameter database values
class PndMvdChargeWeightedPixelMapping : public PndMvdPixelBackMapping
{
	public :
			PndMvdChargeWeightedPixelMapping(std::vector<PndMvdDigiPixel> pixelArray, std::vector<Double_t> params):PndMvdPixelBackMapping(pixelArray, params){};
			PndMvdHit GetCluster(); ///< Main function of class to calculate the PndMvdHit out of the given PndMvdDigis
	private :
			TGeoHMatrix GetTransformation(std::string detName);
			TVector3 GetSensorDimensions(std::string detName);

};

#endif


