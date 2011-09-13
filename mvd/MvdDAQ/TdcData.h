#include <TObject.h>
#include <TClonesArray.h>

class TdcData : public TObject
{
public:
    TdcData();
    TdcData(UShort_t channel, Int_t tdcValue, Bool_t trailing);
    virtual ~TdcData();
    
public:
    UShort_t fChannel;      // TDC channel
    Int_t fTdc;             // raw TDC value
    Bool_t fTrailing;       // leading or trailing edge
    
    ClassDef(TdcData,1);
};
