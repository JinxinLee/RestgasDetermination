/* 
 * @author Alexey Luchinsky
 * @author Stanislav Poslavsky (stvlpos (at) mail.ru)
 */

#ifndef CHIGENRANDOMENGINE_H
#define	CHIGENRANDOMENGINE_H


#include "EvtGenBase/EvtRandomEngine.hh"
#include "Basics.h"
#include "TNamed.h"

namespace chigen {

    class ChiGenRandomEngine
    : public EvtRandomEngine, public Pythia8::RndmEngine, public TNamed {
    public:

        /**
         * Creates generator with fixed seed (1 is the default value)
         * 
         * @param seed seed (1 is the default value)
         */
        ChiGenRandomEngine(long seed = 1L);

        ~ChiGenRandomEngine();
        /**
         * Returns next random double.
         * 
         * @return random double
         */
        virtual double random();
        /**
         * Returns next random double.
         * 
         * @return random double
         */
        virtual double flat();

        /**
         * Returns seed.
         *  
         * @return seed
         */
        int getSeed() const {
            return seed;
        }

    private:

        long seed;
    };
}

#endif	/* CHIGENRANDOMENGINE_H */

