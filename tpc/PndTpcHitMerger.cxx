#include "PndTpcHitMerger.h"
#include "PndTpcPoint.h"

#include <assert.h>
#include <iostream>
#include <map>
#include "TRandom.h"

using namespace std;

PndTpcHitMerger::~PndTpcHitMerger()
{
	
}

void PndTpcHitMerger::ConvertHits(const std::vector<PndTpcPoint*> &Hits, std::vector<PndTpcDEDXStorageHelper> &ConvertedHits) const
{
	vector<PndTpcPoint *>::const_iterator it;
	vector<PndTpcPoint *>::const_iterator end=Hits.end();
	
	for(it=Hits.begin(); it!=end; ++it)
	{
		const PndTpcPoint *Hit=*it;
		ConvertedHits.push_back(PndTpcDEDXStorageHelper(Hit->GetEnergyLoss(),Hit->GetLength()));
	}
}

void PndTpcHitMerger::CombineHitsInTrack(double SegmentLength, const vector<PndTpcDEDXStorageHelper> &Hits, vector<PndTpcDEDXStorageHelper> &CombinedHits)
{
	fSegmentLength=SegmentLength;

	vector<PndTpcDEDXStorageHelper>::const_iterator it;
	vector<PndTpcDEDXStorageHelper>::const_iterator end=Hits.end();
	
	vector<PndTpcDEDXStorageHelper>::const_iterator colHits;
	vector<PndTpcDEDXStorageHelper> segmentCollector;
	
	double segmentedDistance=0.;	//summing up the lengths of the already treated hits 
	double TrackLength=0.;		//summing up the length from hit to hit
	
	for(it=Hits.begin(); it!=end; ++it)
	{
		PndTpcDEDXStorageHelper Hit=*it;
		TrackLength+=Hit.GetLength();
		
		while(TrackLength>fSegmentLength+segmentedDistance)	{
			double de=0.;	//if there are no hits in this segment a hit with eloss=0 will be produced
			for(colHits=segmentCollector.begin();colHits!=segmentCollector.end();++colHits)
			{
				de+=(*colHits).GetEnergyLoss();
			}
			CombinedHits.push_back(PndTpcDEDXStorageHelper(de,fSegmentLength));
			segmentedDistance+=fSegmentLength;
			segmentCollector.clear();
		}
		//cout << "segmentedDistance: " << segmentedDistance << endl;
		segmentCollector.push_back(Hit);

	}//end loop over hits
	
	if(fCatchRemaining)	{
		if(segmentCollector.size())	{	//catch the remaining
			double de=0.;
			for(colHits=segmentCollector.begin();colHits!=segmentCollector.end();++colHits)
			{
				de+=(*colHits).GetEnergyLoss();
			}
			CombinedHits.push_back(PndTpcDEDXStorageHelper(de,fSegmentLength));
			segmentedDistance+=fSegmentLength;
		}
	}
	cout << "summed up length between hits: " << TrackLength << endl;
	cout << "segmentedDistance: " << segmentedDistance << endl;
}


void PndTpcHitMerger::CombineHitsInTrack2(double SegmentLength, const vector<PndTpcDEDXStorageHelper> &Hits,
	vector<PndTpcDEDXStorageHelper> &CombinedHits)
{	//this does the same as CombineHitsInTrack, for me this version was easier to understand
	fSegmentLength=SegmentLength;

	vector<PndTpcDEDXStorageHelper>::const_iterator it;
	vector<PndTpcDEDXStorageHelper>::const_iterator end=Hits.end();
	
	double TrackLength=0.;
	double SegmentedLength=fSegmentLength;	//the first hits will be mapped under fSegmentLength
	double TotalEnergyLoss=0.;	//just for debugging
	
	//somehow i get the impression it could be easier done
	//At first map the Hits to buckets along the track
	multimap<double,const PndTpcDEDXStorageHelper> GatheredHits;
	for(it=Hits.begin(); it!=end; ++it)	
	{
		const PndTpcDEDXStorageHelper Hit=*it;
		TrackLength+=Hit.GetLength();	
		TotalEnergyLoss+=Hit.GetEnergyLoss();
		while( TrackLength > SegmentedLength  )	{	//change the mapping key
			SegmentedLength+=fSegmentLength;
		}
		GatheredHits.insert(pair<double,const PndTpcDEDXStorageHelper>(SegmentedLength, Hit));
	}
	
	SegmentedLength=fSegmentLength;	//reset
	
	//go through all possible values of the mapping key
	//if there are hits for a mapping key sum up the energy loss of the hits belong to the key
	
	multimap<double, const PndTpcDEDXStorageHelper>::const_iterator cit;
	pair<multimap<double,const PndTpcDEDXStorageHelper>::const_iterator,multimap<double,const PndTpcDEDXStorageHelper>::const_iterator> ret;	//needed for equal_range
	double nCombinedHits=TrackLength/fSegmentLength;
	//cout << "nCombinedHits: " << nCombinedHits << endl;
	if(!fCatchRemaining)	{
		--nCombinedHits;
	}
	for(int i=0; i<nCombinedHits; ++i)
	{
		if( GatheredHits.find(SegmentedLength)!=GatheredHits.end() )	{
			ret = GatheredHits.equal_range(SegmentedLength);
			double de=0.;
			unsigned int n=0;
    			for (cit=ret.first; cit!=ret.second; ++cit)
			{
				const PndTpcDEDXStorageHelper Hit=cit->second;
				de+=Hit.GetEnergyLoss();
				++n;
			}
			//cout << n << "hits in " << SegmentedLength << "cm" << endl;
			CombinedHits.push_back(PndTpcDEDXStorageHelper(de,fSegmentLength));
		}
		else	{
			//cout << "Produceing empty hit" << endl;
			CombinedHits.push_back(PndTpcDEDXStorageHelper(0,fSegmentLength));
		}
		SegmentedLength+=fSegmentLength;
	}
	
	cout << "summed up length between hits: " << TrackLength << endl;
	cout << "segmentedDistance: " << SegmentedLength-fSegmentLength << endl;
}

void PndTpcHitMerger::CombineHits(unsigned int nHits, const std::vector<PndTpcDEDXStorageHelper> &Hits, std::vector<PndTpcDEDXStorageHelper> &CombinedHits)
{
	vector<PndTpcDEDXStorageHelper>::const_iterator it;
	vector<PndTpcDEDXStorageHelper>::const_iterator end=Hits.end();
	
	double TrackLength=0.;
	double TotalEnergyLoss=0.;	
	double de=0.;
	unsigned int n=1;
	for(it=Hits.begin(); it!=end; ++it)	
	{
		const PndTpcDEDXStorageHelper Hit=*it;
		TrackLength+=Hit.GetLength();	
		TotalEnergyLoss+=Hit.GetEnergyLoss();
		de+=Hit.GetEnergyLoss();
		if(n==nHits)	{
			//ProduceHit(de, TrackLength, CombinedHits);
			CombinedHits.push_back(PndTpcDEDXStorageHelper(de,TrackLength));
			de=0.;
			TrackLength=0.;
			n=0;
		}
		++n;
	}
	if(fCatchRemaining)	{
		if(de>0.)	{
			//ProduceHit(de, TrackLength, CombinedHits);
			CombinedHits.push_back(PndTpcDEDXStorageHelper(de,TrackLength));
		}
	}
}

void PndTpcHitMerger::MaxDxMerging(double SegmentLength, const vector<PndTpcDEDXStorageHelper> &Hits,
	vector<PndTpcDEDXStorageHelper> &CombinedHits)	//das soll heiﬂen dx ist maximal SegmentLength lang(auﬂer
											//es gibt keine hits in einem Bereich
{											//es werden keine 0 hits hergestellt
	fSegmentLength=SegmentLength;

	vector<PndTpcDEDXStorageHelper>::const_iterator it;
	vector<PndTpcDEDXStorageHelper>::const_iterator end=Hits.end();
	
	vector<PndTpcDEDXStorageHelper>::const_iterator colHits;
	vector<PndTpcDEDXStorageHelper> segmentCollector;
	
	double segmentedDistance=0.;	//summing up the lengths of the already treated hits 
	double TrackLength=0.;		//summing up the length from hit to hit
	
	for(it=Hits.begin(); it!=end; ++it)
	{
		PndTpcDEDXStorageHelper Hit=*it;
		TrackLength+=Hit.GetLength();

		if(TrackLength>=(fSegmentLength+segmentedDistance))	{	//the hits that had been collected till now
			double de=0.;
			double dx=0.;
			segmentedDistance+=fSegmentLength;
			if(segmentCollector.size()==0)	{	//nothing had been collected
				continue;
			}
			for(colHits=segmentCollector.begin();colHits!=segmentCollector.end();++colHits)
			{
				de+=(*colHits).GetEnergyLoss();
                    dx+=(*colHits).GetLength();
			}
			CombinedHits.push_back(PndTpcDEDXStorageHelper(de,dx));
			segmentedDistance-=fSegmentLength;
			segmentedDistance+=dx;
			segmentCollector.clear();
		}
		//cout << "segmentedDistance: " << segmentedDistance << endl;
		segmentCollector.push_back(Hit);

	}//end loop over hits
	
	if(fCatchRemaining)	{
		if(segmentCollector.size())	{	//catch the remaining
			double de=0.;
			double dx=0.;
			for(colHits=segmentCollector.begin();colHits!=segmentCollector.end();++colHits)
			{
				de+=(*colHits).GetEnergyLoss();
				dx+=(*colHits).GetLength();
			}
			CombinedHits.push_back(PndTpcDEDXStorageHelper(de,dx));
			segmentedDistance+=fSegmentLength;
		}
	}
	cout << "summed up length between hits: " << TrackLength << endl;
	cout << "segmentedDistance: " << segmentedDistance << endl;
}

void PndTpcHitMerger::MinDxMerging(double SegmentLength, const vector<PndTpcDEDXStorageHelper> &Hits,
	vector<PndTpcDEDXStorageHelper> &CombinedHits)//das soll heiﬂen dx ist minimal SegmentLength lang,
{											//bis auf eventuell der letzte hit
	fSegmentLength=SegmentLength;					//es gibt keine eloss==0 hits
	
	vector<PndTpcDEDXStorageHelper>::const_iterator it;
	vector<PndTpcDEDXStorageHelper>::const_iterator end=Hits.end();
	
	vector<PndTpcDEDXStorageHelper>::const_iterator colHits;
	vector<PndTpcDEDXStorageHelper> segmentCollector;
	
	double segmentedDistance=0.;	//summing up the lengths of the already treated hits 
	double TrackLength=0.;		//summing up the length from hit to hit
	
	for(it=Hits.begin(); it!=end; ++it)
	{
		PndTpcDEDXStorageHelper Hit=*it;
		TrackLength+=Hit.GetLength();
		segmentCollector.push_back(Hit);
		if(TrackLength>fSegmentLength+segmentedDistance)	{
			double de=0.;	//if there are no hits in this segment a hit with eloss=0 will be produced
			double dx=0.;
			assert(segmentCollector.size());
			for(colHits=segmentCollector.begin();colHits!=segmentCollector.end();++colHits)
			{
				de+=(*colHits).GetEnergyLoss();
                    dx+=(*colHits).GetLength();
			}
			CombinedHits.push_back(PndTpcDEDXStorageHelper(de,dx));
			segmentedDistance+=dx;
			segmentCollector.clear();
		}
		//cout << "segmentedDistance: " << segmentedDistance << endl;
		
	}//end loop over hits
	
	if(fCatchRemaining)	{
		if(segmentCollector.size())	{	//catch the remaining
			double de=0.;
			double dx=0.;
			for(colHits=segmentCollector.begin();colHits!=segmentCollector.end();++colHits)
			{
				de+=(*colHits).GetEnergyLoss();
				dx+=(*colHits).GetLength();
			}
			CombinedHits.push_back(PndTpcDEDXStorageHelper(de,dx));
			segmentedDistance+=dx;
		}
	}
	cout << "summed up length between hits: " << TrackLength << endl;
	cout << "segmentedDistance: " << segmentedDistance << endl;
}


