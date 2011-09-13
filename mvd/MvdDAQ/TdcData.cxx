#include <stdlib.h>
#include <TObject.h>

#include "TdcData.h"

TdcData::TdcData()
{
    fChannel = 0;
    fTdc = 0;
    fTrailing = 0;
}

TdcData::TdcData(UShort_t channel, Int_t tdcValue, Bool_t trailing)
{
    fChannel = channel;
    fTdc = tdcValue;
    fTrailing = trailing;
}

TdcData::~TdcData()
{
}

ClassImp(TdcData);

