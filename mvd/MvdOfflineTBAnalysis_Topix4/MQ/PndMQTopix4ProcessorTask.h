/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
/**
 * PndMQTopix4Sink.h
 *
 * @since 2014-10-10
 * @author A. Rybalchenko
 */

#ifndef PNDMQTOPIX4PROCESSORTASK_H_
#define PNDMQTOPIX4PROCESSORTASK_H_

#include "FairMQDevice.h"
#include "FairMQProcessorTask.h"
#include "PndMvdReadInToPix4TBData.h"

class PndMQTopix4ProcessorTask : public FairMQProcessorTask
{
  public:
    PndMQTopix4ProcessorTask();
    virtual ~PndMQTopix4ProcessorTask();

    virtual InitStatus Init(){ return kSUCCESS; };

    virtual void Exec(Option_t* opt = "0");

  private:
    bool fHasBoostSerialization;

    PndMvdReadInToPix4TBData fTopixDataReader;
};

#endif /* FAIRMQEXAMPLE1SINK_H_ */
