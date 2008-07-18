#ifndef STDOUTKILLER_H
#define STDOUTKILLER_H

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
 
 
