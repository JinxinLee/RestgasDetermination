//*-- AUTHOR Denis Bertini
//*-- modified: 19/06/2005 by Denis Bertini
//*-- created : 18/06/2005

/////////////////////////////////////////////////////////////
// PndSttDigiPar
//
// Container class for Digitisation parameters
//
/////////////////////////////////////////////////////////////

#include "PndSttDigiPar.h"
#include "FairRuntimeDb.h"
#include "FairParIo.h"
#include "FairDetParIo.h"
#include "TClass.h"
#include <iostream> 
#include <iomanip>

ClassImp(PndSttDigiPar)

PndSttDigiPar::PndSttDigiPar(const char* name,const char* title,
			     const char* context)
    : FairParSet(name,title,context) 
{
  // constructor does nothing yet
  detName="Stt";
}

PndSttDigiPar::~PndSttDigiPar() 
{
    // destructor
}

Bool_t PndSttDigiPar::init(FairParIo* inp) 
{
    // intitializes the container from an input
    std::cout << "-I- PndSttDigiPar::init " << std::endl;
    
    FairDetParIo* input=inp->getDetParIo("PndSttParIo");
    std::cout << "-I- PndSttDigiPar::init " << input << std::endl;

    if (input) return (input->init(this));
    return kFALSE;
}

Int_t PndSttDigiPar::write(FairParIo* output) 
{
    // writes the container to an output
    FairDetParIo* out=output->getDetParIo("PndSttParIo");
    if (out) return out->write(this);
    return -1;
}

void PndSttDigiPar::clear() 
{
    // clears the container
    status=kFALSE;
    resetInputVersions();
}

void PndSttDigiPar::printParam() 
{
    // prints the calibration parameters
    // to be done
}

//void PndSttDigiPar::readline(const char *buf, Int_t *set, std::fstream *f) 
void PndSttDigiPar::readline(const char *, Int_t *, std::fstream *) 
{
}

//void PndSttDigiPar::readline(const char *buf, Int_t *set) 
void PndSttDigiPar::readline(const char *, Int_t *) 
{
}

//void PndSttDigiPar::putAsciiHeader(TString& header) 
void PndSttDigiPar::putAsciiHeader(TString& ) 
{
}

//Bool_t PndSttDigiPar::writeline(char *buf, Int_t mod, Int_t strip) 
Bool_t PndSttDigiPar::writeline(char *, Int_t , Int_t ) 
{
    return kTRUE;
}
