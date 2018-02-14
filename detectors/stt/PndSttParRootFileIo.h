#ifndef PNDSTTPARROOTFILEIO_H
#define PNDSTTPARROOTFILEIO_H

#include "FairDetParRootFileIo.h"
#include "TFile.h"
#include "TArrayI.h"

class FairParRootFile;
class FairParSet;
class PndSttDigiPar;

class PndSttParRootFileIo : public FairDetParRootFileIo 
{
public:
  PndSttParRootFileIo(FairParRootFile* f);
  ~PndSttParRootFileIo();
  Bool_t init(FairParSet*,Int_t*);
  Bool_t read(PndSttDigiPar*,Int_t*);
  ClassDef(PndSttParRootFileIo,0) // Class for STT parameter I/O from ROOT file
};

#endif  /* !PNDSTTPARROOTFILEIO_H */










