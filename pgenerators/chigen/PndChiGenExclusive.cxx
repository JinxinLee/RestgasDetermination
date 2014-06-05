/* 
 * @author Alexey Luchinsky
 * @author Stanislav Poslavsky (stvlpos (at) mail.ru)
 */

#include "PndChiGenExclusive.h"
#include "EvtGenBase/EvtParticle.hh"
#include "EvtGenBase/EvtParticleFactory.hh"

chigen::PndChiGenExclusive::PndChiGenExclusive(PartonicModel& pmodel)
: model(pmodel) {
    chigen::ensure_chigen_is_initialized();
    double Mchi = model.mesonMass;
    double p = sqrt(
            (pow(Mchi, 4) - 4 * pow(Mchi, 2) * pow(PROTON_MASS, 2))
            / (4 * pow(PROTON_MASS, 2)));
    double e = sqrt(pow(Mchi, 2) + p * p);
    chiMomentum = EvtVector4R(e, 0., 0., p);
    assert(Mchi * Mchi - 1E-6 <= chiMomentum.mass2()
            && chiMomentum.mass2() <= Mchi * Mchi + 1E-6);
}

chigen::PndChiGenExclusive::~PndChiGenExclusive() {
}

Bool_t chigen::PndChiGenExclusive::ReadEvent(FairPrimaryGenerator* primGen) {
    EvtParticle *chiMeson =
            EvtParticleFactory::particleFactory(model.evtId, chiMomentum);
    chiMeson->setSpinDensityForward(model.spinDensity);

    //decay
    chigen::evtgen::evt_gen->generateDecay(chiMeson);
    //write info
    evtstdhep.init();
    chiMeson->makeStdHep(evtstdhep);

    if (chigen::ostreams::verbose_mode)//<- if verbose then print Pythia event
        chigen::pythia::pythia->event.reset();

    EvtVector4R vertexPoint, evtMomentum;
    for (int particleNumber = 0, size = evtstdhep.getNPart();
            particleNumber < size; ++particleNumber) {
       
        vertexPoint = evtstdhep.getX4(particleNumber);
        evtMomentum = evtstdhep.getP4(particleNumber);

        if (chigen::ostreams::verbose_mode) {
            //printing Pythia event
            int id;
            Pythia8::Particle p;
            p.id(id = evtstdhep.getStdHepID(particleNumber));
            p.px(evtMomentum.get(1));
            p.py(evtMomentum.get(2));
            p.pz(evtMomentum.get(3));
            p.e(evtMomentum.get(0));
            p.daughter1(evtstdhep.getFirstDaughter(particleNumber) + 1);
            p.daughter2(evtstdhep.getLastDaughter(particleNumber) + 1);
            p.mother1(evtstdhep.getFirstMother(particleNumber) + 1);
            p.mother2(evtstdhep.getLastMother(particleNumber) + 1);
            if (id == CHI1_PDG_ID || id == CHI2_PDG_ID || id == PSI_PDG_ID)
                p.status(-33);
            else
                p.status(91);
            chigen::pythia::pythia->event.append(p);
        }

        //skip non final particles
        if (evtstdhep.getFirstDaughter(particleNumber) != -1 ||
                evtstdhep.getLastDaughter(particleNumber) != -1)
            continue;

        primGen->AddTrack(evtstdhep.getStdHepID(particleNumber),
                evtMomentum.get(1), evtMomentum.get(2), evtMomentum.get(3),
                vertexPoint.get(1), vertexPoint.get(2), vertexPoint.get(3),
                -1, true, evtMomentum.get(0));
    }
    if (chigen::ostreams::verbose_mode)
        chigen::pythia::pythia->event.list(*chigen::ostreams::chigen_cout);
    return true;
}

ClassImp(chigen::PndChiGenExclusive)