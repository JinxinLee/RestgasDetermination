#ifndef VABSWRITER_H
#define VABSWRITER_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// VAbsWriter								//
//                                                                      //
// Abstract Writer interface						//
//                                                                      //
// Author: Marcel Kunze, RUB, May 2001					//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//									//
//////////////////////////////////////////////////////////////////////////

#include "TNamed.h"

class VAbsTag;
class VAbsBit;
class PAFEventBase;
class PAFListBase;
class PAFAodBase;
class PAFMcBase;
class PAFMcAssocBase;
class PAFTagBase;
class TCandidate;
class PAFStream;
class TCandList;
class VAbsCollection;
class TEventManager;

class BtaCandAssocMapR;

class  VAbsWriter: public TNamed {
public:
    
    VAbsWriter();
    VAbsWriter(const char *file); 
    VAbsWriter(VAbsCollection *collection); 
    virtual ~VAbsWriter();

    virtual void WriteEvent(TEventManager *evtmgr, TCandList *list=0) = 0;
    virtual UInt_t GetRunNumber() const = 0;    
    virtual UInt_t GetEventNumber() const = 0;    
    virtual PAFEventBase& GetHeader() const = 0;    
    virtual VAbsTag&	  GetTag() const = 0;    
    virtual VAbsBit&	  GetBit() const = 0;    
    virtual PAFListBase&  GetListBase() const = 0;
    
    virtual Bool_t SetStreamStatus(const char *stream,Bool_t status=kTRUE) = 0;
    virtual void InitializeStreams() = 0;
    virtual PAFStream* NewStream(const char* AFileNameBase, const char* ATreeName) = 0;
    virtual void InitWrite(const char *file) = 0;   
    virtual void InitUpdate(const char *file) = 0; 
    virtual void NewEvent() = 0;    
    virtual void NewChargedCandidate() = 0;
    virtual void NewNeutralCandidate() = 0;    
    // Fill Elements which are to be stored in Tree
    virtual void Fill( VAbsTag*) = 0;       
    virtual void Fill( PAFTagBase*) = 0;        
    virtual void Fill( VAbsBit* ) = 0;       
    virtual Int_t Fill( PAFAodBase* ) = 0;        
    virtual void Fill( PAFMcBase* ) = 0;       
    virtual void Fill( PAFMcAssocBase* ) = 0;        
    virtual void Fill( BtaCandAssocMapR* ) = 0;        
    // Write Event to Trees
    virtual void Fill() = 0;   
    virtual void ClearBuffer() = 0;   
    virtual void Store() = 0;   
    virtual void DumpList(TCandList *list) = 0;
    
    virtual VAbsCollection *GetCollection() = 0;
    virtual void Exclude(TCandidate *c) = 0;

    virtual Bool_t Tag() const = 0;
    virtual Bool_t Aod() const = 0;
    virtual Bool_t Mct() const = 0;
    virtual Bool_t Map() const = 0;

private :
    VAbsCollection *fCollection; // Write a collection

public:
    ClassDef(VAbsWriter,1) // Base class to write ROOT data files
};

#endif

