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

#ifndef PndMQTopix4Processor_H_
#define PndMQTopix4Processor_H_

//#include "FairMQDevice.h"
#include "FairMQDevice.h"
#include "PndMvdReadInToPix4TBData.h"

#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>

class PndMQTopix4Processor : public FairMQDevice
{
  public:
    PndMQTopix4Processor();
    virtual ~PndMQTopix4Processor();

    static void CustomCleanup(void* data, void* hint);

    template <class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
    	ar& fPndSdsDigiTopix4Vector;
	}

  protected:
    virtual void Run();
  private:
	#ifndef __CINT__ // for BOOST serialization
    	friend class boost::serialization::access;
	#endif // for BOOST serialization
    std::vector<PndSdsDigiTopix4> fPndSdsDigiTopix4Vector;

    bool fHasBoostSerialization;

    PndMvdReadInToPix4TBData fTopixDataReader;
};

#endif /* FAIRMQEXAMPLE1SINK_H_ */
