#include <TObject.h>
#include <TClonesArray.h>

class QdcData : public TObject
{
public:
    QdcData();
    QdcData(UShort_t channel, Int_t qdcValue);
    virtual ~QdcData();
    
public:
    UShort_t fChannel;      // QDC channel
    Int_t fQdc;             // raw QDC value
    
    ClassDef(QdcData,1);
};
