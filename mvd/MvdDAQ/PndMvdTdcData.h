#include <TObject.h>
#include <TClonesArray.h>

class PndMvdTdcData : public TObject
{
public:
    PndMvdTdcData();
    PndMvdTdcData(UShort_t channel, Int_t tdcValue, Bool_t trailing);
    virtual ~PndMvdTdcData();
    
public:
    UShort_t fChannel;      // TDC channel
    Int_t fTdc;             // raw TDC value
    Bool_t fTrailing;       // leading or trailing edge
    
    ClassDef(PndMvdTdcData,1);
};
