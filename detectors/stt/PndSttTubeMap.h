/*
 * PndSttTubeMap.h
 *
 *  Created on: 02.08.2018
 *      Author: tstockmanns
 */

#ifndef DETECTORS_STT_PNDSTTTUBEMAP_H_
#define DETECTORS_STT_PNDSTTTUBEMAP_H_

#include <TObject.h>
#include <map>

class PndSttTube;

class PndSttTubeMap : public TObject{
public:
    static PndSttTubeMap* Instance();
    virtual ~PndSttTubeMap();

    void AddTube(int tubeId, PndSttTube* tube) { fTubeMap[tubeId] = tube; }
    PndSttTube* GetTube(int tubeId){
        if (fTubeMap.count(tubeId) > 0)
            return fTubeMap[tubeId];
        else return nullptr;
    }
    std::map<int, PndSttTube*> GetMap(){return fTubeMap;}       //todo: change this to an iterator running through all fields of the map to hide the implementation
    int GetMapSize() { return fTubeMap.size(); }
private:
    static PndSttTubeMap* fInstance;
    std::map<int, PndSttTube*> fTubeMap;
    PndSttTubeMap();

    ClassDef(PndSttTubeMap, 1);
};

#endif /* DETECTORS_STT_PNDSTTTUBEMAP_H_ */
