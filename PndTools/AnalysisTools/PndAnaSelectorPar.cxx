#include <iostream>
#include "PndAnaSelectorPar.h"
#include "TMath.h"

PndAnaSelectorPar::PndAnaSelectorPar(const char* name, const char* title, const char* context)
: FairParGenericSet(name,title,context),
fVeryLoose(5),
fLoose(5),
fTight(5),
fVeryTight(5),
fVariable(5)
{
  clear();
}

void PndAnaSelectorPar::clear()
{  
  fVeryLoose.Reset(0.0);
  fLoose.Reset(0.2);
  fTight.Reset(0.5);
  fVeryTight.Reset(0.9);
  fVariable.Reset(0.0);
}

void PndAnaSelectorPar::putParams(FairParamList* list)
{
  if(!list) return;
  
  list->add("VeryLoose", fVeryLoose);
  list->add("Loose", fLoose);
  list->add("Tight", fTight);
  list->add("VeryTight", fVeryTight);
  list->add("Variable", fVariable);

}

Bool_t PndAnaSelectorPar::getParams(FairParamList* list)
{
  // RTDB fills the values
  if (!list) return kFALSE;
  
  if (!list->fill("VeryLoose", &fVeryLoose)) return kFALSE;
  if (!list->fill("Loose", &fLoose)) return kFALSE;
  if (!list->fill("Tight", &fTight)) return kFALSE;
  if (!list->fill("VeryTight", &fVeryTight)) return kFALSE;
  if (!list->fill("Variable", &fVariable)) return kFALSE;
  
  return kTRUE;
}

ClassImp(PndAnaSelectorPar);

void PndAnaSelectorPar::Print()
{
  std::cout<<"Analysis PID Selection Parameters:"<<std::endl;
  std::cout<<" criterion \te\tmu\tpi\tK\tp"<<std::endl;
  std::cout<<"VeryLoose: ";for(int i=0;i<5;i++)std::cout<<"\t"<<fVeryLoose.At(i);std::cout<<std::endl;
  std::cout<<"Loose:     ";for(int i=0;i<5;i++)std::cout<<"\t"<<fLoose.At(i);std::cout<<std::endl;
  std::cout<<"Tight:     ";for(int i=0;i<5;i++)std::cout<<"\t"<<fTight.At(i);std::cout<<std::endl;
  std::cout<<"VeryTight: ";for(int i=0;i<5;i++)std::cout<<"\t"<<fVeryTight.At(i);std::cout<<std::endl;
  std::cout<<"Variable: ";for(int i=0;i<5;i++)std::cout<<"\t"<<fVariable.At(i);std::cout<<std::endl;
  
  return;
}
