#ifndef PNDSTTDIGIPAR_H
#define PNDSTTDIGIPAR_H

#include "TObject.h"
#include "FairParSet.h"
#include <fstream>


class PndSttDigiPar : public FairParSet 
{
 protected:
 public:
    PndSttDigiPar(const char* name="SttDigiPar",
		  const char* title="Stt Digitisation Parameters",
		  const char* context="TestDefaultContext");
    ~PndSttDigiPar();
    Int_t getSize() { return 0; }
    Bool_t init(FairParIo* input);
    Int_t write(FairParIo* output);
    void clear();
    void printParam();
    void readline(const char*, Int_t*);
    void readline(const char*,Int_t*,std::fstream *);
    void putAsciiHeader(TString&);
    Bool_t writeline(char*, Int_t, Int_t);
    ClassDef(PndSttDigiPar,1) // Container for the Stt Digitisation parameters
};
	
#endif  /*!PNDSTTDIGIPAR_H*/







