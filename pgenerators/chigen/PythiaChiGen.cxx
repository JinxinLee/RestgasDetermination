/* 
 * @author Alexey Luchinsky
 * @author Stanislav Poslavsky (stvlpos (at) mail.ru)
 */

#include <boost/random/mersenne_twister.hpp>

#include "PythiaChiGen.h"
#include "EvtGenBase/EvtParticleFactory.hh"
#include "EvtGenBase/EvtParticle.hh"
#include "TError.h"

using namespace Pythia8;
using namespace std;

chigen::PythiaChiGen::PythiaChiGen(
        PartonicModel& partonicModelRef, int tries, bool hadronize)
: partonicModel(partonicModelRef) {
    chigen::ensure_chigen_is_initialized();
    eCM = partonicModelRef.eCM;
    protonPz = sqrt(pow2(eCM) / 4 - 1);
    maxTries = tries;
    doHadronization = hadronize;
}

chigen::PythiaChiGen::~PythiaChiGen() {
}

int chigen::PythiaChiGen::COLORS[] = {101, 102, 103};

void chigen::PythiaChiGen::calculateKinematics() {

    double pT = sqrt(partonicModel.uH * partonicModel.tH / partonicModel.sH);
    double pL = (partonicModel.tH - partonicModel.uH) / (2 * sqrt(partonicModel.sH));
    double mesonE = sqrt(partonicModel.mesonMass2 + pow2(pT) + pow2(pL));
    double gluonE = sqrt(pow2(pT) + pow2(pL));


    //quarkonia
    mesonMomentum = Vec4(0., pT, pL, mesonE);
    //gluon
    gluonMomentum = Vec4(0., -pT, -pL, gluonE);

    //rotate vectors in XY plane
    double hardPhiAngle = chigen::random::random_engine->flat()* 2 * M_PI;
    mesonMomentum.rot(0, hardPhiAngle);
    gluonMomentum.rot(0, hardPhiAngle);

    //boost from hard c.m. to hadronic c.m.    
    boost = Vec4(0., 0., (partonicModel.x1 - partonicModel.x2) * protonPz,
            (partonicModel.x1 + partonicModel.x2) * protonPz);
    //do not boost meson momentum until EvtGen perform decay!!!
    //mesonMomentum.bst(boost) <-- this will destroy polarization
    gluonMomentum.bst(boost);

    //remnants kinematics
    //proton remnants
    calculateHadronRemnants(partonicModel.x1, protonPz, uMomentum, dMomentum);
    //antiproton remnants
    calculateHadronRemnants(partonicModel.x2, -protonPz, ubarMomentum, dbarMomentum);
}

void chigen::PythiaChiGen::calculateHadronRemnants(double x, double hadronPz, Vec4& u, Vec4& d) {

    Vec4 totalRemnant(0, 0, (1 - x) * hadronPz, eCM / 2 - x * abs(hadronPz));
    double halfRemnantInvMass = totalRemnant.mCalc() / 2;
    //make remnants pT
    double theta = chigen::random::random_engine->flat() * M_PI;
    double sine = halfRemnantInvMass * sin(theta), cosine = halfRemnantInvMass * cos(theta);
    u.p(0, sine, cosine, halfRemnantInvMass);
    d.p(0, -sine, -cosine, halfRemnantInvMass);

    //rotate vectors in XY plane
    double phi = chigen::random::random_engine->flat()* 2 * M_PI;
    u.rot(0, phi);
    d.rot(0, phi);

    //boost to lab frame
    u.bst(totalRemnant);
    d.bst(totalRemnant);
}

void chigen::PythiaChiGen::decay(Event& event) {
    //lets decay polarized meson via EvtGen
    EvtVector4R chiMomentum(
            mesonMomentum.e(),
            mesonMomentum.px(),
            mesonMomentum.py(),
            mesonMomentum.pz());


    EvtParticle *chiMeson =
            EvtParticleFactory::particleFactory(partonicModel.evtId, chiMomentum);
    chiMeson->setSpinDensityForward(partonicModel.spinDensity);

    //decay
    chigen::evtgen::evt_gen->generateDecay(chiMeson);
    //write info
    evtstdhep.init();
    chiMeson->makeStdHep(evtstdhep);

    assert(evtstdhep.getNPart() == 5);
    int id;
    EvtVector4R vertexPoint, evtMomentum;
    for (int particleNumber = 0, size = evtstdhep.getNPart();
            particleNumber < size; ++particleNumber) {
        vertexPoint = evtstdhep.getX4(particleNumber);
        evtMomentum = evtstdhep.getP4(particleNumber);

        Pythia8::Particle p;
        p.id(id = evtstdhep.getStdHepID(particleNumber));
        p.px(evtMomentum.get(1));
        p.py(evtMomentum.get(2));
        p.pz(evtMomentum.get(3));
        p.e(evtMomentum.get(0));
        //boost from hard c.m. frame to hadronic c.m. frame
        p.bst(boost);

        p.daughter1(evtstdhep.getFirstDaughter(particleNumber) + 1);
        p.daughter2(evtstdhep.getLastDaughter(particleNumber) + 1);
        p.mother1(evtstdhep.getFirstMother(particleNumber) + 1);
        p.mother2(evtstdhep.getLastMother(particleNumber) + 1);
        if (id == CHI1_PDG_ID || id == CHI2_PDG_ID || id == PSI_PDG_ID)
            p.status(-33);
        else
            p.status(91);

        event.append(p);
    }
}

void chigen::PythiaChiGen::fillColorEvent(Event& event) {
    // first setup colors and anti-colors
    // the following color mask is adopted:
    //
    //               u (bar)      d (bar)        g     
    // color     :    101          102          103
    // anti-color:  COLORS[0]    COLORS[1]    COLORS[2]

    // randomly shuffle colors 
    std::random_shuffle(COLORS, COLORS + 3); //todo use same seed as in rndm

    //already added chi-meson in decay()
    //    //filling the event record:    
    //    //quarkonia
    //    event.append(partonicModel->pdgId/*id*/, 21 /*status code*/,
    //            0, 0/*colors*/,
    //            mesonMomentum /*4-momentum*/, partonicModel->mesonMass /*mass*/);

    //gluon
    event.append(21/*id*/, 23 /*status code (should be hadronized)*/,
            103, COLORS[2]/*colors*/,
            gluonMomentum /*4-momentum*/, 0.0 /*mass*/);


    //u-quark
    event.append(2/*id*/, 23 /*status code (should be hadronized)*/,
            101, 0/*colors*/,
            uMomentum/*4-momentum*/, 0.0/* mass*/);

    //d-quark
    event.append(1/*id*/, 23 /*status code (should be hadronized)*/,
            102, 0/*colors*/,
            dMomentum/*4-momentum*/, 0.0/* mass*/);

    //u-bar
    event.append(-2/*id*/, 23 /*status code (should be hadronized)*/,
            0, COLORS[0]/*colors*/,
            ubarMomentum/*4-momentum*/, 0.0/* mass*/);

    //d-bar
    event.append(-1/*id*/, 23 /*status code (should be hadronized)*/,
            0, COLORS[1]/*colors*/,
            dbarMomentum/*4-momentum*/, 0.0 /*mass*/);
}

Event* chigen::PythiaChiGen::next() {
    Event& event = chigen::pythia::pythia->event;
    //reset event is necessary
    event.reset();

    //number of attempts
    int attempt = 0;
    while (!partonicModel.next() && attempt++ < maxTries);
    //parton level failed
    if (attempt >= maxTries)
        Fatal("PythiaHadronizationModel::next", "Can't set kinematics at parton level.");

    attempt = 0;
    calculateKinematics();
    decay(event);
    fillColorEvent(event);

    if (doHadronization) {
        while (!chigen::pythia::pythia->next() && attempt++ < maxTries) {
            event.reset();
            calculateKinematics();
            decay(event);
            fillColorEvent(event);
        }
        //hadron level failed
        if (attempt >= maxTries)
            Fatal("PythiaHadronizationModel::next", "Failed to hadronize remnants.");
    }
    return &event;
}

ClassImp(chigen::PythiaChiGen);