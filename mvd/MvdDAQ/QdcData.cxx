#include <stdlib.h>
#include <TObject.h>

#include "QdcData.h"

QdcData::QdcData()
{
    fChannel = 0;
    fQdc = 0;
}

QdcData::QdcData(UShort_t channel, Int_t qdcValue)
{
    fChannel = channel;
    fQdc = qdcValue;
}

QdcData::~QdcData()
{
}

ClassImp(QdcData);

