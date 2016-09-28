/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
/**
 * PndMQTopix4TrackFitter.h
 *
 * @since 2014-10-10
 * @author A. Rybalchenko
 */

#ifndef PndMQTopix4TrackFitter_H_
#define PndMQTopix4TrackFitter_H_

#include "FairMQDevice.h"

#include "PndSdsHit.h"
#include "PndSimpleTrack.h"

#include "PndMQStraightLineTrackFinder.h"

#include <boost/serialization/access.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/vector.hpp>

class PndMQTopix4TrackFitter : public FairMQDevice
{
  public:
    PndMQTopix4TrackFitter();
    virtual ~PndMQTopix4TrackFitter();

//    static void CustomCleanup(void *data, void *object);

    template <class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& fTopixData;
		ar& fOutputData;
	}

  protected:
    virtual void Run();

	#ifndef __CINT__ // for BOOST serialization
		friend class boost::serialization::access;
		bool fHasBoostSerialization;
	#endif // for BOOST serialization

    std::vector< std::vector<PndSdsHit> > fTopixData;
    std::vector< std::vector<PndSimpleTrack> > fOutputData;
    std::array<int, 6> fNtracksPerEvent;
    int fEventNr;
};

#endif /* FAIRMQEXAMPLE1SINK_H_ */
