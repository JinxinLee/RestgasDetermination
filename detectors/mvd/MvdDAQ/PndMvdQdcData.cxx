#include <stdlib.h>
#include <TObject.h>

#include "PndMvdQdcData.h"

PndMvdQdcData::PndMvdQdcData() :
    fChannel(0),
    fQdc(0)
{}

PndMvdQdcData::PndMvdQdcData(UShort_t channel, Int_t qdcValue) :
    fChannel(channel),
    fQdc(qdcValue)
{}

PndMvdQdcData::~PndMvdQdcData()
{
}

ClassImp(PndMvdQdcData);

