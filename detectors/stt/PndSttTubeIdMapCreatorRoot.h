/*
 * PndSttTubeIdMapCreatorRoot.h
 *
 *
 *  Created on: 02.08.2018
 *      Author: tstockmanns
 */

#ifndef DETECTORS_STT_PNDSTTTUBEIDMAPCREATORROOT_H_
#define DETECTORS_STT_PNDSTTTUBEIDMAPCREATORROOT_H_

#include <PndSttTubeIdMap.h>
#include <PndSttTubeIdMapCreatorAbs.h>

class PndSttTubeIdMapCreatorRoot: public PndSttTubeIdMapCreatorAbs {
public:
    PndSttTubeIdMapCreatorRoot();
    virtual ~PndSttTubeIdMapCreatorRoot();

    PndSttTubeIdMap* CreateTubeMap();

protected:
    int GetSector(TString& name);
    int GetRow(TString& name);

    ClassDef(PndSttTubeIdMapCreatorRoot, 1);

};

#endif /* DETECTORS_STT_PNDSTTTUBEIDMAPCREATORROOT_H_ */
