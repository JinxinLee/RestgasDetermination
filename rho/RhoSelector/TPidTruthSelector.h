#ifndef TPIDTRUTHSELECTOR_H
#define TPIDTRUTHSELECTOR_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TPidTruthElectronSelector                                            //
// TPidTruthMuonSelector                                                //
// TPidTruthPionSelector                                                //
// TPidTruthKaonSelector                                                //
// TPidTruthProtonSelector                                              //
//                                                                      //
// Selector classes for particle selection                              //
//                                                                      //
// Author List:                                                         //
// Marcel Kunze,  RUB, Feb. 99                                          //
// Copyright (C) 1999-2001, Ruhr-University Bochum.                     //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "RhoBase/VAbsPidSelector.h"

//----------------------------------------------------------------

class TPidTruthElectronSelector : public VAbsPidSelector {
    
public:
    //Constructor
    TPidTruthElectronSelector(const char *name="TPidTruthElectronSelector", Bool_t qc=kFALSE);
    //Destructor
    virtual ~TPidTruthElectronSelector();
    
    //operations
    virtual Bool_t Accept(TCandidate& b);
    virtual Bool_t Accept(VAbsMicroCandidate& b);
    
    ClassDef(TPidTruthElectronSelector,1)  // MC truth particle selector
};      

//----------------------------------------------------------------

class TPidTruthMuonSelector : public VAbsPidSelector {
    
public:
    //Constructor
    TPidTruthMuonSelector(const char *name="TPidTruthMuonSelector", Bool_t qc=kFALSE);
    //Destructor
    virtual ~TPidTruthMuonSelector();
    
    //operations
    virtual Bool_t Accept(TCandidate& b);
    virtual Bool_t Accept(VAbsMicroCandidate& b);
    
    ClassDef(TPidTruthMuonSelector,1)  // MC truth particle selector
};      

//----------------------------------------------------------------

class TPidTruthPionSelector : public VAbsPidSelector {
    
public:
    //Constructor
    TPidTruthPionSelector(const char *name="TPidTruthPionSelector", Bool_t qc=kFALSE);
    //Destructor
    virtual ~TPidTruthPionSelector();
    
    //operations
    virtual Bool_t Accept(TCandidate& b);
    virtual Bool_t Accept(VAbsMicroCandidate& b);
    
    ClassDef(TPidTruthPionSelector,1)  // MC truth particle selector          
};      

//----------------------------------------------------------------

class TPidTruthKaonSelector : public VAbsPidSelector {
    
public:
    //Constructor
    TPidTruthKaonSelector(const char *name="TPidTruthKaonSelector", Bool_t qc=kFALSE);
    //Destructor
    virtual ~TPidTruthKaonSelector();
    
    //operations
    virtual Bool_t Accept(TCandidate& b);
    virtual Bool_t Accept(VAbsMicroCandidate& b);
    
    ClassDef(TPidTruthKaonSelector,1)  // MC truth particle selector
};      

//----------------------------------------------------------------

class TPidTruthProtonSelector : public VAbsPidSelector {
    
public:
    //Constructor
    TPidTruthProtonSelector(const char *name="TPidTruthProtonSelector", Bool_t qc=kFALSE);
    //Destructor
    virtual ~TPidTruthProtonSelector();
    
    //operations
    virtual Bool_t Accept(TCandidate& b);
    virtual Bool_t Accept(VAbsMicroCandidate& b);
    
    ClassDef(TPidTruthProtonSelector,1)  // MC truth particle selector
};      


#endif
