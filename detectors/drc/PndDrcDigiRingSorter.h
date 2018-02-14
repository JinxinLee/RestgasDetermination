/*
 *      Author: stockman
 * PndDrcDigiRingSorter.h
 *
 */
// -------------------------------------------------------------------------
// -----                   PndDrcDigiRingSorter header file              -----
// -----         HARPHOOL KUMAWAT h.kumawat@gsi.de                     -----
// -----                                                               -----
// -------------------------------------------------------------------------

#ifndef PNDDRCDigiRINGSORTER_H_
#define PNDDRCDigiRINGSORTER_H_

#include <FairRingSorter.h>

class PndDrcDigiRingSorter: public FairRingSorter {
public:
	PndDrcDigiRingSorter(int size = 100, double width = 10):FairRingSorter(size, width){};
	virtual ~PndDrcDigiRingSorter();

	virtual FairTimeStamp* CreateElement(FairTimeStamp* data);

	ClassDef (PndDrcDigiRingSorter,1);
};

#endif /* PNDDrcDigiRINGSORTER_H_ */
