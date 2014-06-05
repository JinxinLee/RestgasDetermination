/* 
 * @author Alexey Luchinsky
 * @author Stanislav Poslavsky (stvlpos (at) mail.ru)
 */

#ifndef PYTHIAPARTONMODEL_H
#define	PYTHIAPARTONMODEL_H
#include "ChiGenContext.h"
#include "EvtGenBase/EvtSpinDensity.hh"

using namespace Pythia8;

namespace chigen {

    class PartonicModel {
    public:
        PartonicModel(double ecm, int pdgId, EvtId evtId,
                EvtSpinDensity spinDensity, double mesonMass);
        virtual ~PartonicModel();

        /**
         * Calculates the partonic cross section
         */
        virtual bool next();

        /**
         *  fills kinematic variables
         */
        virtual bool setKin() {
            return false;
        };

        /**
         * calculate f1*f2*dSigma/dT of the hard process for the selected kinematics
         */
        virtual void setSigma() {
        };

        ClassDef(PartonicModel, 1);
    public:
        /**
         * C.m. energy
         */
        double eCM;
        /**
         * Meson id
         */
        int pdgId;
        /**
         * Spin density corresponding to this polarization
         */
        EvtSpinDensity spinDensity;
        /**
         * EvtGen id
         */
        EvtId evtId;
        /**
         * Meson mass and its squared
         */
        double mesonMass, mesonMass2;
        /**
         * Momentum fractions of the incoming partons
         */
        double x1, x2;
        /**
         * Mandelstam variables of the hard process
         */
        double sH, tH, uH;
        /**
         * dsdt
         */
        double sigma;
        /**
         * Maximal sigma
         */
        double MaxSigma, MaxSigmaBackup;

        /**
         * number of attempts
         */
        int nCall;
    };
}

#endif	/* PYTHIAPARTONMODEL_H */

