#ifndef PNDSTTPARASCIIFILEIO_H
#define PNDSTTPARASCIIFILEIO_H

#include <fstream> 

#include "TObject.h"
#include "TArrayI.h"
#include "CbmDetParAsciiFileIo.h"

class CbmParSet;

class PndSttParAsciiFileIo : public CbmDetParAsciiFileIo {
public:
  PndSttParAsciiFileIo(fstream*);
  ~PndSttParAsciiFileIo() {}
  Bool_t init(CbmParSet*);
  Int_t write(CbmParSet*);
  template<class T> Bool_t read(T*, Int_t*, Bool_t needsClear=kFALSE);
  template<class T> Int_t writeFile2(T*);
  ClassDef(PndSttParAsciiFileIo,0) // Class for STT parameter I/O from Ascii files
};

#endif  /* !PNDSTTPARASCIIFILEIO_H */







