//-------------------------------------------------------------------------
// Author:      Mustafa Schmidt (Mustafa.A.Schmidt@physik.uni-giessen.de)
// Changes:     
// Date:        30.11.2015
// Description: Particle Identification
//-------------------------------------------------------------------------

#ifndef PNDDISCPID_H
#define PNDDISCPID_H


#include "TObject.h"


class PndDiscPID : public TObject
{
    public:
        PndDiscPID();
	~PndDiscPID();

        double loglikepion;
        double loglikekaon;
        double loglikeproton;

        double pion;
        double kaon;
        double proton;

        ClassDef(PndDiscPID,2)
};


#endif // PNDDISCPID_H

