/*
 * PndSttTubeMapCreatorRoot.h
 *
 *
 *  Created on: 02.08.2018
 *      Author: tstockmanns
 */

#ifndef DETECTORS_STT_PndSttTubeMapCreatorRoot_H_
#define DETECTORS_STT_PndSttTubeMapCreatorRoot_H_

#include <PndSttTubeMap.h>
#include <PndSttTubeMapCreatorAbs.h>

class PndSttTubeMapCreatorRoot: public PndSttTubeMapCreatorAbs {
public:
    PndSttTubeMapCreatorRoot();
    virtual ~PndSttTubeMapCreatorRoot();
    bool IsEdgeStraw(int tubeId) const;
    int IsSectorBorderStraw(int tubeId) const;
    PndSttTubeMap* CreateTubeMap();

protected:
    void AssignNeighbors(PndSttTube* tube);

    int sRowTubeID[6][29];//[sector][row]
    int eRowTubeID[6][29];//[sector][row]
    int additionalSkewedTubeMap[4576]; // check for additional tube in skewed layers
    ClassDef(PndSttTubeMapCreatorRoot, 1);

};

#endif /* DETECTORS_STT_PndSttTubeMapCreatorRoot_H_ */
