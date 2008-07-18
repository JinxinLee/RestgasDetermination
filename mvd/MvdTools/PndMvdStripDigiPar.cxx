#include <iostream>
#include "PndMvdStripDigiPar.h"
#include "TMath.h"

PndMvdStripDigiPar::PndMvdStripDigiPar(const char* name, const char* title, const char* context)
  : CbmParGenericSet(name,title,context)
{
  clear();
}

void PndMvdStripDigiPar::putParams(CbmParamList* list)
{
  if(!list) return;
  
  list->add("top_pitch", fTopPitch);
  list->add("bot_pitch", fBotPitch);
  list->add("orient", fOrient);
  list->add("skew", fSkew);
  list->add("top_anchor_x", (Double_t)fTopAnchor.X());
  list->add("top_anchor_y", (Double_t)fTopAnchor.Y());
  list->add("bottom_anchor_x", (Double_t)fBotAnchor.X());
  list->add("bottom_anchor_y", (Double_t)fBotAnchor.Y());
  list->add("nr_fe_channels", fFeChannels);
  list->add("nr_fe_top", fTopNrFE);
  list->add("nr_fe_bottom", fBotNrFE);
  list->add("charge_threshold", fThreshold);
  list->add("charge_noise", fNoise);
  list->add("sens_Type", fSensType); //("sens_Type", fSensType, 's', 1)
  list->add("fe_Type", fFeType); //("fe_Type", fFeType, 's', 1)
//   TObjString* sens = new TObjString( fSensType.Data() );
//   list->addBinary("sens_Type", sens); 
//   TObjString* fe = new TObjString( fFeType.Data() );
//   list->addBinary("fe_Type", fe);
}

Bool_t PndMvdStripDigiPar::getParams(CbmParamList* list)
{
  if (!list) return kFALSE;
  
  if (!list->fill("top_pitch", &fTopPitch)) return kFALSE;
  if (!list->fill("bot_pitch", &fBotPitch)) return kFALSE;
  if (!list->fill("orient",&fOrient)) return kFALSE;
  if (!list->fill("skew",&fSkew)) return kFALSE;
  Double_t x,y;
  if (!list->fill("top_anchor_x", &x)) return kFALSE;
  if (!list->fill("top_anchor_y", &y)) return kFALSE;
  fTopAnchor.Set(x,y);
  if (!list->fill("bot_anchor_x", &x)) return kFALSE;
  if (!list->fill("bot_anchor_y", &y)) return kFALSE;
  fBotAnchor.Set(x,y);
  if (!list->fill("nr_fe_channels", &fFeChannels)) return kFALSE;
  if (!list->fill("nr_fe_top", &fTopNrFE)) return kFALSE;
  if (!list->fill("nr_fe_bottom", &fBotNrFE)) return kFALSE;
  if (!list->fill("charge_threshold",&fThreshold)) return kFALSE;
  if (!list->fill("charge_noise",&fNoise)) return kFALSE;
  Text_t stName[80];
  if (!list->fill("sens_Type",stName,80)) return kFALSE;
  fSensType = stName;
  Text_t feName[80];
  if (!list->fill("fe_Type",feName,80)) return kFALSE;
  fFeType = feName;
//   TObjString sens,fe;
//   if (!list->fillBinary("sens_Type",&sens)) return kFALSE;
//   fSensType = sens.GetString();
//   if (!list->fillBinary("fe_Type",&fe)) return kFALSE;
//   fFeType = fe.GetString();
  return kTRUE;
}

ClassImp(PndMvdStripDigiPar);

void PndMvdStripDigiPar::Print()
{
    std::cout<<"MVD Digitization Parameters:"<<std::endl;
    std::cout<<"   Top Pitch    = "<<fTopPitch<<std::endl;
    std::cout<<"   Bottom Pitch = "<<fBotPitch<<std::endl;
    std::cout<<"   Strip Angle (Top) = "<<fOrient<<"rad = "<<fOrient/TMath::Pi()*180.<<" deg"<<std::endl;
    std::cout<<"   Skew Angle (Top->Bottom) = "<<fSkew<<"rad = "<<fSkew/TMath::Pi()*180.<<" deg"<<std::endl;
    std::cout<<"   Top Anchor   = ("<<fTopAnchor.X()<<","<<fTopAnchor.Y()<<")"<<std::endl;
    std::cout<<"   Bottom Anchor= ("<<fBotAnchor.X()<<","<<fBotAnchor.Y()<<")"<<std::endl;
    std::cout<<"   FE Channels  = "<<fFeChannels<<std::endl;
    std::cout<<"   Nr of Frontends (Top Side)   = "<<fTopNrFE<<std::endl;
    std::cout<<"   Nr of Frontends (Bottom Side)= "<<fBotNrFE<<std::endl;
    std::cout<<"   Charge Threshold (e-)        = "<<fThreshold<<std::endl;
    std::cout<<"   Noise (ENC+Dispersion) (e-)  = "<<fNoise<<std::endl;
    std::cout<<"   Sensor type name is          = "<<fSensType.Data()<<std::endl;
    std::cout<<"   Frontend type name is        = "<<fFeType.Data()<<std::endl;
}
