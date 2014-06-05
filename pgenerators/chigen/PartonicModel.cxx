/* 
 * @author Alexey Luchinsky
 * @author Stanislav Poslavsky (stvlpos (at) mail.ru)
 */

#include "PartonicModel.h"

chigen::PartonicModel::PartonicModel(double ecm, int pdg, EvtId evt,
        EvtSpinDensity spin, double m) {
    chigen::ensure_chigen_is_initialized();
    eCM = ecm;
    pdgId = pdg;
    evtId = evt;
    spinDensity = spin;
    mesonMass = m;
    mesonMass2 = m*m;
}

chigen::PartonicModel::~PartonicModel() {
}

bool chigen::PartonicModel::next() {
    nCall = 0;
    // trying 1000 attempts
    for (int i = 0; i < 1000; ++i) {
        ++nCall;
        if (!setKin()) {
            continue;
        };
        setSigma();
        if (sigma > MaxSigma) {
            //todo add proper info
            __chigen_cout__ <<
                    "sigma=" << setiosflags(ios::scientific) << sigma <<
                    " > MasSigma=" << setiosflags(ios::scientific) << MaxSigma <<
                    " Efct=" << sigma / MaxSigma << endl;
            MaxSigma = sigma;
            continue;
        };
        if (sigma / MaxSigma > chigen::random::random_engine->flat()) return true;

    }
    sigma = -1;
    return false;
}

ClassImp(chigen::PartonicModel);