#ifndef TPIDSELECTOR_H
#define TPIDSELECTOR_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TPidChargedSelector                                                  //
// TPidPlusSelector                                                     //
// TPidMinusSelector                                                    //
// TPidNeutralSelector                                                  //
// TPidMassSelector                                                     //
// TPidEnergySelector                                                   //
// TPidMomentumSelector                                                 //
//                                                                      //
// TPidSimpleElectronSelector                                           //
// TPidSimpleMuonSelector                                               //
// TPidSimplePionSelector                                               //
// TPidSimpleKaonSelector                                               //
// TPidSimpleProtonSelector                                             //
//                                                                      //
// Selector classes for particle selection                              //
//                                                                      //
// Author List:                                                         //
// Marcel Kunze,  RUB, Feb. 99                                          //
// Copyright (C) 1999-2001, Ruhr-University Bochum.                     //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <iostream>

#include <float.h>
#include "TNamed.h"
#include "RhoBase/VAbsPidSelector.h"

class TCandidate;
class VAbsMicroCandidate;

//----------------------------------------------------------------

class TPidChargedSelector : public VAbsPidSelector {
    
public:
    //Constructor
    TPidChargedSelector(const char *name="TPidChargedSelector", const char* type=0);
    //Destructor
    virtual ~TPidChargedSelector() {}
    
    //operations
    virtual Bool_t Accept(TCandidate& b);
    virtual Bool_t Accept(VAbsMicroCandidate& b);
    
    ClassDef(TPidChargedSelector,1)	// Charged particle selector
};      

//----------------------------------------------------------------

class TPidPlusSelector : public VAbsPidSelector {
    
public:
    //Constructor
    TPidPlusSelector(const char *name="TPidPlusSelector", const char* type=0);
    //Destructor
    virtual ~TPidPlusSelector() {}
    
    //operations
    virtual Bool_t Accept(TCandidate& b);
    virtual Bool_t Accept(VAbsMicroCandidate& b);   
    
    ClassDef(TPidPlusSelector,1)  // Positive particle selector
};      

//----------------------------------------------------------------

class TPidMinusSelector : public VAbsPidSelector {
    
public:
    //Constructor
    TPidMinusSelector(const char *name="TPidMinusSelector", const char* type=0);
    //Destructor
    virtual ~TPidMinusSelector() {}
    
    //operations
    virtual Bool_t Accept(TCandidate& b);
    virtual Bool_t Accept(VAbsMicroCandidate& b);
    
    ClassDef(TPidMinusSelector,1)  // Negative particle selector
};      

//----------------------------------------------------------------

class TPidNeutralSelector : public VAbsPidSelector {
    
public:
    //Constructor
    TPidNeutralSelector(const char *name="TPidNeutralSelector", const char* type=0);
    //Destructor
    virtual ~TPidNeutralSelector() {}
    
    //operations
    virtual Bool_t Accept(TCandidate& b);
    virtual Bool_t Accept(VAbsMicroCandidate& b);
    
    ClassDef(TPidNeutralSelector,1)  // Neutral particle selector
};      


//----------------------------------------------------------------

class TPidMassSelector : public VAbsPidSelector {
    
public:
    //Constructor
    TPidMassSelector(const char *name="TPidMassSelector", Double_t cv=0,Double_t w=DBL_MAX,const char* type=0);
    //Destructor
    virtual ~TPidMassSelector() {}
    
    //operations
    virtual Bool_t Accept(TCandidate& b);
    virtual Bool_t Accept(VAbsMicroCandidate& b);
    
private:
    Double_t centralValue,window;
    
public:
    ClassDef(TPidMassSelector,1)  // Particle selector (Mass window)
};      

//----------------------------------------------------------------

class TPidEnergySelector : public VAbsPidSelector {
    
public:
    //Constructor
    TPidEnergySelector(const char *name="TPidEnergySelector", Double_t cv=0,Double_t w=DBL_MAX,const char* type=0);
    //Destructor
    virtual ~TPidEnergySelector() {}
    
    //operations
    virtual Bool_t Accept(TCandidate& b);
    virtual Bool_t Accept(VAbsMicroCandidate& b);
    
private:
    Double_t centralValue,window;
    
public:
    ClassDef(TPidEnergySelector,1)  // Particle selector (Energy window)
};      

//----------------------------------------------------------------

class TPidMomentumSelector : public VAbsPidSelector {
    
public:
    //Constructor
    TPidMomentumSelector(const char *name="TPidMomentumSelector", Double_t cv=0,Double_t w=DBL_MAX,const char* type=0);
    //Destructor
    virtual ~TPidMomentumSelector() {}
    
    //operations
    virtual Bool_t Accept(TCandidate& b);
    virtual Bool_t Accept(VAbsMicroCandidate& b);
    
private:
    Double_t centralValue,window;
    
public:
    ClassDef(TPidMomentumSelector,1)  // Particle selector (Momentum window)
};
      
//----------------------------------------------------------------

class TPidSimpleElectronSelector : public VAbsPidSelector {
    
public:
    //Constructor
    TPidSimpleElectronSelector(const char *name="TPidSimpleElectronSelector", const char* type="e-");
    //Destructor
    virtual ~TPidSimpleElectronSelector() {}
    
    //operations
    virtual Bool_t Accept(TCandidate& b);
    virtual Bool_t Accept(VAbsMicroCandidate& b);
    
    
public:
    ClassDef(TPidSimpleElectronSelector,1)  // Particle selector (Electrons)
};      
      
//----------------------------------------------------------------

class TPidSimpleMuonSelector : public VAbsPidSelector {
    
public:
    //Constructor
    TPidSimpleMuonSelector(const char *name="TPidSimpleMuonSelector", const char* type="mu-");
    //Destructor
    virtual ~TPidSimpleMuonSelector() {}
    
    //operations
    virtual Bool_t Accept(TCandidate& b);
    virtual Bool_t Accept(VAbsMicroCandidate& b);
    
    
public:
    ClassDef(TPidSimpleMuonSelector,1)  // Particle selector (Muons)
};      
      
//----------------------------------------------------------------

class TPidSimplePionSelector : public VAbsPidSelector {
    
public:
    //Constructor
    TPidSimplePionSelector(const char *name="TPidSimplePionSelector", const char* type="pi+");
    //Destructor
    virtual ~TPidSimplePionSelector() {}
    
    //operations
    virtual Bool_t Accept(TCandidate& b);
    virtual Bool_t Accept(VAbsMicroCandidate& b);
    
    
public:
    ClassDef(TPidSimplePionSelector,1)  // Particle selector (Pions)
};      
      
//----------------------------------------------------------------

class TPidSimpleKaonSelector : public VAbsPidSelector {
    
public:
    //Constructor
    TPidSimpleKaonSelector(const char *name="TPidSimpleKaonSelector", const char* type="K+");
    //Destructor
    virtual ~TPidSimpleKaonSelector() {}
    
    //operations
    virtual Bool_t Accept(TCandidate& b);
    virtual Bool_t Accept(VAbsMicroCandidate& b);
    
    
public:
    ClassDef(TPidSimpleKaonSelector,1)  // Particle selector (Kaons)
};      
      
//----------------------------------------------------------------

class TPidSimpleProtonSelector : public VAbsPidSelector {
    
public:
    //Constructor
    TPidSimpleProtonSelector(const char *name="TPidSimpleProtonSelector", const char* type="p+");
    //Destructor
    virtual ~TPidSimpleProtonSelector() {}
    
    //operations
    virtual Bool_t Accept(TCandidate& b);
    virtual Bool_t Accept(VAbsMicroCandidate& b);
    
    
public:
    ClassDef(TPidSimpleProtonSelector,1)  // Particle selector (Protons)
};      

#endif
