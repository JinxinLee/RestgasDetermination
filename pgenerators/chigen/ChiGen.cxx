/* 
 * @author Alexey Luchinsky
 * @author Stanislav Poslavsky (stvlpos (at) mail.ru)
 */

#include "ChiGen.h"
#include "EvtGenBase/EvtStdHep.hh"
#include "EvtGenBase/EvtParticle.hh"
#include "EvtGenBase/EvtParticleFactory.hh"

chigen::ChiGen::ChiGen(chigen::PartonicModel& p) : counter(0) {
    chigen::ensure_chigen_is_initialized();
    pchigen = new chigen::PythiaChiGen(p);
    betaZ = sqrt(1 - 4 * pow2(PROTON_MASS) / pow2(p.eCM));
    gamma = 1 / sqrt(1 - pow2(betaZ));
};

chigen::ChiGen::~ChiGen() {
    delete pchigen;
}

chigen::PythiaChiGen* chigen::ChiGen::setPythiaChiGen(chigen::PythiaChiGen* p) {
    chigen::PythiaChiGen* oldPointer = pchigen;
    pchigen = p;
    return oldPointer;
}

void chigen::ChiGen::boost_lab_frame(Pythia8::Particle* particle) {
    double oldPz = particle->pz();
    particle->pz(gamma * (oldPz - betaZ * particle->e()));
    particle->e(gamma * (particle->e() - betaZ * oldPz));
}

Pythia8::Event* chigen::ChiGen::next() {
    Pythia8::Event* event = pchigen->next();
    if (event == NULL)
        return NULL;

    ++counter;

    Pythia8::Particle* particle = 0;
    for (int i = event->size() - 1; i >= 0; --i) {
        particle = &event->at(i);
        boost_lab_frame(particle);
    }

    __chigen_cout__ << "Event number " << counter << "." << std::endl;
    if (chigen::ostreams::verbose_mode)
        event->list(*chigen::ostreams::chigen_cout);

    return event;
}

ClassImp(chigen::ChiGen)