#ifndef TRHO_H
#define TRHO_H
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

#include "TNamed.h"
#include "TString.h"
#include "TTree.h"
#include "TVector3.h"
#include "TDatabasePDG.h"
#include "TList.h"

#include <iostream>

class TRho;
class TEventManager;
class VAbsReader;
class VAbsWriter;
class TPersistenceManager;
class TDirectoryManager;
class TParameterManager;
class TCandListManager;
class TObjectManager;
class PAFDisplay;
class TConditions;
class TEventInfo;
class TAnalysis;
class AbsEnv;
class TRhoTime;
class IfdProxyDict;

R__EXTERN TRho *gRho;

class TRho : public TNamed {
  
private:
  
  TString		    fVersion;		    //Rho version number
  TString		    fVersionDate;	    //Rho version date
  TString		    fEventStore;	    //!Event store to use
  TString		    fParamStore;	    //!Parameter directory to use
  Int_t		    fRun;		    //Run number
  Long_t		    fEvent;		    //Event number
  Int_t		    fMode;		    //Run mode
  Double_t                fField;		    //Default magnetic field
  TEventManager	    *fEventManager;	    //!Handle to event Manager
  VAbsReader	   	    *fEventReader;	    //!Handle to event reader
  VAbsWriter	   	    *fEventWriter;	    //!Handle to event writer
  TDirectoryManager	    *fDirectoryManager;     //!Handle to directory Manager
  TPersistenceManager	    *fPersistenceManager;   //!Handle to persistence Manager
  TParameterManager	    *fParameterManager;     //!Handle to parameter Manager
  TCandListManager	    *fCandListManager;	    //!Handle to list Manager
  TObjectManager	    *fObjectManager;	    //!Handle to selector Manager
  PAFDisplay		    *fDisplay;		    //!Handle to event Display
  TAnalysis		    *fAnalysis;		    //!Handle to the actual analysis
  static TRhoTime	    *fTime;		    //!Handle to the event time
  static TDatabasePDG	    *fPDG;		    //!Handle to particle database
  static TConditions      *fConditions;	    //!Handle to conditions database
  static TEventInfo       *fInfo;		    //!Handle to event info
  void		    Init();
  TList		    *fServices;		    //!List of services
  AbsEnv		    *gblEnv;		    //!Handle to environment
  IfdProxyDict	    *gblPEnv;		    //!Handle to proxy dictionary
  static TRho	            *fRho;		    //!Handle to persistent object bus (singleton)
  
public:
  TRho(const char* t = "Rho Analysis Framework");
  virtual ~TRho();
  static const TRho* Instance();
  static void   Authorize(const char* user=0, const char* passwd=0);
  
  const char*        GetEventStore() const {return fEventStore;}
  const char*        GetParameterStore() const {return fParamStore;} 
  Int_t              GetRun() const { return fRun;}
  Long_t             GetEvent() const { return fEvent;}
  Int_t              GetMode()  const {return fMode;}
  const char*        GetVersion() const {return fVersion;}
  const char*        GetVersionDate() const {return fVersionDate;}
  virtual void       SetEventStore(const char* store) {fEventStore = store; }
  virtual void       SetParameterStore(const char* store) {fParamStore = store; } 
  virtual void       SetRun(Int_t run=1)     {fRun=run;}
  virtual void       SetEvent(Int_t event=1) {fEvent=event;}
  virtual void       SetMode(Int_t mode=0)   {fMode=mode;}
  
  // Services
  
  void		RegisterService(TObject *service);
  TEventManager*	RegisterService(TEventManager *evtmgr);
  VAbsWriter*		RegisterService(VAbsWriter *writer);
  VAbsReader*		RegisterService(VAbsReader *reader);
  TPersistenceManager* RegisterService(TPersistenceManager *persmgr);
  TDirectoryManager*	RegisterService(TDirectoryManager *dirmgr);
  TParameterManager*	RegisterService(TParameterManager *parmgr);
  TCandListManager*	RegisterService(TCandListManager *listmgr);
  TObjectManager*	RegisterService(TObjectManager *objmgr);
  PAFDisplay*	RegisterService(PAFDisplay *display);
  TDatabasePDG*	RegisterService(TDatabasePDG *pdg);
  TConditions*	RegisterService(TConditions *conditions);
  TAnalysis*		RegisterService(TAnalysis *analysis);
  
  // Managers
  TEventManager*	SetEventManager(TEventManager *evtmgr);
  TEventManager*	GetEventManager() const;
  VAbsWriter*		SetEventWriter(VAbsWriter *writer);
  VAbsWriter*		GetEventWriter() const;
  VAbsReader*		SetEventReader(VAbsReader *reader);
  VAbsReader*		GetEventReader() const;
  TPersistenceManager* SetPersistenceManager(TPersistenceManager *persmgr);
  TPersistenceManager* GetPersistenceManager() const;
  TDirectoryManager*	SetDirectoryManager(TDirectoryManager *dirmgr);
  TDirectoryManager*	GetDirectoryManager() const;
  TParameterManager*	SetParameterManager(TParameterManager *parmgr);
  TParameterManager*	GetParameterManager() const;
  TCandListManager*	SetCandListManager(TCandListManager *listmgr);
  TCandListManager*   GetCandListManager() const;
  TObjectManager*	SetObjectManager(TObjectManager *objmgr);
  TObjectManager*	GetObjectManager() const;
  PAFDisplay*	SetDisplay(PAFDisplay *display);
  PAFDisplay*	GetDisplay() const;
  TDatabasePDG*	SetPDG(TDatabasePDG *pdg);
  TDatabasePDG*	GetPDG() const;
  TRhoTime*		SetTime(TRhoTime *time) const;
  TRhoTime*		GetTime() const;
  TConditions*	SetConditions(TConditions *conditions) const;
  TConditions*	GetConditions() const;
  TEventInfo*		SetEventInfo(TEventInfo *info) const;
  TEventInfo*		GetEventInfo() const;
  void		SetMagnetField(Double_t field);
  Double_t		GetMagnetField() const;
  TAnalysis*		SetAnalysis(TAnalysis *analysis);
  TAnalysis*		GetAnalysis() const;
  AbsEnv*		SetAbsEnv(AbsEnv *env);
  AbsEnv*		GetAbsEnv() const;
  IfdProxyDict*	SetProxyDict(IfdProxyDict *proxy);
  IfdProxyDict*	GetProxyDict() const;
  
  void PrintOn( std::ostream& o=std::cout ) const;
  
  ClassDef(TRho, 1)   // Rho Analysis Framework base class 
};

// standalone print
std::ostream&  operator << (std::ostream& o, const TRho&);

#endif
