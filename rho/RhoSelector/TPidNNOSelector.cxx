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

#include <stdlib.h>
#include "TString.h"
#include "TDatabasePDG.h"
#include "RhoBase/TRho.h"
#include "RhoBase/TCandidate.h"
#include "RhoBase/VAbsMicroCandidate.h"
#include "RhoSelector/TPidNNOSelector.h"
#include "RhoSelector/TPidQuality.h"

ClassImp(TPidNNOSelector)

TBuffer &operator>>(TBuffer &buf, TPidNNOSelector *&obj)
{
   obj = (TPidNNOSelector *) buf.ReadObject(TPidNNOSelector::Class());
   return buf;
}

#include <iostream>
using namespace std;

TXMLP* TPidNNOSelector::fEsel;
TXMLP* TPidNNOSelector::fPisel;
TXMLP* TPidNNOSelector::fMusel;
TXMLP* TPidNNOSelector::fKasel;
TXMLP* TPidNNOSelector::fProsel;

Float_t  TPidNNOSelector::fNNOInput[NNODIMENSION];
Double_t TPidNNOSelector::fAprio[5];
VAbsMicroCandidate *TPidNNOSelector::fInCache;

TPidNNOSelector::TPidNNOSelector(const char *name, Bool_t qc) :
VAbsPidSelector(name), fNNOCut(0.5), fQC(0)  
{
    TString baseDirectory = TString(TRho::Instance()->GetParameterStore()) + "/Networks/";
    TString path;

    if (fEsel==0) {
	path = baseDirectory + "NNOElectronSelector.xmlp";
	fEsel    = new TXMLP(path.Data());
    }

    if (fPisel==0) {
	path = baseDirectory + "NNOPionSelector.xmlp";
	fPisel   = new TXMLP(path.Data());
    }

    if (fMusel==0) {
	path = baseDirectory + "NNOMuonSelector.xmlp";
	fMusel   = new TXMLP(path.Data());
    }

    if (fKasel==0) {
	path = baseDirectory + "NNOKaonSelector.xmlp";
	fKasel   = new TXMLP(path.Data());
    }

    if (fProsel==0) {
	path = baseDirectory + "NNOProtonSelector.xmlp";
	fProsel  = new TXMLP(path.Data());
    }
    
    for (int i=0;i<NNODIMENSION;i++) fNNOInput[i] = 0.0;
    fInCache = 0;
    
    SetAprioriProbability(1.,1.,1.,1.,1.);
    SetProbabilityConversion(kFALSE);
    SetRatioConversion(kFALSE);

    if (qc) ActivateQualityControl(name);
}


TPidNNOSelector::~TPidNNOSelector( ) {  
    if ( fActiveEsel)    { delete fEsel; fEsel=0; }
    if ( fActivePisel )  { delete fPisel; fPisel=0; }
    if ( fActiveMusel )  { delete fMusel; fMusel=0; }
    if ( fActiveKasel )  { delete fKasel; fKasel=0; }
    if ( fActiveProsel ) { delete fProsel; fProsel=0; }

    delete fQC;
}

void TPidNNOSelector::SetCriterion(const char *c) 
{
    TString crit(c);

    if (crit=="best")
	fNNOCut = 0.0;
    else if (crit=="veryLoose")
	fNNOCut = 0.1;
    else if (crit=="loose")
	fNNOCut = 0.3;
    else if (crit=="tight")
	fNNOCut = 0.7;
    else if (crit=="veryTight")
	fNNOCut = 0.9;
    else if (crit=="variable")
	fNNOCut = 0.5;
    else
	cerr << GetName() << ": Invalid criterion: " << c << endl;

    VAbsPidSelector::SetCriterion(c);
}

void TPidNNOSelector::SetAprioriProbability(Double_t p0,Double_t p1,Double_t p2,Double_t p3,Double_t p4)
    { fAprio[0] = p0; fAprio[1] = p1; fAprio[2] = p2; fAprio[3] = p3; fAprio[4] = p4; }


void TPidNNOSelector::CalculateRatio()
{
    // Protect against negative values

    if (fOut[0] <= 0.0) fOut[0] = 1.E-6; 
    if (fOut[1] <= 0.0) fOut[1] = 1.E-6; 
    if (fOut[2] <= 0.0) fOut[2] = 1.E-6; 
    if (fOut[3] <= 0.0) fOut[3] = 1.E-6; 
    if (fOut[4] <= 0.0) fOut[4] = 1.E-6; 

    // Calculate the conditional probabilities

    Double_t ratio[5];
    Double_t sum = fAprio[0]*fOut[0]+fAprio[1]*fOut[1]+fAprio[2]*fOut[2]+fAprio[3]*fOut[3]+fAprio[4]*fOut[4];
    if (sum>0) {
        for (int i=0;i<5;i++) ratio[i] = fAprio[i]*fOut[i] / sum;
    }
    else {
        cerr << "TPidNNOSelector: Error in ratio calculation." << endl;
        for (int i=0;i<5;i++) ratio[i] = 0.;
    }
        
    // Redefine the network outputs
        
    fE   = fOut[0] = ratio[0];
    fMu  = fOut[1] = ratio[1];
    fPi  = fOut[2] = ratio[2];
    fKa  = fOut[3] = ratio[3];
    fPro = fOut[4] = ratio[4];
        
}

void TPidNNOSelector::CalculateSignificanceLevel()
{
    fE   = fOut[0] = TPidNNOElectronSelector::SignificanceLevel(fOut[0]);
    fMu  = fOut[1] = TPidNNOMuonSelector::SignificanceLevel(fOut[1]);
    fPi  = fOut[2] = TPidNNOPionSelector::SignificanceLevel(fOut[2]);
    fKa  = fOut[3] = TPidNNOKaonSelector::SignificanceLevel(fOut[3]);
    fPro = fOut[4] = TPidNNOProtonSelector::SignificanceLevel(fOut[4]);
}

void TPidNNOSelector::Recall() {
    
    if ( fActiveMusel ) {
        fMusel->Recallstep(fNNOInput);
        fMu  = fMusel->GetOutput()[0];
    } 
    else fMu = 0;
    
    if ( fActivePisel ) {
        fPisel->Recallstep(fNNOInput);
        fPi  = fPisel->GetOutput()[0];	
    } 
    else fPi = 0;
    
    if ( fActiveEsel ) {
        fEsel->Recallstep(fNNOInput);
        fE   = fEsel->GetOutput()[0];
    } 
    else fE = 0;
    
    if ( fActiveKasel ) {
        fKasel->Recallstep(fNNOInput);
        fKa  = fKasel->GetOutput()[0];
    } 
    else fKa = 0;
    
    if ( fActiveProsel ) {
        fProsel->Recallstep(fNNOInput);
        fPro = fProsel->GetOutput()[0];
    } 
    else fPro = 0;
    
    fOut[0]=fE;fOut[1]=fMu;fOut[2]=fPi;fOut[3]=fKa;fOut[4]=fPro; 

    if (fUseSL) CalculateSignificanceLevel(); // Convert to probability
    if (fUseRatio) CalculateRatio();          // Convert to weighted ratios
}    

Float_t* TPidNNOSelector::GetInput(VAbsMicroCandidate& b) {

    if (fInCache == &b) return fNNOInput; // Just return the cache
    fInCache = &b;                       // Update the cache for a new track
    
    if (b.GetCharge()!=0) {
      Double_t mom   = b.GetMomentum().Mag();
      Double_t theta = b.GetMomentum().Z() / mom;
      fNNOInput[0] =   (Float_t) mom;
      fNNOInput[1] =   (Float_t) TMath::ACos(theta);
      fNNOInput[2] =   (Float_t) 0.1*b.GetSvtMeanDEdx();
      fNNOInput[3] =   (Float_t) b.GetEmcRawEnergy();
      fNNOInput[4] =   (Float_t) 10.*b.GetDrcThetaC();
      fNNOInput[5] =   (Float_t) 0.05*b.GetDchMeanDEdx();
      fNNOInput[6] =   (Float_t) 0.25*(Float_t) b.GetIfrMeasuredInteractionLengths();
      fNNOInput[7] =   (Float_t) 0.25*(Float_t) b.GetIfrExpectedInteractionLengths();
      fNNOInput[8] =   (Float_t) 0.25*(Float_t) b.GetIfrInteractionLengthsBeforeIron();
      fNNOInput[9] =   (Float_t) 0.25*(Float_t) b.GetIfrNumberOfStrips();
      fNNOInput[10]=   (Float_t) 0.25*(Float_t) b.GetIfrFirstHit();
      fNNOInput[11]=   (Float_t) 0.25*(Float_t) b.GetIfrLastHit();
      fNNOInput[12]=   (Float_t) b.GetEmcS1S9();
      fNNOInput[13]=   (Float_t) b.GetEmcS9S25();
/*      fNNOInput[14]=   (Float_t) b.GetEmcZernikeMoment20();
      fNNOInput[15]=   (Float_t) b.GetEmcZernikeMoment31();
      fNNOInput[16]=   (Float_t) b.GetEmcZernikeMoment42();
      fNNOInput[17]=   (Float_t) b.GetEmcLateralShape();
      fNNOInput[18]=   (Float_t) b.GetCharge();
      fNNOInput[19]=   0;
*/    } else {
      fNNOInput[0] =  0; fNNOInput[1] =  0; fNNOInput[2] =  0;
      fNNOInput[3] =  0; fNNOInput[4] =  0; fNNOInput[5] =  0;
      fNNOInput[6] =  0; fNNOInput[7] =  0; fNNOInput[8] =  0;
      fNNOInput[9] =  0; fNNOInput[10] =  0; fNNOInput[11] =  0;
      fNNOInput[12] =  0; fNNOInput[13] =  0; fNNOInput[14] =  0;
      fNNOInput[15] =  0; fNNOInput[16] =  0; fNNOInput[17] =  0;
      fNNOInput[18] =  0; fNNOInput[19] =  0;
    }

    return fNNOInput;
}

Double_t* TPidNNOSelector::Output(VAbsMicroCandidate& b) {
    
    GetInput(b);          // Get the input vector
    Recall();             // Recall all networks
    
    return fOut;
}

// Parameter setting

void TPidNNOSelector::SetParm(const char *key, Float_t value) 
{ 
    fParms.Set(key,(Double_t)value,"TPidNNOSelector",this); 
}
    
void TPidNNOSelector::SetParm(const char *key, const char *value) 
{ 
    fParms.Set(key,value,"TPidNNOSelector",this); 
}
    
Double_t TPidNNOSelector::GetParm(const char *key) 
{ 
    return fParms.GetDouble(key,this); 
}
    
Double_t TPidNNOSelector::GetParmValue(const char *key) 
{ 
    return GetParm(key); 
}

//----------------------------------------------------------------

ClassImp(TPidNNOPionSelector)

TBuffer &operator>>(TBuffer &buf, TPidNNOPionSelector *&obj)
{
   obj = (TPidNNOPionSelector *) buf.ReadObject(TPidNNOPionSelector::Class());
   return buf;
}

TPidNNOPionSelector::TPidNNOPionSelector(const char* name, Bool_t b1, Bool_t b2, Bool_t b3, Bool_t b4, Bool_t b5, Bool_t qc ) :
TPidNNOSelector(name,qc)
{
    fActiveEsel   = b1;
    fActiveMusel  = b2;
    fActivePisel  = b3;
    fActiveKasel  = b4;
    fActiveProsel = b5; 
}

Bool_t TPidNNOPionSelector::Accept(TCandidate& b) 
{
    if (&b == 0) return kFALSE;
    Bool_t decision = Accept(b.GetMicroCandidate());
    if (fQC) fQC->Control(b,decision);	    // Delegate quality control
    if (!decision) return kFALSE;

    // Set the particle type
    static TParticlePDG *plus  = TRho::Instance()->GetPDG()->GetParticle("pi+");
    static TParticlePDG *minus = TRho::Instance()->GetPDG()->GetParticle("pi-");
    if (b.GetCharge()==1) {
	b.SetMass(plus->Mass());
	b.SetType(plus);
    }
    else {
	b.SetMass(minus->Mass());
	b.SetType(minus);
    }
    return kTRUE;
}

Bool_t TPidNNOPionSelector::Accept(VAbsMicroCandidate& b) {
    
    if (&b == 0) return kFALSE;
    if (b.GetCharge()==0) return kFALSE; // Wrong particle type
    
    GetInput(b);
    
    Bool_t decision = kFALSE;	
    if (fNNOCut <= 0) {
        Recall();
        decision = (fPi>fMu && fPi>fE && fPi>fKa && fPi>fPro );
    } 
    else {
        fPisel->Recallstep(fNNOInput);
        Double_t output = fPisel->GetOutput()[0];
        if (fUseSL) output = TPidNNOPionSelector::SignificanceLevel(output);
        decision = ( output > fNNOCut );
    }
    
    return decision;
}

//----------------------------------------------------------------

ClassImp(TPidNNOMuonSelector)

TBuffer &operator>>(TBuffer &buf, TPidNNOMuonSelector *&obj)
{
   obj = (TPidNNOMuonSelector *) buf.ReadObject(TPidNNOMuonSelector::Class());
   return buf;
}

TPidNNOMuonSelector::TPidNNOMuonSelector(const char* name, Bool_t b1, Bool_t b2, Bool_t b3, Bool_t b4, Bool_t b5, Bool_t qc ) :
TPidNNOSelector(name,qc)
{
    fActiveEsel   = b1;
    fActiveMusel  = b2;
    fActivePisel  = b3;
    fActiveKasel  = b4;
    fActiveProsel = b5; 
}

Bool_t TPidNNOMuonSelector::Accept(TCandidate& b) 
{
    if (&b == 0) return kFALSE;
    Bool_t decision = Accept(b.GetMicroCandidate());
    if (fQC) fQC->Control(b,decision);	    // Delegate quality control
    if (!decision) return kFALSE;

    // Set the particle type
    static TParticlePDG *plus  = TRho::Instance()->GetPDG()->GetParticle("mu+");
    static TParticlePDG *minus = TRho::Instance()->GetPDG()->GetParticle("mu-");
    if (b.GetCharge()==1) {
	b.SetMass(plus->Mass());
	b.SetType(plus);
    }
    else {
	b.SetMass(minus->Mass());
	b.SetType(minus);
    }
    return kTRUE;
}

Bool_t TPidNNOMuonSelector::Accept(VAbsMicroCandidate& b) {
    
    if (&b == 0) return kFALSE;
    if (b.GetCharge()==0) return kFALSE; // Wrong particle type
    
    GetInput(b);
    
    Bool_t decision = kFALSE;	
    if (fNNOCut <= 0) {
        Recall();
        decision = (fMu>fPi && fMu>fE && fMu>fKa && fMu>fPro );
    } 
    else {
        fMusel->Recallstep(fNNOInput);
        Double_t output = fMusel->GetOutput()[0];
        if (fUseSL) output = TPidNNOMuonSelector::SignificanceLevel(output);
        decision = ( output > fNNOCut );
    }        
    
    return decision;
}

//----------------------------------------------------------------

ClassImp(TPidNNOElectronSelector)

TBuffer &operator>>(TBuffer &buf, TPidNNOElectronSelector *&obj)
{
   obj = (TPidNNOElectronSelector *) buf.ReadObject(TPidNNOElectronSelector::Class());
   return buf;
}

TPidNNOElectronSelector::TPidNNOElectronSelector(const char* name, Bool_t b1, Bool_t b2, Bool_t b3, Bool_t b4, Bool_t b5, Bool_t qc ) :
TPidNNOSelector(name,qc)
{
    fActiveEsel   = b1;
    fActiveMusel  = b2;
    fActivePisel  = b3;
    fActiveKasel  = b4;
    fActiveProsel = b5; 
}

Bool_t TPidNNOElectronSelector::Accept(TCandidate& b) 
{
    if (&b == 0) return kFALSE;
    Bool_t decision = Accept(b.GetMicroCandidate());
    if (fQC) fQC->Control(b,decision);	    // Delegate quality control
    if (!decision) return kFALSE;

    // Set the particle type
    static TParticlePDG *plus  = TRho::Instance()->GetPDG()->GetParticle("e+");
    static TParticlePDG *minus = TRho::Instance()->GetPDG()->GetParticle("e-");
    if (b.GetCharge()==1) {
	b.SetMass(plus->Mass());
	b.SetType(plus);
    }
    else {
	b.SetMass(minus->Mass());
	b.SetType(minus);
    }
    return kTRUE;
}

Bool_t TPidNNOElectronSelector::Accept(VAbsMicroCandidate& b) {
    
    if (&b == 0) return kFALSE;
    if (b.GetCharge()==0) return kFALSE; // Wrong particle type
    
    GetInput(b);
    
    Bool_t decision = kFALSE;	
    if (fNNOCut <= 0) {
        Recall();
        decision = (fE>fPi && fE>fMu && fE>fKa && fE>fPro );
    } 
    else {
        fEsel->Recallstep(fNNOInput);
        Double_t output = fEsel->GetOutput()[0];
        if (fUseSL) output = TPidNNOElectronSelector::SignificanceLevel(output);
        decision = ( output > fNNOCut );
    }        
    
    return decision;
}

//----------------------------------------------------------------

ClassImp(TPidNNOKaonSelector)

TBuffer &operator>>(TBuffer &buf, TPidNNOKaonSelector *&obj)
{
   obj = (TPidNNOKaonSelector *) buf.ReadObject(TPidNNOKaonSelector::Class());
   return buf;
}

TPidNNOKaonSelector::TPidNNOKaonSelector(const char* name, Bool_t b1, Bool_t b2, Bool_t b3, Bool_t b4, Bool_t b5, Bool_t qc ) :
TPidNNOSelector(name,qc)
{
    fActiveEsel   = b1;
    fActiveMusel  = b2;
    fActivePisel  = b3;
    fActiveKasel  = b4;
    fActiveProsel = b5; 
}

Bool_t TPidNNOKaonSelector::Accept(TCandidate& b) 
{
    if (&b == 0) return kFALSE;
    Bool_t decision = Accept(b.GetMicroCandidate());
    if (fQC) fQC->Control(b,decision);	    // Delegate quality control
    if (!decision) return kFALSE;

    // Set the particle type
    static TParticlePDG *plus  = TRho::Instance()->GetPDG()->GetParticle("K+");
    static TParticlePDG *minus = TRho::Instance()->GetPDG()->GetParticle("K-");
    if (b.GetCharge()==1) {
	b.SetMass(plus->Mass());
	b.SetType(plus);
    }
    else {
	b.SetMass(minus->Mass());
	b.SetType(minus);
    }
    return kTRUE;
}

Bool_t TPidNNOKaonSelector::Accept(VAbsMicroCandidate& b) {
    
    if (&b == 0) return kFALSE;
    if (b.GetCharge()==0) return kFALSE; // Wrong particle type
    
    GetInput(b);
    
    Bool_t decision = kFALSE;	
    if (fNNOCut <= 0) {
        Recall();
        decision = (fKa>fPi && fKa>fMu && fKa>fE && fKa>fPro );
    }
    else {
        fKasel->Recallstep(fNNOInput);
        Double_t output = fKasel->GetOutput()[0];
        if (fUseSL) output = TPidNNOKaonSelector::SignificanceLevel(output);
        decision = ( output > fNNOCut );
    }        
    
    return decision;
}

//----------------------------------------------------------------

ClassImp(TPidNNOProtonSelector)

TBuffer &operator>>(TBuffer &buf, TPidNNOProtonSelector *&obj)
{
   obj = (TPidNNOProtonSelector *) buf.ReadObject(TPidNNOProtonSelector::Class());
   return buf;
}

TPidNNOProtonSelector::TPidNNOProtonSelector(const char* name, Bool_t b1, Bool_t b2, Bool_t b3, Bool_t b4, Bool_t b5, Bool_t qc ) :
TPidNNOSelector(name,qc)
{
    fActiveEsel   = b1;
    fActiveMusel  = b2;
    fActivePisel  = b3;
    fActiveKasel  = b4;
    fActiveProsel = b5; 
}

Bool_t TPidNNOProtonSelector::Accept(TCandidate& b) 
{
    if (&b == 0) return kFALSE;
    Bool_t decision = Accept(b.GetMicroCandidate());
    if (fQC) fQC->Control(b,decision);	    // Delegate quality control
    if (!decision) return kFALSE;

    // Set the particle type
    static TParticlePDG *plus  = TRho::Instance()->GetPDG()->GetParticle("p+");
    static TParticlePDG *minus = TRho::Instance()->GetPDG()->GetParticle("anti-p-");
    if (b.GetCharge()==1) {
	b.SetMass(plus->Mass());
	b.SetType(plus);
    }
    else {
	b.SetMass(minus->Mass());
	b.SetType(minus);
    }
    return kTRUE;
}

Bool_t TPidNNOProtonSelector::Accept(VAbsMicroCandidate& b) {
    
    if (&b == 0) return kFALSE;
    if (b.GetCharge()==0) return kFALSE; // Wrong particle type
    
    GetInput(b);
    
    Bool_t decision = kFALSE;	
    if (fNNOCut <= 0) {
        Recall();    
        decision = (fPro>fPi && fPro>fMu && fPro>fE && fPro>fKa );
    } 
    else {
        fProsel->Recallstep(fNNOInput);
        Double_t output = fProsel->GetOutput()[0];
        if (fUseSL) output = TPidNNOProtonSelector::SignificanceLevel(output);
        decision = ( output > fNNOCut );
    }
    
    return decision;
}

//----------------------------------------------------------------

// Here come the error lookup tables

Double_t TPidNNOElectronSelector::AlphaError(Double_t cut) {
    const int nbins=200;
    const Double_t low=-0.002500;
    const Double_t bw=0.005000;
    static const Double_t alpha[nbins]={
0.000000,0.001209,0.003364,0.005046,0.006045,0.007884,0.008988,0.009829,0.010880,0.011879,
0.013298,0.014297,0.015506,0.016873,0.017819,0.019711,0.021340,0.023127,0.024757,0.026281,
0.028752,0.031275,0.036689,0.044731,0.049409,0.053193,0.056505,0.060026,0.063758,0.066439,
0.069225,0.072168,0.074481,0.077319,0.080473,0.083206,0.085729,0.088305,0.091248,0.094192,
0.096978,0.099448,0.102444,0.105493,0.108331,0.111958,0.114849,0.118423,0.121787,0.125519,
0.128515,0.131301,0.134507,0.137819,0.141866,0.145230,0.148594,0.151380,0.154954,0.158528,
0.160788,0.162891,0.165887,0.169041,0.172089,0.174770,0.177924,0.180342,0.183127,0.185493,
0.188646,0.192063,0.195164,0.198055,0.201104,0.204100,0.207043,0.210144,0.213876,0.217135,
0.220920,0.225282,0.228331,0.232116,0.235900,0.238528,0.241787,0.245098,0.248620,0.251511,
0.254455,0.257976,0.261340,0.264757,0.268384,0.271800,0.275690,0.278318,0.281472,0.284678,
0.287464,0.290723,0.292878,0.295769,0.298554,0.301813,0.305493,0.309014,0.311695,0.314639,
0.318265,0.321419,0.324520,0.328936,0.332510,0.335926,0.337976,0.341761,0.344441,0.347543,
0.350328,0.353903,0.357424,0.360683,0.363417,0.366781,0.370039,0.373929,0.379501,0.383863,
0.388173,0.391748,0.396163,0.398896,0.403101,0.406255,0.409724,0.414717,0.418134,0.423390,
0.427806,0.432851,0.437792,0.441997,0.447148,0.452037,0.458344,0.463022,0.469382,0.475322,
0.481787,0.488936,0.496452,0.503758,0.512957,0.522365,0.531458,0.541550,0.552799,0.564520,
0.575979,0.588331,0.602733,0.618397,0.631695,0.643889,0.654349,0.663758,0.670433,0.676426,
0.681156,0.686360,0.690670,0.695401,0.701077,0.707017,0.712746,0.720000,0.727201,0.734612,
0.741130,0.747805,0.755480,0.761892,0.766570,0.771669,0.777608,0.782076,0.786544,0.791590,
0.797687,0.802891,0.809356,0.817240,0.825388,0.834008,0.844783,0.858922,0.884047,0.952431,
    };
    
    int n=(int)((cut-low)/bw);
    if (n<0) n = 0;
    if (n>nbins-1) n = nbins-1;
    return alpha[n];
}

Double_t TPidNNOElectronSelector::BetaError(Double_t cut) {
    const int nbins=200;
    const Double_t low=-0.002500;
    const Double_t bw=0.005000;
    static const Double_t beta[nbins]={
1.000000,0.612529,0.520361,0.482854,0.458716,0.440916,0.425563,0.412532,0.401471,0.391979,
0.383625,0.376102,0.369410,0.363394,0.357610,0.351779,0.346379,0.340287,0.334641,0.328718,
0.322103,0.313780,0.298057,0.276919,0.264765,0.255796,0.248688,0.242304,0.236981,0.231150,
0.226504,0.220950,0.216335,0.211735,0.206474,0.202135,0.197797,0.193520,0.189505,0.185905,
0.182305,0.178920,0.175582,0.172182,0.168567,0.165013,0.161459,0.158228,0.154674,0.150875,
0.147721,0.144398,0.141013,0.137767,0.134767,0.131906,0.128844,0.126167,0.123798,0.120983,
0.118506,0.116337,0.114352,0.112614,0.110552,0.108691,0.106460,0.104352,0.102398,0.100475,
0.098460,0.096460,0.094675,0.092875,0.091029,0.088983,0.087245,0.085522,0.083799,0.082168,
0.080491,0.078799,0.077183,0.075414,0.074106,0.072476,0.071060,0.069199,0.067245,0.065907,
0.064876,0.063507,0.061861,0.060353,0.058830,0.057722,0.056414,0.055076,0.053845,0.052768,
0.051707,0.050676,0.049784,0.048999,0.048092,0.047015,0.046169,0.045276,0.044569,0.043692,
0.042846,0.041984,0.041199,0.040615,0.039969,0.039292,0.038369,0.037984,0.037446,0.036830,
0.036184,0.035723,0.035246,0.034692,0.034169,0.033692,0.033153,0.032661,0.031600,0.030831,
0.030261,0.029723,0.029307,0.028831,0.028261,0.027661,0.027031,0.026461,0.025892,0.025446,
0.025015,0.024369,0.023769,0.023277,0.022785,0.022092,0.021646,0.021231,0.020631,0.019938,
0.019231,0.018477,0.017631,0.016954,0.016323,0.015646,0.014908,0.013923,0.012985,0.011831,
0.010708,0.009539,0.008677,0.007539,0.006769,0.005985,0.005339,0.004816,0.004508,0.004277,
0.004123,0.003893,0.003677,0.003554,0.003370,0.003093,0.002846,0.002723,0.002462,0.002169,
0.002031,0.001877,0.001739,0.001600,0.001539,0.001354,0.001216,0.001108,0.001031,0.000893,
0.000816,0.000816,0.000693,0.000662,0.000600,0.000462,0.000339,0.000216,0.000108,0.000000,
    };
    
    int n=(int)((cut-low)/bw);
    if (n<0) n = 0;
    if (n>nbins-1) n = nbins-1;
    return beta[n];
}


Double_t TPidNNOMuonSelector::AlphaError(Double_t cut) {
    const int nbins=200;
    const Double_t low=-0.002500;
    const Double_t bw=0.005000;
    static const Double_t alpha[nbins]={
0.000000,0.001960,0.004679,0.006702,0.007903,0.009358,0.011065,0.012709,0.014858,0.016755,
0.019664,0.022888,0.026113,0.032562,0.041667,0.052099,0.060129,0.066958,0.074861,0.082385,
0.089593,0.097180,0.104894,0.112355,0.119626,0.127908,0.136697,0.145739,0.153768,0.161482,
0.168816,0.174949,0.182031,0.188923,0.194992,0.201189,0.207638,0.216932,0.226669,0.236280,
0.246206,0.252466,0.258283,0.264416,0.270359,0.276492,0.281677,0.287367,0.293184,0.298558,
0.303933,0.309497,0.314365,0.320498,0.326442,0.332069,0.338771,0.344208,0.350152,0.355526,
0.361469,0.367286,0.371775,0.376960,0.380754,0.385243,0.390048,0.394474,0.399216,0.403579,
0.408068,0.412936,0.416920,0.421219,0.426783,0.431082,0.435255,0.440503,0.445878,0.451125,
0.456942,0.462759,0.469841,0.476985,0.485647,0.492982,0.502972,0.510053,0.516060,0.522699,
0.530159,0.536672,0.545650,0.555261,0.567147,0.584788,0.607107,0.634168,0.660660,0.684876,
0.700873,0.707132,0.711052,0.715225,0.718513,0.721421,0.724393,0.726733,0.728945,0.730716,
0.732929,0.734573,0.736153,0.737608,0.738999,0.740390,0.742160,0.744183,0.746080,0.747408,
0.748988,0.750506,0.752339,0.754363,0.755880,0.757651,0.758789,0.760875,0.762772,0.764922,
0.766439,0.768336,0.770106,0.772446,0.773900,0.775291,0.777441,0.779464,0.781045,0.783194,
0.785091,0.787620,0.789644,0.791351,0.793943,0.795524,0.798242,0.800835,0.802984,0.804881,
0.807474,0.809623,0.811583,0.812911,0.814302,0.815883,0.817653,0.819550,0.822205,0.824165,
0.826062,0.827453,0.829224,0.830741,0.833207,0.835041,0.837443,0.839213,0.841426,0.844145,
0.846358,0.848698,0.851859,0.854831,0.858055,0.861596,0.864757,0.868045,0.871206,0.874557,
0.877276,0.879805,0.882967,0.886128,0.889416,0.892640,0.895991,0.899659,0.904274,0.909269,
0.913822,0.919828,0.924633,0.932347,0.940567,0.949229,0.956816,0.965605,0.975342,0.987165,
    };
    
    int n=(int)((cut-low)/bw);
    if (n<0) n = 0;
    if (n>nbins-1) n = nbins-1;
    return alpha[n];
}

Double_t TPidNNOMuonSelector::BetaError(Double_t cut) {
    const int nbins=200;
    const Double_t low=-0.002500;
    const Double_t bw=0.005000;
    static const Double_t beta[nbins]={
1.000000,0.832693,0.737018,0.705806,0.686058,0.670913,0.656253,0.644084,0.631359,0.618033,
0.605205,0.592039,0.577540,0.556575,0.527958,0.500088,0.479329,0.461091,0.444334,0.428793,
0.413370,0.400660,0.388037,0.375312,0.360592,0.345888,0.332151,0.320188,0.308811,0.298446,
0.288228,0.279314,0.270679,0.261853,0.254332,0.246342,0.238968,0.230611,0.221155,0.211597,
0.204633,0.198563,0.193564,0.187876,0.182964,0.178288,0.174021,0.169990,0.166090,0.162337,
0.158628,0.154713,0.151210,0.146987,0.143146,0.139114,0.135303,0.132136,0.128925,0.126140,
0.123281,0.120481,0.117989,0.115540,0.113312,0.111039,0.108576,0.106363,0.104281,0.102434,
0.100293,0.097977,0.095719,0.093461,0.091497,0.089591,0.087553,0.085281,0.083346,0.081103,
0.078977,0.076954,0.074681,0.071940,0.069213,0.067102,0.064609,0.062161,0.059830,0.057572,
0.055139,0.052720,0.050169,0.047545,0.044070,0.039540,0.033778,0.027855,0.021009,0.016097,
0.012901,0.011626,0.011040,0.010453,0.009911,0.009529,0.008987,0.008533,0.008210,0.007873,
0.007594,0.007492,0.007272,0.006993,0.006803,0.006627,0.006392,0.006304,0.006158,0.006055,
0.005835,0.005688,0.005586,0.005351,0.005190,0.004999,0.004838,0.004735,0.004589,0.004515,
0.004398,0.004295,0.004164,0.004076,0.003988,0.003929,0.003826,0.003753,0.003650,0.003548,
0.003328,0.003196,0.003035,0.002829,0.002668,0.002536,0.002477,0.002316,0.002243,0.002155,
0.002067,0.001979,0.001891,0.001803,0.001744,0.001656,0.001568,0.001539,0.001495,0.001363,
0.001319,0.001260,0.001128,0.001084,0.001070,0.000982,0.000938,0.000908,0.000865,0.000835,
0.000747,0.000718,0.000659,0.000571,0.000498,0.000483,0.000439,0.000410,0.000351,0.000337,
0.000322,0.000307,0.000307,0.000293,0.000278,0.000263,0.000249,0.000219,0.000219,0.000205,
0.000175,0.000146,0.000131,0.000117,0.000087,0.000014,0.000014,-0.000001,-0.000001,-0.000001,
    };
    
    int n=(int)((cut-low)/bw);
    if (n<0) n = 0;
    if (n>nbins-1) n = nbins-1;
    return beta[n];
}


Double_t TPidNNOPionSelector::AlphaError(Double_t cut) {
    const int nbins=200;
    const Double_t low=-0.002500;
    const Double_t bw=0.005000;
    static const Double_t alpha[nbins]={
0.000000,0.000950,0.001850,0.003900,0.007650,0.009850,0.011100,0.014850,0.024200,0.031900,
0.037850,0.041700,0.045100,0.048400,0.051850,0.054800,0.057700,0.060550,0.063800,0.067150,
0.069850,0.072800,0.076550,0.081300,0.085550,0.091850,0.101500,0.110500,0.119600,0.125350,
0.132100,0.137400,0.141400,0.145650,0.149500,0.153700,0.159350,0.163550,0.167500,0.170850,
0.174500,0.178400,0.182800,0.186850,0.190650,0.193800,0.197250,0.201000,0.203900,0.207300,
0.211350,0.215000,0.219550,0.222650,0.225850,0.229650,0.232700,0.235600,0.239000,0.242350,
0.246150,0.249100,0.252300,0.255650,0.258650,0.262100,0.265700,0.270300,0.273500,0.276450,
0.279850,0.283300,0.286900,0.290600,0.294250,0.297850,0.300750,0.304300,0.307950,0.312200,
0.315750,0.319200,0.323350,0.326300,0.329700,0.334000,0.337650,0.342150,0.346200,0.350200,
0.353850,0.357750,0.362700,0.367300,0.371200,0.375550,0.379500,0.384050,0.388750,0.394250,
0.398400,0.403100,0.406650,0.411500,0.415650,0.420100,0.424050,0.427550,0.431050,0.436300,
0.439450,0.443300,0.446950,0.451150,0.454950,0.457850,0.461550,0.466500,0.470900,0.474850,
0.479000,0.483100,0.487200,0.490750,0.494950,0.499050,0.503750,0.507550,0.511950,0.517050,
0.521000,0.525400,0.529250,0.533500,0.538000,0.542450,0.546550,0.550250,0.555850,0.561200,
0.565600,0.570800,0.575700,0.581350,0.586700,0.592450,0.598700,0.603800,0.609900,0.615700,
0.621600,0.627650,0.635100,0.640600,0.648250,0.653900,0.660600,0.668700,0.675350,0.683700,
0.692100,0.700400,0.708000,0.716650,0.726600,0.736350,0.746950,0.756700,0.766500,0.778950,
0.791150,0.803400,0.818150,0.835550,0.854100,0.874050,0.898450,0.921500,0.948000,0.960750,
0.965150,0.968900,0.972100,0.974450,0.976050,0.977850,0.979800,0.982000,0.984100,0.985650,
0.987550,0.988800,0.991100,0.993300,0.995350,0.997100,0.998450,0.999900,1.000000,1.000000,
    };
    
    int n=(int)((cut-low)/bw);
    if (n<0) n = 0;
    if (n>nbins-1) n = nbins-1;
    return alpha[n];
}

Double_t TPidNNOPionSelector::BetaError(Double_t cut) {
    const int nbins=200;
    const Double_t low=-0.002500;
    const Double_t bw=0.005000;
    static const Double_t beta[nbins]={
1.000000,0.819120,0.768344,0.742433,0.692125,0.664855,0.643739,0.610502,0.542936,0.500843,
0.475229,0.456268,0.440727,0.428326,0.416487,0.405367,0.395511,0.385906,0.376300,0.366445,
0.356058,0.345250,0.333771,0.320854,0.306985,0.291413,0.274607,0.258504,0.244682,0.233421,
0.223206,0.216318,0.210118,0.205260,0.199669,0.193437,0.186518,0.181723,0.177397,0.173273,
0.169540,0.165714,0.161981,0.158592,0.155171,0.151923,0.148861,0.145894,0.143207,0.140662,
0.137991,0.135648,0.133258,0.130947,0.128979,0.127167,0.124934,0.122856,0.120920,0.119014,
0.116609,0.114969,0.113063,0.110970,0.108940,0.107128,0.105770,0.104051,0.102474,0.100912,
0.099241,0.097617,0.095899,0.094165,0.092837,0.091369,0.089870,0.088370,0.086699,0.084950,
0.083529,0.082310,0.081061,0.079718,0.078312,0.077125,0.076016,0.074751,0.073376,0.071893,
0.070706,0.069581,0.068050,0.066754,0.065708,0.064427,0.063052,0.061709,0.060366,0.059085,
0.058023,0.056618,0.055431,0.054384,0.053509,0.052463,0.051510,0.050589,0.049761,0.048902,
0.048027,0.047106,0.046497,0.045450,0.044685,0.044185,0.043482,0.042639,0.041936,0.040921,
0.040280,0.039609,0.038828,0.037906,0.037063,0.036454,0.035829,0.035173,0.034611,0.034033,
0.033236,0.032471,0.031924,0.031300,0.030691,0.029972,0.029332,0.028801,0.028129,0.027614,
0.027005,0.026208,0.025505,0.024755,0.024162,0.023475,0.022772,0.022194,0.021725,0.021132,
0.020398,0.019679,0.018945,0.018258,0.017665,0.016993,0.016290,0.015525,0.014838,0.014338,
0.013666,0.012916,0.012229,0.011448,0.010542,0.009808,0.008996,0.008309,0.007528,0.006809,
0.006169,0.005466,0.004716,0.003936,0.003045,0.002327,0.001421,0.000952,0.000468,0.000296,
0.000265,0.000265,0.000234,0.000171,0.000140,0.000125,0.000125,0.000125,0.000109,0.000093,
0.000093,0.000093,0.000078,0.000047,0.000047,0.000015,0.000015,-0.000000,-0.000000,-0.000000,
    };
    
    int n=(int)((cut-low)/bw);
    if (n<0) n = 0;
    if (n>nbins-1) n = nbins-1;
    return beta[n];
}


Double_t TPidNNOKaonSelector::AlphaError(Double_t cut) {
    const int nbins=200;
    const Double_t low=-0.002500;
    const Double_t bw=0.005000;
    static const Double_t alpha[nbins]={
0.000000,0.003054,0.007787,0.018323,0.023566,0.026467,0.028961,0.031252,0.032524,0.034204,
0.035323,0.037003,0.038377,0.039803,0.040770,0.041889,0.042958,0.043773,0.044892,0.046012,
0.046826,0.047895,0.048812,0.049931,0.050898,0.051611,0.052883,0.054105,0.055785,0.057566,
0.059297,0.060925,0.062758,0.064692,0.066931,0.068560,0.070545,0.072937,0.075075,0.077518,
0.079961,0.082608,0.084542,0.086985,0.089530,0.092177,0.094467,0.096860,0.099150,0.101695,
0.104087,0.107243,0.110399,0.113300,0.115641,0.117881,0.120120,0.122512,0.124803,0.127399,
0.129078,0.131572,0.133608,0.136153,0.138393,0.140887,0.143075,0.145417,0.148216,0.150506,
0.152695,0.154833,0.157887,0.160075,0.162620,0.164554,0.166743,0.169441,0.171782,0.174327,
0.177483,0.180384,0.185983,0.188578,0.192040,0.194483,0.196824,0.200132,0.203237,0.205578,
0.208378,0.210719,0.213773,0.216674,0.219626,0.222426,0.225582,0.228635,0.231588,0.234540,
0.237746,0.240291,0.242836,0.245686,0.247926,0.250216,0.252812,0.256375,0.259022,0.262177,
0.265588,0.268133,0.271492,0.274291,0.277345,0.279941,0.282944,0.285743,0.289205,0.292767,
0.295465,0.298417,0.301776,0.304983,0.308291,0.311193,0.314552,0.317860,0.320711,0.323968,
0.327175,0.330585,0.333588,0.336896,0.340154,0.343818,0.347738,0.351300,0.355067,0.357968,
0.362549,0.366061,0.368911,0.373034,0.377310,0.382247,0.386777,0.390747,0.395887,0.400112,
0.405151,0.409986,0.415840,0.420420,0.426274,0.430804,0.435690,0.440220,0.445412,0.450094,
0.455082,0.458747,0.463277,0.466891,0.471217,0.476460,0.481193,0.486130,0.490660,0.496870,
0.502265,0.507711,0.511987,0.518400,0.523388,0.528376,0.533415,0.539624,0.545834,0.551484,
0.557541,0.563292,0.568891,0.575406,0.582990,0.591083,0.598616,0.605894,0.614140,0.622538,
0.631954,0.641828,0.654349,0.667379,0.683667,0.699547,0.720364,0.745457,0.782969,0.835904,
    };
    
    int n=(int)((cut-low)/bw);
    if (n<0) n = 0;
    if (n>nbins-1) n = nbins-1;
    return alpha[n];
}

Double_t TPidNNOKaonSelector::BetaError(Double_t cut) {
    const int nbins=200;
    const Double_t low=-0.002500;
    const Double_t bw=0.005000;
    static const Double_t beta[nbins]={
1.000000,0.778484,0.585579,0.400348,0.321223,0.282825,0.258174,0.240777,0.228227,0.218736,
0.210426,0.203451,0.197782,0.191802,0.184921,0.179127,0.174824,0.170335,0.166825,0.163718,
0.161248,0.158344,0.155486,0.152767,0.150220,0.148201,0.145498,0.142888,0.140030,0.137514,
0.134873,0.131673,0.128738,0.125895,0.122711,0.119713,0.116917,0.113981,0.111247,0.108731,
0.105966,0.103419,0.100794,0.098215,0.096320,0.094177,0.092421,0.090153,0.088134,0.086239,
0.084406,0.082573,0.080865,0.079141,0.077758,0.076329,0.074962,0.073657,0.072337,0.071079,
0.069976,0.068594,0.067351,0.066031,0.064990,0.063825,0.062784,0.061759,0.061014,0.059895,
0.059072,0.058187,0.057317,0.056354,0.055546,0.054769,0.053931,0.053045,0.052300,0.051492,
0.050451,0.049286,0.047422,0.046195,0.045186,0.044269,0.043337,0.042390,0.041613,0.040790,
0.040122,0.039454,0.038677,0.038025,0.037326,0.036642,0.035866,0.035213,0.034468,0.033660,
0.033054,0.032417,0.031796,0.031346,0.030818,0.030119,0.029497,0.029062,0.028519,0.027851,
0.027494,0.026965,0.026499,0.025971,0.025381,0.024791,0.024387,0.023812,0.023284,0.022818,
0.022414,0.021948,0.021280,0.020768,0.020100,0.019711,0.019121,0.018609,0.018158,0.017692,
0.017382,0.017071,0.016760,0.016403,0.015828,0.015549,0.015160,0.014725,0.014275,0.013840,
0.013374,0.013048,0.012706,0.012333,0.012038,0.011619,0.011262,0.010858,0.010547,0.010081,
0.009693,0.009335,0.008901,0.008668,0.008388,0.008139,0.007673,0.007301,0.007130,0.006772,
0.006524,0.006244,0.005903,0.005639,0.005390,0.005173,0.004955,0.004707,0.004365,0.004210,
0.004023,0.003790,0.003588,0.003355,0.003231,0.003122,0.002982,0.002765,0.002656,0.002548,
0.002361,0.002113,0.001911,0.001817,0.001693,0.001600,0.001538,0.001429,0.001320,0.001181,
0.000963,0.000808,0.000699,0.000575,0.000544,0.000435,0.000342,0.000218,0.000124,0.000031,
    };
    
    int n=(int)((cut-low)/bw);
    if (n<0) n = 0;
    if (n>nbins-1) n = nbins-1;
    return beta[n];
}


Double_t TPidNNOProtonSelector::AlphaError(Double_t cut) {
    const int nbins=200;
    const Double_t low=-0.002500;
    const Double_t bw=0.005000;
    static const Double_t alpha[nbins]={
0.000000,0.006291,0.009331,0.012267,0.016356,0.019501,0.022437,0.025477,0.027888,0.030824,
0.034913,0.037849,0.040889,0.044559,0.049381,0.053995,0.059761,0.063221,0.067624,0.071294,
0.075907,0.081673,0.087020,0.090900,0.095722,0.100860,0.106416,0.110505,0.115748,0.122038,
0.126022,0.131579,0.135248,0.140386,0.145838,0.151919,0.160306,0.165339,0.171839,0.177186,
0.182952,0.188614,0.196687,0.203082,0.209163,0.215559,0.223422,0.229922,0.237681,0.246907,
0.254980,0.265884,0.276892,0.287167,0.296288,0.307716,0.318725,0.327951,0.334452,0.340533,
0.344307,0.346404,0.348186,0.349864,0.351122,0.352799,0.353848,0.355420,0.356993,0.358671,
0.361187,0.362969,0.364227,0.366324,0.368631,0.370518,0.372720,0.374187,0.375446,0.376704,
0.378591,0.379430,0.380688,0.381317,0.382470,0.383938,0.384462,0.385511,0.386245,0.387293,
0.388027,0.389075,0.391067,0.392745,0.393898,0.395995,0.397987,0.399455,0.401237,0.402391,
0.404697,0.407633,0.410464,0.412141,0.415601,0.419166,0.426400,0.431852,0.437933,0.442651,
0.446110,0.449151,0.452716,0.457119,0.459530,0.462885,0.467079,0.470329,0.473894,0.477564,
0.481862,0.486895,0.493185,0.505557,0.527784,0.543615,0.552317,0.561648,0.568358,0.574649,
0.580939,0.587545,0.592472,0.597295,0.601803,0.606416,0.612078,0.618054,0.624974,0.630006,
0.636087,0.641958,0.650136,0.658314,0.665338,0.669952,0.673516,0.677081,0.681275,0.685783,
0.689767,0.692388,0.696058,0.699413,0.702663,0.705494,0.709268,0.713357,0.717656,0.722164,
0.725414,0.728664,0.733487,0.737261,0.742294,0.745859,0.750262,0.754561,0.758125,0.761900,
0.764730,0.767876,0.769658,0.772908,0.775110,0.777521,0.780562,0.785280,0.789159,0.792095,
0.795240,0.799119,0.802055,0.805200,0.808975,0.814112,0.817257,0.821556,0.824911,0.828790,
0.834347,0.839589,0.846299,0.852799,0.860768,0.867792,0.877962,0.893689,0.920738,0.967708,
    };
    
    int n=(int)((cut-low)/bw);
    if (n<0) n = 0;
    if (n>nbins-1) n = nbins-1;
    return alpha[n];
}

Double_t TPidNNOProtonSelector::BetaError(Double_t cut) {
    const int nbins=200;
    const Double_t low=-0.002500;
    const Double_t bw=0.005000;
    static const Double_t beta[nbins]={
1.000000,0.450999,0.380826,0.340162,0.315554,0.297108,0.282180,0.270983,0.260673,0.251651,
0.242657,0.234857,0.227500,0.220210,0.213739,0.207335,0.201576,0.195938,0.190447,0.185198,
0.180915,0.176015,0.171638,0.167168,0.162912,0.159368,0.155340,0.151206,0.147393,0.143782,
0.140331,0.137244,0.133485,0.130061,0.126718,0.123416,0.119778,0.116475,0.112609,0.108890,
0.105386,0.101909,0.098405,0.094753,0.091115,0.087692,0.084107,0.080590,0.077033,0.073757,
0.070025,0.066091,0.062037,0.058318,0.054344,0.050344,0.046155,0.042490,0.039698,0.037791,
0.036449,0.035657,0.034959,0.034489,0.034005,0.033536,0.033133,0.032717,0.032354,0.032005,
0.031495,0.031200,0.030783,0.030314,0.029871,0.029441,0.029119,0.028810,0.028488,0.028058,
0.027978,0.027723,0.027468,0.027320,0.027172,0.027025,0.026890,0.026769,0.026622,0.026353,
0.026071,0.026004,0.025816,0.025548,0.025266,0.025064,0.024729,0.024487,0.024232,0.023937,
0.023735,0.023386,0.023198,0.022970,0.022581,0.022151,0.021520,0.020889,0.019896,0.019547,
0.019211,0.018902,0.018365,0.017922,0.017627,0.017305,0.017063,0.016848,0.016526,0.016123,
0.015855,0.015466,0.015063,0.013975,0.012861,0.011774,0.010740,0.010230,0.009720,0.009250,
0.008726,0.008323,0.008203,0.008028,0.007787,0.007531,0.007129,0.006847,0.006659,0.006377,
0.005961,0.005598,0.005142,0.004753,0.004444,0.004216,0.004028,0.003773,0.003611,0.003531,
0.003450,0.003383,0.003330,0.003182,0.003074,0.002967,0.002860,0.002739,0.002631,0.002524,
0.002403,0.002162,0.001960,0.001866,0.001745,0.001625,0.001544,0.001504,0.001410,0.001316,
0.001249,0.001155,0.001114,0.001074,0.001020,0.000940,0.000859,0.000832,0.000792,0.000779,
0.000738,0.000698,0.000618,0.000591,0.000551,0.000510,0.000457,0.000430,0.000336,0.000309,
0.000282,0.000255,0.000202,0.000175,0.000161,0.000121,0.000094,0.000094,0.000054,0.000040,
    };
    
    int n=(int)((cut-low)/bw);
    if (n<0) n = 0;
    if (n>nbins-1) n = nbins-1;
    return beta[n];
}

