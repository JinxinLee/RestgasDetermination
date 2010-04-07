#include <iostream>
#include "PndMvdPixelDigiPar.h"

PndMvdPixelDigiPar::PndMvdPixelDigiPar(const char* name, const char* title, const char* context)
  : FairParGenericSet(name,title,context)
{
  clear();
}

void PndMvdPixelDigiPar::putParams(FairParamList* list)
{
  if(!list) return;
  list->add("dimX", fDimX);
  list->add("dimY", fDimY);

  list->add("threshold", fThreshold);
  list->add("noise", fNoise);
  list->add("FECols", fFeCols);
  list->add("FERows", fFeRows);
  list->add("ClustRad",fRadius);

  list->add("raising_time", fraisingtime);
  list->add("falling_ratio", ffallingratio);
  list->add("clock_frequency", fclockfrequency);
//   list->add("sensName", sensName);
//   list->add("feName", feName);
}

Bool_t PndMvdPixelDigiPar::getParams(FairParamList* list)
{
  if (!list) return kFALSE;
  if (!list->fill("dimX",&fDimX)) return kFALSE;
  if (!list->fill("dimY",&fDimY)) return kFALSE;

  if (!list->fill("threshold",&fThreshold)) return kFALSE;
  if (!list->fill("noise",&fNoise)) return kFALSE;
  if (!list->fill("FECols",&fFeCols)) return kFALSE;
  if (!list->fill("FERows",&fFeRows)) return kFALSE;
  if (!list->fill("ClustRad",&fRadius)) return kFALSE;

  if (!list->fill("raising_time",&fraisingtime)) return kFALSE;
  if (!list->fill("falling_ratio",&ffallingratio)) return kFALSE;
  if (!list->fill("clock_frequency",&fclockfrequency)) return kFALSE;
//   if (!list->fill("sensName",&sensName)) return kFALSE;
//   if (!list->fill("feName",&feName)) return kFALSE;
  return kTRUE;
}

ClassImp(PndMvdPixelDigiPar);

void PndMvdPixelDigiPar::Print()
{
    std::cout<<"MVD Pixel Digitization Parameters:"<<std::endl;
    std::cout<<"   fDimX (um)                       = "<<fDimX<<std::endl;
    std::cout<<"   fDimY (um)                       = "<<fDimY<<std::endl;
    std::cout<<"   Charge Threshold (e-)            = "<<fThreshold<<std::endl;
    std::cout<<"   Noise (ENC+Dispersion) (e-)      = "<<fNoise<<std::endl;
    std::cout<<"   Columns on FE                    = "<<fFeCols<<std::endl;
    std::cout<<"   Rows on FE                       = "<<fFeRows<<std::endl;
    std::cout<<"   Cluster search radius (channels) = "<<fRadius<<std::endl;

    std::cout<<"   raising time (ns)                = "<<fraisingtime<<std::endl;
    std::cout<<"   falling ratio (e/ns)             = "<<ffallingratio<<std::endl;
    std::cout<<"   clock frequency (MHz)            = "<<fclockfrequency<<std::endl;
}
