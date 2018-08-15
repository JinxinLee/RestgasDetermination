/*
 * PndPattern.cxx
 *
 *  Created on: Nov 8, 2017
 *      Author: Michael Papenbrock
 */

#include "PndPattern.h"

ClassImp(PndPattern)

PndPattern::PndPattern() {
  Clear();


}

PndPattern::~PndPattern() {

}

void PndPattern::Clear() {
  ftubeIDs.clear();
  fmomenta.clear();
  fsectorID = -1;
  fcount = 0;
}
bool PndPattern::IsEmpty() {
  if (ftubeIDs.size() == 0) {
    return true;
  } else {
    return false;
  }
}

void PndPattern::AddMomenta(std::vector<TVector3> momentaVector) {
  fmomenta.insert(std::end(fmomenta), std::begin(momentaVector), std::end(momentaVector));
}
