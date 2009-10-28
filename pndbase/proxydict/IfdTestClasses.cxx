
#include "IfdTestClasses.h"
#include <ostream>
using std::ostream;

int ListA::_instanceScaler = 0;

ostream& operator <<( ostream& o, const NoIfd& n) {
  n.print();
  return o;
}
