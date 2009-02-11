
#ifndef PNDDCHDIGIPAR_H
#define PNDDCHDIGIPAR_H

#include "FairParGenericSet.h"
#include "TMap.h"

class PndDchDigiPar: public FairParGenericSet {
public:

	/** Standard constructor **/
	PndDchDigiPar(const char* name = "PndDchDigiPar", const char* title =
			"Dch Digi Parameters", const char* context = "TestDefaultContext");

	/** Destructor **/
	~PndDchDigiPar(void);

	/** Public method clear() **/
	void clear(void);

	/** Public method putParams(...) **/
	void putParams(FairParamList*);

	/** Public method gutParams(...) **/
	Bool_t getParams(FairParamList*);

	//Double_t GetVariable(const char* key) const { return fVariables; }
	TMap* GetVariables() const {
		return fVariables;
	}
	void SetVariables(TMap* Variables) {
		fVariables = Variables;
	}

	TMap *fVariables; ///< Array of Digi variables


	ClassDef(PndDchDigiPar,1)
};

#endif /* !PNDDCHDIGIPAR_H */
