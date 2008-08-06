//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TRho									//
//                                                                      //
// Application services							//
// Holds generic settings, handles to managers and implements the	//
// persistent object bus. The class takes ownership of the registered	//
// services and deletes the corresponding objects.			//
//                                                                      //
// Author: Marcel Kunze, RUB, 1999-2000					//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#include "stdlib.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TNetFile.h"
#include "TAuthenticate.h"

#include "RhoBase/TRho.h"
#include "RhoBase/TRhoVersion.h"
#include "RhoBase/TPdt.h"

TRho *gRho;
//DllExport TRho *gRho;
//DllImport TSystem *gSystem;
//DllImport TROOT *gRoot;

TRho*  TRho::fRho = 0;
TRhoTime*  TRho::fTime = 0;
TDatabasePDG*  TRho::fPDG = 0;
TConditions* TRho::fConditions = 0;
TEventInfo* TRho::fInfo = 0;

ClassImp(TRho)

#include <iostream>
  //#include <sstream>

using namespace std;

TRho::TRho(const char* t) : 
fVersion(RHOVERSION), fVersionDate(RHODATE), fField(1.51007),
fEventManager(0),fEventReader(0),fEventWriter(0),
fDirectoryManager(0),fPersistenceManager(0),
fParameterManager(0),fDisplay(0),fAnalysis(0),fCandListManager(0),
fObjectManager(0), fEventStore(TString("")), fParamStore(TString("")),
gblEnv(0),gblPEnv(0)
{
    if (!gROOT->Initialized()) {
      static TROOT root("RHO","A ROOT Analysis Framework");
    }

    SetTitle(t);
    fRho = this;
    fServices = new TList();
    Init();
}

TRho::~TRho()
{
    if (fServices != 0) { fServices->Delete(); delete fServices; fServices=0; }
    gRho = fRho = 0;
}

const TRho* TRho::Instance()
{
    if (fRho == 0) gRho = fRho = new TRho();
    return fRho;
}

void TRho::Authorize(const char *u, const char *p)
{
// Authorization for ROOTD might be explicit or by use of environment variables

    TString user,passwd;

    if (u != 0)
       user = u;
    else
       if (getenv("RHOUSER")!=0)  user = getenv("RHOUSER");

    if (p != 0)
       passwd = p;
    else
       if (getenv("RHOPASSWD")!=0) passwd = getenv("RHOPASSWD");

    cout << "TRho::Authorize " << user.Data() << " for ROOTD access" << endl;

    TAuthenticate::SetGlobalUser(user); 
    TAuthenticate::SetGlobalPasswd(passwd);
}

void TRho::Init()
{
    // Try different locations; as a last ressort, BFROOT should be set

    // Set the parameter location
    if (fParamStore=="" && getenv("RHOPARM")!=0) fParamStore = TString(getenv("RHOPARM"));
    if (fParamStore=="" && getenv("RHO")!=0)     fParamStore = TString(getenv("RHO"))+"/RhoParameters";
    if (fParamStore=="" && getenv("BFROOT")!=0)  fParamStore = TString(getenv("BFROOT")) + "/unsupported/PAF/PAFParameters";
	if (fParamStore=="" && getenv("VMCWORKDIR")!=0) fParamStore = TString(getenv("VMCWORKDIR"))+"/rho/RhoParameters";
 
    // Set the data store location
    if (fEventStore=="" && getenv("RHOROOT")!=0) fEventStore = TString(getenv("RHOROOT"));
    if (fEventStore=="" && getenv("BFROOT")!=0)  fEventStore = TString(getenv("BFROOT")) + "/kanga";
}

void TRho::RegisterService(TObject *service)
{
    TString name = TString(service->ClassName());

    if (name=="TEventManager")
	SetEventManager((TEventManager *) service);
    else if (name=="VAbsReader")
	SetEventReader((VAbsReader *) service);
    else if (name=="VAbsWriter")
	SetEventWriter((VAbsWriter *) service);
    else if (name=="TPersistenceManager")
	SetPersistenceManager((TPersistenceManager *) service);
    else if (name=="TDirectoryManager")
	SetDirectoryManager((TDirectoryManager *) service);
    else if (name=="TParameterManager")
	SetParameterManager((TParameterManager *) service);
    else if (name=="TCandListManager")
	SetCandListManager((TCandListManager *) service);
    else if (name=="TObjectManager")
	SetObjectManager((TObjectManager *) service);
    else if (name=="PAFDisplay")
	SetDisplay((PAFDisplay *) service);
    else if (name=="TDatabasePDG")
	SetPDG((TDatabasePDG *) service);
    else if (name=="TConditions")
	SetConditions((TConditions *) service);
    else if (name=="TAnalysis")
	SetAnalysis((TAnalysis *) service);
    else {
	cerr << "TRho: Unknown service " << name << endl;
	return;
    }

    fServices->Add(service);

}

TEventManager*
TRho::RegisterService(TEventManager *service)
{
    fServices->Add((TObject *)service);
    return SetEventManager(service);
}

VAbsReader*
TRho::RegisterService(VAbsReader *service)
{
    fServices->Add((TObject *)service);
    return SetEventReader(service);
}

VAbsWriter*
TRho::RegisterService(VAbsWriter *service)
{
    fServices->Add((TObject *)service);
    return SetEventWriter(service);
}

TPersistenceManager*
TRho::RegisterService(TPersistenceManager *service)
{
    fServices->Add((TObject *)service);
    return SetPersistenceManager(service);
}

TDirectoryManager*
TRho::RegisterService(TDirectoryManager *service)
{
    fServices->Add((TObject *)service);
    return SetDirectoryManager(service);
}

TParameterManager*
TRho::RegisterService(TParameterManager *service)
{
    fServices->Add((TObject *)service);
    return SetParameterManager(service);
}

TCandListManager*
TRho::RegisterService(TCandListManager *service)
{
    fServices->Add((TObject *)service);
    return SetCandListManager(service);
}

TObjectManager*
TRho::RegisterService(TObjectManager *service)
{
    fServices->Add((TObject *)service);
    return SetObjectManager(service);
}

PAFDisplay* 
TRho::RegisterService(PAFDisplay *service)
{
    fServices->Add((TObject *)service);
    return SetDisplay(service);
}

TDatabasePDG* 
TRho::RegisterService(TDatabasePDG *service)
{
    fServices->Add((TObject *)service);
    return SetPDG(service);
}

TConditions* 
TRho::RegisterService(TConditions *service)
{
    fServices->Add((TObject *)service);
    return SetConditions(service);
}

TAnalysis*
TRho::RegisterService(TAnalysis *service)
{
    fServices->Add((TObject *)service);
    return SetAnalysis(service);
}


TEventManager*               
TRho::SetEventManager(TEventManager *evtmgr) 
{
    return fEventManager=evtmgr;
}

TEventManager* 
TRho::GetEventManager() const 
{
    return fEventManager;
}

VAbsReader*               
TRho::SetEventReader(VAbsReader *reader) 
{
    return fEventReader=reader;
}

VAbsReader* 
TRho::GetEventReader() const 
{
    return fEventReader;
}

VAbsWriter*               
TRho::SetEventWriter(VAbsWriter *writer) 
{
    return fEventWriter=writer;
}

VAbsWriter* 
TRho::GetEventWriter() const 
{
    return fEventWriter;
}

TPersistenceManager*               
TRho::SetPersistenceManager(TPersistenceManager *persmgr) 
{
    return fPersistenceManager=persmgr;
}

TPersistenceManager* 
TRho::GetPersistenceManager() const 
{
    return fPersistenceManager;
}

TDirectoryManager *               
TRho::SetDirectoryManager(TDirectoryManager *dirmgr) 
{
    return fDirectoryManager=dirmgr;
}

TDirectoryManager* 
TRho::GetDirectoryManager() const 
{
    return fDirectoryManager;
}

TParameterManager*               
TRho::SetParameterManager(TParameterManager *parmgr) 
{
    return fParameterManager=parmgr;
}

TParameterManager* 
TRho::GetParameterManager() const 
{
    return fParameterManager;
}

TCandListManager*               
TRho::SetCandListManager(TCandListManager *listmgr) 
{
    return fCandListManager=listmgr;
}

TCandListManager* 
TRho::GetCandListManager() const 
{
    return fCandListManager;
}

TObjectManager*               
TRho::SetObjectManager(TObjectManager *objmgr) 
{
    return fObjectManager=objmgr;
}

TObjectManager* 
TRho::GetObjectManager() const 
{
    return fObjectManager;
}

PAFDisplay*               
TRho::SetDisplay(PAFDisplay *display) 
{
    return fDisplay=display;
}

PAFDisplay*      
TRho::GetDisplay() const 
{
    return fDisplay;
}

TDatabasePDG*               
TRho::SetPDG(TDatabasePDG *pdg) 
{
    return fPDG=pdg;
}

TDatabasePDG*      
TRho::GetPDG() const 
{
    if (fPDG == 0) {
      // Instantiate a PDT and complement with further particles
      //TString name = TString("evt.pdl"); //TRho::Instance()->GetParameterStore()+TString("/PDT/pdt.table");
      TString name = TRho::Instance()->GetParameterStore()+TString("/PDT/evt.pdl");
      cout << "TRho::GetPDG: Instantiating TDatabasePDG from " << name.Data() << endl;
      fPDG = new TPdt(name);
      fPDG->AddParticle("Rootino","",0.0,kTRUE,1.e38,0.0,"Artificial",0,-1,0);
    }
    return fPDG;
}

TRhoTime*               
TRho::SetTime(TRhoTime *time) const
{
    return fTime=time;
}

TRhoTime*      
TRho::GetTime() const 
{
    return fTime;
}

TConditions*               
TRho::SetConditions(TConditions *conditions) const
{
    return fConditions=conditions;
}

TConditions*     
TRho::GetConditions() const 
{
    if (fConditions==0) {
        cerr << "TRho::GetConditions: No conditions registered, returning 0." << endl;
    }
    return fConditions;
}

TEventInfo*               
TRho::SetEventInfo(TEventInfo *info) const
{
    return fInfo=info;
}

TEventInfo*     
TRho::GetEventInfo() const 
{
    return fInfo;
}

void               
TRho::SetMagnetField(Double_t field) 
{
    fField = field;
}

Double_t           
TRho::GetMagnetField() const 
{ 
    return fField;
}

TAnalysis*               
TRho::SetAnalysis(TAnalysis *analysis) 
{
    return fAnalysis = analysis;
}

TAnalysis*       
TRho::GetAnalysis() const 
{ 
    return fAnalysis; 
}

AbsEnv*
TRho::SetAbsEnv(AbsEnv *env)
{
    return gblEnv = env;
}

AbsEnv*
TRho::GetAbsEnv() const
{
    return gblEnv;
}

IfdProxyDict*
TRho::SetProxyDict(IfdProxyDict *proxy)
{
    return gblPEnv = proxy;
}

IfdProxyDict*
TRho::GetProxyDict() const
{
    return gblPEnv;
}

void TRho::PrintOn(std::ostream& o) const 
{
    o << endl;
    o << "=============================================================" << endl;
    o << "RHO Analysis Framework " << fVersion.Data() << " (" << fVersionDate.Data() << ")" << endl;
    o << "Root" << gROOT->GetVersion() << endl;
    o << "Default event service " << fEventStore.Data() << endl;
    o << "Default param service " << fParamStore.Data() << endl;
    o << "=============================================================" << endl;
    TDatime theTime;
    o << GetTitle() <<  " run at " << theTime.AsString();
	TString theSystem = "unknown system";
	if (gSystem->Getenv("OS")) theSystem = gSystem->Getenv("OS");
	TString theProcessor = "unknown processor";
	if (gSystem->Getenv("PROCESSOR_IDENTIFIER")) theProcessor = gSystem->Getenv("PROCESSOR_IDENTIFIER");
    o << " on " << theSystem.Data() <<  " / " <<  theProcessor.Data() << endl << endl;

    o << "Registered services:" << endl;
    for (int i=0;i<fServices->GetSize();i++)
	o <<  "  " << fServices->At(i)->ClassName() << endl;
    o << endl;
}

std::ostream&  operator << (std::ostream& o, const TRho& a) { a.PrintOn(o); return o; }

