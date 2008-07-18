//*-- AUTHOR : Denis Bertini
//*-- Created : 20/06/2005

/////////////////////////////////////////////////////////////
//
//  PndSttContFact
//
//  Factory for the parameter containers in libStt
//
/////////////////////////////////////////////////////////////

using namespace std;
#include "PndSttContFact.h"
#include "CbmRuntimeDb.h"
#include "PndGeoSttPar.h"
#include "PndSttDigiPar.h"
#include "CbmParRootFileIo.h"
#include "CbmParAsciiFileIo.h"
#include "PndSttParRootFileIo.h"
#include "PndSttParAsciiFileIo.h"
#include <iostream>
#include <iomanip>

ClassImp(PndSttContFact)

static PndSttContFact gPndSttContFact;

PndSttContFact::PndSttContFact() 
{
  // Constructor (called when the library is loaded)
  fName="PndSttContFact";
  fTitle="Factory for parameter containers in libStt";
  setAllContainers();
  CbmRuntimeDb::instance()->addContFactory(this);
}

void PndSttContFact::setAllContainers() 
{
    /** Creates the Container objects with all accepted contexts and adds them to
     *  the list of containers for the STT library.*/
    CbmContainer* p2= new CbmContainer("PndGeoSttPar",
				       "Stt Geometry Parameters",
				       "TestDefaultContext");
    p2->addContext("TestNonDefaultContext");
    
    containers->Add(p2);
}

CbmParSet* PndSttContFact::createContainer(CbmContainer* c) 
{
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char* name=c->GetName();
  cout << " -I container name " << name << endl;
  CbmParSet* p=0;

  if (strcmp(name,"PndGeoSttPar")==0) {
    p=new PndGeoSttPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  return p;
}
/*
void  PndSttContFact::activateParIo(CbmParIo* io) {
  // activates the input/output class for the parameters
  // needed by the Stt
  if (strcmp(io->IsA()->GetName(),"CbmParRootFileIo")==0) {
    PndSttParRootFileIo* p=new PndSttParRootFileIo(((CbmParRootFileIo*)io)->getParRootFile());
    io->setDetParIo(p);
  }
  if (strcmp(io->IsA()->GetName(),"CbmParAsciiFileIo")==0) {
    PndSttParAsciiFileIo* p=new PndSttParAsciiFileIo(((CbmParAsciiFileIo*)io)->getFile());
    io->setDetParIo(p);
  }
}

*/
