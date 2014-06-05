/* 
 * @author Alexey Luchinsky
 * @author Stanislav Poslavsky (stvlpos (at) mail.ru)
 */

#ifndef PNDCHIGEN_H
#define	PNDCHIGEN_H

#include "FairGenerator.h"
#include "FairPrimaryGenerator.h"
#include "ChiGen.h"

class FairPrimaryGenerator;

namespace chigen {

    /**
     * Event generator for the inclusive charmonium production. 
     */
    class PndChiGen : public FairGenerator {
    public:
        PndChiGen();

        PndChiGen(PartonicModel& partonicModel);

        virtual ~PndChiGen();

        virtual Bool_t ReadEvent(FairPrimaryGenerator* primGen);
        /**
         * Set the internal ChiGen instance by specified one, and return 
         * the old one
         * 
         * @param pchigen 
         * @return old instance
         */
        ChiGen* setChiGen(ChiGen* chigen);

        ClassDef(PndChiGen, 1);

    private:

        ChiGen* chi_gen;

    };
}

#endif	/* PNDCHIGEN_H */