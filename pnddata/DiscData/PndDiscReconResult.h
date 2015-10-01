//-------------------------------------------------------------------------
// Author:      Mustafa Schmidt (Mustafa.A.Schmidt@physik.uni-giessen.de)
// Changes:     
// Date:        30.11.2015
// Description: Reconstruction
//-------------------------------------------------------------------------

#ifndef DISCDIRC_RECONRESULT_HH
#define DISCDIRC_RECONRESULT_HH


#include "TObject.h"


class PndDiscReconResult : public TObject
{
    public:
        PndDiscReconResult();

        double cherenkov_angle;

        int particle;
        int hypothesis;
        int sensor;
        int pixel;
        double time;

        ClassDef(PndDiscReconResult,2)
};


#endif // DISCDIRC_RECONRESULT_HH
