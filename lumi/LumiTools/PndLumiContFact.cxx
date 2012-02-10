#include "PndLumiContFact.h"

static PndLumiContFact gPndLumiContFact;

PndLumiContFact :: PndLumiContFact()
{
	//called when the lib is loaded
	  fName="PndLumiContFact";
	  fTitle="Factory for parameter containers in libPndLumi";
	  fDigiParNames = new TList();
	  fLinFitParNames = new TList();
	  setAllContainers();
	  FairRuntimeDb::instance()->addContFactory(this);
}

PndLumiContFact::~PndLumiContFact()
{
  if(0!=fDigiParNames)
  {
    fDigiParNames->Delete();
    delete fDigiParNames;
  }
  if(0!=fLinFitParNames)
    {
      fLinFitParNames->Delete();
      delete fLinFitParNames;
    }
}
void PndLumiContFact::setAllContainers()
{
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the Lumi library.*/

    FairContainer* p= new FairContainer("PndLumiGeoPar","PndLumi Geometry Parameters","TestDefaultContext");
    p->addContext("TestNonDefaultContext");
    fDigiParNames->Add(new TObjString(p->GetName()));
    containers->Add(p);

    FairContainer* p1= new FairContainer("LumiStripDigiPara","PndLumi Digitization Parameters","TestDefaultContext");
    p1->addContext("TestNonDefaultContext");
    fDigiParNames->Add(new TObjString(p1->GetName()));
    containers->Add(p1);

    FairContainer* p2= new FairContainer("LumiLinFitPar","PndLumi Linear Fit Parameters","TestDefaultContext");
    p2->addContext("TestNonDefaultContext");
    fLinFitParNames->Add(new TObjString(p2->GetName()));
    containers->Add(p2);

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

	if (strcmp(name,"LumiLinFitPar")==0) {
		p=new PndLumiLinFitPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
	}


	return p;
}
ClassImp(PndLumiContFact)

