// #pragma once
#include <TObject.h>
#include <TClonesArray.h>

#ifndef TSEVENT_H
#define TSEVENT_H

typedef unsigned short WORD;
typedef unsigned int DWORD;

class TsEvent : public TObject
{
public:
    TsEvent();
    virtual ~TsEvent();
    
public:
    void Clear(const Option_t* = NULL);
    void SetEventId(UInt_t eventId) {fEventId=eventId;}
    void FillSiHits(int adcChannel, DWORD* hits, WORD count);
    void FillTdcValues(DWORD* tdcValues, WORD count);
    void FillQdcValues(DWORD* qdcValues, WORD count);
    void FillGiTdcValues(DWORD* tdcValues, WORD count);
    void FillGiQdcValues(DWORD* qdcValues, WORD count);
    void FillScalerValues(DWORD* qdcValues, WORD count);
    
    UInt_t GetEventId() {return fEventId;}
    TClonesArray* GetSiHitList() {return fSiHitList;}
    TClonesArray* GetTdcValues() {return fTdcValues;}
    TClonesArray* GetQdcValues() {return fQdcValues;}
    //    TClonesArray* GetGiTdcValues() {return fGiTdcValues;}
    //    TClonesArray* GetGiQdcValues() {return fGiQdcValues;}
    Int_t GetScalerValueCount() {return fNumScalerValues;}
    UInt_t* GetScalerValues() {return fScalerValues;}
protected:
    
    // data:
public:
protected:
    UInt_t fEventId;                //unique ID for the Event
    TClonesArray* fSiHitList;       //list of hit channels on silicon strip detectors
    TClonesArray* fTdcValues;       //TDC raw values
    TClonesArray* fQdcValues;       //QDC raw values
    //    TClonesArray* fGiTdcValues;     //TDC raw values (for Giessen TDC)
    //    TClonesArray* fGiQdcValues;     //QDC raw values (for Giessen QDC)
    Int_t fNumScalerValues;
    UInt_t* fScalerValues;          //[fNumScalerValues] Scaler values (usually 16 UInt_t values in array)
    size_t fScalerValuesAllocSize;   //! allocated memory for fScalerValues
    
private:
    ClassDef(TsEvent,1);
};
#endif
