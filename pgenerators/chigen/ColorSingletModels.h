/* 
 * @author Alexey Luchinsky
 * @author Stanislav Poslavsky (stvlpos (at) mail.ru)
 */

#ifndef COLORSINGLETFACTORY_H
#define	COLORSINGLETFACTORY_H

#include "ChiGenContext.h"
#include "PartonicModel.h"


using namespace Pythia8;

namespace chigen {

    namespace models {


        /**
         * Returns momentum density corresponding to particle with total 
         * spin {@code spin} and spin projection {@code projection}.
         * 
         * @param spin total spin of particle
         * @param projection absolute value of spin projection onto particle 
         *                   momentum and -1 for unpolarized particle 
         */
        extern EvtSpinDensity createSpinDensity(int spin, int projection);

        class ColorSingletPartonicModel : public PartonicModel {
        public:

            ColorSingletPartonicModel(double ecm, int pdgId,
                    EvtId evtId,
                    EvtSpinDensity spinDensity,
                    double mesonMass,
                    double particleR20,
                    double particleMaxSigma);

            /**
             * d sigma/ dt
             * @return d sigma/ dt
             */
            virtual double dsdt() {
                return 0.0;
            };

            virtual void setSigma();
            virtual bool setKin();

            /**
             * Calculates trial kinematics
             */
            bool setTrialKin();

            ClassDef(ColorSingletPartonicModel, 1);
        public:
            /**
             * Proton mass take into account
             */
            double __S__;
            double R20;
            double alpS;
            double Xmin, Xmax, Pfct;

            double PhSp;
            double sH2, tH2, uH2;
        };

        class chi_1 : public ColorSingletPartonicModel {
        public:

            chi_1() :
            ColorSingletPartonicModel(
            CHI1_MASS, CHI1_PDG_ID, chigen::evtgen::chi_c1_evt_id,
            createSpinDensity(1, -1), CHI1_MASS, R20_CHI, 2.07e-06) {
            }

            chi_1(double ecm) :
            ColorSingletPartonicModel(
            ecm, CHI1_PDG_ID, chigen::evtgen::chi_c1_evt_id,
            createSpinDensity(1, -1), CHI1_MASS, R20_CHI, 2.07e-06) {
            }

            virtual double dsdt() {
                return (128 * pow3(alpS) * M_PI * R20) / (9 * sH2 * pow3(mesonMass) * pow4(mesonMass2 - sH)) *
                        ((sH + 4 * tH) * pow4(mesonMass) - 2 * (sH2 + 3 * tH * sH + 2 * tH2) * mesonMass2 + sH * (sH2 + 2 * tH * sH + 2 * tH2));
            }

            ClassDef(chi_1, 1);
        };

        class chi_1_0 : public ColorSingletPartonicModel {
        public:

            chi_1_0() :
            ColorSingletPartonicModel(
            CHI1_MASS, CHI1_PDG_ID, chigen::evtgen::chi_c1_evt_id,
            createSpinDensity(1, 0), CHI1_MASS, R20_CHI, 1.38e-06) {
            }

            chi_1_0(double ecm) :
            ColorSingletPartonicModel(
            ecm, CHI1_PDG_ID, chigen::evtgen::chi_c1_evt_id,
            createSpinDensity(1, 0), CHI1_MASS, R20_CHI, 1.38e-06) {
            }

            virtual double dsdt() {
                return (128 * M_PI * R20 * pow3(alpS) * (tH2 + uH2)) / (9 * pow3(mesonMass) * sH * pow4(-mesonMass2 + sH));
            }

            ClassDef(chi_1_0, 1);
        };

        class chi_1_1 : public ColorSingletPartonicModel {
        public:

            chi_1_1() :
            ColorSingletPartonicModel(
            CHI1_MASS, CHI1_PDG_ID, chigen::evtgen::chi_c1_evt_id,
            createSpinDensity(1, 1), CHI1_MASS, R20_CHI, 1.36e-06) {
            }

            chi_1_1(double ecm) :
            ColorSingletPartonicModel(
            ecm, CHI1_PDG_ID, chigen::evtgen::chi_c1_evt_id,
            createSpinDensity(1, 1), CHI1_MASS, R20_CHI, 1.36e-06) {
            }

            virtual double dsdt() {
                return (512 * M_PI * pow3(alpS) * R20 * uH * tH) / (9 * mesonMass * sH2 * pow4(-mesonMass2 + sH));
            }

            ClassDef(chi_1_1, 1);
        };

        class chi_2 : public ColorSingletPartonicModel {
        public:

            chi_2() :
            ColorSingletPartonicModel(
            CHI2_MASS, CHI2_PDG_ID, chigen::evtgen::chi_c2_evt_id,
            createSpinDensity(2, -1), CHI2_MASS, R20_CHI, 3.65e-06) {
            }

            chi_2(double ecm) :
            ColorSingletPartonicModel(
            ecm, CHI2_PDG_ID, chigen::evtgen::chi_c2_evt_id,
            createSpinDensity(2, -1), CHI2_MASS, R20_CHI, 3.65e-06) {
            }

            virtual double dsdt() {
                return (128 * M_PI * pow3(alpS) * R20 * (6 * pow4(mesonMass2) - 12 * pow3(mesonMass2) * (sH + tH) + sH2 * (sH2 + 2 * sH * tH + 2 * tH2)
                        - 2 * mesonMass2 * sH * (sH2 + 7 * sH * tH + 6 * tH2) + pow2(mesonMass2) * (7 * sH2 + 24 * sH * tH + 12 * tH2))) / (27 * pow3(mesonMass) * pow4(mesonMass2 - sH) * pow3(sH));
            }

            ClassDef(chi_2, 1);
        };

        class chi_2_0 : public ColorSingletPartonicModel {
        public:

            chi_2_0() :
            ColorSingletPartonicModel(
            CHI2_MASS, CHI2_PDG_ID, chigen::evtgen::chi_c2_evt_id,
            createSpinDensity(2, 0), CHI2_MASS, R20_CHI, 5.25e-07) {
            }

            chi_2_0(double ecm) :
            ColorSingletPartonicModel(
            ecm, CHI2_PDG_ID, chigen::evtgen::chi_c2_evt_id,
            createSpinDensity(2, 0), CHI2_MASS, R20_CHI, 5.25e-07) {
            }

            virtual double dsdt() {
                return (128 * M_PI * R20 * pow3(alpS)*(tH2 + uH2)) / (27 * pow3(mesonMass) * pow4(mesonMass2 - sH) * sH);
            }

            ClassDef(chi_2_0, 1);
        };

        class chi_2_1 : public ColorSingletPartonicModel {
        public:

            chi_2_1() :
            ColorSingletPartonicModel(
            CHI2_MASS, CHI2_PDG_ID, chigen::evtgen::chi_c2_evt_id,
            createSpinDensity(2, 1), CHI2_MASS, R20_CHI, 1.58e-06) {
            }

            chi_2_1(double ecm) :
            ColorSingletPartonicModel(
            ecm, CHI2_PDG_ID, chigen::evtgen::chi_c2_evt_id,
            createSpinDensity(2, 1), CHI2_MASS, R20_CHI, 1.58e-06) {
            }

            virtual double dsdt() {
                return (512 * M_PI * pow3(alpS) * R20 * uH * tH) / (9 * mesonMass * pow4(mesonMass2 - sH) * sH2);
            }

            ClassDef(chi_2_1, 1);
        };

        class chi_2_2 : public ColorSingletPartonicModel {
        public:

            chi_2_2() :
            ColorSingletPartonicModel(
            CHI2_MASS, CHI2_PDG_ID, chigen::evtgen::chi_c2_evt_id,
            createSpinDensity(2, 2), CHI2_MASS, R20_CHI, 3.1e-06) {
            }

            chi_2_2(double ecm) :
            ColorSingletPartonicModel(
            ecm, CHI2_PDG_ID, chigen::evtgen::chi_c2_evt_id,
            createSpinDensity(2, 2), CHI2_MASS, R20_CHI, 3.1e-06) {
            }

            virtual double dsdt() {
                return (256 * mesonMass * M_PI * pow3(alpS) * R20 * (tH2 + uH2)) / (9 * pow4(mesonMass2 - sH) * pow3(sH));
            }

            ClassDef(chi_2_2, 1);
        };
    }
}
#endif	/* COLORSINGLETFACTORY_H */

