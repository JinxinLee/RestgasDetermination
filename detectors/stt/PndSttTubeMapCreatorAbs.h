/*
 * PndSttTubeMapCreatorAbs.h
 *
 *  Created on: 02.08.2018
 *      Author: tstockmanns
 */

#ifndef DETECTORS_STT_PndSttTubeMapCreatorAbs_H_
#define DETECTORS_STT_PndSttTubeMapCreatorAbs_H_

#include "TObject.h"

class PndSttTubeMap;

class PndSttTubeMapCreatorAbs : public TObject {
public:
    PndSttTubeMapCreatorAbs();
    virtual ~PndSttTubeMapCreatorAbs();

    virtual PndSttTubeMap* CreateTubeMap() = 0;


    ClassDef(PndSttTubeMapCreatorAbs, 1);
};

#endif /* DETECTORS_STT_PndSttTubeMapCreatorAbs_H_ */
