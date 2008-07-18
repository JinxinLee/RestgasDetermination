//*-- AUTHOR Denis Bertini
//*-- modified: 19/06/2005 by Denis Bertini
//*-- created : 18/06/2005

/////////////////////////////////////////////////////////////
// PndSttDigiPar
//
// Container class for Digitisation parameters
//
/////////////////////////////////////////////////////////////

using namespace std;
#include "PndSttDigiPar.h"
#include "CbmRuntimeDb.h"
#include "CbmParIo.h"
#include "CbmDetParIo.h"
#include "TClass.h"
#include <iostream> 
#include <iomanip>

ClassImp(PndSttDigiPar)

PndSttDigiPar::PndSttDigiPar(const char* name,const char* title,
			     const char* context)
    : CbmParSet(name,title,context) 
{
  // constructor does nothing yet
  detName="Stt";
}

PndSttDigiPar::~PndSttDigiPar() 
{
    // destructor
}

Bool_t PndSttDigiPar::init(CbmParIo* inp) 
{
    // intitializes the container from an input
    cout << "-I- PndSttDigiPar::init " << endl;
    
    CbmDetParIo* input=inp->getDetParIo("PndSttParIo");
    cout << "-I- PndSttDigiPar::init " << input << endl;

    if (input) return (input->init(this));
    return kFALSE;
}

Int_t PndSttDigiPar::write(CbmParIo* output) 
{
    // writes the container to an output
    CbmDetParIo* out=output->getDetParIo("PndSttParIo");
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

void PndSttDigiPar::readline(const char *buf, Int_t *set, fstream *f) 
{
}

void PndSttDigiPar::readline(const char *buf, Int_t *set) 
{
}

void PndSttDigiPar::putAsciiHeader(TString& header) 
{
}

Bool_t PndSttDigiPar::writeline(char *buf, Int_t mod, Int_t strip) 
{
    return kTRUE;
}
