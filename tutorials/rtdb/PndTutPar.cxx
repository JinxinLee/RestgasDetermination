#include <iostream>
#include "PndTutPar.h"
#include "TMath.h"

PndTutPar::PndTutPar(const char* name, const char* title, const char* context)
  : FairParGenericSet(name,title,context)
{
  clear();
}

void PndTutPar::putParams(FairParamList* list)
{
  std::cout<<"-I- PndTutPar::putParams() called"<<std::endl;
  if(!list) return;
  list->add("top_pitch", fTopPitch);
  list->add("top_anchor_x", (Double_t)fTopAnchor.X());
  list->add("top_anchor_y", (Double_t)fTopAnchor.Y());
  list->add("top_anchor_z", (Double_t)fTopAnchor.Z());
  list->add("nr_fe_top", fTopNrFE);
  list->add("fe_Type", fFeType);
}

Bool_t PndTutPar::getParams(FairParamList* list)
{
  std::cout<<"-I- PndTutPar::getParams() called"<<std::endl;
  if (!list) return kFALSE;
  std::cout<<"-I- PndTutPar::getParams() 1 ";
  
  if (!list->fill("top_pitch", &fTopPitch)) return kFALSE;
  std::cout<<"2 ";
  Double_t x,y,z;
  if (!list->fill("top_anchor_x", &x)) return kFALSE;
  if (!list->fill("top_anchor_y", &y)) return kFALSE;
  if (!list->fill("top_anchor_z", &z)) return kFALSE;
  fTopAnchor.SetXYZ(x,y,z);
  std::cout<<"3 ";
  if (!list->fill("nr_fe_top", &fTopNrFE)) return kFALSE;
  Text_t feName[80];
  std::cout<<"4 "<<std::endl;
  if (!list->fill("fe_Type",feName,80)) return kFALSE;
  fFeType = feName;
  return kTRUE;
}

ClassImp(PndTutPar);

void PndTutPar::Print()
{
    std::cout<<"Tutorial Parameters:"<<std::endl;
    std::cout<<"   Top Pitch    = "<<fTopPitch<<std::endl;
    std::cout<<"   Top Anchor   = ("<<fTopAnchor.X()<<","<<fTopAnchor.Y()<<","<<fTopAnchor.Z()<<")"<<std::endl;
    std::cout<<"   Nr of Frontends (Top Side)   = "<<fTopNrFE<<std::endl;
    std::cout<<"   Frontend type name is        = "<<fFeType.Data()<<std::endl;
}
