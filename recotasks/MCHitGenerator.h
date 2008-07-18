#ifndef FITTER_MCHITGENERATOR_H
#define FITTER_MCHITGENERATOR_H

#include "TRandom.h"

#include "SltHit.h"
#include "Slt2DHit.h"
#include "AliceHelixTrackRep.h"
#include "PndTpcRecoHit.h"



class MCHitGenerator {
    private:
        TRandom* rand;
        AliceHelixTrackRep* aHelix;

        int getSeed();
    public:
        MCHitGenerator();
        virtual ~MCHitGenerator();

        SltHit*   generateSL1D(double z, double a, double b, double dy);
        Slt2DHit* generateSL2D(double z, double ax, double bx,
							   double ay, double by, double dx,
							   double dy);


	void initHelix(double initialS,
		       const TMatrixT<double>& initialState,
		       const TMatrixT<double>& initialCov);

        PndTpcRecoHit*   generateTPC(double r,
							  double dr, double dphi, double dz);

};





#endif // FITTER_MCHITGENERATOR_H
