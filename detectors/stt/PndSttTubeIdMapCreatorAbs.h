/*
 * PndSttTubeIdMapCreatorAbs.h
 *
 *  Created on: 02.08.2018
 *      Author: tstockmanns
 */

#ifndef DETECTORS_STT_PNDSTTTUBEIDMAPCREATORABS_H_
#define DETECTORS_STT_PNDSTTTUBEIDMAPCREATORABS_H_

#include "TObject.h"

class PndSttTubeIdMap;

class PndSttTubeIdMapCreatorAbs : public TObject {
public:
    PndSttTubeIdMapCreatorAbs();
    virtual ~PndSttTubeIdMapCreatorAbs();

    virtual PndSttTubeIdMap* CreateTubeMap() = 0;


    ClassDef(PndSttTubeIdMapCreatorAbs, 1);
};

#endif /* DETECTORS_STT_PNDSTTTUBEIDMAPCREATORABS_H_ */
