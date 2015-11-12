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

#ifndef PndMQTopix4DigiToHit_H_
#define PndMQTopix4DigiToHit_H_

//#include "FairMQDevice.h"
#include "FairMQDevice.h"
#include "PndMQGapEventBuilder.h"
#include "PndMvdTopixClusterFinder.h"
#include "PndMvdTopixHitProducer.h"
#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/deque.hpp>
#include <PndMQTopixHitProducer.h>

class PndMQTopix4DigiToHit : public FairMQDevice
{
   public:
	PndMQTopix4DigiToHit();
	virtual ~PndMQTopix4DigiToHit();

    template <class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
    	ar& fTopixDigis;
    	ar& fTopixHitsEvent;
    	ar& fClusterSize;
	}

    std::vector<int> GetClusterSize() {return fClusterSize;}

  protected:
    virtual void Run();
  private:
	#ifndef __CINT__ // for BOOST serialization
    	friend class boost::serialization::access;
	#endif // for BOOST serialization
    std::vector<PndSdsDigiTopix4> fTopixDigis;
    std::deque<std::vector<PndSdsHit> > fTopixHitsEvent;
    std::vector<int> fClusterSize;

    bool fHasBoostSerialization;
    PndMQGapEventBuilder fEventBuilder;
    PndMvdTopixClusterFinder fClusterFinder;
 //   PndMvdTopixHitProducer fHitProducer;
    PndMQTopixHitProducer fHitProducer;
    bool fStatusOutput;
};

#endif /* FAIRMQEXAMPLE1SINK_H_ */
