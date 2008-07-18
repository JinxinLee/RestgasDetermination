
// This Class' Header ------------------
#include "PndTpcRawDEdxCollection.h"

// C/C++ Headers ----------------------
#include <iostream>
#include <vector>
//#include <algorithm>

using namespace std;

// Collaborating Class Headers --------


// Class Member definitions -----------
ClassImp(PndTpcRawDEdxCollection);


void PndTpcRawDEdxCollection::SetRawDEdx(Double_t dE, Double_t dx)
{
	DEDx dedx;
	dedx.fDE=dE;
	dedx.fDx=dx;
	fRawDEDx.push_back( dedx );
}

int d2i(double d)	//TODO
{
  return (int)(d<0?d-.5:d+.5);
}


void PndTpcRawDEdxCollection::Truncate(std::vector<DEDx> &Truncated, double minTruncFactor, double maxTruncFactor) const
{
	int nh=fRawDEDx.size();
	unsigned int nHitsToProcess=d2i(nh*maxTruncFactor);
	unsigned int nHitToStart=d2i(nh*minTruncFactor);
	
	/*
	cout << "\nTruncate..." << endl;
	cout << "maxFax: " << maxTruncFactor << "  minFac: " << minTruncFactor << endl;
	cout << "nHitsToProcess:  "<< nHitsToProcess << endl;
	cout << "nHitToStart:  "<< nHitToStart << endl;
*/
	
	int nConsideredHits=0;
	for (int q=0; q<nHitsToProcess; q++ ) 
	{
		if(q >= nHitToStart)	{ 
			nConsideredHits++;
			Truncated.push_back(fRawDEDx[q]);
		}	
	}
}

Double_t
PndTpcRawDEdxCollection::GetSumDE(double minTruncFactor, double maxTruncFactor) const
{
	vector<DEDx> Truncated;
	Truncate(Truncated,minTruncFactor,maxTruncFactor);
	vector<DEDx>::const_iterator cit;
	vector<DEDx>::const_iterator end=Truncated.end();
	Double_t Sum=0.;
	for(cit=Truncated.begin();cit!=end;++cit)
	{
		Sum+=(*cit).fDE;
	}
	return Sum;
}

Double_t
PndTpcRawDEdxCollection::GetSumDx(double minTruncFactor, double maxTruncFactor) const
{
	vector<DEDx> Truncated;
	Truncate(Truncated,minTruncFactor,maxTruncFactor);
	
	Double_t Sum=0.;
	vector<DEDx>::const_iterator cit;
	vector<DEDx>::const_iterator end=Truncated.end();
	for(cit=Truncated.begin();cit!=end;++cit)
	{
		Sum+=(*cit).fDx;		
	}
	return Sum;
}

Double_t PndTpcRawDEdxCollection::TruncateAndMean(double minTruncFactor, double maxTruncFactor, int dbgLevel) const
{
	if(!CheckTruncation(minTruncFactor,maxTruncFactor) )	{
		return -1.;
	}
	
	vector<DEDx> Truncated;
	Truncate(Truncated,minTruncFactor,maxTruncFactor);
	if (!ChecknConsideredHits(Truncated.size(), dbgLevel ))	{
		return -1;
	}
	
	double sumDE=0;
	double sumDx=0;
	vector<DEDx>::const_iterator cit;
	for(cit=Truncated.begin();cit!=Truncated.end();cit++)
	{
		double de=cit->fDE;
		double dx=cit->fDx;
		PrintDEDx(de,dx,dbgLevel);
		sumDE+=de;
		sumDx+=dx;
	}
	
	if(sumDx)	{
		return (sumDE/sumDx);
	}
	else	{
		if(dbgLevel)	{
			cout << "TruncateAndMean Warning: No distances summed up!" << endl;
		}
		return -1.;
	}
}

Double_t 
PndTpcRawDEdxCollection::TruncateAndMeanAlternative(double minTruncFactor, double maxTruncFactor, int dbgLevel) const
{
	if(!CheckTruncation(minTruncFactor,maxTruncFactor) )	{
		return -1.;
	}
	
	vector<DEDx> Truncated;
	Truncate(Truncated,minTruncFactor,maxTruncFactor);
	if (!ChecknConsideredHits(Truncated.size(), dbgLevel ))	{
		return -1;
	}

	double sumDEdx=0;
	vector<DEDx>::const_iterator cit;
	int nConsideredHits=0;
	for(cit=Truncated.begin();cit!=Truncated.end();cit++)
	{	
		double de=cit->fDE;
		double dx=cit->fDx;
		PrintDEDx(de,dx,dbgLevel ? dbgLevel : 1);
		if(!dx)	{
			///TODO: if fDx is zero, de/dx will be smaller than necessary? (for MC only)	
			continue;
		}
		sumDEdx+=de/dx;
		nConsideredHits++;
	}
	
	
	if(nConsideredHits && ChecknConsideredHits(nConsideredHits, dbgLevel ))	{
		return sumDEdx/nConsideredHits;
	}
	else	{
		if(dbgLevel)	{
			cout << "TruncateAndMean Warning: Nothing done!" << endl;
		}
		return -1.;
	}
}

void PndTpcRawDEdxCollection::PrintDEDx(double de, double dx, int dbgLevel) const
{
	if(dbgLevel)	{
		if(!de)	{
			cout << "Warning: Zero Energy loss!" << endl;
		}
		if(!dx)	{
			cout << "Warning: dx zero!" << endl;
		}
	}
	if(dbgLevel>1)	{
		cout << de << "  -  " << dx;
			if( dx )	{
				cout << "  -  " <<  de/dx << endl;	
			}
	}
}

bool PndTpcRawDEdxCollection::CheckTruncation(double minTruncFactor, double maxTruncFactor) const
{
	if(minTruncFactor > 1.0 || maxTruncFactor > 1.0 || minTruncFactor<0. || maxTruncFactor<0.
		|| minTruncFactor==maxTruncFactor )	{
			cout << "TruncateAndMean Error: minTruncFactor = " << minTruncFactor
			<< " maxTruncFactor = " << maxTruncFactor << endl;
			return false;
	}
	return true;
}

bool PndTpcRawDEdxCollection::ChecknConsideredHits(int nConsideredHits, int dbgLevel) const
{
	if(nConsideredHits<fMinHits)	{
		if(dbgLevel)	{
			cout << "TruncateAndMean Warning: Considered " << nConsideredHits << " Hits only!" << endl;
		}
		return false;
	}
	return true;
}

void PndTpcRawDEdxCollection::Close()
{
	std::sort(fRawDEDx.begin(), fRawDEDx.end());
	/*
	vector<DEDx>::const_iterator cit;
	for(cit=fRawDEDx.begin(); cit!=fRawDEDx.end(); cit++)
	{
	
		cout << "DE: " << cit->fDE << " dx: " << cit->fDx;
		if(cit->fDx)	{
		 	cout << " DE/dx: " << cit->fDE/cit->fDx; 
		}		
		cout << endl;
	}
	*/
}
