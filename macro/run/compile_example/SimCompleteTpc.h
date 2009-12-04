#ifndef SIMCOMPLETETPC_H
#define SIMCOMPLETETPC_H


#include <TString.h>

void SimCompleteTpc(Int_t nEvents, TString const &simEngine, Double_t momentum,
    Bool_t useEvtGen, Bool_t useDpm, Bool_t useBoxGenerator,
    Double_t beamMomentum, TString const &outFile, TString const &outParamsFile,
    TString const &inDigiParamsFile);


#endif
