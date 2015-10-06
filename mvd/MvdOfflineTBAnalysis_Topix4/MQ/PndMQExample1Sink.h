/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
/**
 * PndMQExample1Sink.h
 *
 * @since 2014-10-10
 * @author A. Rybalchenko
 */

#ifndef PNDMQEXAMPLE1SINK_H_
#define PNDMQEXAMPLE1SINK_H_

#include "FairMQDevice.h"

class PndMQExample1Sink : public FairMQDevice
{
  public:
    PndMQExample1Sink();
    virtual ~PndMQExample1Sink();

  protected:
    virtual void Run();
};

#endif /* FAIRMQEXAMPLE1SINK_H_ */
