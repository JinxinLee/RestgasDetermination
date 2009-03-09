/**
 *  @author Christian H&ouml;ppner (Technische Universit&auml;t M&uuml;nchen, original author)
 *  @author Sebastian Neubert  (Technische Universit&auml;t M&uuml;nchen, original author)
 * 
 */

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
 
 
