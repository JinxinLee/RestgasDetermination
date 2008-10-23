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
  list->add("dimX", fDimX);
  list->add("dimY", fDimY);

  list->add("threshold", fThreshold);
  list->add("noise", fNoise);
  list->add("FECols", fFeCols);
  list->add("FERows", fFeRows);
//   list->add("sensName", sensName);
//   list->add("feName", feName);
}

Bool_t PndMvdPixelDigiPar::getParams(CbmParamList* list)
{
  if (!list) return kFALSE;
  if (!list->fill("dimX",&fDimX)) return kFALSE;
  if (!list->fill("dimY",&fDimY)) return kFALSE;

  if (!list->fill("threshold",&fThreshold)) return kFALSE;
  if (!list->fill("noise",&fNoise)) return kFALSE;
  if (!list->fill("FECols",&fFeCols)) return kFALSE;
  if (!list->fill("FERows",&fFeRows)) return kFALSE;

//   if (!list->fill("sensName",&sensName)) return kFALSE;
//   if (!list->fill("feName",&feName)) return kFALSE;
  return kTRUE;
}

ClassImp(PndMvdPixelDigiPar);

void PndMvdPixelDigiPar::Print()
{
    std::cout<<"MVD Pixel Digitization Parameters:"<<std::endl;
    std::cout<<"   fDimX (um)   = "<<fDimX<<std::endl;
    std::cout<<"   fDimY (um)   = "<<fDimY<<std::endl;
    std::cout<<"   Charge Threshold (e-)        = "<<fThreshold<<std::endl;
    std::cout<<"   Noise (ENC+Dispersion) (e-)  = "<<fNoise<<std::endl;
    std::cout<<"   Columns on FE                = "<<fFeCols<<std::endl;
    std::cout<<"   Rows on FE                   = "<<fFeRows<<std::endl;
}
