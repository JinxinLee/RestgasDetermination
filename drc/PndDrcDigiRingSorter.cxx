/*
 * PndDrcDigiRingSorter.cxx
 *
 */
// -------------------------------------------------------------------------
// -----                   PndDrcDigiRingSorter source file            -----
// -----         HARPHOOL KUMAWAT h.kumawat@gsi.de                     -----
// -----                                                               -----
// -------------------------------------------------------------------------

#include <PndDrcDigiRingSorter.h>
#include "PndDrcDigi.h"


ClassImp(PndDrcDigiRingSorter);


PndDrcDigiRingSorter::~PndDrcDigiRingSorter() {
	// TODO Auto-generated destructor stub
}

FairTimeStamp* PndDrcDigiRingSorter::CreateElement(FairTimeStamp* data) {
	return new PndDrcDigi(*(PndDrcDigi*)data);
}
