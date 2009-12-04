// Example of a compiled program to perform a full simulation run: a compiled
// version of the sim_complete_tpc.C macro.
//
// Elwin Dijck, December 2009


#include "SimCompleteTpc.h"

#include <TRint.h>
#include <TROOT.h>

#include <iostream>
#include <sstream>


using namespace std;


template <typename Type>
Type StrTo(char const *str);


int main(int argc, char *argv[])
{
    // Start application.
    gROOT->SetBatch();
    TApplication app("SimCompleteTpc", &argc, argv, 0, -1);

    // Read the command line options or provide default values.
    Int_t nEvents = argc > 1 ? StrTo<Int_t>(argv[1]) : 10;
    TString const &simEngine = argc > 2 ? argv[2] : "TGeant3";
    Double_t momentum = argc > 3 ? StrTo<Double_t>(argv[3]) : 7.24;
    Bool_t useEvtGen = argc > 4 ? StrTo<Bool_t>(argv[4]) : kTRUE;
    Bool_t useDpm = argc > 5 ? StrTo<Bool_t>(argv[5]) : kFALSE;
    Bool_t useBoxGenerator = argc > 6 ? StrTo<Bool_t>(argv[6]) : kFALSE;
    Double_t beamMomentum = argc > 7 ? StrTo<Double_t>(argv[7]) : 15.0;
    TString outFile= argc > 8 ? argv[8] : "sim_complete.root";
    TString outParamsFile= argc > 9 ? argv[9] : "simparams.root";
    TString inDigiParamsFile = argc > 10 ? argv[10] : "emc.par";

    cout << boolalpha;

    cout << endl << "Starting full simulation with TPC:" << endl
        << "    # events               : " << nEvents << endl
        << "    sim engine             : " << simEngine << endl
        << "    momentum               : " << momentum << "GeV/c" << endl
        << "    using EvtGen           : " << useEvtGen << endl
        << "    using Dpm              : " << useDpm << endl
        << "    using BoxGenerator     : " << useBoxGenerator << endl
        << "    beam momentum          : " << beamMomentum << "GeV/c" << endl
        << "    output file            : " << outFile << endl
        << "    output params file     : " << outParamsFile << endl
        << "    input digi params file : " << inDigiParamsFile << endl << endl;

    // Start the simulation.
    SimCompleteTpc(nEvents, simEngine, momentum, useEvtGen, useDpm,
        useBoxGenerator, beamMomentum, outFile, outParamsFile,
        inDigiParamsFile);
    
    return 0;
}


// Helper function to convert an argument to any type.
template <typename Type>
Type StrTo(char const *str)
{
    istringstream iss(str);

    iss >> boolalpha;  // Use true/false instead of 1/0 for booleans.

    Type result;
    iss >> result;

    return result;
}
