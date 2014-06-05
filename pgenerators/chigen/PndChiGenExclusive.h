/* 
 * @author Alexey Luchinsky
 * @author Stanislav Poslavsky (stvlpos (at) mail.ru)
 */

#ifndef PNDCHIGENEXCLUSIVE_H
#define	PNDCHIGENEXCLUSIVE_H

#include "FairGenerator.h"
#include "FairPrimaryGenerator.h"
#include "PartonicModel.h"
#include "EvtGenBase/EvtVector4R.hh"
#include "EvtGenBase/EvtStdHep.hh"

namespace chigen {

    class PndChiGenExclusive : public FairGenerator {
    public:
        PndChiGenExclusive(PartonicModel& model);
        virtual ~PndChiGenExclusive();

        virtual Bool_t ReadEvent(FairPrimaryGenerator* primGen);

        ClassDef(PndChiGenExclusive, 1);
    private:
        //This only needs to extract correct polarizations
        PartonicModel& model;
        //Momentum of chiMeson (fixed))
        EvtVector4R chiMomentum;
        //EvtGen data 
        EvtStdHep evtstdhep;
    };
}

#endif	/* PNDCHIGENEXCLUSIVE_H */

