/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
/**
 * PndMvdMQFileSamplerBursts.h
 *
 * @since 2016-07-04
 * @author T.Stockmanns
 */

#ifndef PndMvdMQFileSamplerBursts_H_
#define PndMvdMQFileSamplerBursts_H_

#include <string>

#include "TClonesArray.h"

#include "FairFileSource.h"
#include "FairRunAna.h"
#include "FairTimeStamp.h"

#include "FairMQDevice.h"

#include "PndBurstVectorBuilder.h"

struct BurstHeader
{
	unsigned int fRunID;
	unsigned int fBurstID;
	std::string fBranchName;

	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & fRunID;
		ar & fBurstID;
		ar & fBranchName;
	}

	#ifndef __CINT__ // for BOOST serialization
		friend class boost::serialization::access;
	#endif // for BOOST serialization

};

struct BurstData
{
	BurstData() : fHeader(), fData(){};
	virtual ~BurstData(){
		for (auto itr : fData)
			delete(itr);
		fData.clear();
	};
	BurstHeader fHeader;
	std::vector<FairTimeStamp*> fData;

	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & fHeader;
		ar & fData;
	}

	#ifndef __CINT__ // for BOOST serialization
		friend class boost::serialization::access;
	#endif

};

class PndMvdMQFileSamplerBursts : public FairMQDevice
{
  public:
    enum
    {
        InputFileName = FairMQDevice::Last,
        Last
    };

    PndMvdMQFileSamplerBursts();
    virtual ~PndMvdMQFileSamplerBursts();

    void AddInputFileName  (std::string tempString) { fFileNames.push_back(tempString);}
    void AddInputChannelBranchName(std::pair<std::string, std::string> tempString) {
    	fPortBranchNameMap.insert(std::pair<std::string, std::string>(tempString.first, tempString.second));
    	fBranchNames.push_back(tempString);
    	fPorts.insert(tempString.first);
    }
    
    void SetMaxIndex(int64_t tempInt) {fMaxIndex=tempInt;}

    template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & fHitVector;
	}

	#ifndef __CINT__ // for BOOST serialization
		friend class boost::serialization::access;
	#endif

 protected:
    virtual void Run();
    virtual void InitTask();
    
    virtual void InitBurstBuilder(std::string branchName);

 private: 
    FairRunAna*     fRunAna;
    FairFileSource* fSource;
    std::set<std::string> fPorts;				//< set of output ports
    std::multimap<std::string, std::string>				fPortBranchNameMap;
    std::vector<std::pair<std::string, std::string> >    fBranchNames;			//< vector of output ports and branch names. One port can have multiple branches send to it
    std::map<std::string, std::vector< std::vector<FairTimeStamp*> > >        fOutputData;  //< map of branch and data object to be send
    std::map<std::string, TObject* >				fInputBranches;									//< branch name and TClonesArray in root tree
    std::map<std::string, PndBurstVectorBuilderBase*>		fBurstBuilder;							//< branch name and associated burst builder
    std::vector<FairTimeStamp*> fHitVector;
    FairEventHeader* fEventHeader;


    int             fNObjects;
    int64_t         fMaxIndex;

    std::vector<std::string>     fFileNames;
};

#endif /* PndMvdMQFileSamplerBursts_H_ */
