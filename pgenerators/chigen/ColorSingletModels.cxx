/* 
 * @author Alexey Luchinsky
 * @author Stanislav Poslavsky (stvlpos (at) mail.ru)
 */

#include <stdexcept>

#include "ColorSingletModels.h"
#include "EvtGenBase/EvtComplex.hh"

EvtSpinDensity chigen::models::createSpinDensity(int spin, int projection) {
    assert(spin == 1 || spin == 2);
    assert(-1 <= projection);
    assert(2 >= projection);

    chigen::ensure_chigen_is_initialized();
    
    //state = 9   -> unpolarized
    //state = 10  -> (1,0)
    //state = 11  -> (1,1)
    //state = 20  -> (2,0)
    //state = 21  -> (2,1)
    //state = 22  -> (2,2)
    //state = 19  -> unpolarized
    
    const int state = 10 * spin + projection;
    EvtSpinDensity matrix;
    switch (state) {
        case 9:
            matrix.setDiag(3);
            return matrix;
        case 10:
            matrix.setDim(3);
            matrix.set(2, 2, EvtComplex(1.0, 0.0));
            return matrix;
        case 11:
            matrix.setDim(3);
            matrix.set(0, 0, EvtComplex(1.0, 0.0));
            matrix.set(1, 1, EvtComplex(1.0, 0.0));
            return matrix;
        case 19:
            matrix.setDiag(5);
            return matrix;
        case 20:
            matrix.setDim(5);
            matrix.set(4, 4, EvtComplex(1.0, 0.0));
            return matrix;
        case 21:
            matrix.setDim(5);
            matrix.set(2, 2, EvtComplex(1.0, 0.0));
            matrix.set(3, 3, EvtComplex(1.0, 0.0));
            return matrix;
        case 22:
            matrix.setDim(5);
            matrix.set(0, 0, EvtComplex(1.0, 0.0));
            matrix.set(1, 1, EvtComplex(1.0, 0.0));
            return matrix;
        default:
            throw std::runtime_error("Not a valid polarization.");
    }
}

chigen::models::ColorSingletPartonicModel::ColorSingletPartonicModel(
        double ecm, int pdg, EvtId evt, EvtSpinDensity spin, double mm,
        double particleR20, double particleMaxSigma)
: chigen::PartonicModel(ecm, pdg, evt, spin, mm) {
    R20 = particleR20;
    __S__ = eCM * eCM - 4 * pow2(PROTON_MASS);
    alpS = 0.3;
    Xmin = 1.01 * mesonMass2 / __S__;
    Xmax = 1;
    Pfct = 20;
    MaxSigma = particleMaxSigma * R20 * pow3(alpS);
    MaxSigmaBackup = MaxSigma;
}

void chigen::models::ColorSingletPartonicModel::setSigma() {
    // hard cross section
    sigma = dsdt();
    // Multiply by PDF's
    sigma = sigma * chigen::pythia::pdf->xf(1, x1, mesonMass2) * chigen::pythia::pdf->xf(1, x2, mesonMass2) / x1 / x2;
    // Correct on change of integration variables
    sigma = sigma*PhSp;
}

bool chigen::models::ColorSingletPartonicModel::setKin() {
    PhSp = 1;

    //double Pfct = -1;
    double Amin = pow(Xmin, -Pfct), Amax = pow(Xmax, -Pfct);
    double X = pow(Amin + chigen::random::random_engine->flat()*(Amax - Amin), -1 / Pfct);
    PhSp *= -1 / Pfct * (Amax - Amin) * pow(X, Pfct + 1);
    sH = X*__S__;



    //    double Xmin = 1.1 * mChi2 / S;
    //    double Xmax=1;
    //    double X = Xmin + (1 - Xmin) * rndm.flat();
    //    PhSp *= (1 - Xmin);

    double Ymax = log(1 / X) / 2;
    double Y = Ymax * (2 * chigen::random::random_engine->flat() - 1);
    PhSp *= 2 * Ymax;

    x1 = sqrt(X) * exp(Y);
    x2 = sqrt(X) * exp(-Y);

    if (x1 * x2 * __S__ < mesonMass2) {
        return false;
    }

    sH = x1 * x2*__S__;

    tH = (mesonMass2 - sH) * chigen::random::random_engine->flat();
    PhSp = PhSp * (sH - mesonMass2);
    uH = mesonMass2 - sH - tH;
    sH2 = pow2(sH);
    tH2 = pow2(tH);
    uH2 = pow2(uH);
    return true;
}

bool chigen::models::ColorSingletPartonicModel::setTrialKin() {
    PhSp = 1;

    double xmin = 0.67;
    double X = 1.1 * xmin;
    PhSp *= (1 - xmin);
    double Ymax = log(1 / xmin) / 2;
    double Y = 0;
    PhSp *= 2 * Ymax;


    x1 = sqrt(X) * exp(Y);
    x2 = sqrt(X) * exp(-Y);

    if (x1 * x2 * __S__ < mesonMass2) {
        return false;
    }
    sH = X*__S__;
    tH = (mesonMass2 - sH) * chigen::random::random_engine->flat();
    PhSp = PhSp * (sH - mesonMass2);
    uH = mesonMass2 - sH - tH;

    //    MaxSigma = 100000*setSigma();
    setSigma();
    MaxSigma = 16 * 10000 * sigma;
    MaxSigmaBackup = MaxSigma;
    return true;
}

ClassImp(chigen::models::ColorSingletPartonicModel);
ClassImp(chigen::models::chi_1);
ClassImp(chigen::models::chi_1_0);
ClassImp(chigen::models::chi_1_1);
ClassImp(chigen::models::chi_2);
ClassImp(chigen::models::chi_2_0);
ClassImp(chigen::models::chi_2_1);
ClassImp(chigen::models::chi_2_2);
ClassImp(chigen::models::x3872);
ClassImp(chigen::models::x3872_0);
ClassImp(chigen::models::x3872_1);
