#ifndef PNDMVDCALCPIXEL_H
#define PNDMVDCALCPIXEL_H

#include "PndMvdPixel.h"
#include "FairGeoVector.h"
#include <iostream>
#include <vector>
//#include <XYVector.h>
//#include "CLHEP/Vector/TwoVector.h"
//! Calculates the pixels (digis) fired from a particle hitting the sensor
/**
 *  @author Tobias Stockmanns <t.stockmanns@fz-juelich.de>
 * This class gets as an input the point where the particle enters the sensor,
 * where it leaves the sensor and the deposited energy. It calculates the track
 * of the particle between these two points and determines the pixels lying on the
 * track. The total energy of the particle is then distributed acording to the
 * pathlength to the pixels. The output is a vector<PndMvdPixel> of the fired sensor pixels
 * with the energy deposited in the pixels.
 * If a noise value is given a gausian distributed energy with a sigma of (noise) is added (or substracted) to
 * the energy deposited in one pixel.
 * If a threshold value is given all pixels with energies below threshold are ignored
*/
class PndMvdCalcPixel
{
	public :  
		///Default constructor
		PndMvdCalcPixel();			

	  ///Main constructor
	  ///@param w width of sensor pixel [cm]
	  ///@param l length of sensor pixel [cm]
	  ///@param threshold discriminator threshold of pixel [e]
	  ///@param noise total noise of pixel [e]
	  PndMvdCalcPixel(Double_t w, Double_t l, Double_t threshold = 0, Double_t noise = 0);
		  
	  ///Main function to calculate the vector<PndMvdPixel> of fired pixel
	  std::vector<PndMvdPixel> GetPixels (Double_t inx, Double_t iny, Double_t inz,
	  				   Double_t outx, Double_t outy, Double_t outz, Double_t energy);
						   
	  void SetVerboseLevel(Int_t level){ fVerboseLevel = level;};
	  std::ostream& operator<<(std::ostream& out);
	private :
		std::vector<PndMvdPixel> fPixels;
		Double_t fPixelWidth;
		Double_t fPixelLength;
		Double_t fThreshold;
		Double_t fNoise;
		Double_t fEnergy;
		Int_t fVerboseLevel;
			  
		typedef enum {kQuadUNDEF, kUR, kUL, kDL, kDR} EQuadrantType;
		typedef enum {kPixelUNDEF, kU, kD, kL, kR} ENextPixelType;
			
		EQuadrantType fQuad;
		ENextPixelType fNextPixel;
			
		FairGeoVector fIn;	//entry point of particle in sensor
		FairGeoVector fOut;	//exit point of particle out of sensor
		FairGeoVector fDir;	//direction of particle inside sensor
		FairGeoVector fPos;	//actual position of stepping inside sensor
			
		FairGeoVector fCon;
		
		PndMvdPixel fActivePixel;
	  Double_t fCperL;		//Charge per Tracklength;
		bool fStop;
		void CalcConMatrix();
		void ApplyConMatrix();
		void CalcQuadrant();
		void CalcCperL(Double_t Energy);
		void CalcStartPixel();		//pixel 0,0 is in lower left corner
		void CalcPixel();
		void ConvertPixels();
};

#endif
