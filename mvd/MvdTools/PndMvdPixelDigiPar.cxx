#include <iostream>
#include "PndMvdPixelDigiPar.h"

PndMvdPixelDigiPar::PndMvdPixelDigiPar(const char* name, const char* title, const char* context)
  : CbmParGenericSet(name,title,context)
{
  clear();
}

void PndMvdPixelDigiPar::putParams(CbmParamList* list)
{
  if(!list) return;
  list->add("dimX", dimX);
  list->add("dimY", dimY);
  
  list->add("threshold", threshold);
  list->add("noise", noise);
  list->add("FECols", _feCols);
  list->add("FERows", _feRows);
//   list->add("sensName", sensName);
//   list->add("feName", feName);
}

Bool_t PndMvdPixelDigiPar::getParams(CbmParamList* list)
{
  if (!list) return kFALSE;
  if (!list->fill("dimX",&dimX)) return kFALSE;
  if (!list->fill("dimY",&dimY)) return kFALSE;
  
  if (!list->fill("threshold",&threshold)) return kFALSE;
  if (!list->fill("noise",&noise)) return kFALSE;
  if (!list->fill("FECols",&_feCols)) return kFALSE;
  if (!list->fill("FERows",&_feRows)) return kFALSE;
    
//   if (!list->fill("sensName",&sensName)) return kFALSE;
//   if (!list->fill("feName",&feName)) return kFALSE;
  return kTRUE;
}

ClassImp(PndMvdPixelDigiPar);

void PndMvdPixelDigiPar::Print()
{
    std::cout<<"MVD Pixel Digitization Parameters:"<<std::endl;
    std::cout<<"   dimX (um)   = "<<dimX<<std::endl;
    std::cout<<"   dimY (um)   = "<<dimY<<std::endl;
    std::cout<<"   Charge Threshold (e-)        = "<<threshold<<std::endl;
    std::cout<<"   Noise (ENC+Dispersion) (e-)  = "<<noise<<std::endl;
    std::cout<<"   Columns on FE                = "<<_feCols<<std::endl;
    std::cout<<"   Rows on FE                   = "<<_feRows<<std::endl;
}
