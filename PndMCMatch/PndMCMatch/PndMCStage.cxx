/*
 * PndMCStage.cpp
 *
 *  Created on: Dec 1, 2009
 *      Author: stockman
 */

#include "PndMCStage.h"

ClassImp(PndMCStage);

PndMCStage::PndMCStage():fWeight(1.0) {
}

PndMCStage::~PndMCStage() {
}

PndMCStage::PndMCStage(fDetectorType id, std::string fileName, std::string branchName, Double_t weight)
: PndMCObject(id), fFileName(fileName), fBranchName(branchName), fWeight(weight)
{}

