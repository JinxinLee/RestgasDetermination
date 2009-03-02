// Original Authors: Christian Hoeppner & Sebastian Neubert (TUM)
#include "StdoutKiller.h"


StdoutKiller::StdoutKiller() {
  devnull = open("/dev/null", O_RDONLY);
  dummy = dup(STDOUT_FILENO);
  dup2(devnull, STDOUT_FILENO);
}

StdoutKiller::~StdoutKiller() {
  dup2(dummy, STDOUT_FILENO);
  close(dummy);
  close(devnull);
}
 
 
