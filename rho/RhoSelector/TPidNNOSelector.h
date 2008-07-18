#ifndef TPIDNNOSELECTOR_H
#define TPIDNNOSELECTOR_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TPidNNOSelector							//
//                                                                      //
// Selector class for artificial neural networks for particle           //
// identification				                        //
//                                                                      //
// Author List:                                                         //
// Marcel Kunze,  RUB, Feb. 99                                          //
// Copyright (C) 1999-2001, Ruhr-University Bochum.                     //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <iostream>

#include "RhoNNO/TXMLP.h"
#include "RhoBase/VAbsPidSelector.h"
#include "RhoSelector/TPidQuality.h"

#define NNODIMENSION 20

class TCandidate;

class TPidNNOSelector : public VAbsPidSelector {
    
public:
    
    //Constructor
    TPidNNOSelector(const char *name, Bool_t qc);
    //Destructor
    virtual ~TPidNNOSelector();
    
    //operations
    virtual Bool_t Accept(TCandidate&) = 0;
    virtual Bool_t Accept(VAbsMicroCandidate&) = 0;
    virtual void SetCriterion(const char *c);
    void SetNNOCut(Double_t cut) { fNNOCut = cut; }
    
    void Recall();
    
    Float_t* GetInput(VAbsMicroCandidate&);
    Double_t* Output(VAbsMicroCandidate&);

    inline void SetProbabilityConversion(Bool_t yesno=kFALSE) {fUseSL=yesno;}
    inline void SetRatioConversion(Bool_t yesno=kFALSE) {fUseRatio=yesno;}
    static void SetAprioriProbability(Double_t p0=1.,Double_t p1=1.,Double_t p2=1.,Double_t p3=1.,Double_t p4=1.);

    void ActivateQualityControl(const char* name) { fQC = new TPidQuality(name); }

    void SetParm(const char *key, Float_t value);
    void SetParm(const char *key, const char *value);
    Double_t GetParm(const char *key);
    Double_t GetParmValue(const char *key);

    ClassDef(TPidNNOSelector,1) // Neural Network Selector base class

protected:
    void CalculateRatio();
    void CalculateSignificanceLevel();
    static TXMLP    *fEsel,*fPisel,*fMusel,*fKasel,*fProsel; //! Do not stream
    Bool_t	    fActiveEsel,fActivePisel,fActiveMusel,fActiveKasel,fActiveProsel;	                    
    Double_t	    fPi,fMu,fE,fKa,fPro;
    Double_t	    fOut[5];
    Bool_t	    fUseSL,fUseRatio;
    Double_t	    fNNOCut;
    static Float_t  fNNOInput[NNODIMENSION];
    static Double_t fAprio[5];
    static VAbsMicroCandidate    *fInCache; //! Do not stream
    TPidQuality *fQC;		//! Produce quality plots
    TParm	    fParms;
};      

class TPidNNOElectronSelector : public TPidNNOSelector {
public:
    TPidNNOElectronSelector(const char* name="TPidNNOElectronSelector", Bool_t b1=kTRUE, Bool_t b2=kTRUE, Bool_t b3=kTRUE, Bool_t b4=kTRUE, Bool_t b5=kTRUE, Bool_t qc=kFALSE);
    virtual ~TPidNNOElectronSelector() {}
    virtual Bool_t Accept(TCandidate&);
    virtual Bool_t Accept(VAbsMicroCandidate&);
    static Double_t SignificanceLevel(Double_t output) {return AlphaError(output);}
    static Double_t AlphaError(Double_t cut);
    static Double_t BetaError(Double_t cut);
    double Consistency(Double_t output) { return  SignificanceLevel(output);  } // Backwards compatibility
    double Efficiency(Double_t cut) { return 1. - AlphaError(cut); }
    double Purity(Double_t cut) { return 1. - BetaError(cut); }
    double Misid(Double_t cut) { return BetaError(cut); }

    ClassDef(TPidNNOElectronSelector,1)    // Neural network based electron selector

private:
};      

class TPidNNOMuonSelector : public TPidNNOSelector {
public:
    TPidNNOMuonSelector(const char* name="TPidNNOMuonSelector", Bool_t b1=kTRUE, Bool_t b2=kTRUE, Bool_t b3=kTRUE, Bool_t b4=kTRUE, Bool_t b5=kTRUE, Bool_t qc=kFALSE);
    virtual ~TPidNNOMuonSelector() {}
    virtual Bool_t Accept(TCandidate&);
    virtual Bool_t Accept(VAbsMicroCandidate&);
    static Double_t SignificanceLevel(Double_t output) {return AlphaError(output);}
    static Double_t AlphaError(Double_t cut);
    static Double_t BetaError(Double_t cut);
    double Consistency(Double_t output) { return  SignificanceLevel(output);  } // Backwards compatibility
    double Efficiency(Double_t cut) { return 1. - AlphaError(cut); }
    double Purity(Double_t cut) { return 1. - BetaError(cut); }
    double Misid(Double_t cut) { return BetaError(cut); }

    ClassDef(TPidNNOMuonSelector,1)    // Neural network based muon selector

private:
};      

class TPidNNOPionSelector : public TPidNNOSelector {
public:
    TPidNNOPionSelector(const char* name="TPidNNOPionSelector", Bool_t b1=kTRUE, Bool_t b2=kTRUE, Bool_t b3=kTRUE, Bool_t b4=kTRUE, Bool_t b5=kTRUE, Bool_t qc=kFALSE);
    virtual ~TPidNNOPionSelector() {}
    virtual Bool_t Accept(TCandidate&);
    virtual Bool_t Accept(VAbsMicroCandidate&);
    static Double_t SignificanceLevel(Double_t output) {return AlphaError(output);}
    static Double_t AlphaError(Double_t cut);
    static Double_t BetaError(Double_t cut);
    double Consistency(Double_t output) { return  SignificanceLevel(output);  } // Backwards compatibility
    double Efficiency(Double_t cut) { return 1. - AlphaError(cut); }
    double Purity(Double_t cut) { return 1. - BetaError(cut); }
    double Misid(Double_t cut) { return BetaError(cut); }

    ClassDef(TPidNNOPionSelector,1)    // Neural network based pion selector

private:
};      

class TPidNNOKaonSelector : public TPidNNOSelector {
public:
    TPidNNOKaonSelector(const char* name="TPidNNOKaonSelector", Bool_t b1=kTRUE, Bool_t b2=kTRUE, Bool_t b3=kTRUE, Bool_t b4=kTRUE, Bool_t b5=kTRUE, Bool_t qc=kFALSE);
    virtual ~TPidNNOKaonSelector() {}
    virtual Bool_t Accept(TCandidate&);
    virtual Bool_t Accept(VAbsMicroCandidate&);
    static Double_t SignificanceLevel(Double_t output) {return AlphaError(output);}
    static Double_t AlphaError(Double_t cut);
    static Double_t BetaError(Double_t cut);
    double Consistency(Double_t output) { return  SignificanceLevel(output);  } // Backwards compatibility
    double Efficiency(Double_t cut) { return 1. - AlphaError(cut); }
    double Purity(Double_t cut) { return 1. - BetaError(cut); }
    double Misid(Double_t cut) { return BetaError(cut); }

    ClassDef(TPidNNOKaonSelector,1)    // Neural network based kaon selector

private:
};      

class TPidNNOProtonSelector : public TPidNNOSelector {
public:
    TPidNNOProtonSelector(const char* name="TPidNNOProtonSelector", Bool_t b1=kTRUE, Bool_t b2=kTRUE, Bool_t b3=kTRUE, Bool_t b4=kTRUE, Bool_t b5=kTRUE, Bool_t qc=kFALSE);
    virtual ~TPidNNOProtonSelector() {}
    virtual Bool_t Accept(TCandidate&);
    virtual Bool_t Accept(VAbsMicroCandidate&);
    static Double_t SignificanceLevel(Double_t output) {return AlphaError(output);}
    static Double_t AlphaError(Double_t cut);
    static Double_t BetaError(Double_t cut);
    double Consistency(Double_t output) { return  SignificanceLevel(output);  } // Backwards compatibility
    double Efficiency(Double_t cut) { return 1. - AlphaError(cut); }
    double Purity(Double_t cut) { return 1. - BetaError(cut); }
    double Misid(Double_t cut) { return BetaError(cut); }

    ClassDef(TPidNNOProtonSelector,1)    // Neural network based proton selector

private:
};      

#endif
