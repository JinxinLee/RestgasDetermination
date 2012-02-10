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
#include "PndLumiLinFitPar.h"

#include <iostream>
#include <iomanip>

#include "TList.h"
#include "TObjString.h"
class FairContainer;

class PndLumiContFact : public FairContFact
{
public:

	/** Constuctor */
	PndLumiContFact();

	/** Destructor */
	~PndLumiContFact();

	FairParSet* createContainer(FairContainer* c);
	TList* GetDigiParNames() {return fDigiParNames;};
	TList* GetLinFitParNames() {return fLinFitParNames;};

private:

	void setAllContainers();
	TList* fDigiParNames; //!
	TList* fLinFitParNames; //!
	ClassDef(PndLumiContFact,1)
};
#endif /* PNDLUMICONTFACT_H_ */
