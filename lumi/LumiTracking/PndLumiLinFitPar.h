/*
 * PndLumiLinFitPar.h
 *
 *  Created on: Jul 28, 2010
 *      Author: tsito
 */

#ifndef PNDLUMILINFITPAR_H_
#define PNDLUMILINFITPAR_H_

#include "FairParGenericSet.h"
#include "FairParamList.h"

#include<iostream>

using namespace std;

class PndLumiLinFitPar : public FairParGenericSet
{
public:

	/** Constructor*/
	PndLumiLinFitPar(const char* name = "PndLumiLinFitPar",
			const char* title = "PndLumi Linear Fit Parameters",
			const char* context = "PndLumiDefaultContext");

	/** Destructor */
	~PndLumiLinFitPar(void){}

	void clean(void){}

	void putParams(FairParamList* list);

	Bool_t getParams(FairParamList* list);

	void Print();

	/** Accessor methods */
	Double_t r11()   const {return fr11;}
	Double_t r12()   const {return fr12;}
	Double_t r13()   const {return fr13;}
	Double_t r14()   const {return fr14;}
	Double_t r21()   const {return fr21;}
	Double_t r22()   const {return fr22;}
	Double_t r23()   const {return fr23;}
	Double_t r24()   const {return fr24;}
	Double_t r31()   const {return fr31;}
	Double_t r32()   const {return fr32;}
	Double_t r33()   const {return fr33;}
	Double_t r34()   const {return fr34;}
	Double_t r41()   const {return fr41;}
	Double_t r42()   const {return fr42;}
	Double_t r43()   const {return fr43;}
	Double_t r44()   const {return fr44;}
	Double_t sigma1()   const {return fsigma1;}
	Double_t sigma2()   const {return fsigma2;}
	Double_t sigma3()   const {return fsigma3;}
	Double_t sigma4()   const {return fsigma4;}

	/** Assign methods */

private:

	Double_t fr11, fr12, fr13, fr14;
	Double_t fr21, fr22, fr23, fr24;
	Double_t fr31, fr32, fr33, fr34;
	Double_t fr41, fr42, fr43, fr44;
	Double_t fsigma1, fsigma2, fsigma3, fsigma4;

	ClassDef(PndLumiLinFitPar,1);

};

#endif /* PNDLUMILINFITPAR_H_ */
