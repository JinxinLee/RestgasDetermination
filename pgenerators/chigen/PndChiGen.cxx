/* 
 * @author Alexey Luchinsky
 * @author Stanislav Poslavsky (stvlpos (at) mail.ru)
 */

#include "PndChiGen.h"

chigen::PndChiGen::PndChiGen() {
}

chigen::PndChiGen::PndChiGen(chigen::PartonicModel& model) {
    chigen::ensure_chigen_is_initialized();
    chi_gen = new chigen::ChiGen(model);
}

chigen::PndChiGen::~PndChiGen() {
}

chigen::ChiGen* chigen::PndChiGen::setChiGen(chigen::ChiGen* p) {
    chigen::ChiGen* oldPointer = chi_gen;
    chi_gen = p;
    return oldPointer;
}

Bool_t chigen::PndChiGen::ReadEvent(FairPrimaryGenerator* primGen) {
    Pythia8::Event* event = chi_gen->next();
    if (event == NULL)
        return kFALSE;

    Pythia8::Particle* particle = 0;
    for (int i = event->size() - 1; i >= 0; --i) {
        particle = &event->at(i);

        if (particle->status() < 0)
            continue; // adding only final particles

        primGen->AddTrack(particle->id(),
                particle->px(), particle->py(), particle->pz(),
                particle->xProd(), particle->yProd(), particle->zProd(),
                -1, true, particle->e());

    };

    return kTRUE;
}

ClassImp(chigen::PndChiGen);
