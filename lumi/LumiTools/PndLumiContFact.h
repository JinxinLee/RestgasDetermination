/*
 * PndLumiContFact.h
 *
 *       Author: tsito
 *
 */


#ifndef PNDLUMICONTFACT_H_
#define PNDLUMICONTFACT_H_

#include "FairContFact.h"
#include "FairRuntimeDb.h"
#include "FairParRootFileIo.h"
#include "FairParAsciiFileIo.h"

#include "PndLumiDigiPara.h"
#include "PndLumiGeoPar.h"

#include <iostream>
#include <iomanip>

class FairContainer;

class PndLumiContFact : public FairContFact
{
public:

	/** Constuctor */
	PndLumiContFact();

	/** Destructor */
	~PndLumiContFact(){}

	FairParSet* createContainer(FairContainer* c);

private:

	void setAllContainers();

	ClassDef(PndLumiContFact,1)
};
#endif /* PNDLUMICONTFACT_H_ */
