/*
 * PndGeoHypGeModifiedCTFrame.h
 * 
 * Copyright 2012 Marcell Steinen <steinen@kph.uni-mainz.de>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */


#ifndef PNDGEOHYPGEMODIFIEDCTFRAME_H
#define PNDGEOHYPGEMODIFIEDCTFRAME_H

#include "TGeoManager.h"
#include "TGeoMedium.h"
#include "TGeoBBox.h"
#include "TGeoVolume.h"
#include "TGeoMatrix.h"
#include "TMath.h"
#include <stdio.h>

class PndGeoHypGeModifiedCTFrame 
{
	private:
		TGeoMedium					*Al;
		TGeoVolume 					*ModifiedCTFrame;
		TGeoVolume					*ModifiedCTFrameAssembly;
		
	public:
		
		PndGeoHypGeModifiedCTFrame(TGeoMedium *ExtAl,TGeoManager *geom);
		virtual ~PndGeoHypGeModifiedCTFrame();

		void BuildFrame();
		
		void PlaceCTFrame(TGeoVolume *top);
		void PlaceCTFrame(TGeoVolume *top,TGeoMatrix *ModifiedCTFramePlaceAndDirectionTransformation);
	
};

#endif /* PNDGEOHYPGEMODIFIEDCTFRAME_H */ 
