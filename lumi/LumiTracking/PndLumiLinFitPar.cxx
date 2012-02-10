/*
 * PndLumiLinFitPar.cxx
 *
 *  Created on: Jul 28, 2010
 *      Author: tsito
 */
#include "PndLumiLinFitPar.h"

ClassImp(PndLumiLinFitPar);

PndLumiLinFitPar :: PndLumiLinFitPar(const char* name,const char* title,const char* context)
                 : FairParGenericSet(name,title,context)
{
	clear();
}


void PndLumiLinFitPar :: putParams (FairParamList* list)
{
	if(!list) return;
	list->add("r11",fr11);
	list->add("r12",fr12);
	list->add("r13",fr13);
	list->add("r14",fr14);
	list->add("r21",fr21);
	list->add("r22",fr22);
	list->add("r23",fr23);
	list->add("r24",fr24);
	list->add("r31",fr31);
	list->add("r32",fr32);
	list->add("r33",fr33);
	list->add("r34",fr34);
	list->add("r41",fr41);
	list->add("r42",fr42);
	list->add("r43",fr43);
	list->add("r44",fr44);
	list->add("sigma1",fsigma1);
	list->add("sigma2",fsigma2);
	list->add("sigma3",fsigma3);
	list->add("sigma4",fsigma4);

}


Bool_t PndLumiLinFitPar::getParams(FairParamList* list)
{
	if(!list) return kFALSE;
	if(!list->fill("r11", &fr11))                    return kFALSE;
	if(!list->fill("r12", &fr12))                    return kFALSE;
	if(!list->fill("r13", &fr13))                    return kFALSE;
	if(!list->fill("r14", &fr14))                    return kFALSE;
	if(!list->fill("r21", &fr21))                    return kFALSE;
	if(!list->fill("r22", &fr22))                    return kFALSE;
	if(!list->fill("r23", &fr23))                    return kFALSE;
	if(!list->fill("r24", &fr24))                    return kFALSE;
	if(!list->fill("r31", &fr31))                    return kFALSE;
	if(!list->fill("r32", &fr32))                    return kFALSE;
	if(!list->fill("r33", &fr33))                    return kFALSE;
	if(!list->fill("r34", &fr34))                    return kFALSE;
	if(!list->fill("r41", &fr41))                    return kFALSE;
	if(!list->fill("r42", &fr42))                    return kFALSE;
	if(!list->fill("r43", &fr43))                    return kFALSE;
	if(!list->fill("r44", &fr44))                    return kFALSE;
	if(!list->fill("sigma1", &fsigma1))                    return kFALSE;
	if(!list->fill("sigma2", &fsigma2))                    return kFALSE;
	if(!list->fill("sigma3", &fsigma3))                    return kFALSE;
	if(!list->fill("sigma4", &fsigma4))                    return kFALSE;

    return kTRUE;

}

void PndLumiLinFitPar::Print()
{
	cout<<" ---------------------------------------------------------- "<<endl;
	cout<<" --------  The Parameters for The Linear Fit   ------------ "<<endl;
	cout<<" ---------------------------------------------------------- "<<endl;
	cout<<" --------       Correlation Coefficients    ---------"<<endl;
	cout<<" ---------------------------------------------------------- "<<endl;
	cout<<"  \t -1- \t \t-2- \t \t-3- \t \t-4-"<<endl;
	cout<<endl;
    cout<<" -1- \t  "<<fr11<<"\t \t "<<fr12<<"\t \t "<<fr13<<"\t \t "<<fr14<<endl;
    cout<<" -2- \t  "<<fr21<<"\t \t "<<fr22<<"\t \t "<<fr23<<"\t \t "<<fr24<<endl;
    cout<<" -3- \t  "<<fr31<<"\t \t "<<fr32<<"\t \t "<<fr33<<"\t \t "<<fr34<<endl;
    cout<<" -4- \t  "<<fr41<<"\t \t "<<fr42<<"\t \t "<<fr43<<"\t \t "<<fr44<<endl;
    cout<<" ------------------------------------------------------------ "<<endl;
    cout<<" -------         Error on Reconstructed Hits          -------"<<endl;
    cout<<" ------------------------------------------------------------ "<<endl;
    cout<<" | \t "<<fsigma1<<" \t | \t "<<fsigma2<<" \t | \t "<<fsigma3<<" \t | \t  "<<fsigma4<<" \t |"<<endl;
    cout<<" ------------------------------------------------------------ "<<endl;


}
