#include "PndLumiContFact.h"

static PndLumiContFact gPndLumiContFact;

PndLumiContFact :: PndLumiContFact()
{
	//called when the lib is loaded
	  fName="PndLumiContFact";
	  fTitle="Factory for parameter containers in libPndLumi";
	  setAllContainers();
	  FairRuntimeDb::instance()->addContFactory(this);
}

void PndLumiContFact::setAllContainers()
{
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the Lumi library.*/

    FairContainer* p= new FairContainer("PndLumiGeoPar","PndLumi Geometry Parameters","TestDefaultContext");
    p->addContext("TestNonDefaultContext");
    containers->Add(p);

    FairContainer* p1= new FairContainer("LumiStripDigiPara","PndLumi Digitization Parameters","TestDefaultContext");
    p1->addContext("TestNonDefaultContext");
    containers->Add(p1);

}

FairParSet* PndLumiContFact :: createContainer(FairContainer* c)
{
	const char* name=c->GetName();
	  FairParSet* p=NULL;
	  if (strcmp(name,"PndLumiGeoPar")==0) {
	    p=new PndLumiGeoPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
	  }

	  if (strcmp(name,"LumiStripDigiPara")==0) {
	  p=new PndLumiDigiPara(c->getConcatName().Data(),c->GetTitle(),c->getContext());
	  }

	  return p;
}
ClassImp(PndLumiContFact)

