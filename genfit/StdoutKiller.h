#ifndef STDOUTKILLER_H
#define STDOUTKILLER_H
// Original Authors: Christian Hoeppner (TUM)

#include"unistd.h"
#include"fcntl.h"

class StdoutKiller{
 public:
  StdoutKiller();
  virtual ~StdoutKiller();
 private:
  int devnull;
  int dummy;
};

#endif
 
 
