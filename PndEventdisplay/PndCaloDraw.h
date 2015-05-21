/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
// -------------------------------------------------------------------------
// -----              PndCaloDraw header file                       -----
// -----          Created 26/03/09  by T. Stockmanns                   -----
// -------------------------------------------------------------------------


/** PndCaloDraw
 * @author T. Stockmanns
 * @since 26.3.2009
 *   Base class to display 3D Points in Eve as a BoxSet
 *   One has to overwrite the method GetVector which takes a TObject and writes out a TVector3 which is then taken as an input
 *   to place the points.
 *   If one wants to change the color of the points one has to overwrite the method GetValue. This method takes a TObject and
 *   an integer and translates this into an integer as input for the EveBoxSet method DigitValue
 **
 **/

#ifndef PndCaloDraw_H
#define PndCaloDraw_H

#include "FairTask.h"                   // for FairTask, InitStatus

#include "FairTSBufferFunctional.h"     // IWYU pragma: keep needed for cint

#include "Rtypes.h"                     // for Double_t, Int_t, Bool_t, etc
#include "TH2.h"
#include "TEveCaloData.h"
#include "TEveCalo.h"

class FairBoxSet;
class TObject;
class TVector3;
class TClonesArray;
class FairEventManager;
class FairRootManager;

class PndCaloDraw : public FairTask
{

  public:

    /** Default constructor **/
    PndCaloDraw();


    /** Standard constructor
    *@param name        Name of task
    *@param iVerbose    Verbosity level
    **/
    PndCaloDraw(const char* name, Int_t iVerbose = 1);

    /** Destructor **/
    virtual ~PndCaloDraw();

    virtual Double_t GetTimeWindowPlus() {return fTimeWindowPlus;}
    virtual Double_t GetTimeWindowMinus() {return fTimeWindowMinus;}

    /** Set verbosity level. For this task and all of the subtasks. **/
    virtual void SetVerbose(Int_t iVerbose) {fVerbose = iVerbose;};

    virtual void SetTimeWindowMinus(Double_t val);
    virtual void SetTimeWindowPlus(Double_t val);
    virtual void SetStartTime(Double_t val) {fStartTime = val;}
    virtual void UseEventTimeAsStartTime(Bool_t val = kTRUE) {fUseEventTime = val;}

    /** Executed task **/
    virtual void Exec(Option_t* option);

    FairBoxSet* CreateBoxSet();

    void Reset();

  protected:

    Int_t   fVerbose;       //  Verbosity level
    virtual void SetParContainers() ;
    virtual InitStatus Init();
    /** Action after each event**/
    virtual void Finish();

    virtual void FillHisto(TObject* obj);
    virtual TVector3 GetVector(TObject* obj) = 0;
    virtual Double_t GetEnergy(TObject* obj) = 0;

    TClonesArray* fList; //!
    FairEventManager* fEventManager;   //!
    FairRootManager* fManager;
    TH2F* fCaloHisto;
    TEveCaloDataHist* fEveHist;
    TEveCalo3D* fCalo3D;

    Double_t fTimeWindowPlus;
    Double_t fTimeWindowMinus;
    Double_t fStartTime;
    Bool_t fUseEventTime;


  private:
    PndCaloDraw(const PndCaloDraw&);
    PndCaloDraw& operator=(const PndCaloDraw&);
    BinaryFunctor* fStartFunctor;
    BinaryFunctor* fStopFunctor;

    ClassDef(PndCaloDraw,1);

};


#endif
