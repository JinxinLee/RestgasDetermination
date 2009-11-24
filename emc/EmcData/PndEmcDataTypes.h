#ifndef PNDEMCDATATYPES_HH
#define PNDEMCDATATYPES_HH

#include "CollectionUtils.h"
#include <map>
#include <set>

class PndEmcTwoCoordIndex;
class PndEmcDigi;
class PndEmcXtal;
typedef std::map<PndEmcTwoCoordIndex*, PndEmcDigi*, PndPtrLess> PndEmcDigiPtrDict;
typedef std::map<PndEmcTwoCoordIndex*, PndEmcXtal*, PndPtrLess> PndEmcTciXtalMap;
typedef std::set< PndEmcTwoCoordIndex*, PndPtrLess > PndEmcCoordIndexSet;
#endif
