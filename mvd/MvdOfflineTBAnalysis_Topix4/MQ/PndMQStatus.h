/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
/**
 * PndMQStatus.h
 *
 * @since 2015-11-04
 * @author T. Stockmanns
 */

#ifndef PndMQStatus_H_
#define PndMQStatus_H_

struct PndMQStatus
{
    enum {
    	UNDEFINED,
        RUNNING,
        STOP,
		Last
    };
};

#endif /* PndMQStatus_H_ */
