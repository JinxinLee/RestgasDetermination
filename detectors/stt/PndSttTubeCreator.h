/*
 * PndSttTubeCreator.h
 *
 *  Created on: 02.08.2018
 *      Author: tstockmanns
 */

#ifndef DETECTORS_STT_PNDSTTTUBECREATOR_H_
#define DETECTORS_STT_PNDSTTTUBECREATOR_H_

#include <TObject.h>

#include "PndSttTube.h"

class PndSttTubeCreator : public TObject
{
public:
    PndSttTubeCreator();
    virtual ~PndSttTubeCreator();

    PndSttTube* GetTube(int tubeId);


    ClassDef(PndSttTubeCreator, 1);
};

#endif /* DETECTORS_STT_PNDSTTTUBECREATOR_H_ */
