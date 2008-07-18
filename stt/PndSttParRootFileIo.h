#ifndef PNDSTTPARROOTFILEIO_H
#define PNDSTTPARROOTFILEIO_H

#include "CbmDetParRootFileIo.h"
#include "TFile.h"
#include "TArrayI.h"

class CbmParRootFile;
class CbmParSet;
class PndSttDigiPar;

class PndSttParRootFileIo : public CbmDetParRootFileIo 
{
public:
  PndSttParRootFileIo(CbmParRootFile* f);
  ~PndSttParRootFileIo();
  Bool_t init(CbmParSet*,Int_t*);
  Bool_t read(PndSttDigiPar*,Int_t*);
  ClassDef(PndSttParRootFileIo,0) // Class for STT parameter I/O from ROOT file
};

#endif  /* !PNDSTTPARROOTFILEIO_H */










