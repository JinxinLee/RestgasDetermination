#include <iostream>
#include "PndSdsPixelDigiPar.h"

PndSdsPixelDigiPar::PndSdsPixelDigiPar(const char* name, const char* title, const char* context)
: FairParGenericSet(name,title,context),
    fFeCols(0),
    fFeRows(0),
    fDimX(0.),
    fDimY(0.),
    fRadius(0.),
    fThreshold(0.),
    fNoise(0.),
    fCSigma(0.),
    fFeBusClock(0.),
    fChargeConvMethod(0),
    fPixelSorterCellWidth(0.),
    fPixelSorterNumberOfCells(0)
{
  clear();
}

void PndSdsPixelDigiPar::putParams(FairParamList* list)
{
  if(!list) return;
  list->add("dimX", fDimX);
  list->add("dimY", fDimY);
  
  list->add("threshold", fThreshold);
  list->add("noise", fNoise);
  list->add("FECols", fFeCols);
  list->add("FERows", fFeRows);
  list->add("ClustRad",fRadius);
  list->add("QCloudSigma",fCSigma);
  list->add("fe_BusClock", fFeBusClock);
  
  list->add("chargeconv_method", fChargeConvMethod);
  
  list->add("PixelSorterCellWidth", fPixelSorterCellWidth);
  list->add("PixelSorterNumberOfCells", fPixelSorterNumberOfCells);

  //   list->add("sensName", sensName);
  //   list->add("feName", feName);
}

Bool_t PndSdsPixelDigiPar::getParams(FairParamList* list)
{
  if (!list) return kFALSE;
  if (!list->fill("dimX",&fDimX)) return kFALSE;
  if (!list->fill("dimY",&fDimY)) return kFALSE;
  if (!list->fill("threshold",&fThreshold)) return kFALSE;
  if (!list->fill("noise",&fNoise)) return kFALSE;
  if (!list->fill("FECols",&fFeCols)) return kFALSE;
  if (!list->fill("FERows",&fFeRows)) return kFALSE;
  if (!list->fill("ClustRad",&fRadius)) return kFALSE;
  if (!list->fill("QCloudSigma",&fCSigma)) return kFALSE;
  if (!list->fill("fe_BusClock",&fFeBusClock)) return kFALSE;
  if (!list->fill("chargeconv_method",&fChargeConvMethod)) return kFALSE;
  if (!list->fill("PixelSorterCellWidth", &fPixelSorterCellWidth)) return kFALSE;
  if (!list->fill("PixelSorterNumberOfCells", &fPixelSorterNumberOfCells)) return kFALSE;
  
  //   if (!list->fill("sensName",&sensName)) return kFALSE;
  //   if (!list->fill("feName",&feName)) return kFALSE;
  return kTRUE;
}

ClassImp(PndSdsPixelDigiPar);

void PndSdsPixelDigiPar::Print(std::ostream& out) const
{
  out <<"SDS Pixel Digitization Parameters:"<<std::endl;
  out <<"   fDimX (cm)   = "<<fDimX<<std::endl;
  out <<"   fDimY (cm)   = "<<fDimY<<std::endl;
  out <<"   Charge Threshold (e-)        = "<<fThreshold<<std::endl;
  out <<"   Noise (ENC+Dispersion) (e-)  = "<<fNoise<<std::endl;
  out <<"   Columns on FE                = "<<fFeCols<<std::endl;
  out <<"   Rows on FE                   = "<<fFeRows<<std::endl;
  out <<"   Cluster search radius (channels) = "<<fRadius<<std::endl;
  out <<"   Charge cloud sigma (cm) = "<<fCSigma<<std::endl;
  out <<"   charge conv. (0:ideal, 1:TOT)    = "<<fChargeConvMethod<<std::endl;
}
