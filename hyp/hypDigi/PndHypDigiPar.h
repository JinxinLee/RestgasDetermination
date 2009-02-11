//
// C++ Interface: MvdDigiPar
//
// Description: 
//
//
// Author: t.stockmanns <stockman@ikp455>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//// slightly modified by A. Sanchez for hyp purpose

#ifndef PNDHYPDIGIPAR_H
#define PNDHYPDIGIPAR_H

#include "FairParGenericSet.h"
#include "FairParamList.h"

class PndHypDigiPar : public FairParGenericSet
{
	public :
		Double_t dimX;		  // PixelDimension in x
		Double_t dimY;		  // PixelDimension in y
		//Double_t skew;      // skewing angle if one exists
		Double_t threshold;	// Discriminator threshold
		Double_t noise;		  // Complete noise including threshold dispersion
//     Text_t   sensName;  // Sensor name (Strip, Pixel, etc...)
//     Text_t   feName;    // Frontend name (APV25, nyxiter, ...)

		PndHypDigiPar (const char* name="PndHypParTest",
			    const char* title="Hyp digi parameter",
			    const char* context="TestDefaultContext");
		~PndHypDigiPar(void){};
		void clear(void){};
		void putParams(FairParamList* list);
		Bool_t getParams(FairParamList* list);
	ClassDef(PndHypDigiPar,2);
};

#endif
