/*
 * PndMCResult.cpp
 *
 *  Created on: Dec 1, 2009
 *      Author: stockman
 */

#include "PndMCResult.h"

ClassImp(PndMCResult);

PndMCResult::PndMCResult() {
}

PndMCResult::~PndMCResult() {
}

PndMCResult::PndMCResult(fDetectorType start, fDetectorType stop)
: fStartType(start), fStopType(stop)
{}

