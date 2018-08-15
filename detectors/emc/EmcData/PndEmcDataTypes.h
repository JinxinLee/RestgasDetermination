#ifndef PNDEMCDATATYPES_HH
#define PNDEMCDATATYPES_HH

#include <map>
#include <set>

class PndEmcTwoCoordIndex;
class PndEmcDigi;
class PndEmcXtal;
typedef std::map<PndEmcTwoCoordIndex*, PndEmcDigi*> PndEmcDigiPtrDict;
typedef std::map<PndEmcTwoCoordIndex*, PndEmcXtal*> PndEmcTciXtalMap;
typedef std::set< PndEmcTwoCoordIndex*> PndEmcCoordIndexSet;
#endif
