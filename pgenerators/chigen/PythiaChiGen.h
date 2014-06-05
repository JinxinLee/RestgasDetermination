/* 
 * @author Alexey Luchinsky
 * @author Stanislav Poslavsky (stvlpos (at) mail.ru)
 */

#ifndef PYTHIAHADRONIZATIONMODEL_H
#define	PYTHIAHADRONIZATIONMODEL_H

#include "ChiGenContext.h"
#include "PartonicModel.h"
#include "ColorSingletModels.h"
#include "EvtGenBase/EvtStdHep.hh"


using namespace Pythia8;

namespace chigen {

    class PythiaChiGen {
    public:

        /**
         * @param partonicModel partonic model
         * @param maxTries max errors number
         * @param doHadronization perform hadronization
         */
        PythiaChiGen(PartonicModel& partonicModel,
                int maxTries = 30, bool doHadronization = true);

        PartonicModel& getPartonicModel() const {
            return partonicModel;
        }

        virtual ~PythiaChiGen();
        /**
         * Calculates and returns the next event
         * @return event
         */
        Event* next();

        ClassDef(PythiaChiGen, 1);

    private:

        /**
         * Hadronic c.m. energy
         */
        double eCM;
        /**
         * Underlying parton model
         */
        PartonicModel& partonicModel;
        /**
         * Maximum number of tries to hadronize with diff kinematics
         */
        int maxTries;
        /**
         * Proton momentum
         */
        double protonPz;
        /**
         * Do hadronization or not
         */
        bool doHadronization;
        /**
         * Proton remnants
         */
        Vec4 uMomentum, dMomentum;
        /**
         * Antiproton remnants
         */
        Vec4 ubarMomentum, dbarMomentum;
        /**
         * Hard process particles
         */
        Vec4 mesonMomentum, gluonMomentum;
        /**
         * Boost from hard c.m. frame to hadronic c.m. frame
         */
        Vec4 boost;
        /**
         * EvtGen data on chi_c decay 
         */
        EvtStdHep evtstdhep;
        /**
         * Colors of particles:
         * 
         *  u-quark       : 101
         *  d-quark       : 102
         *  gluon (color) : 103  
         */
        static int COLORS[];
        /**
         * Calculates hadron-level kinematics 
         */
        void calculateKinematics();
        /**
         * 
         * Calculates remnants kinematics    
         * @param x x-fraction
         * @param hadronPz hadron z-momentum 
         * @param u u (anti) quark
         * @param d d (anti) quark
         */
        void calculateHadronRemnants(double x, double hadronPz, Vec4& u, Vec4& d);
        
        /**
         * Perform decay of polarized chi-meson.
         * 
         * @param event event reference
         */
        void decay(Event& event);
        /**
         * Fills the event by hadronic remnants & gluon
         * 
         * @param event event reference
         */
        void fillColorEvent(Event& event);
    };
}

#endif	/* PYTHIAHADRONIZATIONMODEL_H */

