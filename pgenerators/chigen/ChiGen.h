/* 
 * @author Alexey Luchinsky
 * @author Stanislav Poslavsky (stvlpos (at) mail.ru)
 */

#ifndef CHIGEN_H
#define	CHIGEN_H

#include "ChiGenContext.h"
#include "PythiaChiGen.h"

namespace chigen {

    class ChiGen {
    public:
        /**
         * Constructs ChiGen from given partonic model
         * @param partonic model
         */
        ChiGen(PartonicModel& partonicModel);

        ~ChiGen();

        /**
         * Simulates next event and return pointer to it or NULL if no more 
         * events can be generated
         * 
         * @return pointer to simulated event or NULL if no more 
         * events can be generated
         */
        Pythia8::Event* next();

        /**
         * Set the internal PythiaChiGen instance by specified one, and return 
         * the old one
         * 
         * @param pchigen 
         * @return old instance
         */
        PythiaChiGen* setPythiaChiGen(PythiaChiGen* pchigen);

        ClassDef(ChiGen, 1);

    private:
        /**
         * Events counter
         */
        unsigned long long counter;
        
        PythiaChiGen* pchigen;
        /**
         * boost velocity
         */
        double betaZ, gamma;
        /**
         * Boost particle
         */
        void boost_lab_frame(Pythia8::Particle* particle);
    };
}

#endif	/* CHIGEN_H */

