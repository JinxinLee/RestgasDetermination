/*
 * PndSttNeighborhoodCreator.h
 *
 *  Created on: 03.08.2018
 *      Author: tstockmanns
 */

#ifndef DETECTORS_STT_PNDSTTNEIGHBORHOODCREATOR_H_
#define DETECTORS_STT_PNDSTTNEIGHBORHOODCREATOR_H_

#include <TObject.h>
#include <vector>

class PndSttTube;

class PndSttNeighborhoodCreator : public TObject {
public:
    PndSttNeighborhoodCreator();
    virtual ~PndSttNeighborhoodCreator();

    std::vector<int> FindNeighbors(PndSttTube* tube);

protected:
    std::vector< std::pair<int, int> > GetListOfSectorRowsToCompare(PndSttTube* tube);
    bool Compare(PndSttTube* tube1, PndSttTube* tube2, double tolerance);
    double GetTolerance(PndSttTube* tube1, PndSttTube* tube2);


    ClassDef(PndSttNeighborhoodCreator, 1);
};

#endif /* DETECTORS_STT_PNDSTTNEIGHBORHOODCREATOR_H_ */
