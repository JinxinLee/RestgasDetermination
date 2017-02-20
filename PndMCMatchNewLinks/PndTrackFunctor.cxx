/*
 * PndTrackFunctor.cxx
 *
 *  Created on: 06.02.2017
 *      Author: Stockmanns
 */
#include "PndTrackFunctor.h"

PndTrackFunctor* PndTrackFunctor::make_PndTrackFunctor(std::string functorName)
{
	if (functorName.compare("StandardTrackFunctor") == 0)
		return new StandardTrackFunctor();
	if (functorName.compare("OnlySttFunctor") == 0)
		return new OnlySttFunctor();
	if (functorName.compare("RiemannMvdSttGemFunctor") == 0)
		return new RiemannMvdSttGemFunctor();
	if (functorName.compare("CircleHoughTrackFunctor") == 0)
		return new CircleHoughTrackFunctor();
	if (functorName.compare("FtsTrackFunctor") == 0)
		return new FtsTrackFunctor();
	if (functorName.compare("NoFtsTrackFunctor") == 0)
		return new NoFtsTrackFunctor();
	else
		return new AllTracksFunctor();
}

