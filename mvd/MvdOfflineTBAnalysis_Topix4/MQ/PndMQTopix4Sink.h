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

#ifndef PNDMQTOPIX4SINK_H_
#define PNDMQTOPIX4SINK_H_

#include "FairMQDevice.h"
#include "PndMvdReadInToPix4TBData.h"

#include "PndSdsDigiTopix4.h"

class PndMQTopix4Sink : public FairMQDevice
{
  public:
    PndMQTopix4Sink();
    virtual ~PndMQTopix4Sink();

    static void CustomCleanup(void *data, void *object);

  protected:
    virtual void Run();

    std::vector<PndSdsDigiTopix4> fTopixData;

    PndMvdReadInToPix4TBData fTopixDataReader;
};

#endif /* FAIRMQEXAMPLE1SINK_H_ */
