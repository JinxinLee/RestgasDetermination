#include <stdlib.h>
#include <TObject.h>

#include "PndMvdTdcData.h"

PndMvdTdcData::PndMvdTdcData()
{
    fChannel = 0;
    fTdc = 0;
    fTrailing = 0;
}

PndMvdTdcData::PndMvdTdcData(UShort_t channel, Int_t tdcValue, Bool_t trailing)
{
    fChannel = channel;
    fTdc = tdcValue;
    fTrailing = trailing;
}

PndMvdTdcData::~PndMvdTdcData()
{
}

ClassImp(PndMvdTdcData);

