/*
g++ statTreeAnalyzer.cxx  statTreeAnalyzer_auto.C  fitStatMaker.cxx `root-config --glibs` -I`root-config --incdir` -o fitStatMaker
*/

#include "statTree.h"
#include "statTreeAnalyzer.h"

int main() {
  statTreeAnalyzer s("statTree","statTree.root","fitStats.root");
  s.Loop();
}
