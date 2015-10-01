//-------------------------------------------------------------------------
// Author:      Oliver Merle (Oliver.Merle@exp2.physik.uni-giessen.de)
// Changes:     Mustafa Schmidt (Mustafa.A.Schmidt@physik.uni-giessen.de)
// Date:        30.11.2015
// Description: Customization of FairRoot Write out buffer
//              for time-based simulations
//-------------------------------------------------------------------------

#ifndef DISCDIRC_WRITEOUTBUFFER_HH
#define DISCDIRC_WRITEOUTBUFFER_HH

// ---------------------------
// vanilla PROOT
// ---------------------------
//#define RRTTI
#define FWB_USE_INHERITANCE
// ---------------------------


#include "FairWriteoutBuffer.h"
#include "PndDiscDigitizedHit.h"
#include <iostream>


class DiscDIRC_WriteoutBuffer : public FairWriteoutBuffer {
public:

#ifdef RRTTI
/// Functor to sort the pointers in std::map according to the target objects less function:
struct CompareDigits {
    bool operator() (const FairTimeStamp* const & p1, const FairTimeStamp* const & p2) const 
        { return (*p1) < (*p2); }
};
#endif

public:
    DiscDIRC_WriteoutBuffer();
    DiscDIRC_WriteoutBuffer(TString branchName, TString folderName, Bool_t persistance);
    virtual ~DiscDIRC_WriteoutBuffer();

    // the following should become a functor:
    virtual std::vector<std::pair<double, FairTimeStamp*> > Modify(std::pair<double, FairTimeStamp*> oldData, std::pair<double, FairTimeStamp*> newData);

    
#ifdef FWB_USE_INHERITANCE
    // Interface due to lack of templates, will be replaced by 
    // RTTI based implementations in the base class:
    virtual void AddNewDataToTClonesArray(FairTimeStamp*);
    virtual double FindTimeForData(FairTimeStamp* data);
    virtual void FillDataMap(FairTimeStamp* data, double activeTime);
    virtual void EraseDataFromDataMap(FairTimeStamp* data);
#endif

protected:  

#ifndef RRTTI
    std::map<PndDiscDigitizedHit, double> fData_map;
#else
    typedef std::map<FairTimeStamp*, double, CompareDigits> DataMap_t;
    DataMap_t fData_map;
    TBuffer * buffer;
#endif

    ClassDef(DiscDIRC_WriteoutBuffer, 1);
};

#endif //DISCDIRC_WRITEOUTBUFFER_HH
