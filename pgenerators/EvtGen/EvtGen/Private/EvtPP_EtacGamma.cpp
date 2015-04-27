/* 
 * File:   EvtPP_EtacGamma.cpp
 * Author: luchinsky
 * 
 * Created on August 23, 2014, 10:25 PM
 */

#include "EvtGenModels/EvtPP_EtacGamma.hh"
using namespace std;

/*
 squared matrix element calculated using 
 *      '/home/luchinsky/Dropbox/DskD/Work/PANDA/2014/Math/sigma_EtaCGamma.nb'
 * notebook
 * 
 * Diagrams:
 *      diag1: p pbar -> psi -> eta_c gamma
 *      diag2: p pbar -> psi(2S) -> eta_c gamma
 *      diag3: p pbar -> h_c -> eta_c gamma
 *      diag4: p pbar -> eta_c gamma (non-resonance)
 */
double EvtPP_EtacGamma::matr2(double s, double t, double u) {
    EvtComplex 
            $mPsi(mPsi,wPsi/2),
            den1 = 1. / (s - $mPsi*mPsi),
            cden1 = conj(den1);
    EvtComplex 
            $mPsi2S(mPsi2S, wPsi2S/2),
            den2 = 1. / (s - $mPsi2S*$mPsi2S),
            cden2 = conj(den2);
    EvtComplex 
            $mHc(mHc, wHc/2),
            den3 = 1. / (s - $mHc*$mHc),
            cden3 = conj(den3);
    double den41 = 1. / (u - mp * mp), cden41 = den41;
    double den42 = 1. / (t - mp * mp), cden42 = den42;


    double g_psi_pp = 0.00159714;
    double g_psi2_pp = 0.000936224;
    double g_hc_pp = 0.000419811;
    double g_eta_pp = 0.0228828;
    double g_psi_eta = 0.655179;
    double g_psi2_eta = 0.0452935;
    double g_hc_eta = 0.0452935;
    double e = 0.303;

    EvtComplex ii(0., 1.);

    int diag1=1, diag2=1, diag3=1, diag4=1;
    
    EvtComplex Matr2 = -4 * diag3 * diag4 * e * g_eta_pp * g_hc_eta * g_hc_pp * mHc * mp * ((cden41 * den3 + 2 * cden42 * den3 + cden3 * (den41 + 2 * den42)) * t -
            (2 * cden41 * den3 + cden42 * den3 + cden3 * (2 * den41 + den42)) * u + (cden41 * den3 - cden42 * den3 + cden3 * (den41 - den42)) * pow(mp, 2)) +
            4 * pow(diag4, 2) * pow(e, 2) * pow(g_eta_pp, 2)*((cden42 * den41 + cden41 * den42) * s * t + (cden41 + cden42)*(den41 + den42) * t * u +
            (cden42 * den41 + cden41 * den42) * s * (s + u) - (2 * (cden42 * (2 * den41 + den42) + cden41 * (den41 + 2 * den42)) * s +
            3 * (cden41 + cden42)*(den41 + den42)*(t + u)) * pow(mp, 2) + 5 * (cden41 + cden42)*(den41 + den42) * pow(mp, 4)) +
            2 * (cden3 * den1 - cden1 * den3) * diag1 * diag3 * g_hc_eta * g_hc_pp * g_psi_eta * g_psi_pp * ii * mHc * s * (-t + u) * pow(mPsi, -1) +
            2 * (cden3 * den2 - cden2 * den3) * diag2 * diag3 * g_hc_eta * g_hc_pp * g_psi2_eta * g_psi2_pp * ii * mHc * s * (-t + u) * pow(mPsi2S, -1) +
            cden1 * den1 * pow(diag1, 2) * pow(g_psi_eta, 2) * pow(g_psi_pp, 2) * pow(mPsi, -2)*
            (-2 * (s - t - u)*(t + u) * pow(mp, 2) + 2 * (s - 4 * (t + u)) * pow(mp, 4) + 8 * pow(mp, 6) + s * (pow(t, 2) + pow(u, 2))) +
            cden2 * den2 * pow(diag2, 2) * pow(g_psi2_eta, 2) * pow(g_psi2_pp, 2) * pow(mPsi2S, -2)*
            (-2 * (s - t - u)*(t + u) * pow(mp, 2) + 2 * (s - 4 * (t + u)) * pow(mp, 4) + 8 * pow(mp, 6) + s * (pow(t, 2) + pow(u, 2))) +
            (cden2 * den1 + cden1 * den2) * diag1 * diag2 * g_psi2_eta * g_psi2_pp * g_psi_eta * g_psi_pp * pow(mPsi, -1) * pow(mPsi2S, -1)*
            (-2 * (s - t - u)*(t + u) * pow(mp, 2) + 2 * (s - 4 * (t + u)) * pow(mp, 4) + 8 * pow(mp, 6) + s * (pow(t, 2) + pow(u, 2))) +
            4 * cden3 * den3 * pow(diag3, 2) * pow(g_hc_eta, 2) * pow(g_hc_pp, 2) * pow(mHc, 2)*
            (-2 * (t + u)*(s + t + u) * pow(mp, 2) + 2 * (s + 4 * (t + u)) * pow(mp, 4) - 8 * pow(mp, 6) + s * (pow(t, 2) + pow(u, 2))) * pow(t + u - 2 * pow(mp, 2), -2)\
    + 2 * (cden41 * den1 + cden42 * den1 - cden1 * (den41 + den42)) * diag1 * diag4 * e * g_eta_pp * g_psi_eta * g_psi_pp * ii * mp * pow(mPsi, -1) *
            pow(t + u - 2 * pow(mp, 2), 2) + 2 * (cden41 * den2 + cden42 * den2 - cden2 * (den41 + den42)) * diag2 * diag4 * e * g_eta_pp * g_psi2_eta * g_psi2_pp * ii * mp *
            pow(mPsi2S, -1) * pow(t + u - 2 * pow(mp, 2), 2);


    return real(Matr2);
}

void EvtPP_EtacGamma::decay(EvtParticle* root) {

    root->initializePhaseSpace(getNDaug(), getDaugs());

    double s = root->getP4Lab().mass2();
    if (first_run) {
        p2 = EvtVector4R(mp, 0, 0, 0);
        p1 = root->getP4Lab() - p2;

        first_run = false;
    };
    EvtVector4R kEta = root->getDaug(0)->getP4Lab(),
            kGamma = root->getDaug(1)->getP4Lab();

    double t = (p1 - kEta).mass2(), u = (p1 - kGamma).mass2();

    setProb(matr2(s, t, u));
}


EvtPP_EtacGamma::EvtPP_EtacGamma(const EvtPP_EtacGamma& orig) : p1(orig.p1), p2(orig.p2),
	mp(orig.mp), mPsi(orig.mPsi),  mEtaC(orig.mEtaC),  mPsi2S(orig.mPsi2S), 
	mHc(orig.mHc), wPsi(orig.wPsi), wPsi2S(orig.wPsi2S), wHc(orig.wHc),
        first_run(orig.first_run)
{
};

void EvtPP_EtacGamma::init() {
    checkNArg(0);
    mp = EvtPDL::getMass(EvtPDL::getId("p+"));
    mPsi = EvtPDL::getMass(EvtPDL::getId("J/psi"));
    wPsi = EvtPDL::getWidth(EvtPDL::getId("J/psi"));
    mPsi2S = EvtPDL::getMass(EvtPDL::getId("psi(2S)"));
    wPsi2S = EvtPDL::getWidth(EvtPDL::getId("psi(2S)"));
    mHc = EvtPDL::getMass(EvtPDL::getId("h_c"));
    wHc = EvtPDL::getWidth(EvtPDL::getId("h_c"));
    mEtaC = EvtPDL::getMass(EvtPDL::getId("eta_c"));
}

