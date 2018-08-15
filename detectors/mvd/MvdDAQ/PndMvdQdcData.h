#include <TObject.h>
#include <TClonesArray.h>

class PndMvdQdcData : public TObject
{
public:
    PndMvdQdcData();
    PndMvdQdcData(UShort_t channel, Int_t qdcValue);
    virtual ~PndMvdQdcData();
    
public:
    UShort_t fChannel;      // QDC channel
    Int_t fQdc;             // raw QDC value
    
    ClassDef(PndMvdQdcData,1);
};
