/*
 * PndSttTubeMap.cxx
 *
 *  Created on: 02.08.2018
 *      Author: tstockmanns
 */

#include <PndSttTubeMap.h>
#include "PndSttTube.h"

ClassImp(PndSttTubeMap);

PndSttTubeMap* PndSttTubeMap::fInstance= NULL;

PndSttTubeMap::PndSttTubeMap() {
    // TODO Auto-generated constructor stub

}

PndSttTubeMap::~PndSttTubeMap() {
    // TODO Auto-generated destructor stub
}

PndSttTubeMap* PndSttTubeMap::Instance(){
    if ( !fInstance){
        fInstance = new PndSttTubeMap();
    }
    return fInstance;
}

